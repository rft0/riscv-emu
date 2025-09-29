#include "mem.h"

#include <stdlib.h>
#include <string.h>

#include "trap.h"

#define RAM_SIZE (512 * 1024 * 1024) // 512 MB

#define PTE_V   (1ULL << 0)
#define PTE_R   (1ULL << 1)
#define PTE_W   (1ULL << 2)
#define PTE_X   (1ULL << 3)
#define PTE_U   (1ULL << 4)
#define PTE_G   (1ULL << 5)
#define PTE_A   (1ULL << 6)
#define PTE_D   (1ULL << 7)

typedef struct tlb_enrty {
    uint64_t vpn;
    uint64_t ppn;
    struct tlb_entry* next;
} tlb_entry_t;

typedef struct mem_region {
    uint64_t base;
    uint64_t size;
    uint8_t* mem;
    uint8_t r;
    uint8_t w;
    uint8_t x;

    // int (*read)(struct mem_region* region, uint64_t offset, void* out, size_t size);
    // int (*write)(struct mem_region* region, uint64_t offset, void* val, size_t size);
} mem_region_t;

typedef struct {
    mem_region_t regions[32];
    int num_regions;
} mem_map_t;

mem_map_t g_memory_map = { 0 };

static void mem_add_region(uint64_t base, uint64_t size, uint8_t r, uint8_t w, uint8_t x, uint8_t* mem) {
    mem_region_t* rgn = &g_memory_map.regions[g_memory_map.num_regions++];
    rgn->base = base;
    rgn->size = size;
    rgn->r = r;
    rgn->w = w;
    rgn->x = x;
    rgn->mem = mem;
}

void mem_init() {
    mem_add_region(0x80000000, RAM_SIZE, 1, 1, 1, malloc(RAM_SIZE));
    mem_add_region(0x10000000, 0x1000, 1, 1, 1, malloc(0x1000)); // UART0
    mem_add_region(0x02000000, 0x10000, 1, 1, 1, malloc(0x10000)); // CLINT
    mem_add_region(0x0c000000, 0x100000, 1, 1, 1, malloc(0x100000)); // PLIC
    //! TODO: Add virtio here
    //! TODO: Check dtb used for qemu
}

void mem_free() {
    for (int i = 0; i < g_memory_map.num_regions; i++)
        if (g_memory_map.regions[i].mem)
            free(g_memory_map.regions[i].mem);

    g_memory_map.num_regions = 0;
}

static mem_region_t* find_region(uint64_t pa) {
    for (int i = 0; i < g_memory_map.num_regions; i++) {
        mem_region_t* r = &g_memory_map.regions[i];
        if (pa >= r->base && pa < r->base + r->size)
            return r;
    }

    return NULL;
}

