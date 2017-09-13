/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   boards_samr21-xpro
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped ADC
 *
 * @author    Fracisco Acosta <francisco.acosta@inria.fr>
 */

#ifndef ADC_PARAMS_H
#define ADC_PARAMS_H

#include "board.h"
#include "periph/adc.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    ADC pin configuration
 */
static const  saul_adc_params_t saul_adc_params[] =
{
    {
        .name = "ADC0_1(PA06)",
        .line = ADC_LINE(0),
        .res  = ADC_RES_10BIT,
    },
    {
        .name = "ADC0_2(PA07)",
        .line = ADC_LINE(1),
        .res  = ADC_RES_10BIT,
    },
};

#ifdef __cplusplus
}
#endif

#endif /* ADC_PARAMS_H */
/** @} */
