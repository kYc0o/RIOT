# name of your application
APPLICATION = bootloader

# If no BOARD is found in the environment, use this default:
BOARD ?= iotlab-m3

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
#CFLAGS += -DDEVELHELP

# Mark this example as the bootloader
BOOTLOADER = 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Use fw_slots module to manage images on ROM
USEMODULE += fw_slots
#USEMODULE += fw_requests

# Use ethos (ethernet over serial) for network communication and stdio over
# UART
GNRC_NETIF_NUMOF := 1
USEMODULE += ethos gnrc_netdev

# ethos baudrate can be configured from make command
ETHOS_BAUDRATE ?= 115200
CFLAGS += '-DETHOS_UART=UART_DEV(0)' -DETHOS_BAUDRATE=$(ETHOS_BAUDRATE) -DUSE_ETHOS_FOR_STDIO
FEATURES_REQUIRED += periph_uart

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
#USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_udp
# Add a routing protocol
USEMODULE += gnrc_rpl
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_tftp



#CFLAGS += -DGNRC_PKTBUF_SIZE=512 -DGNRC_IPV6_NC_SIZE=1

include $(RIOTBASE)/Makefile.include