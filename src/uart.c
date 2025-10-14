#include "uart.h"

int uart_read(mem_region_t* region, cpu_t* cpu, uint64_t addr, void* out, size_t size) {
    return 1;
}

int uart_write(mem_region_t* region, cpu_t* cpu, uint64_t addr, void* val, size_t size) {
    return 1;
}