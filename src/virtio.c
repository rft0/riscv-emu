#include "virtio.h"

int virtio_blk_read(uint64_t addr, void* out, size_t size) {
    return 1;
}

int virtio_blk_write(uint64_t addr, void* pval, size_t size) {
    return 1;
}

int virtio_net_read(uint64_t addr, void* out, size_t size) {
    return 1;
}

int virtio_net_write(uint64_t addr, void* pval, size_t size) {
    return 1;
}