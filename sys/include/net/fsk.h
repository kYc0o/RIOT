/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_lora LoRa modulation
 * @ingroup     net
 * @brief       LoRa modulation header definitions
 * @{
 *
 * @file
 * @brief       LoRa modulation header definitions
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef NET_LORA_H
#define NET_LORA_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup net_fsk_conf  FSK modulation compile configurations
 * @ingroup  config
 * @{
 */
/** @brief Frequency step in Hz */
#ifndef FSK_FREQ_STEP
#define FSK_FREQ_STEP                          (61.03515625)
#endif

#ifdef __cplusplus
}
#endif

#endif /* NET_LORA_H */
/** @} */
