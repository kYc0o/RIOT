/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       riotboot bootloader test
 *
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "slot_util.h"
#include "shell.h"

static int cmd_print_slot_nr(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("Current slot=%d\n", slot_util_current_slot());
    return 0;
}

static int cmd_print_slot_hdr(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int current_slot = slot_util_current_slot();
    slot_util_print_slot_hdr(current_slot);
    return 0;
}

static int cmd_print_slot_addr(int argc, char **argv)
{
    (void)argc;

    int reqslot=atoi(argv[1]);
    printf("Slot %d address=0x%08lx\n", reqslot, slot_util_get_image_startaddr(reqslot));
    return 0;
}

static int cmd_dumpaddrs(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    slot_util_dump_addrs();
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "curslotnr", "Print current slot number", cmd_print_slot_nr },
    { "curslothdr", "Print current slot header", cmd_print_slot_hdr },
    { "getslotaddr", "Print address of requested slot", cmd_print_slot_addr },
    { "dumpaddrs", "Prints all slot data in header", cmd_dumpaddrs },
    { NULL, NULL, NULL }
};

int main(void)
{
    int current_slot;

    puts("Hello riotboot!");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    /* print some information about the running image */
    current_slot = slot_util_current_slot();
    if (current_slot != -1) {
        printf("riotboot_test: running from slot %d\n", current_slot);
        slot_util_print_slot_hdr(current_slot);
    }
    else {
        printf("[FAILED] You're not running riotboot\n");
    }

    /* run the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
