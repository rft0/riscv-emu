#include "plic.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define PLIC_NUM_INTERRUPTS  31     // adjust as needed

#define PLIC_BASE_ADDR       0x0C000000
#define PLIC_PRIORITY_BASE   0x0C000000
#define PLIC_PENDING_BASE    0x0C001000
#define PLIC_ENABLE_BASE     0x0C002000

#define PLIC_THRESHOLD_BASE  0x0C200000
#define PLIC_CLAIM_BASE      0x0C200004
#define PLIC_CONTEXT_STRIDE  0x1000

typedef struct {
    uint32_t priority[PLIC_NUM_INTERRUPTS + 1];
    uint32_t pending;
    uint32_t enable[HART_COUNT];
    uint32_t threshold[HART_COUNT];
    uint32_t claim[HART_COUNT];
} plic_t;

plic_t plic;

void plic_init() {
    memset(&plic, 0, sizeof(plic));
}

void plic_set_pending(uint32_t irq) {
    if (irq == 0 || irq > PLIC_NUM_INTERRUPTS) return;
    plic.pending |= (1 << irq);
}

void plic_clear_pending(uint32_t irq) {
    if (irq == 0 || irq > PLIC_NUM_INTERRUPTS) return;
    plic.pending &= ~(1 << irq);
}

uint32_t plic_get_priority(uint32_t irq) {
    if (irq > PLIC_NUM_INTERRUPTS) return 0;
    return plic.priority[irq];
}

void plic_set_priority(uint32_t irq, uint32_t prio) {
    if (irq == 0 || irq > PLIC_NUM_INTERRUPTS) return;
    plic.priority[irq] = prio;
}

uint32_t plic_claim(uint32_t context) {
    if (context >= HART_COUNT) return 0;

    uint32_t highest_prio = 0;
    int irq_to_claim = 0;

    for (int i = 1; i <= PLIC_NUM_INTERRUPTS; i++) {
        if ((plic.pending & (1 << i)) && (plic.enable[context] & (1 << i))) {
            if (plic.priority[i] > highest_prio && plic.priority[i] > plic.threshold[context]) {
                highest_prio = plic.priority[i];
                irq_to_claim = i;
            }
        }
    }

    if (irq_to_claim) {
        plic.claim[context] = irq_to_claim;
        plic_clear_pending(irq_to_claim);
    }

    return irq_to_claim;
}

void plic_complete(uint32_t context, uint32_t irq) {
    if (context >= HART_COUNT) return;
    if (irq == 0 || irq > PLIC_NUM_INTERRUPTS) return;

    plic.claim[context] = 0;
}

// read write problematic but works for now bcs hart count is 1
int plic_read(uint64_t addr, void* out, size_t size) {
    uint32_t val = 0;
    if (addr >= PLIC_PRIORITY_BASE && addr < PLIC_PENDING_BASE) {
        int irq = (addr - PLIC_PRIORITY_BASE) / 4;
        val = plic_get_priority(irq);
    } else if (addr >= PLIC_PENDING_BASE && addr < PLIC_ENABLE_BASE) {
        val = plic.pending;
    } else if (addr >= PLIC_ENABLE_BASE && addr < PLIC_THRESHOLD_BASE) {
        int context = (addr - PLIC_ENABLE_BASE) / 0x80;
        val = plic.enable[context];
    } else if (addr >= PLIC_THRESHOLD_BASE && addr < PLIC_THRESHOLD_BASE + 8) {
        int context = (addr - PLIC_THRESHOLD_BASE) / 4;
        val = plic.threshold[context];
    } else if (addr >= PLIC_CLAIM_BASE && addr < PLIC_CLAIM_BASE + 8) {
        int context = (addr - PLIC_CLAIM_BASE) / 4;
        val = plic_claim(context);
    }

    uint64_t res = reg_read_multi(val, addr & 3, size);
    memcpy(out, &res, size);

    return 1;
}

int plic_write(uint64_t addr, void* pval, size_t size) {
    uint32_t val = *(uint32_t*)pval;

    if (addr >= PLIC_PRIORITY_BASE && addr < PLIC_PENDING_BASE) {
        int irq = (addr - PLIC_PRIORITY_BASE) / 4;
        if (irq == 0 || irq > PLIC_NUM_INTERRUPTS) return 1;
        uint64_t old = plic.priority[irq];
        reg_write_multi(&old, addr & 3, size, val);
        plic.priority[irq] = old;
    } else if (addr >= PLIC_ENABLE_BASE && addr < PLIC_THRESHOLD_BASE) {
        int context = (addr - PLIC_ENABLE_BASE) / 0x80;
        if (context >= HART_COUNT) return 1;
        uint64_t old = plic.enable[context];
        reg_write_multi(&old, addr & 3, size, val);
        plic.enable[context] = old;
    } else if (addr >= PLIC_THRESHOLD_BASE && addr < PLIC_THRESHOLD_BASE + 8) {
        int context = (addr - PLIC_THRESHOLD_BASE) / 4;
        if (context >= HART_COUNT) return 1;
        uint64_t old = plic.threshold[context];
        reg_write_multi(&old, addr & 3, size, val);
        plic.threshold[context] = old;
    } else if (addr >= PLIC_CLAIM_BASE && addr < PLIC_CLAIM_BASE + 8) {
        int context = (addr - PLIC_CLAIM_BASE) / 4;
        if (context >= HART_COUNT) return 1;
        // writing to claim register signals completion
        plic_complete(context, val & 0xFF);
    }

    return 1;
}
