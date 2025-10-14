#include "virtio.h"

int virtio_blk_read(mem_region_t* region, cpu_t* cpu, uint64_t addr, void* out, size_t size) {
    return 1;
}

int virtio_blk_write(mem_region_t* region, cpu_t* cpu, uint64_t addr, void* val, size_t size) {
    return 1;
}

int virtio_net_read(mem_region_t* region, cpu_t* cpu, uint64_t addr, void* out, size_t size) {
    return 1;
}

int virtio_net_write(mem_region_t* region, cpu_t* cpu, uint64_t addr, void* val, size_t size) {
    return 1;
}