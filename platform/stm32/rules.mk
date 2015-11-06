MCU ?= $(PLATFORM)

MCU_INFO = $(shell $(BASE_DIR)/tools/libopencm3-device-info $(MCU) || $(BASE_DIR)/tools/stm32-mcu-info $(MCU))

CFLAGS += $(MCU_INFO)
LFLAGS += -mthumb

SOURCES += $(wildcard $(PLATFORM_DIR)/*.cpp)
HEADERS += $(BUILD_DIR)/stm32defs.h $(wildcard $(PLATFORM_DIR)/*.h)
INCLUDE_PATH += $(BUILD_DIR) $(PLATFORM_DIR)

include $(PLATFORM_DIR)/../cortexm/rules.mk

OUTPUT_FILES = $(BUILD_DIR)/$(PROJECT_NAME).elf $(BUILD_DIR)/$(PROJECT_NAME).bin

all: $(BUILD_DIR) $(OUTPUT_FILES)
	$(SIZE) $(BUILD_DIR)/$(PROJECT_NAME).elf

$(BUILD_DIR)/stm32defs.h:
	$(BASE_DIR)/tools/svd2header `$(BASE_DIR)/tools/svd-from-mcu $(MCU)` $@

$(BUILD_DIR)/script.ld: $(PLATFORM_DIR)/script.ld.S
	$(CC) -P -E $(MCU_INFO) $(PLATFORM_DIR)/script.ld.S > $@

load: $(BUILD_DIR)/$(PROJECT_NAME).bin
	st-flash write $(BUILD_DIR)/$(PROJECT_NAME).bin 0x08000000

