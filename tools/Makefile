all: svd-from-mcu svd2header stm32-mcu-info

svd-from-mcu: svd-from-mcu.cpp
	g++ -O2 -o svd-from-mcu svd-from-mcu.cpp

svd2header: svd2header.cpp
	g++ -O2 -o svd2header svd2header.cpp -ltinyxml

stm32-mcu-info: stm32-mcu-info.cpp
	g++ -O2 -o stm32-mcu-info stm32-mcu-info.cpp -ljsoncpp

clean:
	rm -fv svd-from-mcu svd2header stm32-mcu-info
