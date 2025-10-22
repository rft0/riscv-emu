#include "helpers.h"

#include "dram.h"
#include "mem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>

uint64_t g_tohost_addr = 0;

uint8_t* load_binary(const char* path, size_t* out_size) {
    FILE* f = fopen(path, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    rewind(f);

    uint8_t* buffer = (uint8_t*)malloc(size);
    if (!buffer) {
        fclose(f);
        return NULL;
    }

    fread(buffer, 1, size, f);
    // buffer[size] = '\0';
    fclose(f);

    if (out_size)
        *out_size = size;

    return buffer;
}

uint64_t mulu128(uint64_t a, uint64_t b, uint64_t* hi_result) {
    uint64_t a_lo = a & 0xFFFFFFFFULL;
    uint64_t a_hi = a >> 32;
    uint64_t b_lo = b & 0xFFFFFFFFULL;
    uint64_t b_hi = b >> 32;
    
    uint64_t p0 = a_lo * b_lo;  // LL
    uint64_t p1 = a_lo * b_hi;  // LH
    uint64_t p2 = a_hi * b_lo;  // HL
    uint64_t p3 = a_hi * b_hi;  // HH
    
    uint64_t mid = (p0 >> 32) + (p1 & 0xFFFFFFFFULL) + (p2 & 0xFFFFFFFFULL);
    uint64_t lo = (p0 & 0xFFFFFFFFULL) + (mid << 32);
    
    uint64_t carry = mid >> 32;
    uint64_t hi = p3 + (p1 >> 32) + (p2 >> 32) + carry;
    
    *hi_result = hi;
    return lo;
}

int64_t mul128(int64_t a, int64_t b, int64_t* hi_result) {
    uint64_t hi;
    uint64_t lo = mulu128((uint64_t)a, (uint64_t)b, &hi);

    if (a < 0) hi -= (uint64_t)b;
    if (b < 0) hi -= (uint64_t)a;
    
    *hi_result = (int64_t)hi;
    return (int64_t)lo;
}

uint64_t mulsu128(int64_t a, uint64_t b, int64_t* hi_result) {
    uint64_t hi;
    uint64_t lo = mulu128((uint64_t)a, b, &hi);
    
    // Adjust only for signed a
    if (a < 0) hi -= b;
    
    *hi_result = (int64_t)hi;
    return lo;
}

// ----------------------------------- //
// ELF Helpers Starts Here
// ----------------------------------- //
void find_tohost_32(FILE* f, Elf32_Ehdr* eh, uint32_t base_addr) {
    if (eh->e_shoff == 0) return;

    Elf32_Shdr* sh_tbl = (Elf32_Shdr*)malloc(eh->e_shentsize * eh->e_shnum);
    if (!sh_tbl) return;

    fseek(f, eh->e_shoff, SEEK_SET);
    if (fread(sh_tbl, eh->e_shentsize, eh->e_shnum, f) != eh->e_shnum) {
        free(sh_tbl);
        return;
    }

    Elf32_Shdr* symtab_hdr = NULL;
    Elf32_Shdr* strtab_hdr = NULL;

    for (int i = 0; i < eh->e_shnum; i++) {
        if (sh_tbl[i].sh_type == SHT_SYMTAB) {
            symtab_hdr = &sh_tbl[i];
            strtab_hdr = &sh_tbl[symtab_hdr->sh_link];
            break;
        }
    }

    if (!symtab_hdr) {
        free(sh_tbl);
        return;
    }

    Elf32_Sym* sym_tbl = (Elf32_Sym*)malloc(symtab_hdr->sh_size);
    char* str_tbl_data = (char*)malloc(strtab_hdr->sh_size);
    if (!sym_tbl || !str_tbl_data) {
        free(sh_tbl); free(sym_tbl); free(str_tbl_data);
        return;
    }

    fseek(f, symtab_hdr->sh_offset, SEEK_SET);
    fread(sym_tbl, 1, symtab_hdr->sh_size, f);

    fseek(f, strtab_hdr->sh_offset, SEEK_SET);
    fread(str_tbl_data, 1, strtab_hdr->sh_size, f);

    int num_symbols = symtab_hdr->sh_size / sizeof(Elf32_Sym);
    for (int i = 0; i < num_symbols; i++) {
        const char* name = str_tbl_data + sym_tbl[i].st_name;
        if (strcmp(name, "tohost") == 0) {
            g_tohost_addr = base_addr + sym_tbl[i].st_value;
            break;
        }
    }

    free(sh_tbl);
    free(sym_tbl);
    free(str_tbl_data);
}

void find_tohost_64(FILE* f, Elf64_Ehdr* eh, uint64_t base_addr) {
    if (eh->e_shoff == 0) return;

    Elf64_Shdr* sh_tbl = (Elf64_Shdr*)malloc(eh->e_shentsize * eh->e_shnum);
    if (!sh_tbl) return;

    fseek(f, eh->e_shoff, SEEK_SET);
    if (fread(sh_tbl, eh->e_shentsize, eh->e_shnum, f) != eh->e_shnum) {
        free(sh_tbl);
        return;
    }

    Elf64_Shdr* symtab_hdr = NULL;
    Elf64_Shdr* strtab_hdr = NULL;

    for (int i = 0; i < eh->e_shnum; i++) {
        if (sh_tbl[i].sh_type == SHT_SYMTAB) {
            symtab_hdr = &sh_tbl[i];
            strtab_hdr = &sh_tbl[symtab_hdr->sh_link];
            break;
        }
    }

    if (!symtab_hdr) {
        free(sh_tbl);
        return;
    }

    Elf64_Sym* sym_tbl = (Elf64_Sym*)malloc(symtab_hdr->sh_size);
    char* str_tbl_data = (char*)malloc(strtab_hdr->sh_size);
    if (!sym_tbl || !str_tbl_data) {
        free(sh_tbl); free(sym_tbl); free(str_tbl_data);
        return;
    }

    fseek(f, symtab_hdr->sh_offset, SEEK_SET);
    fread(sym_tbl, 1, symtab_hdr->sh_size, f);

    fseek(f, strtab_hdr->sh_offset, SEEK_SET);
    fread(str_tbl_data, 1, strtab_hdr->sh_size, f);

    int num_symbols = symtab_hdr->sh_size / sizeof(Elf64_Sym);
    for (int i = 0; i < num_symbols; i++) {
        const char* name = str_tbl_data + sym_tbl[i].st_name;
        if (strcmp(name, "tohost") == 0) {
            g_tohost_addr = base_addr + sym_tbl[i].st_value;
            break;
        }
    }

    free(sh_tbl);
    free(sym_tbl);
    free(str_tbl_data);
}

// Helper function to load program segments
static int load_segments_32(emu_t* emu, FILE* f, Elf32_Ehdr* eh, uint32_t base_addr) {
    for (int i = 0; i < eh->e_phnum; i++) {
        Elf32_Phdr ph;
        fseek(f, eh->e_phoff + i * eh->e_phentsize, SEEK_SET);
        if (fread(&ph, sizeof(ph), 1, f) != 1)
            continue;
        
        if (ph.p_type != PT_LOAD)
            continue;

        uint32_t load_addr = base_addr + ph.p_vaddr;
        
        // Allocate and read segment data
        uint8_t *buf = malloc(ph.p_filesz);
        if (!buf)
            continue;

        fseek(f, ph.p_offset, SEEK_SET);
        if (fread(buf, 1, ph.p_filesz, f) != ph.p_filesz) {
            free(buf);
            continue;
        }
        
        dram_write(load_addr, buf, ph.p_filesz);
        free(buf);

        // Zero out BSS section
        if (ph.p_memsz > ph.p_filesz) {
            size_t zero_sz = ph.p_memsz - ph.p_filesz;
            uint8_t *zero = calloc(1, zero_sz);
            if (zero) {
                dram_write(load_addr + ph.p_filesz, zero, zero_sz);
                free(zero);
            }
        }
    }
    return 1;
}

static int load_segments_64(emu_t* emu, FILE* f, Elf64_Ehdr* eh, uint64_t base_addr) {
    for (int i = 0; i < eh->e_phnum; i++) {
        Elf64_Phdr ph;
        fseek(f, eh->e_phoff + i * eh->e_phentsize, SEEK_SET);
        if (fread(&ph, sizeof(ph), 1, f) != 1)
            continue;
        
        if (ph.p_type != PT_LOAD)
            continue;

        uint64_t load_addr = base_addr + ph.p_vaddr;
        
        uint8_t *buf = malloc(ph.p_filesz);
        if (!buf)
            continue;

        fseek(f, ph.p_offset, SEEK_SET);
        if (fread(buf, 1, ph.p_filesz, f) != ph.p_filesz) {
            free(buf);
            continue;
        }
        
        dram_write(load_addr, buf, ph.p_filesz);
        free(buf);

        if (ph.p_memsz > ph.p_filesz) {
            size_t zero_sz = ph.p_memsz - ph.p_filesz;
            uint8_t *zero = calloc(1, zero_sz);
            if (zero) {
                dram_write(load_addr + ph.p_filesz, zero, zero_sz);
                free(zero);
            }
        }
    }
    return 1;
}

// Process dynamic section and relocations for 32-bit
static int process_dynamic_32(emu_t* emu, FILE* f, Elf32_Ehdr* eh, uint32_t base_addr) {
    Elf32_Phdr dyn_ph = {0};
    int found_dynamic = 0;
    
    // Find PT_DYNAMIC segment
    for (int i = 0; i < eh->e_phnum; i++) {
        Elf32_Phdr ph;
        fseek(f, eh->e_phoff + i * eh->e_phentsize, SEEK_SET);
        if (fread(&ph, sizeof(ph), 1, f) != 1)
            continue;
        
        if (ph.p_type == PT_DYNAMIC) {
            dyn_ph = ph;
            found_dynamic = 1;
            break;
        }
    }
    
    if (!found_dynamic)
        return 1; // No dynamic section, that's OK for static PIE
    
    // Read dynamic section
    size_t dyn_count = dyn_ph.p_filesz / sizeof(Elf32_Dyn);
    Elf32_Dyn* dyn_entries = malloc(dyn_ph.p_filesz);
    if (!dyn_entries)
        return 0;
    
    fseek(f, dyn_ph.p_offset, SEEK_SET);
    if (fread(dyn_entries, dyn_ph.p_filesz, 1, f) != 1) {
        free(dyn_entries);
        return 0;
    }
    
    // Find relocation tables
    uint32_t rela_addr = 0, rela_size = 0;
    uint32_t rel_addr = 0, rel_size = 0;
    
    for (size_t i = 0; i < dyn_count; i++) {
        if (dyn_entries[i].d_tag == DT_NULL)
            break;
        
        switch (dyn_entries[i].d_tag) {
            case DT_RELA:
                rela_addr = dyn_entries[i].d_un.d_ptr;
                break;
            case DT_RELASZ:
                rela_size = dyn_entries[i].d_un.d_val;
                break;
            case DT_REL:
                rel_addr = dyn_entries[i].d_un.d_ptr;
                break;
            case DT_RELSZ:
                rel_size = dyn_entries[i].d_un.d_val;
                break;
        }
    }
    
    // Process RELA relocations
    if (rela_addr && rela_size) {
        size_t rela_count = rela_size / sizeof(Elf32_Rela);
        Elf32_Rela* rela_entries = malloc(rela_size);
        if (rela_entries) {
            // Read from loaded memory
            phys_read(&emu->cpu, base_addr + rela_addr, (uint8_t*)rela_entries, rela_size);
            
            for (size_t i = 0; i < rela_count; i++) {
                uint32_t type = ELF32_R_TYPE(rela_entries[i].r_info);
                uint32_t offset = base_addr + rela_entries[i].r_offset;
                
                if (type == R_RISCV_RELATIVE) {
                    uint32_t value = base_addr + rela_entries[i].r_addend;
                    dram_write(offset, (uint8_t*)&value, sizeof(value));
                }
            }
            free(rela_entries);
        }
    }
    
    // Process REL relocations (similar pattern)
    if (rel_addr && rel_size) {
        size_t rel_count = rel_size / sizeof(Elf32_Rel);
        Elf32_Rel* rel_entries = malloc(rel_size);
        if (rel_entries) {
            phys_read(&emu->cpu, base_addr + rel_addr, (uint8_t*)rel_entries, rel_size);
            
            for (size_t i = 0; i < rel_count; i++) {
                uint32_t type = ELF32_R_TYPE(rel_entries[i].r_info);
                uint32_t offset = base_addr + rel_entries[i].r_offset;
                
                if (type == R_RISCV_RELATIVE) {
                    uint32_t addend;
                    phys_read(&emu->cpu, offset, (uint8_t*)&addend, sizeof(addend));
                    uint32_t value = base_addr + addend;
                    dram_write(offset, (uint8_t*)&value, sizeof(value));
                }
            }
            free(rel_entries);
        }
    }
    
    free(dyn_entries);
    return 1;
}

// Process dynamic section and relocations for 64-bit
static int process_dynamic_64(emu_t* emu, FILE* f, Elf64_Ehdr* eh, uint64_t base_addr) {
    Elf64_Phdr dyn_ph = {0};
    int found_dynamic = 0;
    
    for (int i = 0; i < eh->e_phnum; i++) {
        Elf64_Phdr ph;
        fseek(f, eh->e_phoff + i * eh->e_phentsize, SEEK_SET);
        if (fread(&ph, sizeof(ph), 1, f) != 1)
            continue;
        
        if (ph.p_type == PT_DYNAMIC) {
            dyn_ph = ph;
            found_dynamic = 1;
            break;
        }
    }
    
    if (!found_dynamic)
        return 1;
    
    size_t dyn_count = dyn_ph.p_filesz / sizeof(Elf64_Dyn);
    Elf64_Dyn* dyn_entries = malloc(dyn_ph.p_filesz);
    if (!dyn_entries)
        return 0;
    
    fseek(f, dyn_ph.p_offset, SEEK_SET);
    if (fread(dyn_entries, dyn_ph.p_filesz, 1, f) != 1) {
        free(dyn_entries);
        return 0;
    }
    
    uint64_t rela_addr = 0, rela_size = 0;
    uint64_t rel_addr = 0, rel_size = 0;
    
    for (size_t i = 0; i < dyn_count; i++) {
        if (dyn_entries[i].d_tag == DT_NULL)
            break;
        
        switch (dyn_entries[i].d_tag) {
            case DT_RELA:
                rela_addr = dyn_entries[i].d_un.d_ptr;
                break;
            case DT_RELASZ:
                rela_size = dyn_entries[i].d_un.d_val;
                break;
            case DT_REL:
                rel_addr = dyn_entries[i].d_un.d_ptr;
                break;
            case DT_RELSZ:
                rel_size = dyn_entries[i].d_un.d_val;
                break;
        }
    }
    
    // Process RELA relocations
    if (rela_addr && rela_size) {
        size_t rela_count = rela_size / sizeof(Elf64_Rela);
        Elf64_Rela* rela_entries = malloc(rela_size);
        if (rela_entries) {
            phys_read(&emu->cpu, base_addr + rela_addr, (uint8_t*)rela_entries, rela_size);
            
            for (size_t i = 0; i < rela_count; i++) {
                uint64_t type = ELF64_R_TYPE(rela_entries[i].r_info);
                uint64_t offset = base_addr + rela_entries[i].r_offset;
                
                if (type == R_RISCV_RELATIVE) {
                    uint64_t value = base_addr + rela_entries[i].r_addend;
                    dram_write(offset, (uint8_t*)&value, sizeof(value));
                }
            }
            free(rela_entries);
        }
    }
    
    // Process REL relocations
    if (rel_addr && rel_size) {
        size_t rel_count = rel_size / sizeof(Elf64_Rel);
        Elf64_Rel* rel_entries = malloc(rel_size);
        if (rel_entries) {
            phys_read(&emu->cpu, base_addr + rel_addr, (uint8_t*)rel_entries, rel_size);
            
            for (size_t i = 0; i < rel_count; i++) {
                uint64_t type = ELF64_R_TYPE(rel_entries[i].r_info);
                uint64_t offset = base_addr + rel_entries[i].r_offset;
                
                if (type == R_RISCV_RELATIVE) {
                    uint64_t addend;
                    phys_read(&emu->cpu, offset, (uint8_t*)&addend, sizeof(addend));
                    uint64_t value = base_addr + addend;
                    dram_write(offset, (uint8_t*)&value, sizeof(value));
                }
            }
            free(rel_entries);
        }
    }
    
    free(dyn_entries);
    return 1;
}

int load_elfex(emu_t* emu, const char* path, int change_pc, uint64_t force_base) {
    FILE* f = fopen(path, "rb");
    if (!f)
        return 0;

    unsigned char ident[EI_NIDENT];
    if (fread(ident, 1, EI_NIDENT, f) != EI_NIDENT) {
        fclose(f);
        return 0;
    }

    if (memcmp(ident, ELFMAG, SELFMAG) != 0) {
        fclose(f);
        return 0;
    }

    fseek(f, 0, SEEK_SET);
    
    if (ident[EI_CLASS] == ELFCLASS32) {
        Elf32_Ehdr eh;
        if (fread(&eh, sizeof(eh), 1, f) != 1) {
            fclose(f);
            return 0;
        }
        if (eh.e_machine != EM_RISCV) {
            fclose(f);
            return 0;
        }

        uint32_t base_addr = 0;
        
        if (eh.e_type == ET_DYN) {
            base_addr = (force_base != 0) ? (uint32_t)force_base : 0x40000000;
        } else if (eh.e_type != ET_EXEC) {
            fprintf(stderr, "Unsupported ELF type: %d\n", eh.e_type);
            fclose(f);
            return 0;
        }

#ifdef EMU_TESTS_ENABLED
        find_tohost_32(f, &eh, base_addr);
#endif

        if (!load_segments_32(emu, f, &eh, base_addr)) {
            fclose(f);
            return 0;
        }

        // Process dynamic relocations if needed
        if (eh.e_type == ET_DYN) {
            if (!process_dynamic_32(emu, f, &eh, base_addr)) {
                fclose(f);
                return 0;
            }
        }

        if (change_pc)
            emu->cpu.pc = base_addr + eh.e_entry;

        fclose(f);
        return 1;

    } else if (ident[EI_CLASS] == ELFCLASS64) {
        Elf64_Ehdr eh;
        if (fread(&eh, sizeof(eh), 1, f) != 1) {
            fclose(f);
            return 0;
        }
        if (eh.e_machine != EM_RISCV) {
            fclose(f);
            return 0;
        }

        uint64_t base_addr = 0;
        
        if (eh.e_type == ET_DYN) {
            // Use forced base if provided, otherwise use default PIE base
            base_addr = (force_base != 0) ? force_base : 0x0000400000000000ULL;
        } else if (eh.e_type != ET_EXEC) {
            fprintf(stderr, "Unsupported ELF type: %d\n", eh.e_type);
            fclose(f);
            return 0;
        }

#ifdef EMU_TESTS_ENABLED
        find_tohost_64(f, &eh, base_addr);
#endif

        if (!load_segments_64(emu, f, &eh, base_addr)) {
            fclose(f);
            return 0;
        }

        if (eh.e_type == ET_DYN) {
            if (!process_dynamic_64(emu, f, &eh, base_addr)) {
                fclose(f);
                return 0;
            }
        }

        if (change_pc)
            emu->cpu.pc = base_addr + eh.e_entry;

        fclose(f);
        return 1;
    }

    fclose(f);
    return 0;
}

// Convenience wrapper for normal ELF loading
int load_elf(emu_t* emu, const char* path, int change_pc) {
    return load_elfex(emu, path, change_pc, 0);
}