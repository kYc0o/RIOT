/** @file   fota.h
 *  @brief  OTA Image R/W and Verification
 *  @author Mark Solters <msolters@gmail.com>
 */

#ifndef FOTA_H
#define FOTA_H

/*
 *  OTA_METADATA_SPACE:
 *    We allow some space for the OTA metadata, which is placed immediately
 *    before firmware image content.  However, the VTOR table in firmware must
 *    be 256-byte aligned.  So, the minimum space we can set for metadata is
 *    0x100, not the mere 0x10 that it requires.
 *
 *  OTA_METADATA_LENGTH:
 *    This is just the size of the OTAMetadata_t struct, which is 4-byte
 *    aligned.  We use 14 bytes currently, so this struct will be 16 bytes.
 */
#define OTA_METADATA_SPACE   0x100
#define OTA_METADATA_LENGTH  sizeof(OTA_metadata_t)

typedef struct OTA_metadata {
    uint16_t crc;
    uint16_t crc_shadow;
    uint32_t size;            /* Size of firmware image */
    uint32_t uuid;            /* Integer representing unique firmware ID */
    uint16_t version;         /* Integer representing firmware version */
} OTA_metadata_t;


/**
 * @brief  Print formatted OTA image metadata to STDIO.
 *
 * @param[in] metadata          Metadata struct to fill with firmware metadata
 *
 */
void print_metadata(OTA_metadata_t *metadata);

/**
 * @brief   Get the internal metadata belonging to an OTA slot in internal
 *          flash.
 *
 * @param[in] ota_slot             The OTA slot to be read for metadata.
 *
 * @param[in] *ota_slot_metadata   Pointer to the OTAMetadata_t struct where
 *                                 the metadata is to be written.
 *
 * @return  0 on success or error code
 */
int get_int_metadata(uint8_t ota_slot_page, OTA_metadata_t *ota_metadata);

/**
 * @brief   Get the metadata belonging to an OTA slot in external flash.
 *
 * @param[in] ota_slot            The OTA slot to be read for metadata.
 *
 * @param[in] *ota_slot_metadata  Pointer to the OTAMetadata_t struct where
 *                                the metadata is to be written.
 *
 * @return  0 on success or error code
 */
int get_ota_slot_metadata(uint8_t ota_slot, OTA_metadata_t *ota_slot_metadata);

/**
 * @brief   Get the metadata belonging to an OTA slot in internal flash.
 *
 * @param[in] ota_slot             The OTA slot to be read for metadata.
 *
 * @param[in] *ota_slot_metadata   Pointer to the OTAMetadata_t struct where
 *                                 the metadata is to be written.
 *
 * @return  0 on success or error code
 */
int get_int_ota_slot_metadata(uint8_t ota_slot, OTA_metadata_t *ota_slot_metadata);

/**
 * @brief   Get the address corresponding to a given slot
 *
 * @param[in] ota_slot             The OTA slot to get the address.
 *
 *
 * @return  0 on success or error code
 */
uint32_t get_slot_ota_address(uint8_t ota_slot);

/**
 * @brief   Get the page corresponding to a given slot
 *
 * @param[in] ota_slot             The OTA slot to get the page.
 *
 *
 * @return  0 on success or error code
 */
uint32_t get_slot_ota_page(uint8_t ota_slot);

/**
 * @brief   Write new metadata to a specific OTA slot in internal flash.
 *
 * @param   ota_slot             The OTA slot to be modified.
 *
 * @param   *ota_slot_metadata   Pointer to the new OTAMetadata_t data.
 *
 * @return  0 on success or error code
 */
int overwrite_int_ota_slot_metadata(uint8_t ota_slot, OTA_metadata_t *ota_slot_metadata);

/**
 * @brief   Write new metadata to a specific OTA slot in external flash.
 *
 * @param   ota_slot             The OTA slot to be modified.
 *
 * @param   *ota_slot_metadata   Pointer to the new OTAMetadata_t data.
 *
 * @return  0 on success or error code
 */
int overwrite_ota_slot_metadata(uint8_t ota_slot, OTA_metadata_t *ota_slot_metadata);

