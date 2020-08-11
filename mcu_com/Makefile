proto_dir := ./proto
build_dir := ./build

.PHONY: all

all : build/mcu_com.pb.c


%.pb: %.proto
	protoc -o$@ $<
%.pb.c: %.pb
	nanopb_generator.py $<


$(build_dir)/%.pb: $(proto_dir)/%.proto
	mkdir -p $(build_dir) && protoc -o$@ --proto_path=$(proto_dir) $<
$(build_dir)/%.pb.c: $(build_dir)/%.pb
	mkdir -p $(build_dir) && cd $(build_dir) && nanopb_generator.py $(notdir $<)
