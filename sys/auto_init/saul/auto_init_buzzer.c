/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of buzzer driver.
 *
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#ifdef MODULE_BUZZER

#include "log.h"
#include "saul_reg.h"
#include "buzzer.h"
#include "buzzer_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define BUZZER_NUMOF    (sizeof(buzzer_params) / sizeof(buzzer_params[0]))

/**
 * @brief   Allocation of memory for device descriptors
 */
static buzzer_t buzzer_devs[BUZZER_NUMOF];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[BUZZER_NUMOF];

/**
 * @brief   Reference the driver structs.
 * @{
 */
extern const saul_driver_t buzzer_saul_driver;
/** @} */

void auto_init_buzzer(void)
{
    for (unsigned i = 0; i < BUZZER_NUMOF; i++) {
        LOG_DEBUG("[auto_init_saul] initializing buzzer #%u\n", i);

        if (buzzer_init(&buzzer_devs[i],
                         buzzer_params[i].device,
                         buzzer_params[i].channel,
                         buzzer_params[i].freq) == 0) {
            LOG_ERROR("[auto_init_saul] error initializing buzzer #%u\n", i);
            continue;
        }

        /* illuminance */
        saul_entries[i].dev = &(buzzer_devs[i]);
        saul_entries[i].name = buzzer_saul_reg_info[i].name;
        saul_entries[i].driver = &buzzer_saul_driver;

        /* register to saul */
        saul_reg_add(&(saul_entries[i]));
    }
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_BUZZER */
