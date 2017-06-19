APPDIR_REL:=$(APPDIR:$(RIOTBASE)/%=%)
NINJA_TARGET:=$(APPDIR_REL)/bin/$(BOARD)/$(APPLICATION).elf

ifeq ($(QUIET), 0)
	NINJA_ARGS += -v
endif

ifneq ($(filter $(MAKECMDGOALS),clean),)
	CLEAN=clean
endif

.PHONY: clean all

all: | $(CLEAN)
	LAZE_WHITELIST=$(BOARD) ninja -C $(RIOTBASE) $(NINJA_ARGS) $(NINJA_TARGET)

clean:
	ninja -C $(RIOTBASE) -t clean $(NINJA_TARGET)

buildtest:
	ninja -C $(RIOTBASE) $(NINJA_ARGS) $(APPDIR_REL)
