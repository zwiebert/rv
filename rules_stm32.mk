.PHONY: fetch_libopencm3 build_libopencm3

#  remove prefix stm32- on these targets
stm32_tgts_auto = doxy-dev-build doxy-usr-build doxy-api-build doxy-dev-view doxy-usr-view doxy-api-view doxy-usr-auto doxy-api-auto doxa-dev-auto \
                  test.cm.configure test.cm.build test.cm.ctest  test.cm.ctest.regex

# change prefix from stm32- to stm32f1.fw.  on these targets
stm32_tgts_auto2 = all menuconfig configure build flash


pre_build.stm: fetch_libopencm3 build_libopencm3
build_libopencm3: stm32/rv/external/libopencm3/lib/libopencm3_stm32f1.a
fetch_libopencm3: stm32/rv/external/libopencm3

stm32/rv/external/libopencm3/lib/libopencm3_stm32f1.a:
	make -C stm32/rv/external/libopencm3
stm32/rv/external/libopencm3:
	git submodule update --init --recursive


define GEN_RULE
.PHONY: stm32-$(1)
stm32-$(1):
	make -C stm32/rv  $(1)
endef
$(foreach tgt,$(stm32_tgts_auto),$(eval $(call GEN_RULE,$(tgt))))

define GEN_RULE2
.PHONY: stm32-$(1)
stm32-$(1):
	make -C stm32/rv  stm32f1.fw.$(1)
endef
$(foreach tgt,$(stm32_tgts_auto2),$(eval $(call GEN_RULE2,$(tgt))))


