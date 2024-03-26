# rules related to esp32/netmcu


# Targets to forward to sub-directory esp32/netmcu
#
# remove prefix esp32- on these targets
esp32_tgts_auto = doxy-dev-build doxy-usr-build doxy-api-build doxy-dev-view doxy-usr-view doxy-api-view doxy-dev-auto doxy-usr-auto doxy-api-auto \
		  test.cm.configure test.cm.build test.cm.ctest  test.cm.ctest.regex
#
# keep prefix esp32- on these targets
esp32_tgts_auto2 = menuconfig flash configure all


# expand the forwarding rules
#
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



