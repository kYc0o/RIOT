/** @file   fota.c
 *  @brief  OTA Image R/W and Verification
 *  @author Mark Solters <msolters@gmail.com>
 *
 *  Notes:
 *  uint16_t crc16() function is Copyright Texas Instruments
 */

#include <stdio.h>
#include <string.h>

#include "cpu_conf.h"
#include "fota.h"
#include "irq.h"
#include "periph/flashpage.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief       Read internal flash to a buffer at specific address.
 *
 * @param[in]   address - Address to be read.
 * @param[in]   count - count in bytes.
 *
 * @param[out]  data_buffer - The buffer filled with the read information.
 *
 */
static void int_flash_read(uint8_t *data_buffer, uint32_t address, uint32_t count)
{
    uint8_t *read_addres = (uint8_t*)address;
    while (count--) {
        *data_buffer++ = *read_addres++;
    }
}

/**
 * @brief       Run the CRC16 Polynomial calculation over the byte parameter.
 *
 * @param[in]       crc - Running CRC calculated so far.
 * @param[in]       val - Value on which to run the CRC16.
 *
 * @return          crc - Updated for the run.
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

uint32_t get_slot_ota_address(uint8_t ota_slot)
{
    /*
     * TODO Optimise for portability
     */
    switch (ota_slot) {
        case 1:
            return OTA_SLOT_1;
            break;

        case 2:
            return OTA_SLOT_2;
            break;
    }

    return 0;
}

uint32_t get_slot_ota_page(uint8_t ota_slot)
{
    /*
     * TODO Optimise for portability
     */
    switch (ota_slot) {
        case 1:
            return OTA_SLOT_1_PAGE;
            break;

        case 2:
            return OTA_SLOT_2_PAGE;
            break;
    }

    return 0;
}

static uint16_t verify_ota_crc(uint32_t addr_begin, uint32_t addr_end)
{
    DEBUG("fota.c: Verifying CRC from %#lx to %#lx\n", addr_begin, addr_end);
    uint16_t image_CRC = 0;
    uint32_t firmware_address = addr_begin;
    uint32_t firmware_end_address = addr_end;

    /* Read the firmware image, one word at a time */
    int idx;

    while (firmware_address < firmware_end_address) {
        uint8_t _word[4];

        int_flash_read(_word, firmware_address, 4);
        for (idx = 0; idx < 4; idx++) {
            image_CRC = crc16(image_CRC, _word[idx]);
        }
        firmware_address += 4; /* move 4 bytes forward */
    }

    /* Compute two more CRC iterations using value of 0 */
    image_CRC = crc16(image_CRC, 0);
    image_CRC = crc16(image_CRC, 0);

    printf("fota.c: CRC Calculated: %#x\n", image_CRC);

    return image_CRC;
}

void print_metadata(OTA_metadata_t *metadata)
{
    printf("Firmware Size: %ld\n", metadata->size);
    printf("Firmware Version: %#x\n", metadata->version);
    printf("Firmware UUID: %#lx\n", metadata->uuid);
    printf("Firmware CRC: %#x\n", metadata->crc);
    printf("Firmware CRC: %#x\n", metadata->crc_shadow);
}

int get_int_metadata(uint8_t ota_slot_page, OTA_metadata_t *ota_metadata)
{
    uint32_t ota_address;

    ota_address = ota_slot_page * FLASHPAGE_SIZE + CPU_FLASH_BASE;

    DEBUG("fota.c: Getting internal metadata on page %d at address %#lx\n",
            ota_slot_page, ota_address);
    int_flash_read((uint8_t*)ota_metadata, ota_address, sizeof(OTA_metadata_t));

    return 0;
}

int get_ota_slot_metadata(uint8_t ota_slot, OTA_metadata_t *ota_slot_metadata)
{
    /*
     * TODO
     */

    return 0;
}

