# Introduction

This example intends to explain usage of the riotboot OTA update scheme using
CoAP.

# Quickstart

- run "make" in "dist/tools/firmware"

- run "bin/firmware genkeys sec.key pub.key" in "dist/tools/firmware" to create
  keypair

- flash image and bootloader

    $ BOARD=samr21-xpro APP_VER=$(date +%s) make -j4 riotboot/flash

- recompile using

    $ BOARD=samr21-xpro APP_VER=$(date +%s) make -j4 clean riotboot

- send upate via coap, e.g.,

    $ coap-cli -m put coap://[<node-ip-address]/firmware -b 64 -f bin/samr21-xpro/ota_example-slot2.bin

## Testing locally using ethos

One possibility to easily test the OTA scheme is using the serial interface
with the ethos (Ethernet Over Serial) driver to deliver the update.
Ethos support has to be added to the application in the application 
Makefile:

```
GNRC_NETIF_NUMOF := 2
USEMODULE += ethos

# ethos baudrate can be configured from make command
ETHOS_BAUDRATE ?= 115200
CFLAGS += -DETHOS_BAUDRATE=$(ETHOS_BAUDRATE) -DUSE_ETHOS_FOR_STDIO
```

The ethos and UHCP tools have to be then built.
First ethos in `/dist/tools/ethos` using:

    $ make clean all

And then UHCP in `/dist/tools/uhcpd` using:

    $ make clean all

Once the OTA application is built and flashed the networking stack can be
setup using `start_network.sh` from the `/dist/tools/ethos` directory, for
example:

    $ sudo sh start_network.sh /dev/ttyACM0 tap0 2001:db8::/64

The board will now be reachable and can be for example ping-ed using:

    $ ping6 <node-ip-address>%tap0

The node ip address is printed at application startup, for example:

```
Waiting for address autoconfiguration...
Configured network interfaces:
Iface  5  HWaddr: 02:05:d6:16:21:31 
          MTU:1500  HL:64  Source address length: 6
          Link type: wired
          inet6 addr: fe80::5:d6ff:fe16:2131  scope: local  VAL
          inet6 group: ff02::1
          inet6 group: ff02::1:ff16:2131
```

In this case the address to be used is `fe80::5:d6ff:fe16:2131`

Furthermore a new update can be now pushed using coap-client, for example:

    $ coap-client -m put coap://[<node-ip-address>%tap0]/firmware -b 16 -f bin/saml21-xpro/ota_example-slot2.signed.bin

Warning: it is suggested to use rather small COAP packet size (ie. <= 16 or 32)
as the serial buffer may otherwise get full and packet lost depending on the
board used.
