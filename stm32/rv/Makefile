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


	