# add prefix esp32- at command line to make them using esp32/netmcu/Makefile without the esp32-prefix
esp32_tgts_auto = doxy-dev-build doxy-usr-build doxy-api-build doxy-dev-view doxy-usr-view doxy-api-view doxy-dev-auto doxy-usr-auto doxy-api-auto \
		  test.cm.configure test.cm.build test.cm.ctest  test.cm.ctest.regex

# add prefix esp32- at command line to make them using esp32/netmcu/Makefile with the esp32-prefix
esp32_tgts_auto2 = menuconfig flash configure all


define GEN_RULE
.PHONY: esp32-$(1)
esp32-$(1):
	make -C esp32/netmcu  $(1)
endef
$(foreach tgt,$(esp32_tgts_auto),$(eval $(call GEN_RULE,$(tgt))))
define GEN_RULE2
.PHONY: esp32-$(1)
esp32-$(1):
	make -C esp32/netmcu  esp32-$(1)
endef
$(foreach tgt,$(esp32_tgts_auto2),$(eval $(call GEN_RULE2,$(tgt))))



