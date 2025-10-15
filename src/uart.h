#ifndef __UART_H__
#define __UART_H__

#include "mem.h"

#define UART_BASE  0x10000000
#define UART_SIZE  0x100

int uart_read(uint64_t addr, void* out, size_t size);
int uart_write(uint64_t addr, void* pval, size_t size);

#endif