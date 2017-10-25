ifdef SLOT2_SIZE
    include $(RIOTMAKE)/boot/multislot.mk
else
    ifdef SLOT1_SIZE
       include $(RIOTMAKE)/boot/singleslot.mk
    endif
endif # SLOT2_SIZE