int get_int_ota_slot_metadata(uint8_t ota_slot, OTA_metadata_t *ota_slot_metadata)
{
    DEBUG("fota.c: Getting internal OTA slot %d metadata\n", ota_slot);
    if (ota_slot > MAX_OTA_SLOTS || ota_slot == 0) {
        printf("fota.c: OTA slot not valid, should be <= %d and > 0\n",
                MAX_OTA_SLOTS);
        return -1;
    }

    /*
     * TODO: Optimize for portability
     */

    switch (ota_slot) {
        case 1:
            return get_int_metadata(OTA_SLOT_1_PAGE, ota_slot_metadata);
            break;

        case 2:
            return get_int_metadata(OTA_SLOT_2_PAGE, ota_slot_metadata);
            break;
    }

    return 0;
}

int overwrite_int_ota_slot_metadata(uint8_t ota_slot, OTA_metadata_t *ota_slot_metadata)
{
    /*
     * TODO
     */
    return 0;
}



int overwrite_ota_slot_metadata(uint8_t ota_slot, OTA_metadata_t *ota_slot_metadata)
{
    /*
     * TODO
     */
    return 0;
}

int backup_golden_image(void)
{
    /*
     * TODO
     */
    return 0;
}

int verify_int_ota_slot(uint8_t ota_slot)
{
    /* Determine the external flash address corresponding to the OTA slot */
    uint32_t ota_image_address;

    if (ota_slot > MAX_OTA_SLOTS || ota_slot == 0) {
        printf("fota.c: OTA slot not valid, should be <= %d and > 0\n",
                MAX_OTA_SLOTS);
        return -1;
    }

    ota_image_address = get_slot_ota_address(ota_slot);

    /* Read the metadata of the corresponding OTA slot */
    OTA_metadata_t ota_metadata;

    if(get_int_ota_slot_metadata(ota_slot, &ota_metadata) == 0) {
        print_metadata(&ota_metadata);
    } else {
        printf("fota.c: ERROR cannot get slot metadata.\n");
    }

    /* Compute the CRC16 over the entire image */
    uint16_t image_CRC = 0;

    /* this is where the OTA binary starts */
    ota_image_address += OTA_METADATA_SPACE;
    uint32_t ota_image_end_address = ota_image_address + ota_metadata.size;

    image_CRC = verify_ota_crc(ota_image_address, ota_image_end_address);

    /* Update the CRC shadow with our newly calculated value */
    ota_metadata.crc_shadow = image_CRC;

    if (ota_metadata.crc != ota_metadata.crc_shadow) {
        printf("CRC Check failed!\n");
    } else {
        printf("CRC check succeeded!\n");
    }

    return 0;
}

int validate_ota_metadata(OTA_metadata_t *metadata)
{
    /* Is the OTA slot erased?
     * First, we check to see if every byte in the metadata is 0xFF.
     * If this is the case, this metadata is "erased" and therefore we assume
     * the OTA slot to be empty.
     */
    int erased = 1;
    uint8_t *metadata_ptr = (uint8_t*)metadata;
    int b = OTA_METADATA_LENGTH;

    while (b--) {
        if (*metadata_ptr++ != 0xff) {
            /* We encountered a non-erased byte.
             * There's some non-trivial data here.
             */
            erased = 0;
            break;
        }
    }

    /*If the OTA slot is erased, it's not valid!  No more work to do here. */
    if (erased) {
        return 0;
    }

    /* Check the CRC entries to validate the OTA data itself. */
    if ((metadata->crc) == (metadata->crc_shadow)) {
        return 1;
    }

    /* If we get this far, all metadata bytes were cleared (0xff) */
    return 0;
}

