THIS_ROOT := $(realpath .)
BUILD_BASE ?= $(THIS_ROOT)/build
SRC_DIRS = src/main


help:
	@$(info targets: stm32f1.fw.configure, .build, .flash  (Set BUILD_TYPE to Debug,Release,Cellar on commandline))
	@$(info targets: host.fw.configure, .build)
	@$(info targets: doxy-usr-build, doxy-usr-view)
	@$(info targets: doxy-dev-build, doxy-dev-view)
	@$(info targets: doxy-api-build, doxy-api-view)
	
############# STM32F1/FIRMWARE ####################
BUILD_TYPE ?= Debug # Debug,Release,Cellar

STM32F1_FW_BUILD_DIR = $(BUILD_BASE)/stm32f1/$(BUILD_TYPE)

stm32f1.fw.configure:
	rm -fr $(STM32F1_FW_BUILD_DIR)
	mkdir -p $(STM32F1_FW_BUILD_DIR)
	env PATH=.:/usr/bin:$$PATH cmake -S $(realpath .) -B $(STM32F1_FW_BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)  -DTARGET_ARCH=STM32F1  #-G Ninja
	
stm32f1.fw.build.make := make -C $(STM32F1_FW_BUILD_DIR) -k  $(make_verbose_opts)
stm32f1.fw.build.ninja := (cd $(STM32F1_FW_BUILD_DIR) && cmake -G Ninja $(THIS_ROOT) &&  ninja -k 0 --extra-verbose $(ninja_verbose_opts))
	
stm32f1.fw.build:
	$(stm32f1.fw.build.make) -- bin
	
stm32f1.fw.flash:
	$(stm32f1.fw.build.make) -- flash


############# HOST/FIRMWARE ####################
HOST_FW_BUILD_DIR=$(BUILD_BASE)/host/xxx

host.fw.configure:
	rm -fr $(HOST_FW_BUILD_DIR)
	mkdir -p $(HOST_FW_BUILD_DIR)
	cmake -B $(HOST_FW_BUILD_DIR) -DTARGET_ARCH=HOST -S $(realpath .) #-G Ninja
	
host.fw.build.make := make -C $(HOST_FW_BUILD_DIR) -k  $(make_verbose_opts)
host.fw.build.ninja := (cd $(HOST_FW_BUILD_DIR) && cmake -G Ninja $(THIS_ROOT) &&  ninja -k 0 --extra-verbose $(ninja_verbose_opts))

	
host.fw.build:
	$(host.fw.build.make) 


############## HOST/TESTS_ONLY ########################
HOST_TESTS_BUILD_DIR=$(BUILD_BASE)/host/test

.PHONY: host.tests.configure host.tests.build

host.tests.configure:
	rm -fr $(HOST_TESTS_BUILD_DIR)
	mkdir -p $(HOST_TESTS_BUILD_DIR)
	cmake -B $(HOST_TESTS_BUILD_DIR) -DTARGET_ARCH=HOST  -DBUILD_ONLY_TESTS=ON -S $(realpath .) #-G Ninja

host.tests.build.make := make -C $(HOST_TESTS_BUILD_DIR) -k  $(make_verbose_opts)
host.tests.build.ninja := (cd $(HOST_TESTS_BUILD_DIR) && cmake -G Ninja $(THIS_ROOT) &&  ninja -k 0 --extra-verbose $(ninja_verbose_opts))
host.tests.build = $(host.tests.build.make)

host.tests.build:
	$(host.tests.build) 

host.tests.ctest: host.tests.build
	$(host.tests.build) test
	
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



