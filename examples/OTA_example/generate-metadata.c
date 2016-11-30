/*
 * Copyright (C) 2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     OTA
 * @{
 *
 * @file
 * @brief       Meta-data generation for OTA update images
 *
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define FLASH_WORD_SIZE 4
#define is_bigendian() ((*(char*)&i) == 0)

uint32_t firmware_size = 0;

typedef struct OTA_metadata {
    uint16_t crc;
    uint16_t crc_shadow;
    uint32_t size;            /* Size of firmware image */
    uint32_t uuid;            /* Integer representing unique firmware ID */
    uint16_t version;         /* Integer representing firmware version */
} OTA_metadata_t;

/* Input firmware .bin file */
FILE *firmware_bin;

/* Output metadata .bin file */
FILE *metadata_bin;

const int i = 1;

/*
 * @fn          crc16
 *
 * @brief       Run the CRC16 Polynomial calculation over the byte parameter.
 *
 * @param       crc - Running CRC calculated so far.
 * @param       val - Value on which to run the CRC16.
 *
 * @return      crc - Updated for the run.
 */
static uint16_t crc16(uint16_t crc, uint8_t val)
{
    const uint16_t poly = 0x1021;
    uint8_t cnt;

    for (cnt = 0; cnt < 8; cnt++, val <<= 1) {
        uint8_t msb = (crc & 0x8000) ? 1 : 0;

        crc <<= 1;

        if (val & 0x80) {
            crc |= 0x0001;
        }

        if (msb) {
            crc ^= poly;
        }
    }

    return crc;
}

static uint16_t crc_calc_word(uint8_t *_word, uint16_t image_CRC)
{
    uint16_t _crc = image_CRC;
    int idx;

    for (idx = 0; idx < FLASH_WORD_SIZE; idx++) {
        _crc = crc16(_crc, _word[idx]);
    }

    firmware_size += 4;

    return _crc;
}

/*
 * @fn      crcCalc
 *
 * @brief   Run the CRC16 Polynomial calculation over the provided firmware .bin
 *
 * @param   None
 *
 * @return  The CRC16 calculated.
 */
static uint16_t crc_calc(void)
{
    uint16_t image_CRC = 0;

    uint8_t _word[FLASH_WORD_SIZE]; /* a 4-byte buffer */
    size_t nret;

    while ((nret = fread(_word, FLASH_WORD_SIZE, 1, firmware_bin)) == 1) {
        image_CRC = crc_calc_word(_word, image_CRC);
    }

    if (nret) {
        image_CRC = crc_calc_word(_word, image_CRC);
    }

    /*
     * IAR note explains that poly must be run with value zero for each byte of
     * the crc.
     */
    image_CRC = crc16(image_CRC, 0);
    image_CRC = crc16(image_CRC, 0);

    /* Return the CRC calculated over the image. */
    return image_CRC;
}

int main(int argc, char *argv[])
{
    (void)argc;

    if (!argv[1]) {
        printf("Please provide a .bin file to compute the CRC on as the first argument.\n");
        return -1;
    }

    if (!argv[2]) {
        printf("Please provide a 16-bit hex firmware version integer as the second argument.\n");
        return -1;
    }

    if (!argv[3]) {
        printf("Please provide a 32-bit hex UUID integer as the third argument.\n");
        return -1;
    }

    if (!argv[4]) {
        printf("Please provide 0 or 1 to indicate whether this image is pre-verified or not as the fourth argument.\n");
        return -1;
    }

    /* (1) Open the firmware .bin file */
    firmware_bin = fopen( argv[1], "rb");
    int firmware_verified;
    sscanf(argv[4], "%d", &firmware_verified);

    /* (2) Run the CRC16 calculation over the file.  Print result. */
    uint16_t crc_result = crc_calc();
    printf("%#x\n", crc_result);

    /* (3) Close the .bin file. */
    fclose(firmware_bin);

    /* (4) Generate OTA image metadata */
    OTA_metadata_t metadata;
    metadata.crc = crc_result;

    if (firmware_verified) {
        metadata.crc_shadow = crc_result;
    } else {
        metadata.crc_shadow = 0;
    }

    metadata.size = firmware_size;
    sscanf(argv[2], "%xu", (unsigned int *)&(metadata.version));
    sscanf(argv[3], "%xu", &(metadata.uuid));
    uint8_t output_buffer[sizeof(OTA_metadata_t)];
    memcpy(output_buffer, (uint8_t*)&metadata, sizeof(OTA_metadata_t));

    printf("Firmware Size: %d\n", metadata.size);
    printf("Firmware Version: %#x\n", metadata.version);
    printf("Firmware UUID: %#x\n", metadata.uuid);
    printf("Firmware CRC: %#x\n", metadata.crc);
    printf("Firmware CRC: %#x\n", metadata.crc_shadow);

    /* (5) Open the output firmware .bin file */
    metadata_bin = fopen("firmware-metadata.bin", "wb");

    /* (6) Write the metadata */
    fwrite(output_buffer, sizeof(output_buffer), 1, metadata_bin);

    /* (7) 0xff spacing until firmware binary starts */
    uint8_t blank_buffer[240];

    for (int b=0; b<240; b++) {
        blank_buffer[ b ] = 0xff;
    }

    fwrite( blank_buffer, 240, 1, metadata_bin);

    /* (8) Close the metadata file */
    fclose( metadata_bin );

    return 0;
}