int sv39_translate(cpu_t* cpu, uint64_t va, access_type_t access, uint64_t* pa_out) {
    uint64_t satp = cpu->csr.satp;
    uint64_t mode = extract64(satp, 63, 60);
    
    if (mode != 8) {  // Sv39
        *pa_out = va;
        return 1;
    }

    //! TODO: TLB check here
    
    uint64_t ppn = extract64(satp, 43, 0);
    uint64_t a = ppn << 12;
    
    uint64_t vpn[3];
    vpn[2] = extract64(va, 38, 30);
    vpn[1] = extract64(va, 29, 21);
    vpn[0] = extract64(va, 20, 12);
    uint64_t offset = extract64(va, 11, 0);
    
    int level = 0;
    uint64_t pte = 0;
    for (level = 0; level < 3; level++) {
        uint64_t pte_addr = a + vpn[level] * 8;  // 8 bytes per PTE
        if (!phys_read(cpu, pte_addr, &pte, 8))
            return 0;
        
        // Check if PTE is valid
        if (!(pte & PTE_V)) {
            uint64_t cause = (access == ACCESS_FETCH) ? CAUSE_INSTR_PF : (access == ACCESS_STORE) ? CAUSE_STORE_PF : CAUSE_LOAD_PF;
            raise_trap(cpu, cause, va, 0);
            return 0;
        }
        
        // Check if this is a leaf PTE (R, W, or X bit set)
        if (pte & (PTE_R | PTE_W | PTE_X)) {
            int ok = 0;
            switch (access) {
                case ACCESS_FETCH:
                    ok = (pte & PTE_X) != 0;
                    break;
                case ACCESS_LOAD:
                    ok = (pte & PTE_R) != 0;
                    break;
                case ACCESS_STORE:
                    ok = (pte & PTE_W) != 0;
                    break;
            }
            
            if (!ok || (cpu->mode == 0 && !(pte & PTE_U))) {
                uint64_t cause = (access == ACCESS_FETCH) ? CAUSE_INSTR_PF : (access == ACCESS_STORE) ? CAUSE_STORE_PF : CAUSE_LOAD_PF;
                raise_trap(cpu, cause, va, 0);
                return 0;
            }
            
            // Check for misaligned superpage
            uint64_t pte_ppn = extract64(pte, 53, 10);
            for (int i = level + 1; i < 3; i++) {
                if (extract64(pte_ppn, (i * 9) + 8, i * 9) != 0) {
                    uint64_t cause = (access == ACCESS_FETCH) ? CAUSE_INSTR_PF : (access == ACCESS_STORE) ? CAUSE_STORE_PF : CAUSE_LOAD_PF;
                    raise_trap(cpu, cause, va, 0);
                    return 0;
                }
            }
            
            int update_pte = 0;
            uint64_t new_pte = pte;
            
            if (!(pte & PTE_A)) {
                new_pte |= PTE_A;
                update_pte = 1;
            }
            
            if (access == ACCESS_STORE && !(pte & PTE_D)) {
                new_pte |= PTE_D;
                update_pte = 1;
            }
            
            if (update_pte) {
                if (!phys_write(cpu, pte_addr, &new_pte, 8))
                    return 0;

                pte = new_pte;
            }
            
            uint64_t pa = 0;
            
            // Add PPN bits from PTE (mask out lower bits for superpages)
            pa |= (pte_ppn & ~((1ULL << ((level + 1) * 9)) - 1)) << 12;
            
            for (int i = level + 1; i < 3; i++)
                pa |= vpn[i] << (12 + (i * 9));

            
            pa |= offset;
            *pa_out = pa;
            return 1;
        }
        
        uint64_t pte_ppn = extract64(pte, 53, 10);
        a = pte_ppn << 12;
    }
    
    // Should not reach here
    uint64_t cause = (access == ACCESS_FETCH) ? CAUSE_INSTR_PF : (access == ACCESS_STORE) ? CAUSE_STORE_PF : CAUSE_LOAD_PF;
    raise_trap(cpu, cause, va, 0);
    return 0;
}

int phys_read(cpu_t* cpu, uint64_t pa, void* out, size_t size) {
    mem_region_t* rgn = find_region(pa);
    if (!rgn || !rgn->r || pa + size > rgn->base + rgn->size) {
        raise_trap(cpu, CAUSE_LOAD_PF, pa, 0);
        return 0;
    }

    memcpy(out, rgn->mem + (pa - rgn->base), size);
    return 1;
}

int phys_write(cpu_t* cpu, uint64_t pa, void* val, size_t size) {
    mem_region_t* rgn = find_region(pa);
    if (!rgn || !rgn->w || pa + size > rgn->base + rgn->size) {
        raise_trap(cpu, CAUSE_STORE_PF, pa, 0);
        return 0;
    }

    memcpy(rgn->mem + (pa - rgn->base), val, size);
    return 1;
}

int va_fetch(cpu_t* cpu, uint64_t va, void* out, size_t size) {
    if ((va & (size - 1)) != 0) {
        raise_trap(cpu, CAUSE_INSTR_ADDR_MISALIGNED, va, 0);
        return 0;
    }

    uint64_t pa;
    if (!sv39_translate(cpu, va, ACCESS_FETCH, &pa))
        return 0;

    return phys_read(cpu, pa, out, size);
}

int va_load(cpu_t* cpu, uint64_t va, void* out, size_t size) {
    if ((va & (size - 1)) != 0) {
        raise_trap(cpu, CAUSE_LOAD_ADDR_MISALIGNED, va, 0);
        return 0;
    }

    uint64_t pa;
    if (!sv39_translate(cpu, va, ACCESS_LOAD, &pa))
        return 0;

    return phys_read(cpu, pa, out, size);
}

int va_store(cpu_t* cpu, uint64_t va, void* val, size_t size) {
    if ((va & (size - 1)) != 0) {
        raise_trap(cpu, CAUSE_STORE_ADDR_MISALIGNED, va, 0);
        return 0;
    }

    uint64_t pa;
    if (!sv39_translate(cpu, va, ACCESS_STORE, &pa))
        return 0;

    return phys_write(cpu, pa, val, size);
}