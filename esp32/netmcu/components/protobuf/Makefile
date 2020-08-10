proto_dir := ../../../../mcu_com

all : build/mcu_com.pb.c


build/%.pb: $(proto_dir)/%.proto
	mkdir -p build && protoc -o$@ --proto_path=$(proto_dir) $<
build/%.pb.c: build/%.pb
	cd build && nanopb_generator.py $(notdir $<)
	mkdir -p include/protobuf && cp build/mcu_com.pb.h include/protobuf
