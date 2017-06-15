ifdef SLOT0_SIZE

IMGTOOL ?= $(RIOTBASE)/dist/tools/mcuboot/imgtool.py
override IMGTOOL := $(abspath $(IMGTOOL))

BINFILE ?= $(BINDIR)/$(APPLICATION).bin
SIGN_BINFILE = $(BINDIR)/signed-$(APPLICATION).bin
MCUBOOT_KEYFILE ?= $(BINDIR)/key.pem
OFLAGS = -O binary
MCUBOOT_BIN ?= $(BINDIR)/mcuboot.bin
MCUBOOT_BIN_URL ?= http://download.riot-os.org/mynewt.mcuboot.bin
MCUBOOT_BIN_MD5 ?= 0c71a0589bd3709fc2d90f07a0035ce7

create-key: $(MCUBOOT_KEYFILE)

ifeq ($(BINDIR)/key.pem,$(MCUBOOT_KEYFILE))
$(MCUBOOT_KEYFILE):
	$(Q)mkdir -p $(BINDIR)
	$(Q)$(IMGTOOL) keygen -k $@ -t rsa-2048
endif

mcuboot: create-key link
	@$(COLOR_ECHO)
	@$(COLOR_ECHO) '${COLOR_PURPLE}Re-linking for MCUBoot at $(SLOT0_SIZE)...${COLOR_RESET}'
	@$(COLOR_ECHO)
	$(Q)$(_LINK) $(LINKFLAGPREFIX)--defsym=offset="$$(($(SLOT0_SIZE) + $(IMAGE_HDR_SIZE)))" \
	$(LINKFLAGPREFIX)--defsym=length="$$(($(SLOT1_SIZE) - $(IMAGE_HDR_SIZE)))" \
	$(LINKFLAGPREFIX)--defsym=image_header="$(IMAGE_HDR_SIZE)" -o $(ELFFILE) && \
	$(OBJCOPY) $(OFLAGS) $(ELFFILE) $(BINFILE) && \
	$(IMGTOOL) sign --key $(MCUBOOT_KEYFILE) --version $(IMAGE_VERSION) --align \
	$(IMAGE_ALIGN) -H $(IMAGE_HDR_SIZE) $(BINFILE) $(SIGN_BINFILE)
	@$(COLOR_ECHO)
	@$(COLOR_ECHO) '${COLOR_PURPLE}Signed with $(MCUBOOT_KEYFILE) for version $(IMAGE_VERSION)\
	${COLOR_RESET}'
	@$(COLOR_ECHO)

$(MCUBOOT_BIN):
	$(Q)$(DLCACHE) $(MCUBOOT_BIN_URL) $(MCUBOOT_BIN_MD5) $@

.PHONY: flash-bootloader flash-mcuboot

flash-bootloader: HEXFILE = $(MCUBOOT_BIN)
flash-bootloader: $(MCUBOOT_BIN) $(FLASHDEPS)
	FLASH_ADDR=0x0 $(FLASHER) $(FFLAGS)

flash-mcuboot: HEXFILE = $(SIGN_BINFILE)
flash-mcuboot: mcuboot $(FLASHDEPS) flash-bootloader
	FLASH_ADDR=$(SLOT0_SIZE) $(FLASHER) $(FFLAGS)

else
mcuboot:
	$(Q)echo "error: mcuboot not supported on board $(BOARD)!"
	$(Q)false

ELFSLOT0 := $(ELFFILE:%.elf=%.slot0.elf)
ELFSLOT1 := $(ELFFILE:%.elf=%.slot1.elf)
ELFSLOT2 := $(ELFFILE:%.elf=%.slot2.elf)

BINSLOT0 := $(ELFSLOT0:%.elf=%.bin)
BINSLOT1 := $(ELFSLOT1:%.elf=%.bin)
BINSLOT2 := $(ELFSLOT2:%.elf=%.bin)

