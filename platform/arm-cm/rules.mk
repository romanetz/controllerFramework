TARGET ?= arm-none-eabi-

INCLUDE_PATH += $(FRAMEWORK_DIR)/platform/arm-cm
CSOURCES += $(wildcard $(FRAMEWORK_DIR)/platform/arm-cm/*.c)

CFLAGS += -mno-thumb-interwork -falign-functions=16 -ffunction-sections -fdata-sections -fno-common
LFLAGS += --static -nostartfiles -T$(BUILD_DIR)/script.ld
LFLAGS += -Wl,--gc-sections -lnosys -lm

$(BUILD_DIR)/%.elf: $(BUILD_DIR)/script.ld $(OBJECTS) Makefile
	@echo "Linking $(notdir $@)..."
	$(Q)$(LD) $(CFLAGS) -o $@ $(OBJECTS) $(LFLAGS)

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf Makefile
	@echo "Creating $(notdir $@)..."
	$(Q)$(OBJCOPY) -Obinary $< $@
