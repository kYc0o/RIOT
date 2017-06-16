#ifndef FIRMWARE_H
#define FIRMWARE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "hashes/sha256.h"
#include "tweetnacl.h"

/**
 *  @brief FIRMWARE_SIG_LEN:
 *         Provisional length for signed hash
 */
#define FIRMWARE_SIG_LEN            (crypto_sign_BYTES)
#define FIRMWARE_SECKEY_LEN         (crypto_sign_SECRETKEYBYTES)
#define FIRMWARE_PUBKEY_LEN         (crypto_sign_PUBLICKEYBYTES)

/**
 *  @brief FIRMWARE_CHECKSUM_WORDS:
 *         number of words to be fletcher32()'ed
 *
 */
#define FIRMWARE_CHECKSUM_WORDS     (10)
#define FIRMWARE_CHECKSUM_LEN       (20)
#define FIRMWARE_SIGN_BYTES         (FIRMWARE_CHECKSUM_LEN + 4 + SHA256_DIGEST_LENGTH)
#define FIRMWARE_PADDING            (256-(FIRMWARE_SIGN_BYTES + FIRMWARE_SIG_LEN))
#define FIRMWARE_METADATA_REALLEN   (sizeof(firmware_metadata_t) - FIRMWARE_PADDING)

/**
 * @brief Structure to store firmware metadata
 * @{
 */
typedef struct {
    uint32_t magic_number;              /**< metadata magic_number (always "RIOT")  */
    uint32_t appid;                     /**< Integer representing the application ID*/
    uint32_t version;                   /**< Integer representing firmware version  */
    uint32_t size;                      /**< Size of firmware image                 */
    uint32_t start_addr;                /**< Start address in flash                 */
    uint32_t chksum;                    /**< checksum of metadata                   */
    uint8_t hash[SHA256_DIGEST_LENGTH]; /**< SHA256 Hash of firmware image          */
    uint8_t sig[FIRMWARE_SIG_LEN];      /**< Firmware signature                     */
    uint8_t pad[FIRMWARE_PADDING];      /**< padding to total of 256 bytes          */
} firmware_metadata_t;
/** @} */

/**
 * @brief  Print formatted FW image metadata to STDIO.
 *
 * @param[in] metadata  ptr to firmware metadata
 *
 */
void firmware_metadata_print(firmware_metadata_t *metadata);

/**
 * @brief  Validate FW image metadata
 *
 * @param[in] metadata  ptr to firmware metadata
 *
 */
int firmware_validate_metadata_checksum(firmware_metadata_t *metadata);

/**
 * @brief  Calculate metadata checksum
 *
 * @param[in] metadata  ptr to firmware metadata
 *
 */
uint32_t firmware_metadata_checksum(firmware_metadata_t *metadata);

/**
 * @brief  Sign metadata
 *
 * @param[in] metadata  ptr to firmware metadata
 * @param[in] sk        NaCL secret signing key to use
 *
 */
int firmware_sign_metadata(firmware_metadata_t *metadata, unsigned char *sk);

/**
 * @brief  Validate FW metadata signature
 *
 * @param[in] metadata  ptr to firmware metadata
 * @param[in] pk        NaCL public signing key to use
 *
 */
int firmware_validate_metadata_signature(firmware_metadata_t *metadata, const unsigned char *pk);

/**
 * @brief  Jump to image
 *
 * @param[in] metadata  ptr to firmware metadata
 *
 */
void firmware_jump_to_image(firmware_metadata_t *metadata);

/**
 * @brief  Get currently running image slot
 */
int firmware_current_slot(void);
int firmware_target_slot(void);

/**
 * @brief  Get metadata of firmware slot
 */
firmware_metadata_t *firmware_get_metadata(unsigned slot);

/**
 * @brief  Get jump-to address of firmware slot
 */
unsigned firmware_get_image_startaddr(unsigned slot);

/**
 * @brief  Boot into image in slot @p slot
 */
void firmware_jump_to_slot(unsigned slot);

/**
 * @brief  Dump firmware slot addresses
 */
void firmware_dump_slot_addrs(void);

extern const unsigned firmware_num_slots;

#ifdef __cplusplus
}
#endif

#endif /* FIRMWARE_H */
