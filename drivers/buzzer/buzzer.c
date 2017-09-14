/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     driver_buzzer
 * @{
 *
 * @file
 * @brief       Buzzer driver implementation
 *
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#include "buzzer.h"
#include "periph/pwm.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

int buzzer_init(buzzer_t *dev, pwm_t pwm, int pwm_channel, unsigned int freq)
{
    int actual_frequency;

    actual_frequency = pwm_init(pwm, PWM_LEFT, freq, BUZZER_RESOLUTION);

    DEBUG("[buzzer] requested %d hz, got %d hz\n", freq, actual_frequency);

    if (actual_frequency < 0) {
        /* PWM error */
        return -1;
    }

    dev->device = pwm;
    dev->channel = pwm_channel;
    dev->freq = freq;

    return actual_frequency;
}

int buzzer_set(const buzzer_t *dev, unsigned int freq)
{
    int actual_frequency;
    actual_frequency = pwm_init(dev->device, PWM_LEFT, freq, BUZZER_RESOLUTION);

    if (actual_frequency < 0) {
            /* PWM error */
            return -1;
    }

    DEBUG("[buzzer] requested %d hz, got %d hz\n", freq, actual_frequency);
    pwm_set(dev->device, dev->channel, BUZZER_DUTYCYCLE);

    return actual_frequency;
}
