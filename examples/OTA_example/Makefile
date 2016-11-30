%.o: %.c
	gcc -Wall -Wextra -pedantic -std=c99 -c $< -o $@

generate-metadata: generate-metadata.o
	gcc -o $@ generate-metadata.o
	rm -f *.o

.PHONY: bootloader ota-image-example master-hex

bootloader:
	printf "Making RIOT Bootloader\n";
	cd ./default_bootloader; \
		make clean; \
		make; \

ota-image-example: generate-metadata
	rm -f firmware*.hex firmware*.bin;
	printf "\nMaking OTA Example Image\n"
	cd ./gnrc_networking_ota; \
		make clean; \
		make; \
	printf "\nAdding metadata to OTA Example Image\n";
	./generate-metadata gnrc_networking_ota/bin/iotlab-m3/gnrc_networking_ota.bin 0x0 0xabcd1234 0
	srec_cat gnrc_networking_ota/bin/iotlab-m3/gnrc_networking_ota.bin -binary -offset 0x100 firmware-metadata.bin -binary -o ota-image.bin -binary

master-hex: bootloader ota-image-example
	printf "\nMerging OTA Example Image with Bootloader\n"
	srec_cat default_bootloader/bin/iotlab-m3/default_bootloader.hex \
	-intel -crop 0x08000000 0x08004000 \
	ota-image.bin -binary -offset 0x08004000 -crop 0x08004000 0x08040000 -o firmware.hex -intel

flash:
	HEXFILE=firmware.hex OPENOCD_CONFIG=../../boards/iotlab-m3/dist/openocd.cfg ../../dist/tools/openocd/openocd.sh flash
	
term:
	../../dist/tools/pyterm/pyterm  -p "$(firstword $(sort $(wildcard /dev/tty.usbserial*B)))" -b "500000"
