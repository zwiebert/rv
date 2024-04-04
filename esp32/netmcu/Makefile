#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#
.PHONY: xxx win_copy win_copy_release esp32wlan esp32poe esp32gateway clean

.PHONY: esp32-all esp32-flash esp32-flash-ocd esp32-flash-app esp32-flash-app-ocd

flavors = esp32 esp32dbg
flavor ?= esp32
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
BUILD_BASE ?= $(THIS_ROOT)/build/$(flavor)
esp32_src_dir := $(THIS_ROOT)/flavors/$(flavor)
esp32_build_dir := $(BUILD_BASE)
esp32_build_cmd := idf.py -G Ninja -C $(esp32_src_dir) -B $(esp32_build_dir)  -p $(PORT)  $(esp32_build_opts)
esp32_cmake_cmd := cmake -S $(esp32_src_dir) -B $(esp32_build_dir) -G Ninja


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
esp32-cellar: pre_build
	make esp32-all CELLAR=1
esp32-flash: pre_build
	$(esp32_build_cmd) -p $(PORT) flash

############# graphviz #################################
esp32-png: $(esp32_build_dir)/netmcu.png
esp32-dot: $(esp32_build_dir)/netmcu.dot
esp32-graph-view: esp32-png
	xdg-open $(esp32_build_dir)/netmcu.png&

$(esp32_build_dir)/netmcu.dot: FORCE
	$(esp32_cmake_cmd) --graphviz=$(esp32_build_dir)/netmcu.dot
	
	
%.png:%.dot
	dot -Tpng -o $@ $<

FORCE:


############ openocd ###################################
esp32_ocd_sh :=  "$(esp32_src_dir)/esp32_ocd.sh" $(esp32_src_dir) $(esp32_build_dir)

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
	
	
############## On Host ########################
HOST_TEST_BUILD_PATH=$(BUILD_BASE)/../host/test
HOST_TEST_SRC_PATH=$(THIS_ROOT)/flavors/host_test

.PHONY: test.cm.configure test.cm.build

test.cm.configure:
	rm -fr $(HOST_TEST_BUILD_PATH)
	mkdir -p $(HOST_TEST_BUILD_PATH)/config
	cp $(THIS_ROOT)/flavors/host_test/sdkconfig.h $(THIS_ROOT)/flavors/host_test/sdkconfig.cmake $(HOST_TEST_BUILD_PATH)/config/
	-cat $(THIS_ROOT)/flavors/host_test/.local_sdkconfig.h >>$(HOST_TEST_BUILD_PATH)/config/sdkconfig.h #Openweather-URL
	cmake -B $(HOST_TEST_BUILD_PATH) -D BUILD_HOST_TESTS=ON -S  $(HOST_TEST_SRC_PATH) #-G Ninja)

cm_build := make -C $(HOST_TEST_BUILD_PATH) -k -j  -s --no-print-dir $(make_verbose_opts)
#cm_build := (cd $(HOST_TEST_BUILD_PATH) && cmake -G Ninja $(THIS_ROOT) &&  ninja -k 0 --verbose $(ninja_verbose_opts))

	
test.cm.build: pre_build
	$(cm_build)

test.cm.ctest: test.cm.build
	cd  $(HOST_TEST_BUILD_PATH) && ctest --output-on-failure
	
TEST ?= test.weather.test_
test.cm.ctest.regex: test.cm.build
	(cd  $(HOST_TEST_BUILD_PATH) && ctest --verbose  --output-on-failure -R "$(TEST)")


host-test-all:
	make -s --no-print-directory  test.cm.configure test.cm.ctest


############# Doxygen ###################
doxy_flavors=usr dev api
DOXY_BUILD_PATH=$(THIS_ROOT)/build/doxy
DOXYFILE_PATH=$(THIS_ROOT)
include doxygen_rules.mk

$(DOXY_BUILD_PATH)/usr/input_files: $(DOXY_BUILD_PATH)/usr FORCE
	echo "" > $@
	
$(DOXY_BUILD_PATH)/api/input_files: $(DOXY_BUILD_PATH)/api FORCE
	git ls-files '*.h' '*.hh' README.md | fgrep -e include -e README.md | sed "s~^~$(THIS_ROOT)/~" > $@
	cd components-mcu && git ls-files '*.h' '*.hh' | fgrep -e include  | sed "s~^~$(THIS_ROOT)/components-mcu/~" >> $@
	
$(DOXY_BUILD_PATH)/dev/input_files: $(DOXY_BUILD_PATH)/dev FORCE
	git ls-files '*.h' '*.c' '*.hh' '*.cc' '*.cpp' README.md | sed "s~^~$(THIS_ROOT)/~" > $@
	cd components-mcu && git ls-files '*.h' '*.c' '*.hh' '*.cc' '*.cpp' | sed "s~^~$(THIS_ROOT)/components-mcu/~" >> $@


