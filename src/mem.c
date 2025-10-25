#include "mem.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "emu.h"
#include "trap.h"

#include "dram.h"
#include "clint.h"
#include "uart.h"
#include "plic.h"
#include "virtio.h"

#define RISCV_TESTS_TOHOST_ADDR     0x80001000

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

typedef struct {
    mem_region_t regions[32];
    int num_regions;
} mem_map_t;

extern uint64_t g_tohost_addr;

mem_map_t g_memory_map = { 0 };

static void mem_add_region(uint64_t base, uint64_t size, fn_mem_read read, fn_mem_write write) {
    mem_region_t* rgn = &g_memory_map.regions[g_memory_map.num_regions++];
    rgn->base = base;
    rgn->size = size;
    rgn->read = read;
    rgn->write = write;
}

void mem_init() {
    dram_init();

    mem_add_region(DRAM_BASE, DRAM_SIZE, dram_read, dram_write); // DRAM
    mem_add_region(UART_BASE, UART_SIZE, uart_read, uart_write); // UART0
    mem_add_region(CLINT_BASE, CLINT_SIZE, clint_read, clint_write); // CLINT
    mem_add_region(PLIC_BASE, PLIC_SIZE, plic_read, plic_write); // PLIC
    mem_add_region(VIRTIO_BLK_BASE, VIRTIO_SIZE, virtio_blk_read, virtio_blk_write); // VIRTIO Block
    mem_add_region(VIRTIO_NET_BASE, VIRTIO_SIZE, virtio_net_read, virtio_net_write); // VIRTIO Net
    //! TODO: Possibly add first 1MB as ROM
    //! TODO: Check dtb used for qemu
}

void mem_free() {
    dram_free();

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
    if (cpu->mode == PRIV_M) {
        *pa_out = va;
        return 1;
    }

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
    for (level = 2; level >= 0; level--) {
        uint64_t pte_addr = a + vpn[level] * 8;  // 8 bytes per PTE
        if (!phys_read(cpu, pte_addr, &pte, 8))
            return 0;

        printf("SV39: level=%d, pte_addr=0x%lX, pte=0x%lX\n", level, pte_addr, pte);
        
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
            for (int i = 0; i < level; i++) {
                // Check if PPN[i] (which is bits i*9+8 : i*9) is non-zero
                if (extract64(pte_ppn, (i * 9) + 8, i * 9) != 0) {
                    uint64_t cause = (access == ACCESS_FETCH) ? CAUSE_INSTR_PF : (access == ACCESS_STORE) ? CAUSE_STORE_PF : CAUSE_LOAD_PF;
                    raise_trap(cpu, cause, va, 0);
                    return 0;
                }
            }

            if (!(pte & PTE_A)) {
                uint64_t cause = (access == ACCESS_FETCH) ? CAUSE_INSTR_PF : (access == ACCESS_STORE) ? CAUSE_STORE_PF : CAUSE_LOAD_PF;
                raise_trap(cpu, cause, va, 0);
                return 0;
            }
            
            // Dirty check
            if (access == ACCESS_STORE && !(pte & PTE_D)) {
                raise_trap(cpu, CAUSE_STORE_PF, va, 0);
                return 0;
            }
            
            uint64_t pa = (pte_ppn << 12);
            if (level > 0) {
                uint64_t va_mask = (1ULL << (level * 9)) - 1;
                va_mask = va_mask << 12;
                pa = (pa & ~va_mask) | (va & va_mask);
            }
            
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

    if (!rgn || pa + size > rgn->base + rgn->size) {
        raise_trap(cpu, CAUSE_LOAD_PF, pa, 0);
        return 0;
    }

    return rgn->read(pa, out, size);
}

int phys_write(cpu_t* cpu, uint64_t pa, void* val, size_t size) {
    mem_region_t* rgn = find_region(pa);
    if (!rgn || pa + size > rgn->base + rgn->size) {
        raise_trap(cpu, CAUSE_STORE_PF, pa, 0);
        return 0;
    }

#ifdef EMU_TESTS_ENABLED
    if (g_tohost_addr && pa == g_tohost_addr) {
        // printf("Tohost write: 0x%lX\n", *(uint32_t*)val);
        emu_free();
        uint32_t value = *(uint32_t*)val;
        if (value == 1)
            exit(0);
        else
            exit(value >> 1);
    }
#endif

    return rgn->write(pa, val, size);
}

int va_fetch(cpu_t* cpu, uint64_t va, void* out, size_t size) {
    if ((va & 1) != 0) {
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