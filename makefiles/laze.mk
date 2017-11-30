APPDIR_REL:=$(APPDIR:$(RIOTBASE)/%=%)
NINJA_TARGET:=$(APPDIR_REL)/bin/$(BOARD)/$(APPLICATION).elf

ifeq ($(QUIET), 0)
	NINJA_ARGS += -v
endif

ifneq ($(filter $(MAKECMDGOALS),clean),)
	CLEAN=clean
endif

.PHONY: clean all $(RIOTBASE)/.laze_args

export LAZE_WHITELIST=$(BOARD)
export LAZE_APPS=$(APPDIR_REL)

all: $(RIOTBASE)/build.ninja | $(CLEAN)
	ninja -C $(RIOTBASE) $(NINJA_ARGS) $(NINJA_TARGET)

$(RIOTBASE)/build.ninja: $(RIOTBASE)/.laze_args

$(RIOTBASE)/.laze_args:
	@if [ ! -f $(RIOTBASE)/.laze_args \
		-o "$$(cat $(RIOTBASE)/.laze_args)" != "$$(echo "$(LAZE_APPS) $(LAZE_WHITELIST)" | md5sum)" ]; \
		then \
		echo "laze: rebuilding build files" ; \
		echo "$(LAZE_APPS) $(LAZE_WHITELIST)" | md5sum > $@ ; \
		cd $(RIOTBASE) && laze generate project.yml ;\
	fi

clean:
	ninja -C $(RIOTBASE) -t clean $(NINJA_TARGET)

buildtest:
	ninja -C $(RIOTBASE) $(NINJA_ARGS) $(APPDIR_REL)
