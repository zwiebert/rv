#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#
.PHONY: xxx win_copy win_copy_release esp32wlan esp32poe esp32gateway clean

.PHONY: esp32-wlan esp32-lan esp32-flash esp32-flash-ocd esp32-flash-app esp32-flash-app-ocd

default: print-help
clean: esp32-clean

ifdef V
esp32_build_opt_verbose = -v
endif
ifndef BUILD_BASE
esp32_build_cmd := idf.py $(esp32_build_opt_verbose) -C .
esp32_build_dir := ./build
else
esp32_build_cmd := idf.py $(esp32_build_opt_verbose) -C . -B $(BUILD_BASE)
esp32_build_dir := $(BUILD_BASE)
endif
esp32_ocd_sh :=  sh $(realpath ./esp32_ocd.sh)

.PHONY: http_content http_content_lan http_proxy
http_content :
	cd components/http_server_content/webapp && make 
http_content_lan :
	cd components/http_server_content/webapp && make  FLAVOR_LAN=1
http_proxy:
	cd components/http_server_content/webapp && make proxy



esp32_tgts_auto := menuconfig clean fullclean app
define GEN_RULE
.PHONY: esp32-$(1)
esp32-$(1):
	$(esp32_build_cmd) $(1)
endef
$(foreach tgt,$(esp32_tgts_auto),$(eval $(call GEN_RULE,$(tgt))))

esp32-wlan: http_content
	$(esp32_build_cmd) reconfigure all
esp32-lan: http_content_lan
	env FLAVOR_LAN=1 $(esp32_build_cmd) reconfigure all
esp32-cellar:
	make esp32-lan CELLAR=1 FLAVOR_LAN=1
esp32-flash: 
	$(esp32_build_cmd) -p /dev/ttyUSB2 flash




esp32-flash-ocd: 
	(cd $(esp32_build_dir) && $(esp32_ocd_sh) flash)
esp32-flash-app-ocd: 
	(cd $(esp32_build_dir) && $(esp32_ocd_sh) flash_app)
esp32-ocd:
	$(esp32_ocd_sh) server
esp32-ocd-loop:
	$(esp32_ocd_sh) server_loop

IPADDR ?= 192.168.1.79

tcpterm:
	socat -d -d -v pty,link=/tmp/ttyVirtual0,raw,echo=0,unlink-close,waitslave tcp:$(IPADDR):7777,forever,reuseaddr&
	gtkterm -p /tmp/ttyVirtual0 -c tcpterm




win_copy:
	echo "copy files to windows"
	./win_cp.sh

win_copy_release:
	$(MAKE) clean
	$(MAKE) win_copy DISTRIBUTION=1
