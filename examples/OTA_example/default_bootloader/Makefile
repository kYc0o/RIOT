# name of your application
APPLICATION = default_bootloader

# If no BOARD is found in the environment, use this default:
BOARD = iotlab-m3

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Require access to write internal flash
FEATURES_REQUIRED = periph_flashpage

# Mark this example as the bootloader
BOOTLOADER = 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Modules to include:
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# Use FOTA module to manage images on ROM
USEMODULE += fota

include $(RIOTBASE)/Makefile.include
