#ifndef __TRAP_H__
#define __TRAP_H__

#include "../cpu.h"

// Interrupts
#define CAUSE_INT_RESERVED              0
#define CAUSE_INT_SUPERVISOR_SOFT       1
#define CAUSE_INT_MACHINE_SOFT          3
#define CAUSE_INT_SUPERVISOR_TIMER      5
#define CAUSE_INT_VSUPERVISOR_TIMER     6
#define CAUSE_INT_MACHINE_TIMER         7
#define CAUSE_INT_SUPERVISOR_EXT        9
#define CAUSE_INT_VSUPERVISOR_EXT       10
#define CAUSE_INT_MACHINE_EXT           11
#define CAUSE_INT_SUPERVISOR_GUEST_EXT  12

// Exceptions
#define CAUSE_INSTR_ADDR_MISALIGNED     0
#define CAUSE_INSTR_ADDR_FAULT          1
#define CAUSE_ILLEGAL_INSTR             2
#define CAUSE_BREAKPOINT                3
#define CAUSE_LOAD_ADDR_MISALIGNED      4
#define CAUSE_LOAD_ACCESS_FAULT         5
#define CAUSE_STORE_ADDR_MISALIGNED     6
#define CAUSE_STORE_ACCESS_FAULT        7
#define CAUSE_ECALL_FROM_U              8
#define CAUSE_ECALL_FROM_HS             9
#define CAUSE_ECALL_FROM_VS             10
#define CAUSE_ECALL_FROM_M              11
#define CAUSE_INSTR_PF                  12
#define CAUSE_LOAD_PF                   13
#define CAUSE_STORE_PF                  15
#define CAUSE_INSTR_GUEST_PF            20
#define CAUSE_LOAD_GUEST_PF             21
#define CAUSE_VIRT_INSTR                22
#define CAUSE_VIRT_STORE_PF             23

void raise_trap(cpu_t* cpu, uint64_t cause, uint64_t tval, int is_interrupt);

#endif