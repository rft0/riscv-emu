#ifndef __VIRTIO_H__
#define __VIRTIO_H__

#include "mem.h"

#define VIRTIO_BLK_BASE 0x10001000
#define VIRTIO_NET_BASE 0x10002000
#define VIRTIO_SIZE 0x1000

int virtio_blk_read(uint64_t addr, void* out, size_t size);
int virtio_blk_write(uint64_t addr, void* val, size_t size);

int virtio_net_read(uint64_t addr, void* out, size_t size);
int virtio_net_write(uint64_t addr, void* pval, size_t size);

#endif