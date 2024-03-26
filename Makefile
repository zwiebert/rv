.PHONY: pre_build build fetch_libopencm3 build_libopencm3

help:
	less makefile_help.txt

all-%: esp32-% stm32-%

include rules_esp32.mk
include rules_stm32.mk



# add prefix esp32- at command line to make them using esp32/netmcu/Makefile without the esp32-prefix
all_tgts_auto = all configure build



define GEN_RULE
.PHONY: all-$(1)
$(1): esp32-$(1) stm32-$(1)
endef
$(foreach tgt,$(all_tgts_auto),$(eval $(call GEN_RULE,$(tgt))))

