THIS_ROOT := $(realpath .)
BUILD_BASE ?= $(THIS_ROOT)/build
SRC_DIRS = src/main



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
	