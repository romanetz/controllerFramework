all: stm32-mcu-info svd2header svd-from-mcu compiler

stm32-mcu-info: stm32-mcu-info.cpp
	g++ -O2 -o stm32-mcu-info stm32-mcu-info.cpp -ljsoncpp

svd-from-mcu: svd-from-mcu.cpp
	g++ -O2 -o svd-from-mcu svd-from-mcu.cpp

svd2header: svd2header.cpp
	g++ -O2 -o svd2header svd2header.cpp -ltinyxml

clean:
	rm -fv stm32-mcu-info svd2header svd-from-mcu

updateMetadata:
	mkdir -p ../metadata
	./update_stm32_catalog
	./update_libopencm3_device_db