#include "helpers.h"

#include "mem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>

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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>
#include "cpu.h"
#include "mem.h"

int load_elf(emu_t* emu, const char* path, int change_pc) {
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

        for (int i = 0; i < eh.e_phnum; i++) {
            Elf32_Phdr ph;
            fseek(f, eh.e_phoff + i * eh.e_phentsize, SEEK_SET);
            if (fread(&ph, sizeof(ph), 1, f) != 1)
                continue;
            if (ph.p_type != PT_LOAD)
                continue;

            uint8_t *buf = malloc(ph.p_filesz);
            if (!buf)
                continue;

            fseek(f, ph.p_offset, SEEK_SET);
            fread(buf, 1, ph.p_filesz, f);
            phys_write(&emu->cpu, ph.p_paddr, buf, ph.p_filesz);
            free(buf);

            if (ph.p_memsz > ph.p_filesz) {
                size_t zero_sz = ph.p_memsz - ph.p_filesz;
                uint8_t *zero = calloc(1, zero_sz);
                if (zero) {
                    phys_write(&emu->cpu, ph.p_paddr + ph.p_filesz, zero, zero_sz);
                    free(zero);
                }
            }
        }

        if (change_pc)
            emu->cpu.pc = eh.e_entry;

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

        for (int i = 0; i < eh.e_phnum; i++) {
            Elf64_Phdr ph;
            fseek(f, eh.e_phoff + i * eh.e_phentsize, SEEK_SET);
            if (fread(&ph, sizeof(ph), 1, f) != 1)
                continue;
            if (ph.p_type != PT_LOAD)
                continue;

            uint8_t *buf = malloc(ph.p_filesz);
            if (!buf)
                continue;

            fseek(f, ph.p_offset, SEEK_SET);
            fread(buf, 1, ph.p_filesz, f);
            phys_write(&emu->cpu, ph.p_paddr, buf, ph.p_filesz);
            free(buf);

            if (ph.p_memsz > ph.p_filesz) {
                size_t zero_sz = ph.p_memsz - ph.p_filesz;
                uint8_t *zero = calloc(1, zero_sz);
                if (zero) {
                    phys_write(&emu->cpu, ph.p_paddr + ph.p_filesz, zero, zero_sz);
                    free(zero);
                }
            }
        }

        if (change_pc)
            emu->cpu.pc = eh.e_entry;

        fclose(f);
        return 1;
    }

    fclose(f);
    return 0;
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