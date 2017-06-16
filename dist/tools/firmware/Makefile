RIOTBASE := ../../..
RIOT_INCLUDE = $(RIOTBASE)/sys/include
SHA256_DIR := $(RIOTBASE)/sys/hashes
SHA256_INCLUDE := $(RIOT_INCLUDE)/hashes

COMMON_SRC := common.c
COMMON_HDR := common.h

TWEETNACL_SRC := tweetnacl.c randombytes.c
TWEETNACL_HDR := tweetnacl.h

RIOT_FIRMWARE_SRC := \
	$(SHA256_DIR)/sha256.c \
	$(RIOTBASE)/sys/checksum/fletcher32.c \
	$(RIOTBASE)/sys/firmware/firmware.c

RIOT_FIRMWARE_HDR := $(RIOT_INCLUDE)/firmware.h \
	$(RIOT_INCLUDE)/hashes/sha256.h \
	$(RIOT_INCLUDE)/checksum/fletcher32.h

FIRMWARE_SRC := $(COMMON_SRC) $(TWEETNACL_SRC) $(RIOT_FIRMWARE_SRC) firmware.c verify.c genkeys.c genmeta.c
FIRMWARE_HDR := $(COMMON_HDR) $(TWEETNACL_HDR) $(RIOT_FIRMWARE_HDR)

CFLAGS += -g -I. -O3 -Wall -Wextra -pedantic -std=c99

all: bin/firmware

bin/:
	mkdir -p bin

bin/firmware: $(FIRMWARE_HDR) $(FIRMWARE_SRC) Makefile | bin/
	$(CC) $(CFLAGS) -I$(RIOT_INCLUDE) $(FIRMWARE_SRC) -o $@

clean:
	rm -rf bin/firmware
