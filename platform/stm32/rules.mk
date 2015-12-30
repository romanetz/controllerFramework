INCLUDE_PATH += $(PLATFORM_DIR)
CSOURCES += $(wildcard $(PLATFORM_DIR)/*.c)
HEADERS += $(BUILD_DIR)/include/stm32defs.h

MCU ?= $(PLATFORM)
MCU_INFO = $(shell $(FRAMEWORK_DIR)/tools/libopencm3-device-info $(MCU))

CFLAGS += $(MCU_INFO)
LFLAGS += -mthumb

STM32_BOOT_SERIALPORT ?= /dev/ttyUSB0

BINARIES = $(BUILD_DIR)/$(PROJECT_NAME).elf $(BUILD_DIR)/$(PROJECT_NAME).bin

include $(PLATFORM_DIR)/../arm-cm/rules.mk 

all: $(BUILD_DIR) $(BINARIES)
	$(Q)$(SIZE) $(BUILD_DIR)/$(PROJECT_NAME).elf

$(BUILD_DIR)/script.ld: $(PLATFORM_DIR)/script.ld.S Makefile
	@echo "Creating $(notdir $@)..."
	$(Q)$(CC) -P -E $(MCU_INFO) $(PLATFORM_DIR)/script.ld.S > $@

$(BUILD_DIR)/include/stm32defs.h: Makefile
	@echo "Creating $(notdir $@)..."
	$(Q)$(FRAMEWORK_DIR)/tools/svd2header `$(FRAMEWORK_DIR)/tools/svd-from-mcu $(MCU)` $@

load: $(BUILD_DIR)/$(PROJECT_NAME).bin
	st-flash write $(BUILD_DIR)/$(PROJECT_NAME).bin 0x08000000

boot: $(BUILD_DIR)/$(PROJECT_NAME).bin
	stm32flash -w $(BUILD_DIR)/$(PROJECT_NAME).bin $(STM32_BOOT_SERIALPORT)
