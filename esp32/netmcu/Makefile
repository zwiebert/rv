#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#
.PHONY: xxx win_copy win_copy_release esp32wlan esp32poe esp32gateway
xxx :
	$(MAKE) -C main/userio/http_data
	$(MAKE) all
	
esp32poe :
	$(MAKE) -C main/userio/http_data
	$(MAKE) all BOARD_ESP32POE=1

esp32wlan :
	$(MAKE) -C main/userio/http_data
	$(MAKE) all BOARD_ESP32WLAN=1

esp32gateway :
	$(MAKE) -C main/userio/http_data
	$(MAKE) all BOARD_ESP32GATEWAY=1
	
win_copy: xxx
	echo "copy files to windows"
	./win_cp.sh

win_copy_release:
	$(MAKE) clean
	$(MAKE) win_copy DISTRIBUTION=1
	
PROJECT_NAME := netmcu

include $(IDF_PATH)/make/project.mk

