#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#
.PHONY: xxx win_copy win_copy_release esp32wlan esp32poe esp32gateway clean

.PHONY: esp32-all esp32-flash esp32-flash-ocd esp32-flash-app esp32-flash-app-ocd

flavors = esp32-release esp32-debug
flavor ?= esp32-release
PORT ?= /dev/ttyUSB1

default: print-help
clean: esp32-clean http_clean


# Add the python binary of python-venv to the path to make idf.py work in Eclipse
# XXX: maybe its better to do this from the shell script which starts Eclipse (which runs export.sh anyway)
export PATH := $(IDF_PYTHON_ENV_PATH)/bin:$(PATH) 



env:
	env | grep IDF

ifdef V
esp32_build_opt_verbose = -v
endif

THIS_ROOT := $(realpath .)

CMAKE_SRC_PATH :=$(THIS_ROOT)/configs/$(flavor)
BUILD_BASE ?=$(THIS_ROOT)/build/$(flavor)
BUILD_PATH :=$(BUILD_BASE)

compile_commands_json_latest :=$(THIS_ROOT)/build/compile_commands.json
sdkconfig_h_latest :=$(THIS_ROOT)/build/sdkconfig.h

esp32_cmake_generator := -G Ninja
esp32_build_args :=$(esp32_cmake_generator) -C $(CMAKE_SRC_PATH) -B $(BUILD_PATH)  -p $(PORT)  $(esp32_build_opts)
esp32_build_cmd :=idf.py $(esp32_build_args)
esp32_cmake_cmd :=/usr/bin/cmake -S $(CMAKE_SRC_PATH) -B $(BUILD_PATH) $(esp32_cmake_generator)


$(compile_commands_json_latest) $(sdkconfig_h_latest): FORCE
	cp $(BUILD_BASE)/compile_commands.json $(compile_commands_json_latest)
	cp $(BUILD_BASE)/config/sdkconfig.h $(sdkconfig_h_latest)


#need bash for "source" command
SHELL := /bin/bash
#provide IDF_PATH if not in env
export IDF_PATH ?= "$(HOME)/esp/esp-idf"
.PHONY: idf_make
#Rule for use in vscode, which does not inherit the idf exports
#so we source $IDF_PAtH/export.sh for each run of make
#
#usage: make idf_target my_target=<target>
#example: make idf_target my_target=esp32-all  (instead of make esp32-all)
#
idf_make:
	(source "$(IDF_PATH)/export.sh" && make $(my_target))

.PHONY: http_proxy http_clean
http_proxy:
	cd components/webapp && make proxy
http_clean:
	cd components/webapp && make clean


.PHONY: pre_build generate_protobuf

../../mcu_com/build/mcu_com.pb.h ../../mcu_com/build/mcu_com.pb.c:
	make -C ../../mcu_com pbgen=$(THIS_ROOT)/components-mcu/nanopb/nanopb/generator/nanopb_generator.py
	
generate_protobuf: ../../mcu_com/build/mcu_com.pb.h ../../mcu_com/build/mcu_com.pb.c

pre_build: generate_protobuf


esp32_tgts_auto := menuconfig clean fullclean app monitor gdb gdbgui gdbtui
define GEN_RULE
.PHONY: esp32-$(1)
esp32-$(1): pre_build
	$(esp32_build_cmd) $(1)
endef
$(foreach tgt,$(esp32_tgts_auto),$(eval $(call GEN_RULE,$(tgt))))

esp32-all: pre_build
	$(esp32_build_cmd) reconfigure all
	make $(compile_commands_json_latest)
esp32-cellar: pre_build
	make esp32-all CELLAR=1
	make $(compile_commands_json_latest)
esp32-flash: pre_build
	$(esp32_build_cmd) -p $(PORT) flash
	make $(compile_commands_json_latest)

############# graphviz #################################
esp32-png: $(BUILD_PATH)/netmcu.png
esp32-dot: $(BUILD_PATH)/netmcu.dot
esp32-graph-view: esp32-png
	xdg-open $(BUILD_PATH)/netmcu.png&

$(BUILD_PATH)/netmcu.dot: FORCE
	$(esp32_cmake_cmd) --graphviz=$(BUILD_PATH)/netmcu.dot
	
	
%.png:%.dot
	dot -Tpng -o $@ $<

FORCE:


############ openocd ###################################
esp32_ocd_sh :=  "$(CMAKE_SRC_PATH)/esp32_ocd.sh" $(CMAKE_SRC_PATH) $(BUILD_PATH)

esp32-flash-ocd:
	$(esp32_ocd_sh) flash
esp32-flash-app-ocd:
	$(esp32_ocd_sh) flash_app
esp32-ocd:
	$(esp32_ocd_sh)  server
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
	
############## On Host Tests ##############
kconfigs=components/app_config/Kconfig.projbuild external/*/Kconfig
TEST ?= test.weather.test_

include ./host_test_rules.mk
	

############# Doxygen ###################
doxy_flavors=usr dev api
DOXY_BUILD_PATH=$(THIS_ROOT)/build/doxy
DOXYFILE_PATH=$(THIS_ROOT)
include doxygen_rules.mk

ext=. external/*

$(DOXY_BUILD_PATH)/api/input_files: $(DOXY_BUILD_PATH)/api FORCE
	-rm $@
	for dir in $(ext) ; do \
		(cd $${dir} && git ls-files README.md 'components/**.h' 'components/**.hh'  | xargs realpath) | fgrep include  >> $@ ; \
	done

$(DOXY_BUILD_PATH)/dev/input_files: $(DOXY_BUILD_PATH)/dev FORCE
	-rm $@
	for dir in $(ext) ; do \
		(cd $${dir} && git ls-files README.md 'components/**.h' 'components/**.hh' 'components/**.c' 'components/**.cc' 'components/**.cpp' | xargs realpath) >> $@ ; \
	done


$(DOXY_BUILD_PATH)/usr/input_files: $(DOXY_BUILD_PATH)/usr FORCE
	echo "" > $@
	


