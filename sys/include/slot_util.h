/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *                    Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_slot_util   Helpers to manipulate partitions (slots)
 *                              on internal flash
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       Slot management tools
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#ifndef SLOT_UTIL_H
#define SLOT_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "riot_hdr.h"

/**
 * @brief  Get currently running image slot
 *
 * returns nr of currently active slot
 */
int slot_util_current_slot(void);

/**
 * @brief  Get jump-to address of image slot
 *
 * @param[in]   slot    slot nr to work on
 *
 * @returns address of first byte of @p slot
 */
uint32_t slot_util_get_image_startaddr(unsigned slot);

/**
 * @brief  Boot into image in slot @p slot
 *
 * @param[in]   slot    slot nr to jump to
 */
void slot_util_jump(unsigned slot);

/**
 * @brief  Get header from a given flash slot
 *
 * @param[in]   slot    slot nr to work on
 *
 * @returns header of image slot nr @p slot
 */
const riot_hdr_t *slot_util_get_hdr(unsigned slot);

/**
 * @brief  Validate slot
 *
 * @param[in] slot    slot nr to work on
 *
 * @returns 0 if ok.
 */
static inline int slot_util_validate(unsigned slot)
{
    return riot_hdr_validate(slot_util_get_hdr(slot));
}

/**
 * @brief  Print formatted slot header to STDIO
 *
 * @param[in]   slot    slot nr to work on
 *
 */
static inline void slot_util_print_slot_hdr(unsigned slot)
{
    riot_hdr_print(slot_util_get_hdr(slot));
}

/**
 * @brief   Number of configured firmware slots (incl. bootloader slot)
 */
extern const unsigned slot_util_num_slots;

/**
 * @brief   Storage for header pointers of the configured slots
 */
extern const riot_hdr_t * const slot_util_slots[];

#ifdef __cplusplus
}
#endif

#endif /* SLOT_UTIL_H */