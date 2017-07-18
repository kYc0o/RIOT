/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     unittests
 * @{
 *
 * @file
 * @brief       tweetnacl NaCl crypto library tests
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include <tweetnacl.h>

#include "xtimer.h"

static const char message[] = {0x21, 0x03, 0xb7, 0x2f, 0x54, 0xc3, 0x88, 0xc0,
                               0xab, 0x54, 0x2c, 0xee, 0x60, 0x6a, 0xa4, 0xc7,
                               0xd7, 0x7f, 0x16, 0xe4, 0x60, 0x4c, 0xce, 0x97,
                               0x2e, 0x30, 0x1b, 0xc5, 0x10, 0xe7, 0x5f, 0xd9};

static char r[sizeof(message)];

#define MLEN (sizeof(message) + crypto_box_ZEROBYTES)

static unsigned char alice_pk[crypto_box_PUBLICKEYBYTES];
static unsigned char alice_sk[crypto_box_SECRETKEYBYTES];
static unsigned char bob_pk[crypto_box_PUBLICKEYBYTES];
static unsigned char bob_sk[crypto_box_SECRETKEYBYTES];
static unsigned char m[MLEN];
static unsigned char c[MLEN];
static const unsigned char n[crypto_box_NONCEBYTES];
static unsigned char result[MLEN];

int main(void)
{
    int res;

    memset(m, 0, crypto_box_ZEROBYTES);
    memcpy(m + crypto_box_ZEROBYTES, message, MLEN - crypto_box_ZEROBYTES);
    memset(result, '\0', sizeof(result));
    memset(r, 0, sizeof(r));

    printf("Plain message: ");
    for (int i = 0; i < sizeof(m); i++) {
            printf("0x%02x ", m[i]);
    }
    puts("");

    /* Creating keypair ALICE... */
    crypto_box_keypair(alice_pk, alice_sk);

    /* Creating keypair BOB... */
    crypto_box_keypair(bob_pk, bob_sk);

    printf("[benchmark] NOW: %lu\n", xtimer_now_usec());
    /* Encrypting using pk_bob... */
    crypto_box(c, m, MLEN, n, bob_pk, alice_sk);
    printf("[benchmark] AFTER: %lu\n", xtimer_now_usec());

    printf("Encrypted message: ");
    for (int i = 0; i < sizeof(c); i++) {
        printf("0x%02x ", c[i]);
    }
    printf("\n");

    printf("[benchmark] NOW: %lu\n", xtimer_now_usec());
    /* Decrypting... */
    res = crypto_box_open(result, c, MLEN, n, alice_pk, bob_sk);
    printf("[benchmark] AFTER: %lu\n", xtimer_now_usec());

    if (res == 0) {
        printf("Decrypted successfully!\n");
        for (int i = 0; i < sizeof(result); i++) {
            printf("0x%02x ", result[i]);
        }
        memcpy(r, result + crypto_box_ZEROBYTES, MLEN - crypto_box_ZEROBYTES);

        for (int i = 0; i < sizeof(r); i++) {
            printf("0x%02x ", r[i]);
        }
        printf("\n");

    }
    else {
        printf("Error decrypting!\n");
    }
}
