#ifndef __MEM_H__
#define __MEM_H__

#include "../cpu.h"

typedef enum {
    ACCESS_FETCH = 1,
    ACCESS_LOAD = 2, 
    ACCESS_STORE = 3
} access_type_t;

int sv39_translate(cpu_t* cpu, uint64_t va, access_type_t access, uint64_t* pa_out);

int phys_read(cpu_t* cpu, uint64_t pa, void* out, size_t size);
int phys_write(cpu_t* cpu, uint64_t pa, void* val, size_t size);

int va_fetch(cpu_t* cpu, uint64_t va, void* out, size_t size);
int va_load(cpu_t* cpu, uint64_t va, void* out, size_t size);
int va_store(cpu_t* cpu, uint64_t va, void* val, size_t size);

void mem_init();
void mem_free();

#endif