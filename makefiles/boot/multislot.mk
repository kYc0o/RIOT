
include $(RIOTMAKE)/boot/variables.mk
include $(RIOTMAKE)/boot/bootloader.mk

create-key: $(MCUBOOT_KEYFILE)

ifeq ($(BINDIR)/key.pem,$(MCUBOOT_KEYFILE))
$(MCUBOOT_KEYFILE):
	$(Q)mkdir -p $(BINDIR)
	$(Q)$(IMGTOOL) keygen -k $@ -t rsa-2048
endif

.PHONY: flash-bootloader flash-mcuboot

MAKETARGETS += combined flash-multislot

$(BOOTLOADER_BIN):
	@env -i PATH=$(PATH) BOARD=$(BOARD) make -C $(RIOTBASE)/bootloader bootloader

$(FIRMWARE):
	$(Q)env -i PATH=$(PATH) CFLAGS+=-DFIRMWARE_METADATA_SIZE=$(FIRMWARE_METADATA_SIZE) \
	make clean all -C $(FIRMWARE_TOOLS)

$(SECKEY) $(PUBKEY): $(FIRMWARE)
	$(Q)$(GENKEYS) $(SECKEY) $(PUBKEY)
	$(Q)cp $(PUBKEY) ./ota_public_key
	$(Q)xxd -i ota_public_key > $(PUBKEY_DIR)/ota_pubkey.h
	$(Q)rm ./ota_public_key


multislot: $(SECKEY) $(PUBKEY) link $(BOOTLOADER_BIN)
	$(Q)$(_LINK) \
		$(LINKFLAGPREFIX)--defsym=offset="$(SLOT0_SIZE)+$(FIRMWARE_METADATA_SIZE)" \
		$(LINKFLAGPREFIX)--defsym=length="$(SLOT1_SIZE)-$(FIRMWARE_METADATA_SIZE)" \
		-o $(ELFSLOT1) && \
	$(OBJCOPY) -Obinary $(ELFSLOT1) $(BINSLOT1) && \
	$(GENMETA) $(BINSLOT1) $(APP_VERSION) $(APP_ID) $$(($(SLOT0_ADDR)+$(SLOT0_SIZE)+$(FIRMWARE_METADATA_SIZE))) $(SECKEY) $(BINSLOT1).meta && \
	cat $(BINSLOT1).meta $(BINSLOT1) > $(IMAGE_SLOT1) && \
	\
	$(_LINK) \
		$(LINKFLAGPREFIX)--defsym=offset="$(SLOT0_SIZE)+$(SLOT1_SIZE)+$$(($(FIRMWARE_METADATA_SIZE)))" \
		$(LINKFLAGPREFIX)--defsym=length="$(SLOT2_SIZE)-$(FIRMWARE_METADATA_SIZE)" \
		-o $(ELFSLOT2) && \
	$(OBJCOPY) -Obinary $(ELFSLOT2) $(BINSLOT2) && \
	$(GENMETA) $(BINSLOT2) $(APP_VERSION) $(APP_ID) $$(($(SLOT0_ADDR)+$(SLOT0_SIZE)+$(SLOT1_SIZE)+$(FIRMWARE_METADATA_SIZE))) $(SECKEY) $(BINSLOT2).meta && \
	cat $(BINSLOT2).meta $(BINSLOT2) > $(IMAGE_SLOT2) \
	; \

combined: multislot
	$(Q)sh -c 'cat $(BOOTLOADER_BIN) $(IMAGE_SLOT1) > $(COMBINED_BIN)'

.PHONY: flash-multislot verify firmware-tools-clean

ifneq (, $(filter $(BOARD),iotlab-m3))
ifneq (, $(filter flash-multislot, $(MAKECMDGOALS)))
export BINFILE = $(COMBINED_BIN)
FFLAGS = flash-bin
FLASH_ADDR = $(SLOT0_ADDR)
endif
else
flash-multislot: HEXFILE = $(COMBINED_BIN)
endif

flash-multislot: combined $(FLASHDEPS)
	$(FLASHER) $(FFLAGS)

verify: $(FIRMWARE) $(PUBKEY)
	$(Q)$(VERIFY) $(IMAGE_SLOT1) $(PUBKEY)
	$(Q)$(VERIFY) $(IMAGE_SLOT2) $(PUBKEY)

clean-firmware-tools:
	$(Q)env -i PATH=$(PATH) make clean -C $(FIRMWARE_TOOLS)

