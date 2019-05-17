#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#
.PHONY: xxx win_copy
xxx :
	$(MAKE) -C main/userio/http_data
	$(MAKE) all
	
win_copy: xxx
	echo "copy files to windows"
	./win_cp.sh

PROJECT_NAME := netmcu

include $(IDF_PATH)/make/project.mk

