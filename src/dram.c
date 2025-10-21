#include "dram.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

uint8_t* g_dram = NULL;

int dram_init() {
    g_dram = (uint8_t*)malloc(DRAM_SIZE);
    if (!g_dram)
        return 0;

    memset(g_dram, 0, DRAM_SIZE);
    return 1;
}

void dram_free() {
    free(g_dram);
}

int dram_read(uint64_t addr, void* out, size_t size) {
    if (!g_dram || addr + size > DRAM_BASE + DRAM_SIZE)
        return 0;

    memcpy(out, g_dram + (addr - DRAM_BASE), size);
    return 1;
}

int dram_write(uint64_t addr, void* pval, size_t size) {
    if (!g_dram || addr + size > DRAM_BASE + DRAM_SIZE)
        return 0;

    memcpy(g_dram + (addr - DRAM_BASE), pval, size);

    return 1;
}

void dram_dump(uint64_t addr, size_t size) {
    if (!g_dram || addr + size > DRAM_BASE + DRAM_SIZE)
        return;

    for (size_t i = 0; i < size; i++) {
        if (i % 16 == 0) {
            if (i != 0) printf("\n");
            printf("%08lX: ", addr + i);
        }

        printf("%02X ", g_dram[addr - DRAM_BASE + i]);
    }

    printf("\n");
}

void dram_dumpfile(const char* filename, uint64_t addr, size_t size) {
    if (!g_dram || addr + size > DRAM_BASE + DRAM_SIZE)
        return;

    FILE* f = fopen(filename, "wb");
    if (!f)
        return;

    fwrite(g_dram + (addr - DRAM_BASE), 1, size, f);
    fclose(f);
}