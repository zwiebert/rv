THIS_ROOT := $(realpath .)
BUILD_BASE ?= $(THIS_ROOT)/build
SRC_DIRS = src/main


help:
	@$(info targets: stm32f1.fw.menuconfig, .configure, .build, .flash  (Set BUILD_TYPE to Debug,Release,Cellar on commandline))
	@$(info targets: host.fw.configure, .build)
	@$(info targets: doxy-usr-build, doxy-usr-view, doxy-usr-auto)
	@$(info targets: doxy-dev-build, doxy-dev-view, doxy-dev-auto)
	@$(info targets: doxy-api-build, doxy-api-view, doxy-dev-auto)


## create default configuration for use with STCubeMx IDE
.PHONY: config_ide
src/main/rv_config.h config_ide:
	python -m kconfgen  --kconfig $(THIS_ROOT)/src/main/Kconfig --output header src/main/rv_config.h

############# STM32F1/FIRMWARE ####################
# configurations: Debug,Release,Cellar,...
# create a directory in ./configs/$(BUILD_TYPE)
#
BUILD_TYPE ?=Debug
CMAKE_ARGS_GENERATOR ?=#-G Ninja
STM32F1_FW_BUILD_DIR = $(BUILD_BASE)/stm32f1/$(BUILD_TYPE)
rv_config_h:=$(STM32F1_FW_BUILD_DIR)/rv_config.h
config_dir:=$(THIS_ROOT)/configs/$(BUILD_TYPE)
_config:=$(config_dir)/.config

stm32f1.fw.all: stm32f1.fw.configure stm32f1.fw.build

stm32f1.fw.menuconfig $(_config):
	mkdir -p $(config_dir) && cd $(config_dir) && menuconfig $(THIS_ROOT)/Kconfig.projbuild

$(rv_config_h): $(_config)
	 python -m kconfgen  --kconfig $(THIS_ROOT)/Kconfig.projbuild --config $(_config) --output header $(rv_config_h)
	 cp $(rv_config_h) $(THIS_ROOT)/src/main/rv_config.h
external/libopencm3/lib/libopencm3_stm32f1.a:
	make -C ./external/libopencm3 lib/libopencm3_stm32f1.a


stm32f1.fw.configure:
	rm -fr $(STM32F1_FW_BUILD_DIR)
	mkdir -p $(STM32F1_FW_BUILD_DIR)
	make  $(rv_config_h)
	(cmake -S $(realpath .) -B $(STM32F1_FW_BUILD_DIR) -DCMAKE_TOOLCHAIN_FILE=arm-gcc-toolchain.cmake -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)  -DTARGET_ARCH=STM32F1 $(CMAKE_ARGS_GENERATOR))

	
stm32f1.fw.build:
	(cmake --build $(STM32F1_FW_BUILD_DIR) -- bin)
	
stm32f1.fw.flash:
	(cmake --build $(STM32F1_FW_BUILD_DIR) -- flash)

############# HOST/FIRMWARE ####################
HOST_FW_BUILD_DIR=$(BUILD_BASE)/host/xxx

host.fw.configure:
	rm -fr $(HOST_FW_BUILD_DIR)
	mkdir -p $(HOST_FW_BUILD_DIR)
	(cmake -B $(HOST_FW_BUILD_DIR) -DTARGET_ARCH=HOST -S $(realpath .) $(CMAKE_ARGS_GENERATOR))

host.fw.build:
	(cmake --build $(HOST_FW_BUILD_DIR))


############## HOST/TESTS_ONLY ########################
HOST_TESTS_BUILD_DIR=$(BUILD_BASE)/host/test

.PHONY: host.tests.configure host.tests.build

host.tests.configure:
	rm -fr $(HOST_TESTS_BUILD_DIR)
	mkdir -p $(HOST_TESTS_BUILD_DIR)
	(cmake -B $(HOST_TESTS_BUILD_DIR) -DTARGET_ARCH=HOST  -DBUILD_ONLY_TESTS=ON -S $(realpath .) $(CMAKE_ARGS_GENERATOR))


host.tests.build:
	(cmake --build $(HOST_TESTS_BUILD_DIR))

host.tests.ctest: host.tests.build
	(cmake --build $(HOST_TESTS_BUILD_DIR) -- test)
	
host.tests.ctest.current: host.tests.build
	(cd  $(HOST_TESTS_BUILD_DIR) && ctest --verbose -R "fernotron_trx.test_send")


host-test-all:
	make -s --no-print-directory  host.tests.configure host.tests.ctest



############# Doxygen ###################
doxy_flavors=usr dev api
DOXY_BUILD_PATH=$(THIS_ROOT)/build/doxy
DOXYFILE_PATH=$(THIS_ROOT)
include doxygen_rules.mk

$(DOXY_BUILD_PATH)/usr/input_files: $(DOXY_BUILD_PATH)/usr FORCE
	echo "" > $@
	
$(DOXY_BUILD_PATH)/api/input_files: $(DOXY_BUILD_PATH)/api FORCE
	git ls-files '*.h' '*.hh' README.md | sed "s~^~$(THIS_ROOT)/~" > $@
	
$(DOXY_BUILD_PATH)/dev/input_files: $(DOXY_BUILD_PATH)/dev FORCE
	git ls-files '*.h' '*.c' '*.hh' '*.cc' '*.cpp' README.md | sed "s~^~$(THIS_ROOT)/~" > $@



