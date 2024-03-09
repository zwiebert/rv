.PHONY: pre_build build fetch_libopencm3 build_libopencm3

help:
	less makefile_help.txt

configure.esp: 
	make -C esp32/netmcu sp32-menuconfig 
	
build.esp: pre_build
	make -C esp32/netmcu esp32-all 


pre_build.stm: fetch_libopencm3 build_libopencm3

build_libopencm3: stm32/rv/external/libopencm3/lib/libopencm3_stm32f1.a
fetch_libopencm3: stm32/rv/external/libopencm3

stm32/rv/external/libopencm3/lib/libopencm3_stm32f1.a:
	make -C stm32/rv/external/libopencm3
stm32/rv/external/libopencm3:
	git submodule update --init --recursive