int find_matching_int_ota_slot(uint16_t version)
{
    int matching_slot = -1; /* Assume there is no matching OTA slot. */

    /* Iterate through each of the 3 OTA download slots. */
    for (int slot = 1; slot <= MAX_OTA_SLOTS; slot++) {

        /* Get the metadata of the current OTA download slot. */
        OTA_metadata_t ota_slot_metadata;
        if(get_int_ota_slot_metadata(slot, &ota_slot_metadata) == 0) {
            print_metadata(&ota_slot_metadata);
        } else {
            printf("fota.c: ERROR cannot get slot metadata.\n");
        }

        /* Is this slot empty? If yes, skip. */
        if (validate_ota_metadata(&ota_slot_metadata) == false) {
            continue;
        }

        /* Does this slot's FW version match our search parameter? */
        if (ota_slot_metadata.version == version) {
            matching_slot = slot;
            break;
        }
    }

    if (matching_slot == -1) {
        printf("fota.c: No OTA slot matches Firmware v%i\n", version);
    } else {
        printf("fota.c: OTA slot #%i matches Firmware v%i\n", matching_slot,
                version);
    }

    return matching_slot;
}

int find_empty_int_ota_slot(void)
{
    /* Iterate through each of the MAX_OTA_SLOTS internal slots. */
    for (int slot = 1; slot <= MAX_OTA_SLOTS; slot++) {

        /* Get the metadata of the current OTA download slot. */
        OTA_metadata_t ota_slot_metadata;

        if(get_int_ota_slot_metadata(slot, &ota_slot_metadata) == 0) {
            print_metadata(&ota_slot_metadata);
        } else {
            printf("fota.c: ERROR cannot get slot metadata.\n");
        }

        /* Is this slot invalid? If yes, let's treat it as empty. */
        if (validate_ota_metadata(&ota_slot_metadata) == false) {
            return slot;
        }
    }

    printf("fota.c: Could not find any empty OTA slots!"
            "\nSearching for oldest OTA slot...\n");
    /*
     * If execution goes this far, no empty slot was found. Now, we look for
     * the oldest OTA slot instead.
     */
    return find_oldest_int_ota_image();
}

int find_oldest_int_ota_image(void)
{
    /* The oldest firmware should be the v0 */
    int oldest_ota_slot = 1;
    uint16_t oldest_firmware_version = 0;

    /* Iterate through each of the MAX_OTA_SLOTS internal slots. */
    for (int slot = 1; slot <= MAX_OTA_SLOTS; slot++) {
        /* Get the metadata of the current OTA download slot. */
        OTA_metadata_t ota_slot_metadata;

        if(get_int_ota_slot_metadata(slot, &ota_slot_metadata) == 0) {
            print_metadata(&ota_slot_metadata);
        } else {
            printf("fota.c: ERROR cannot get slot metadata.\n");
        }

        /* Is this slot populated? If not, skip. */
        if (validate_ota_metadata(&ota_slot_metadata) == false) {
            continue;
        }

        /* Is this the oldest image we've found thus far? */
        if (oldest_firmware_version) {
            if (ota_slot_metadata.version < oldest_firmware_version) {
                oldest_ota_slot = slot;
                oldest_firmware_version = ota_slot_metadata.version;
            }
        } else {
            oldest_ota_slot = slot;
            oldest_firmware_version = ota_slot_metadata.version;
        }
    }

    printf("fota.c: Oldest OTA slot: #%u; Firmware v%u\n", oldest_ota_slot,
            oldest_firmware_version);

    return oldest_ota_slot;
}

int find_newest_int_ota_image(void)
{
    /* At first, we only assume knowledge of version v0 */
    int newest_ota_slot = 0;
    uint16_t newest_firmware_version = 0;

    /* Iterate through each of the MAX_OTA_SLOTS. */
    for (int slot = 1; slot <= MAX_OTA_SLOTS ; slot++) {
        /* Get the metadata of the current OTA download slot. */
        OTA_metadata_t ota_slot_metadata;

        if(get_int_ota_slot_metadata(slot, &ota_slot_metadata) == 0) {
            print_metadata(&ota_slot_metadata);
        } else {
            printf("fota.c: ERROR cannot get slot metadata.\n");
        }

        /* Is this slot populated? If not, skip. */
        if (validate_ota_metadata( &ota_slot_metadata) == false) {
            continue;
        }

        /* Is this the newest non-Golden Image image we've found thus far? */
        if ( ota_slot_metadata.version > newest_firmware_version ) {
            newest_ota_slot = slot;
            newest_firmware_version = ota_slot_metadata.version;
        }
    }

    printf("Newest OTA slot: #%u; Firmware v%u\n", newest_ota_slot,
            newest_firmware_version);

    return newest_ota_slot;
}

