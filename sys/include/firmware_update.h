#ifndef FIRMWARE_UPDATE_H
#define FIRMWARE_UPDATE_H

#include "firmware.h"
#include "periph/flashpage.h"

typedef struct {
    unsigned state;
    int target_slot;
    size_t offset;
    unsigned flashpage;
    sha256_context_t sha256;
    uint8_t flashpage_buf[FLASHPAGE_SIZE];
    uint8_t metadata_buf[FLASHPAGE_SIZE];
} firmware_update_t;

int firmware_update_init(firmware_update_t *firmware_update, int target_slot);
int firmware_update_putbytes(firmware_update_t *firmware_update, size_t offset, const uint8_t *bytes, size_t len);
int firmware_update_finish(firmware_update_t *firmware_update);

#endif /* FIRMWARE_UPDATE_H */
