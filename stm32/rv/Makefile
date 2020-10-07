THIS_ROOT := $(realpath .)
BUILD_BASE ?= $(THIS_ROOT)/build
SRC_DIRS = src/main


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



############# Doxygen ###################
DOXY_BUILD_PATH=$(THIS_ROOT)/build/doxy

DOXY_INPUT_FILE =  /tmp/doxy_input_file.txt
DOXY_CONFIG_FILES = ./Doxyfile $(DOXY_INPUT_FILE)
DOXY_CONFIG_FILE = /tmp/Doxyfile

.PHONY: doxy doxy.cm.configure doxy.cm.build

$(DOXY_CONFIG_FILE): $(DOXY_INPUT_FILE)
	cat $(DOXY_CONFIG_FILES) >$(DOXY_CONFIG_FILE)

doxy-build: $(DOXY_CONFIG_FILE)
	mkdir -p $(DOXY_BUILD_PATH)
	doxygen $(DOXY_CONFIG_FILE)
	
doxy-view: doxy-build
	xdg-open $(DOXY_BUILD_PATH)/html/index.html
	
$(DOXY_INPUT_FILE):
	find $(SRC_DIRS) '(' -name '*.h' -o -name '*.hh' -o -name '*.c' -o -name '*.cc' -o -name '*.hpp' -o -name '*.cpp' ')' -printf 'INPUT += %p\n' > $@
	

############# Doxygen ###################
DOXY_BUILD_PATH=$(THIS_ROOT)/build/doxy

.PHONY: FORCE
.PHONY: doxy-usr-view  doxy-dev-view  doxy-api-view
.PHONY: doxy-usr-build doxy-dev-build doxy-api-build 

$(DOXY_BUILD_PATH)/%/html/index.html: /tmp/doxy_%_file 
	mkdir -p $(dir $@)
	doxygen $^
	

doxy-usr-build: $(DOXY_BUILD_PATH)/usr/html/index.html FORCE
doxy-usr-view: doxy-usr-build
	xdg-open $(DOXY_BUILD_PATH)/usr/html/index.html &

doxy-dev-build: $(DOXY_BUILD_PATH)/dev/html/index.html FORCE
doxy-dev-view: doxy-dev-build
	xdg-open $(DOXY_BUILD_PATH)/dev/html/index.html &
	
doxy-api-build: $(DOXY_BUILD_PATH)/api/html/index.html FORCE
doxy-api-view: doxy-api-build
	xdg-open $(DOXY_BUILD_PATH)/api/html/index.html &



/tmp/doxy_%_file: ./Doxyfile_% /tmp/doxy_input_%
	cat $^ > $@

/tmp/doxy_input_dev: FORCE
	git ls-files '*.h' '*.c' '*.hh' '*.cc' '*.cpp' | fgrep -v -e CMSIS -e STM32F10x | sed "s~^~INPUT += $(THIS_ROOT)/~" > $@
/tmp/doxy_input_api: FORCE
	git ls-files '*.h' '*.hh' | fgrep -v -e CMSIS -e STM32F10x | sed "s~^~INPUT += $(THIS_ROOT)/~" > $@

/tmp/doxy_input_usr: FORCE
	echo "" > $@
	
doxy-%-view: doxy-%-build FORCE
	xdg-open $(DOXY_BUILD_PATH)/$*/html/index.html
	
	
FORCE:
	