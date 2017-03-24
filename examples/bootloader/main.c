/*
 * Copyright (C)2016 Inria
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
 * @brief       Default bootloader application to manage FW slots
 *
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#include "net/ipv6/addr.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/ipv6/netif.h"
#include "fw_slots.h"

extern int tftp_client_init(const char *addr, const char *filename);
bool slot_ready = false;

static int boot_img(void)
{
    uint8_t boot_slot = 0;
    boot_slot = fw_slots_find_newest_int_image();

    if (boot_slot > 0) {
        if (fw_slots_verify_int_slot(boot_slot) == 0) {
            uint32_t address;
            printf("Image on slot %d verified! Booting...\n", boot_slot);
            address = fw_slots_get_slot_address(boot_slot);
            fw_slots_jump_to_image(address);
        } else {
            printf("Slot %u inconsistent!\n", boot_slot);
        }
    } else {
        (void) puts("No bootable slot found!\n");
        return -1;
    }

    /* Shouldn't happen */
    return 0;
}

int main(void)
{
    fw_slots_erase_int_image(1);
    fw_slots_erase_int_image(2);

    kernel_pid_t ifs[GNRC_NETIF_NUMOF];
    (void) puts("Welcome to RIOT bootloader!\n");
    (void) puts("Trying to boot the newest firmware version\n");

    size_t numof = gnrc_netif_get(ifs);

    if (numof > 0) {
        gnrc_ipv6_netif_t *entry = gnrc_ipv6_netif_get(ifs[0]);
        for (int i = 0; i < GNRC_IPV6_NETIF_ADDR_NUMOF; i++) {
            if ((ipv6_addr_is_link_local(&entry->addrs[i].addr)) && !(entry->addrs[i].flags & GNRC_IPV6_NETIF_ADDR_FLAGS_NON_UNICAST)) {
                char ipv6_addr[IPV6_ADDR_MAX_STR_LEN];
                ipv6_addr_to_str(ipv6_addr, &entry->addrs[i].addr, IPV6_ADDR_MAX_STR_LEN);
                printf("My address is %s\n", ipv6_addr);
            }
        }
    }

    if (boot_img() != 0) {
        puts("Starting tftp client to retrieve image");
        tftp_client_init("ff02::1", "slot-image-0xabcd1234-0x1.bin");
    }

    while (!slot_ready);

    if (boot_img() != 0) {
        puts("Everything failed!\n");
    }

    /* Should not happen */
    return 0;
}
