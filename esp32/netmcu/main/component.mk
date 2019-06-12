#
# Main component makefile.
#
# This Makefile can be left empty. By default, it will take the sources in the 
# src/ directory, compile them and link them into lib(subdirectory_name).a 
# in the build directory. This behaviour is entirely configurable,
# please read the ESP-IDF documents if you need to do this.
#
COMPONENT_PRIV_INCLUDEDIRS = .
COMPONENT_SRCDIRS+= ./userio ./config ./userio/http_data ./cli ./main ./misc

COMPONENT_EMBED_TXTFILES := howsmyssl_com_root_cert.pem ca_cert.pem

ifdef BOARD_ESP32POE
CFLAGS+=-DBOARD_ESP32POE
else
ifdef BOARD_ESP32GATEWAY
CFLAGS+=-DBOARD_ESP32GATEWAY
else
CFLAGS+=-DBOARD_ESP32WLAN
endif
endif


