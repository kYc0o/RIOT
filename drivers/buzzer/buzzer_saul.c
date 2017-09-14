/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_buzzer
 * @{
 *
 * @file
 * @brief       SAUL wrapper for direct access to a buzzer
 *
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "saul/periph.h"
#include "phydat.h"
#include "periph/pwm.h"
#include "buzzer.h"

static int set_buzzer(const void *dev, phydat_t *value)
{
    if (buzzer_set((const buzzer_t*)dev, value->val[0]) == -1) {
        return -1;
    }

    /* PWM unit contains (in order) duty cycle, frequency and resolution */
    value->unit = UNIT_PWM;
    value->scale = 0;
    return 1;
}

const saul_driver_t buzzer_saul_driver = {
    .read = saul_notsup,
    .write = set_buzzer,
    .type = SAUL_ACT_BUZZER,
};
