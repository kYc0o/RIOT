RIOTBASE := ../../..
RIOT_INCLUDE := $(RIOTBASE)/sys/include
RIOT_NATIVE := $(RIOTBASE)/cpu/native
RIOT_CORE_INCLUDE := $(RIOTBASE)/core/include
CLANG_COMPAT_INCLUDE := $(RIOT_NATIVE)/include
CRC_CCITT_INCLUDE := $(RIOT_INCLUDE)/checksum
CRC_CCITT_SRC := $(RIOTBASE)/sys/checksum/crc16_ccitt.c
ADDR_TO_STR_SRC := $(RIOTBASE)/sys/net/network_layer/ipv6/addr/ipv6_addr_to_str.c
FW_REQUESTS_SRC := $(RIOTBASE)/sys/fw_requests/fw_requests.c
FW_SERVER_SRC := server.c $(FW_REQUESTS_SRC) $(CRC_CCITT_SRC)  $(ADDR_TO_STR_SRC)
FW_SERVER_HDR := $(RIOT_INCLUDE)/fw_requests.h $(RIOT_CORE_INCLUDE)/clist.h

CFLAGS += -g3 -O3 -Wall -Wextra -pedantic -std=c99

all: bin bin/server

bin:
	mkdir bin

bin/server: $(FW_SERVER_SRC) $(FW_SERVER_HDR)
	$(CC) $(CFLAGS) -I$(RIOT_INCLUDE) -I$(RIOT_CORE_INCLUDE) \
	-I$(CLANG_COMPAT_INCLUDE)  $(FW_SERVER_SRC) -o $@

clean:
	rm -rf bin/server
