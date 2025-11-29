#include "uart.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint8_t tx_fifo[16];
    int tx_len;
    uint8_t rx_fifo[16];
    int rx_len;

    uint8_t ier; // 0x01, RW
    uint8_t iir; // 0x02, RO
    uint8_t fcr; // 0x02, WO
    uint8_t lcr; // 0x03, RW
    uint8_t lsr; // 0x05, RO
} uart_t;

static uart_t uart = {
    .tx_fifo = {0},
    .tx_len = 0,
    .rx_fifo = {0},
    .rx_len = 0,

    .ier = 0,
    .iir = 1,
    .fcr = 0,
    .lcr = 0,
    .lsr = 0x60,
};

int uart_read(uint64_t addr, void* out, size_t size) {
    // printf("UART READ\n");
    // exit(1);

    uint64_t offset = addr - UART_BASE;
    uint32_t val = 0;

    switch (offset) {
        case 0x00: // RXDATA
            if (uart.rx_len > 0) {
                val = uart.rx_fifo[0];
                memmove(uart.rx_fifo, uart.rx_fifo + 1, uart.rx_len - 1);
                uart.rx_len--;

                if (uart.rx_len == 0)
                    uart.lsr &= ~1; // clear DR
            } else {
                val = 0; // no input
            }
            break;
        case 0x01: // IER
            val = uart.ier;
            break;
        case 0x02: // IIR
            val = uart.iir;
            break;
        case 0x03: // LCR
            val = uart.lcr;
            break;
        case 0x05: // LSR
            val = uart.lsr;
            break;
        default:
            val = 0;
            break;
        
    }

    memcpy(out, &val, size);
    return 1;
}

int uart_write(uint64_t addr, void* pval, size_t size) {
    // printf("UART WRITE\n");
    // exit(1);

    uint64_t offset = addr - UART_BASE;
    uint32_t value;
    memcpy(&value, pval, size);

    switch (offset) {
        case 0x00: {
            // uint8_t ch = value & 0xFF;
            // putchar(ch);
            // fflush(stdout);

            uint8_t ch = value & 0xFF;
            if (uart.tx_len < (int)sizeof(uart.tx_fifo)) {
                uart.tx_fifo[uart.tx_len++] = ch;
            } else {
                memmove(uart.tx_fifo, uart.tx_fifo + 1, uart.tx_len - 1);
                uart.tx_fifo[uart.tx_len - 1] = ch;
            }

            uart.lsr &= ~((1 << 5) | (1 << 6));
            }
            break;
        case 0x01: // IER
            uart.ier = value;
            break;
        case 0x02: // FCR
            uart.fcr = value;
            break;
        case 0x03: // LCR
            uart.lcr = value;
            break;
    }

    return 1;
}

// void uart_inject(uint8_t ch) {
//     if (uart.rx_len < 16)
//         uart.rx_fifo[uart.rx_len++] = ch;
// }
