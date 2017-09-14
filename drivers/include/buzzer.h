/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    driver_buzzer Buzzer Driver
 * @ingroup     drivers_actuators
 * @brief       High-level driver for buzzers
 * @{
 *
 * @file
 * @brief       High-level driver for easy handling of buzzers
 *
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 */

#ifndef BUZZER_H
#define BUZZER_H

#include "periph/pwm.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BUZZER_RESOLUTION
#define BUZZER_RESOLUTION           (1000)
#endif

#ifndef BUZZER_FREQUENCY
#define BUZZER_FREQUENCY            (1000)
#endif

#ifndef BUZZER_DUTYCYCLE
#define BUZZER_DUTYCYCLE            (BUZZER_RESOLUTION / 2)
#endif

/**
 * @brief Descriptor struct for a buzzer
 */
typedef struct {
    pwm_t device;           /**< the PWM device driving the buzzer */
    int channel;            /**< the channel the buzzer is connected to */
    unsigned int vol;       /**< minimum pulse width, in us */
    unsigned int freq;      /**< maximum pulse width, in us */
} buzzer_t;

/**
 * @brief Device initialization parameters
 */
typedef buzzer_t buzzer_params_t;

/**
 * @brief Initialize a buzzer motor by assigning it a PWM device and channel
 *
 * The buzzer is initialized with default PWM values:
 * - frequency: 1KHz (1ms interval)
 * - resolution: 1000 (1000 steps per ms)
 *
 * These default values can be changed by setting BUZZER_RESOLUTION and
 * BUZZER_FREQUENCY macros.
 *
 * @param[out] dev          struct describing the buzzer
 * @param[in] pwm           the PWM device the buzzer is connected to
 * @param[in] pwm_channel   the PWM channel the buzzer is connected to
 * @param[in] freq          Frequency of the buzzer
 *
 * @return                  The actual frequency on success
 * @return                  <0 on error
 */
int buzzer_init(buzzer_t *dev, pwm_t pwm, int pwm_channel, unsigned int freq);

/**
 * @brief Set the buzzer motor to a specified volume and frequency
 *
 * The volume of the buzzer is specified in the pulse width that
 * controls the buzzer. With default configurations, a value of 500
 * means a pulse width of 0.5 ms, which is the center position.
 * However, depending on the buzzer this can affect more or less the
 * volume, depending on the reponse (e.g. a duty cycle of 50% can be
 * the highest volume).
 *
 * @param[in] dev           the buzzer to set
 * @param[in] freq          the frequency of the produced sound
 *
 * @return                  The actual frequency on success
 * @return                  <0 on error
 */
int buzzer_set(const buzzer_t *dev, unsigned int freq);

#ifdef __cplusplus
}
#endif

#endif /* BUZZER_H */
/** @} */
