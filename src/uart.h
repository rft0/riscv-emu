#ifndef __UART_H__
#define __UART_H__

#include "mem.h"

#define UART_BASE  0x10000000
#define UART_SIZE  0x100

int uart_read(mem_region_t* region, cpu_t* cpu, uint64_t addr, void* out, size_t size);
int uart_write(mem_region_t* region, cpu_t* cpu, uint64_t addr, void* val, size_t size);

#endif