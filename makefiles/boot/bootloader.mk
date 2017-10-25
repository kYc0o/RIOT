ifdef SLOT0_SIZE

include $(RIOTMAKE)/boot/variables.mk

create-key: $(MCUBOOT_KEYFILE)

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



bootloader: link
	@$(_LINK) \
		$(LINKFLAGPREFIX)--defsym=length="$(SLOT0_SIZE)" \
		-o $(ELFSLOT0) && \
		$(OBJCOPY) -Obinary $(ELFSLOT0) $(BINSLOT0) && \
		truncate -s $$(($(SLOT0_SIZE))) $(BINSLOT0)


clean-bootloader:
	@env -i PATH=$(PATH) BOARD=$(BOARD) make clean -C $(RIOTBASE)/bootloader

clean-multislot: clean-firmware-tools clean-bootloader

else
mcuboot:
	$(Q)echo "error: mcuboot not supported on board $(BOARD)!"
	$(Q)false

endif # SLOT0_SIZE
