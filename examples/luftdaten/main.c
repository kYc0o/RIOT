/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Hello World application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>
#include "periph/uart.h"
#include "board.h"
#include "xtimer.h"
#include "ringbuffer.h"

#define UART_BUFSIZE 10
/*****************************************************************
 * start SDS011 sensor                                           *
 *****************************************************************/
const uint8_t start_SDS_cmd[] = {0xAA, 0xB4, 0x06, 0x01, 0x01, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
                                 0xFF, 0x06, 0xAB};

/*****************************************************************
 * stop SDS011 sensor                                            *
 *****************************************************************/

const uint8_t stop_SDS_cmd[] = {0xAA, 0xB4, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
                                0xFF, 0x05, 0xAB};

/*****************************************************************
 * Get version SDS011 sensor
 * For some reason we need to read the version to get the data
 *****************************************************************/
const uint8_t version_SDS_cmd[] = {0xAA, 0xB4, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
                                   0xFF, 0x05, 0xAB};

typedef struct {
    char rx_mem[UART_BUFSIZE];
    ringbuffer_t rx_buf;
} uart_ctx_t;

static uart_ctx_t ctx;

static void rx_cb(void *arg, uint8_t data)
{
    uart_t dev = (uart_t)arg;

    //printf("I got something!\n");
    //printf("%x ", data);

    ringbuffer_add_one(&(ctx.rx_buf), data);
    if (data == 0xAB) {
//        for (size_t i = 0; i < sizeof(ctx.rx_mem); i++) {
//            printf(" %02X ", ctx.rx_mem[i]);
//        }
        char c;
        printf("\n");
        printf("UART_DEV(%i) RX: ", dev);
        do {
            c = (int)ringbuffer_get_one(&(ctx.rx_buf));
            if (c == '\n') {
                puts("\\n");
            }
//            else if (c >= ' ' && c <= '~') {
//                printf("%c", c);
//            }
            else {
                printf("0x%02x", (unsigned char)c);
            }
        } while (c != 0xAB);
    }
    /*
    ringbuffer_add_one(&(ctx[dev].rx_buf), data);
    if (data == '\n') {
        msg_t msg;
        msg.content.value = (uint32_t)dev;
        msg_send(&msg, printer_pid);
    }
    */
}

int main(void)
{
    uart_t uart;
    int err;

    uart = UART_DEV(1);

    puts("Hello World!");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    err = uart_init(uart, 9600, rx_cb, (void*)1);

    if (err < 0) {
        printf("Error initialising UART 2!\n");
        return 1;
    }

    printf("Sending command start_SDS_cmd to SDS011\n");
    for (size_t i = 0; i < sizeof(start_SDS_cmd); i++) {
            printf(" %02X ", start_SDS_cmd[i]);
    }
    printf("\n");

    uart_write(uart, start_SDS_cmd, sizeof(start_SDS_cmd));

    xtimer_usleep(100);

   /* printf("Sending command version_SDS_cmd to SDS011\n");
    for (size_t i = 0; i < sizeof(version_SDS_cmd); i++) {
        printf(" %02X ", version_SDS_cmd[i]);
    }
    */

    //uart_write(uart, version_SDS_cmd, sizeof(version_SDS_cmd));

    while(1);

    return 0;
}
