proto_dir := ./proto
build_dir := ./build

.PHONY: all

all : build/mcu_com.pb.c build/mcu_com.pb.h build/mcu_com.js

pbgen ?= nanopb_generator.py

%.pb: %.proto
	protoc -o$@ $<
%.pb.c: %.pb
	$(pbgen) $<


$(build_dir)/%.pb: $(proto_dir)/%.proto
	mkdir -p $(build_dir) && protoc -o$@ --proto_path=$(proto_dir) $<
$(build_dir)/%.pb.c $(build_dir)/%.pb.h: $(build_dir)/%.pb
	mkdir -p $(build_dir) && cd $(build_dir) && $(pbgen) $(notdir $<)

$(build_dir)/%.js: $(proto_dir)/%.proto
	protoc  --proto_path=proto --js_out=import_style=closure,library=mcu_com,binary:build  proto/mcu_com.proto
	protoc  --proto_path=proto --js_out=import_style=commonjs,binary:build  proto/mcu_com.proto

