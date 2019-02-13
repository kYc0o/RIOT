/*
 * Copyright (C) 2019 Kugu Home GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_fsk FSK modulation
 * @ingroup     net
 * @brief       FSK modulation header definitions
 * @{
 *
 * @file
 * @brief       FSK modulation header definitions
 *
 * @author      Francisco Acosta <f.acosta.ext@kugu-home.com>
 */

#ifndef NET_FSK_H
#define NET_FSK_H

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

#endif /* NET_FSK_H */
/** @} */
