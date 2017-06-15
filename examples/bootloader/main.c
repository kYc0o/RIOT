#include "firmware.h"
#include "cpu.h"
#include "panic.h"

void kernel_init(void)
{
    uint32_t version = 0;
    uint32_t slot = 0;
    for (unsigned i = 1; i < firmware_num_slots; i++) {
        firmware_metadata_t *slot_metadata = firmware_get_metadata(i);
        if (firmware_validate_metadata_checksum(slot_metadata)) {
            /* skip slot if metadata broken */
            continue;
        }
        if (slot_metadata->start_addr != firmware_get_image_startaddr(i)) {
            continue;
        }
        if (!slot || slot_metadata->version > version) {
            version = slot_metadata->version;
            slot = i;
        }
    }

    if (slot) {
        firmware_jump_to_slot(slot);
    }

    /* serious trouble! */
    while(1) {}
}

NORETURN void core_panic(core_panic_t crash_code, const char *message)
{
    (void)crash_code;
    (void)message;
    while(1) {}
}