int erase_int_ota_image(uint8_t ota_slot)
{
    /* Get page address of the ota_slot in internal flash */
    uint32_t ota_image_base_address;
    /* Get the page where the ota_slot is located */
    uint8_t slot_page;

    if (ota_slot > MAX_OTA_SLOTS || ota_slot == 0) {
        printf("fota.c: OTA slot not valid, should be <= %d and > 0\n",
                MAX_OTA_SLOTS);
        return -1;
    }

    ota_image_base_address = get_slot_ota_address(ota_slot);

    printf("fota.c: Erasing OTA slot %u [%#lx, %#lx]...\n", ota_slot,
            ota_image_base_address,
            ota_image_base_address + (OTA_SLOT_PAGES * FLASHPAGE_SIZE) - 1);

    slot_page = get_slot_ota_page(ota_slot);

    /* Erase each page in the OTA internal slot! */
    for (int page = slot_page; page < slot_page + OTA_SLOT_PAGES; page++) {
        DEBUG("fota.c: Erasing page %d\n", page);
        flashpage_write(page, NULL);
    }

    printf("fota.c: Erase successful\n");

    return 0;
}

int update_firmware(uint8_t ota_slot)
{
    /*
     * TODO
     */
    return 0;
}

int store_firmware_data( uint32_t ext_address, uint8_t *data, size_t data_length )
{
    /*
     * TODO
     */
    return 0;
}

/*
 * _estack pointer needed to reset PSP position
 */
extern uint32_t _estack;

void jump_to_image(uint32_t destination_address)
{
    if (destination_address) {
        /*
         * Only add the metadata length offset if destination_address is NOT 0!
         * (Jumping to 0x0 is used to reboot the device)
         */
        destination_address += OTA_METADATA_SPACE;
    }

    /* Disable IRQ */
    (void)irq_disable();

    /* Move PSP to the end of the stack */
    __set_PSP((uint32_t)&_estack);

    /* Move to the second pointer on VTOR (reset_handler_default) */
    destination_address += OTA_RESET_VECTOR;

    /* Load the destination address */
    __asm("LDR R0, [%[dest]]"::[dest]"r"(destination_address));
    /* Make sure the Thumb State bit is set. */
    __asm("ORR R0, #1");
    /* Branch execution */
    __asm("BX R0");
}

#if OTA
int get_current_metadata(OTA_metadata_t *ota_slot_metadata)
{
    return get_int_metadata(CURRENT_FIRMWARE_PAGE, ota_slot_metadata);
}

int verify_current_firmware(OTA_metadata_t *current_firmware_metadata)
{
    printf("fota.c: Recomputing CRC16 on internal flash image within range "
            "[%05X, %05X].\n", CURRENT_FIRMWARE_ADDR, CURRENT_FIURMWARE_END);

    /* Determine the internal flash address corresponding to the OTA slot */
    uint32_t firmware_address = CURRENT_FIRMWARE_ADDR + OTA_METADATA_SPACE;
    uint32_t firmware_end_address = firmware_address +
            (current_firmware_metadata->size);

    /* Compute the CRC16 over the entire image */
    uint16_t image_CRC = 0;

    image_CRC = verify_ota_crc(firmware_address, firmware_end_address);

    /* Update the CRC shadow with our newly calculated value */
    current_firmware_metadata->crc_shadow = image_CRC;

    /* Finally, update metadata stored in internal flash */
    if (flashpage_write_and_verify(CURRENT_FIRMWARE_PAGE, current_firmware_metadata) != FLASHPAGE_OK) {
        printf("fota.c: ERROR! Cannot write new metadata.\n");
        return -1;
    }

    return 0;
}
#endif
