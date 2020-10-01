#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#
.PHONY: xxx win_copy win_copy_release esp32wlan esp32poe esp32gateway clean

.PHONY: esp32-wlan esp32-lan esp32-flash esp32-flash-ocd esp32-flash-app esp32-flash-app-ocd

default: print-help
clean: esp32-clean http_clean


THIS_ROOT := $(realpath .)
BUILD_BASE ?= $(THIS_ROOT)/build


ifdef V
esp32_build_opt_verbose = -v
endif

esp32_src_dir := $(THIS_ROOT)
esp32_build_dir := $(BUILD_BASE)/esp32
esp32_cmake_cmd := cmake -S $(esp32_src_dir) -B $(esp32_build_dir) -G Ninja
esp32_build_cmd := idf.py $(esp32_build_opt_verbose) -C . -B $(esp32_build_dir) -p /dev/ttyUSB0
esp32_ocd_sh :=  sh $(realpath ./esp32_ocd.sh)

.PHONY: http_proxy http_clean
http_proxy:
	cd components/webapp && make proxy
http_clean:
	cd components/webapp && make clean



esp32_tgts_auto := menuconfig clean fullclean app monitor
define GEN_RULE
.PHONY: esp32-$(1)
esp32-$(1):
	$(esp32_build_cmd) $(1)
endef
$(foreach tgt,$(esp32_tgts_auto),$(eval $(call GEN_RULE,$(tgt))))

esp32-wlan:
	$(esp32_build_cmd) reconfigure all
esp32-lan:
	env FLAVOR_LAN=1 $(esp32_build_cmd) reconfigure all
esp32-cellar:
	make esp32-lan CELLAR=1 FLAVOR_LAN=1
esp32-flash: 
	$(esp32_build_cmd) -p /dev/ttyUSB2 flash

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
	
	
	############## On Host ########################
HOST_TEST_BUILD_PATH=$(BUILD_BASE)/host/test

.PHONY: test.cm.configure test.cm.build

test.cm.configure:
	rm -fr $(HOST_TEST_BUILD_PATH)
	mkdir -p $(HOST_TEST_BUILD_PATH)
	cmake -B $(HOST_TEST_BUILD_PATH) -D BUILD_HOST_TESTS=ON -S $(realpath .) #-G Ninja

cm_build := make -C $(HOST_TEST_BUILD_PATH) -k  $(make_verbose_opts)
#cm_build := (cd $(HOST_TEST_BUILD_PATH) && cmake -G Ninja $(THIS_ROOT) &&  ninja -k 0 --extra-verbose $(ninja_verbose_opts))

	
test.cm.build:
	$(cm_build)

test.cm.ctest: test.cm.build
	$(cm_build) test
	
test.cm.ctest.current: test.cm.build
	(cd  $(HOST_TEST_BUILD_PATH) && ctest --verbose -R "fernotron_trx.test_send")


host-test-all:
	make -s --no-print-directory  test.cm.configure test.cm.ctest

	
