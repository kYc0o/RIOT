/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       Over the air (OTA) via TFTP upgrade module
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>

#include "thread.h"
#include "net/gnrc/tftp.h"
#include "firmware_update.h"

#include "log.h"

/* the message queues */
#define TFTP_QUEUE_SIZE     (4)
static msg_t _tftp_msg_queue[TFTP_QUEUE_SIZE];

/* allocate the stack */
char _tftp_stack[THREAD_STACKSIZE_MAIN + THREAD_EXTRA_STACKSIZE_PRINTF + 4096];

static firmware_update_t _state;

/**
 * @brief called at every transcation start
 */
static bool _tftp_server_start_cb(tftp_action_t action, tftp_mode_t mode,
                                  const char *file_name, size_t *len)
{
    LOG_INFO("%s: start %s %u\n", __func__, file_name, *len);

    /* make sure this is a write using octets request */
    if (mode != TTM_OCTET || action != TFTP_WRITE) {
        return false;
    }

    /* initialize firmware upgrade state struct */
    firmware_update_init(&_state, firmware_target_slot());

    /* we accept the transfer to take place so we return true */
    return true;
}

/**
 * @brief called to get or put data, depending on the mode received by `_tftp_start_cb(action, ...)`
 */
static int _tftp_server_data_cb(uint32_t offset, void *data, size_t data_len)
{
    printf("%s: stub: 0x%08x 0x%08x 0x%08x\n", __func__, (unsigned)offset, (unsigned)data, (unsigned)data_len);
    return firmware_update_putbytes(&_state, offset, data, data_len);
}

/**
 * @brief the transfer has stopped, see the event argument to determined if it was successful
 *        or not.
 */
static void _tftp_server_stop_cb(tftp_event_t event, const char *msg)
{
    /* decode the stop event received */
    const char *cause = "UNKOWN";

    if (event == TFTP_SUCCESS) {
        cause = "SUCCESS";
        firmware_update_finish(&_state);
    }
    else if (event == TFTP_PEER_ERROR) {
        cause = "ERROR From Client";
    }
    else if (event == TFTP_INTERN_ERROR) {
        cause = "ERROR Internal Server Error";
    }

    /* print the transfer result to the console */
    printf("ota: tftp: transfer stopped: %s: %s\n", cause, (msg == NULL) ? "NULL" : msg);
}

/**
 * @brief the TFTP server thread
 */
static void *tftp_server_wrapper(void *arg)
{
    (void)arg;

    /* A message queue is needed to register for incoming packets */
    msg_init_queue(_tftp_msg_queue, TFTP_QUEUE_SIZE);

    /* inform the user */
    puts("ota: Starting TFTP service at port 69");

    /* run the TFTP server */
    gnrc_tftp_server(_tftp_server_data_cb, _tftp_server_start_cb, _tftp_server_stop_cb, true);

    /* the TFTP server has been stopped */
    puts("ota: Stopped TFTP service");

    return NULL;
}

/**
 * @brief start the TFTP server by creating a thread
 */
void ota_start(void)
{
    thread_create(_tftp_stack, sizeof(_tftp_stack),
                  1, THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
                  tftp_server_wrapper, NULL, "OTA TFTP Server");
}

/**
 * @brief stop the TFTP server by sending a message to the thread
 */
void tftp_server_stop(void)
{
    gnrc_tftp_server_stop();
}
