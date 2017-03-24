/*
 * Copyright (C) 2015 Engineering-Spirit
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
 * @brief       Demonstrating the sending and receiving of data via the TFTP client
 *
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "cpu_conf.h"

#include "net/gnrc/tftp.h"
#include "periph/flashpage.h"

static const char *_tftp_default_host = "::1";
static tftp_action_t _tftp_action;
static uint8_t buf[FLASHPAGE_SIZE];
static uint32_t buf_ptr = 0;
static uint32_t page1 = FW_SLOT_1_PAGE;
static uint32_t page2 = FW_SLOT_2_PAGE;
extern bool slot_ready;

static bool tftp_save_to_slot1(uint8_t *data, size_t data_len)
{
    for (int i = 0; i < data_len; i++) {
        if (buf_ptr < FLASHPAGE_SIZE) {
            buf[buf_ptr] = data[i];
            buf_ptr++;
        } else {
            int err;
            err = flashpage_write_and_verify(page1, buf);
            if (err == FLASHPAGE_OK) {
                printf("Successfully written page %lu\n", page1);
                page1++;
                buf_ptr = 0;
            } else {
                printf("Flash program failed with error %d\n", err);
                return false;
            }
        }
    }

    return true;
}

static int write_last_page(void)
{
    int err;

    if (page1 != FW_SLOT_1_PAGE) {
        err = flashpage_write_and_verify(page1, buf);
        if (err == FLASHPAGE_OK) {
            printf("Successfully written page %lu\n", page1);
            page1 = FW_SLOT_1_PAGE;
            buf_ptr = 0;
            memset(buf, 0, sizeof(buf));
            return true;
        } else {
            printf("Flash program failed with error %d\n", err);
            return false;
        }
    }

    if (page2 != FW_SLOT_2_PAGE) {
        err = flashpage_write_and_verify(page2, buf);
        if (err == FLASHPAGE_OK) {
            printf("Successfully written page %lu\n", page2);
            page2 = FW_SLOT_2_PAGE;
            buf_ptr = 0;
            memset(buf, 0, sizeof(buf));
            return true;
        } else {
            printf("Flash program failed with error %d\n", err);
            return false;
        }
    }

    return false;
}

/**
 * @brief called at every transaction start
 */
static bool _tftp_client_start_cb(tftp_action_t action, tftp_mode_t mode,
                                  const char *file_name, size_t *len)
{
    /* translate the mode */
    const char *str_mode = "ascii";

    if (mode == TTM_OCTET) {
        str_mode = "bin";
    }
    else if (mode == TTM_MAIL) {
        str_mode = "mail";
    }

    /* translate the action */
    const char *str_action = "read";
    if (action == TFTP_WRITE) {
        str_action = "write";
    }

    /* display the action being performed */
    printf("tftp_client: %s %s %s:%lu\n", str_mode, str_action, file_name, (unsigned long)*len);

    /* return the length of the text, if this is an read action
    if (action == TFTP_READ) {
        *len = sizeof(_tftp_client_hello);
    }*/

    /* remember the action of the current transfer */
    _tftp_action = action;

    /* we accept the transfer to take place so we return true */
    return true;
}

/**
 * @brief called to get or put data, depending on the mode received by `_tftp_start_cb(action, ...)`
 */
static int _tftp_client_data_cb(uint32_t offset, void *data, size_t data_len)
{
    char *c = (char *) data;
    /* we received a data block which we write to the buffer */
    tftp_save_to_slot1((uint8_t*)c, data_len);

    /* return the length of the data block */
    return data_len;
}

/**
 * @brief the transfer has stopped, see the event argument to determined if it was successful
 *        or not.
 */
static void _tftp_client_stop_cb(tftp_event_t event, const char *msg)
{
    /* decode the stop event received */
    const char *cause = "UNKOWN";

    if (event == TFTP_SUCCESS) {
        write_last_page();
        slot_ready = true;
        cause = "SUCCESS";
    }
    else if (event == TFTP_PEER_ERROR) {
        cause = "ERROR From Client";
    }
    else if (event == TFTP_INTERN_ERROR) {
        cause = "ERROR Internal Server Error";
    }

    /* print the transfer result to the console */
    printf("tftp_client: %s: %s\n", cause, msg);
}

int tftp_client_init(const char *addr, const char *filename)
{
    ipv6_addr_t ip;
    const char *file_name = filename;
    tftp_mode_t mode = TTM_OCTET;
    bool use_options = true;

    ipv6_addr_from_str(&ip, _tftp_default_host);

    _tftp_action = TFTP_READ;
    mode = TTM_OCTET;
    use_options = false;

    if (!ipv6_addr_from_str(&ip, addr)) {
        puts("tftp: invalid IP address");
        return -1;
    }

    if (_tftp_action == TFTP_READ) {
        int err;
        puts("tftp: starting read request");
        err = gnrc_tftp_client_read(&ip, file_name, mode, _tftp_client_data_cb,
                                      _tftp_client_start_cb, _tftp_client_stop_cb, use_options);
        if (err < 0) {
            printf("Initialisation failed with error %d\n", err);
        }
        else {
            printf("Initialisation successful for address %s file %s\n", addr, file_name);
        }
    }

    return 0;
}
