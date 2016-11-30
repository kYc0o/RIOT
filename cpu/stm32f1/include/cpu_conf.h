/*
 * Copyright (C) 2013 INRIA
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_stm32f1 STM32F1
 * @ingroup         cpu
 * @brief           CPU specific implementations for the STM32F1
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Alaeddine Weslati <alaeddine.weslati@intia.fr>
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include "cpu_conf_common.h"

#if defined(CPU_MODEL_STM32F103CB) || defined(CPU_MODEL_STM32F103RB)
#include "stm32f103xb.h"
#elif defined(CPU_MODEL_STM32F103RE)
#include "stm32f103xe.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Flash page configuration
 * @{
 */
#define FLASHPAGE_SIZE      (2048U)

#if defined(CPU_MODEL_STM32F103CB) || defined(CPU_MODEL_STM32F103RB)
#define FLASHPAGE_NUMOF     (64U)
#elif defined(CPU_MODEL_STM32F103RE)
#define FLASHPAGE_NUMOF     (256U)
#endif
/** @} */

/*
 * @brief   Offset to reset handler on VTOR
 */
#define OTA_RESET_VECTOR     0x4

#if defined(CPU_MODEL_STM32F103RE)
/*
 * @brief   Flash partitioning for OTA updates
 * @{
 */
#define MAX_OTA_SLOTS               (2)
#define OTA_SLOT_PAGES              (120)
#define OTA_SLOT_1                  (0x08004000)
#define OTA_SLOT_1_END              (0x0803FFFF)
#define OTA_SLOT_1_PAGE             (8)
#define OTA_SLOT_2                  (0x08040000)
#define OTA_SLOT_2_END              (0x0807BFFF)
#define OTA_SLOT_2_PAGE             (128)

#if OTA
    #if OTA_SLOT == 1
    #define CURRENT_FIRMWARE_ADDR       OTA_SLOT_1
    #define CURRENT_FIRMWARE_PAGE       OTA_SLOT_1_PAGE
    #define CURRENT_FIURMWARE_END       OTA_SLOT_1_END
    #endif

    #if OTA_SLOT == 2
    #define CURRENT_FIRMWARE_ADDR       OTA_SLOT_2
    #define CURRENT_FIRMWARE_PAGE       OTA_SLOT_2_PAGE
    #define CURRENT_FIURMWARE_END       OTA_SLOT_2_END
#endif

#endif /* OTA */

#endif /* defined(CPU_MODEL_STM32F103RE) */
/** @} */

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO         (1U)
#define CPU_IRQ_NUMOF                (60U)

#if OTA
#define CPU_FLASH_BASE               (CURRENT_FIRMWARE_ADDR + OTA_METADATA_SPACE)
#else
#define CPU_FLASH_BASE               FLASH_BASE
#endif
/** @} */

/**
 * @brief   Flash page configuration
 * @{
 */
#define FLASHPAGE_SIZE      (2048U)

#if defined(CPU_MODEL_STM32F103C8)
#define FLASHPAGE_NUMOF     (32U)
#elif defined(CPU_MODEL_STM32F103CB) || defined(CPU_MODEL_STM32F103RB)
#define FLASHPAGE_NUMOF     (64U)
#elif defined(CPU_MODEL_STM32F103RE)
#define FLASHPAGE_NUMOF     (256U)
#endif
/** @} */

/**
 * @brief Configure the CPU's clock system
 *
 * @param[in] source    source clock frequency
 * @param[in] target    target clock frequency
 * @param[in] prescale  prescaler to use
 */
void cpu_clock_scale(uint32_t source, uint32_t target, uint32_t *prescale);

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