COMBINED_BIN := $(ELFFILE:%.elf=%.combined.bin)

FIRMWARE_METADATA_SIZE = 256

APP_ID ?= 0
APP_VERSION ?= 0

ifneq (, $(filter $(USEMODULE),bootloader))
$(info BOOTLOADER BUILD)
BOOTLOADER = 1
else
$(info NON BOOTLOADER BUILD)
BOOTLOADER = 0
BOOTLOADER_BIN = $(RIOTBASE)/examples/bootloader/bin/$(BOARD)/bootloader.slot0.bin
$(BOOTLOADER_BIN):
	@env -i PATH=$(PATH) BOARD=$(BOARD) make -C $(RIOTBASE)/examples/bootloader clean multislot
endif

GENMETA = $(RIOTBASE)/dist/tools/firmware/bin/firmware
SECKEY ?= $(RIOTBASE)/key.sec
PUBKEY ?= $(RIOTBASE)/key.pub

$(SECKEY) $(PUBKEY):
	$(GENMETA) genkeys $(SECKEY) $(PUBKEY)

multislot: all $(BOOTLOADER_BIN) $(SECKEY) $(PUBKEY)
	$(Q)[ $(BOOTLOADER) -ne 1 ] || { \
		$(_LINK) $(LINKFLAGPREFIX)--defsym=offset=0x0 -Wl,--defsym=length=$(SLOT0_SIZE) -o $(ELFSLOT0) ; \
		$(OBJCOPY) -Obinary $(ELFSLOT0) $(ELFSLOT0:%.elf=%.bin) ; \
		truncate $(ELFSLOT0:%.elf=%.bin) --size=$$(($(SLOT0_SIZE))) ; \
	}

	$(Q)[ $(BOOTLOADER) -ne 0 ] || { \
		$(_LINK) \
			$(LINKFLAGPREFIX)--defsym=offset="$(SLOT0_SIZE)+$(FIRMWARE_METADATA_SIZE)" \
			$(LINKFLAGPREFIX)--defsym=length="$(SLOT1_SIZE)-$(FIRMWARE_METADATA_SIZE)" \
			-o $(ELFSLOT1) && \
		$(OBJCOPY) -Obinary $(ELFSLOT1) $(BINSLOT1) && \
		$(GENMETA) genmeta $(BINSLOT1) $(APP_VERSION) $(APP_ID) $$(($(SLOT0_SIZE)+$(FIRMWARE_METADATA_SIZE))) $(SECKEY) $(BINSLOT1).meta && \
		cat $(BINSLOT1).meta $(BINSLOT1) > $(BINSLOT1).img && \
		\
		$(_LINK) \
			$(LINKFLAGPREFIX)--defsym=offset="$(SLOT0_SIZE)+$(SLOT1_SIZE)+$$(($(FIRMWARE_METADATA_SIZE)*2))" \
			$(LINKFLAGPREFIX)--defsym=length="$(SLOT2_SIZE)-$(FIRMWARE_METADATA_SIZE)" \
			-o $(ELFSLOT2) && \
		$(OBJCOPY) -Obinary $(ELFSLOT2) $(BINSLOT2) && \
		$(GENMETA) genmeta $(BINSLOT2) $(APP_VERSION) $(APP_ID) $$(($(SLOT0_SIZE)+$(SLOT1_SIZE)+$(FIRMWARE_METADATA_SIZE)*2)) $(SECKEY) $(BINSLOT2).meta && \
		cat $(BINSLOT2).meta $(BINSLOT2) > $(BINSLOT2).img \
		; \
	}

combined: multislot
	sh -c 'cat $(BOOTLOADER_BIN) $(BINSLOT1).img > $(COMBINED_BIN)'

.PHONY: flash-multislot
flash-multislot: HEXFILE = $(COMBINED_BIN)
flash-multislot: combined $(FLASHDEPS)
	$(FLASHER) $(FFLAGS)

endif # SLOT0_SIZE
