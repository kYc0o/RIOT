/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   drivers_buzzer
 * @{
 *
 * @file
 * @brief     Default SAUL configuration for a buzzer
 *
 * @author    Francisco Acosta <francisco.acosta@inria.fr>
 */

#ifndef BUZZER_PARAMS_H
#define BUZZER_PARAMS_H

#include "board.h"
#include "periph/pwm.h"
#include "saul_reg.h"
#include "buzzer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters for the buzzer
 * @{
 */
#ifndef BUZZER_PARAM_PWM_DEV
#define BUZZER_PARAM_PWM_DEV         PWM_DEV(1)
#endif
#ifndef BUZZER_PARAM_CHANNEL
#define BUZZER_PARAM_CHANNEL         (0)
#endif
#ifndef BUZZER_PARAM_FREQ
#define BUZZER_PARAM_FREQ            (1000)
#endif

#define BUZZER_PARAMS_DEFAULT        { .device     = BUZZER_PARAM_PWM_DEV, \
                                       .channel    = BUZZER_PARAM_CHANNEL,    \
                                       .freq       = BUZZER_PARAM_FREQ }
/**@}*/

/**
 * @brief   Configure BUZZER
 */
static const buzzer_params_t buzzer_params[] =
{
#ifdef BUZZER_PARAMS_CUSTOM
    BUZZER_PARAMS_CUSTOM,
#else
    BUZZER_PARAMS_DEFAULT,
#endif
};

/**
 * @brief   Allocate and configure entries to the SAUL registry
 */
saul_reg_info_t buzzer_saul_reg_info[] =
{
    { .name = "buzzer" }
};

#ifdef __cplusplus
}
#endif

#endif /* BUZZER_PARAMS_H */
/** @} */
