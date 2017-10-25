IMGTOOL ?= $(RIOTBASE)/dist/tools/mcuboot/imgtool.py
override IMGTOOL := $(abspath $(IMGTOOL))

BINFILE ?= $(BINDIR)/$(APPLICATION).bin
SIGN_BINFILE = $(BINDIR)/signed-$(APPLICATION).bin
MCUBOOT_KEYFILE ?= $(BINDIR)/key.pem
OFLAGS = -O binary
MCUBOOT_BIN ?= $(BINDIR)/mcuboot.bin
MCUBOOT_BIN_URL ?= http://download.riot-os.org/mynewt.mcuboot.bin
MCUBOOT_BIN_MD5 ?= 0c71a0589bd3709fc2d90f07a0035ce7

ELFSLOT0        := $(ELFFILE:%.elf=%.slot0.elf)
ELFSLOT1        := $(ELFFILE:%.elf=%.slot1.elf)
ELFSLOT2        := $(ELFFILE:%.elf=%.slot2.elf)
BINSLOT0        := $(ELFSLOT0:%.elf=%.bin)
BINSLOT1        := $(ELFSLOT1:%.elf=%.bin)
BINSLOT2        := $(ELFSLOT2:%.elf=%.bin)
IMAGE_SLOT1     := $(ELFFILE:%.elf=%-slot1-$(APP_ID)-$(APP_VERSION).bin)
IMAGE_SLOT2     := $(ELFFILE:%.elf=%-slot2-$(APP_ID)-$(APP_VERSION).bin)
COMBINED_BIN    := $(ELFFILE:%.elf=%.combined.bin)
BOOTLOADER_BIN  := $(RIOTBASE)/bootloader/bin/$(BOARD)/bootloader.slot0.bin

FIRMWARE_METADATA_SIZE ?= 0x100
FIRMWARE_TOOLS = $(RIOTBASE)/dist/tools/firmware
FIRMWARE       = $(FIRMWARE_TOOLS)/bin/firmware
PUBKEY_DIR     = $(RIOTBASE)/sys/include
GENMETA        = $(FIRMWARE) genmeta
GENKEYS        = $(FIRMWARE) genkeys
VERIFY         = $(FIRMWARE) verify

SECKEY ?= $(RIOTBASE)/key.sec
PUBKEY ?= $(RIOTBASE)/key.pub