/**
 * @brief   Copy the current firmware into OTA slot 0 as the "Golden Image"
 *
 * @return  0 for success or error code
 */
int backup_golden_image(void);

/**
 * @brief   Given an OTA slot, verify the firmware content against the metadata.
 *          If everything is fine, update the metadata to indicate this OTA slot
 *          is valid.
 *
 * @param[in]  ota_slot  - OTA slot index to verify. (1-3)
 *
 * @return  0 for success or error code
 */
int verify_int_ota_slot(uint8_t ota_slot);

/**
 * @brief   Returns true only if the metadata provided indicates the OTA slot
 *          is populated and valid.
 *
 * @param[in] *metadata          OTA metadata to be validated
 *
 * @return  True if the OTA slot is populated and valid. Otherwise, false.
 */
int validate_ota_metadata(OTA_metadata_t *metadata);

/**
 * @brief   Find an OTA slot containing firmware matching the supplied
 *          firmware version number. Will only find the first matching
 *          slot.
 *
 * @param[in] version            OTA update version.
 *
 * @return  The OTA slot index of the matching OTA slot. Return -1 in the event
 *          of no match.
 */
int find_matching_int_ota_slot(uint16_t version);

/**
 * @brief   Find the first empty OTA download slot.  Failing this, find the slot
 *          with the most out-of-date firmware version.
 *
 * @return  The OTA slot index of the empty/oldest OTA slot.  This will never be
 *          0 because the Golden Image should never be erased.
 */
int find_empty_int_ota_slot(void);

/**
 * @brief   Find the OTA slot containing the most out-of-date firmware version.
 *          OTA slots are in external flash.
 *
 * @return  The OTA slot index of the oldest firmware version.
 */
int find_oldest_int_ota_image(void);

/**
 * @brief   Find the OTA slot containing the most recent firmware version.
 *          OTA slots are in external flash.
 *
 * @return  The OTA slot index of the newest firmware version.
 */
int find_newest_int_ota_image(void);

/**
 * @brief   Clear an OTA slot in external flash.
 *
 * @param[in] ota_slot   The OTA slot index of the firmware image to be copied.
 *
 * @return  0 or error code
 */
int erase_int_ota_image(uint8_t ota_slot);

/**
 * @brief   Overwrite firmware located in internal flash with the firmware
 *          stored in an external flash OTA slot.
 *
 * @param[in] ota_slot   The OTA slot index of the firmware image to be copied.
 *                       0 = "Golden Image" backup, aka factory restore
 *                       1, 2, 3 = OTA Download slots
 *
 * @return  0 or error code
 */
int update_firmware(uint8_t ota_slot);

/**
 * @brief   Store firmware data in external flash at the specified
 *          address.
 *
 * @param[in] ext_address   External flash address to begin writing data.
 *
 * @param[in] data          Pointer to the data buffer to be written.
 *                          Note: page_data can be larger than 4096 bytes, but
 *                          only the first 4096 bytes will be written!
 *
 * @return  0 or error code
 */
int store_firmware_data(uint32_t ext_address, uint8_t *data, size_t data_length);

/**
 * @brief   Begin executing another firmware binary located in internal flash.
 *
 * @param[in] destination_address Internal flash address of the vector table
 *                                for the firmware binary that is to be booted
 *                                into. Since this OTA lib prepends metadata
 *                                to each binary, the true VTOR start address
 *                                will be OTA_METADATA_SPACE bytes past this
 *                                address.
 *
 */
void jump_to_image(uint32_t destination_address);

#if OTA
/**
 * @brief                         Get the metadata belonging of whatever
 *                                firmware is currently installed to the
 *                                internal flash.
 *
 * @param[in] ota_slot_metadata   OTAMetadata_t object read from the
 *                                current firmware
 *
 *
 * @return                        0 on success -1 on failure
 */
int get_current_metadata(OTA_metadata_t *ota_slot_metadata);

/**
 * @brief   Rerun the CRC16 algorithm over the contents of internal flash.
 *
 * @param[in]  *current_firmware_metadata
 *
 * @return  0 for success or error code
 */
int verify_current_firmware(OTA_metadata_t *current_firmware_metadata);
#endif

#endif /* FOTA_H */
