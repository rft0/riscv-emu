#ifndef __CSR_H__
#define __CSR_H__

#include "../cpu.h"

void csr_write(cpu_t* cpu, uint32_t addr, uint64_t val);

#endif