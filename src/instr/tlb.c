#include <stdint.h>

#define TLB_SIZE 128
#define HASH_SIZE 64
#define HASH_MASK (HASH_SIZE - 1)

typedef struct tlb_entry {
    uint64_t vpn;
    uint64_t ppn;
    uint32_t lru_counter;
    uint16_t hash_next_idx;
    uint8_t valid;
} tlb_entry_t;

typedef struct {
    tlb_entry_t entries[TLB_SIZE];
    uint16_t hash_table[HASH_SIZE];   // indices, not pointers
    uint32_t global_counter;          // for LRU approximation
    uint16_t num_entries;
} tlb_t;

// Fast hash using bit operations
static inline uint32_t tlb_hash(uint64_t vpn) {
    uint32_t h = (uint32_t)(vpn ^ (vpn >> 32));
    return h & HASH_MASK;
}

void tlb_init(tlb_t *tlb) {
    for (int i = 0; i < HASH_SIZE; i++)
        tlb->hash_table[i] = TLB_SIZE;  // invalid index
    
    for (int i = 0; i < TLB_SIZE; i++)
        tlb->entries[i].valid = 0;
    
    tlb->num_entries = 0;
    tlb->global_counter = 0;
}

// Lookup with approximate LRU (no linked list manipulation)
int tlb_lookup(tlb_t* tlb, uint64_t vpn, uint64_t* ppn_out) {
    uint32_t h = tlb_hash(vpn);
    uint16_t idx = tlb->hash_table[h];
    
    while (idx < TLB_SIZE) {
        tlb_entry_t *e = &tlb->entries[idx];
        if (e->vpn == vpn && e->valid) {
            *ppn_out = e->ppn;
            e->lru_counter = ++tlb->global_counter;  // touch
            return 1;
        }
    
        idx = e->hash_next_idx;
    }
    
    return 0;
}

// Find LRU victim
static uint16_t tlb_find_victim(tlb_t* tlb) {
    uint32_t min_counter = UINT32_MAX;
    uint16_t victim = 0;
    
    for (uint16_t i = 0; i < tlb->num_entries; i++) {
        if (tlb->entries[i].lru_counter < min_counter) {
            min_counter = tlb->entries[i].lru_counter;
            victim = i;
        }
    }

    return victim;
}

void tlb_insert(tlb_t* tlb, uint64_t vpn, uint64_t ppn) {
    uint16_t idx;
    
    if (tlb->num_entries < TLB_SIZE) {
        idx = tlb->num_entries++;
    } else {
        // evict LRU entry
        idx = tlb_find_victim(tlb);
        tlb_entry_t *victim = &tlb->entries[idx];
        
        // remove from old hash chain
        uint32_t old_h = tlb_hash(victim->vpn);
        uint16_t *prev = &tlb->hash_table[old_h];
        while (*prev != idx && *prev < TLB_SIZE) {
            prev = &tlb->entries[*prev].hash_next_idx;
        }
        if (*prev == idx)
            *prev = victim->hash_next_idx;
    }
    
    // insert into new hash chain
    uint32_t h = tlb_hash(vpn);
    tlb_entry_t *e = &tlb->entries[idx];
    e->vpn = vpn;
    e->ppn = ppn;
    e->valid = 1;
    e->lru_counter = ++tlb->global_counter;
    e->hash_next_idx = tlb->hash_table[h];
    tlb->hash_table[h] = idx;
}

void tlb_flush(tlb_t* tlb) {
    tlb_init(tlb);
}