
all : mcu_com.pb.c


%.pb: %.proto
	protoc -o$@ $<
%.pb.c: %.pb
	nanopb_generator.py $<
