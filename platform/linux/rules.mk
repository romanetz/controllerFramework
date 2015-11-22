TARGET ?=

SOURCES += $(wildcard $(PLATFORM_DIR)/*.cpp)
INCLUDE_PATH += $(PLATFORM_DIR)

LFLAGS += -lstdc++ -lpthread

OUTPUT_FILES = $(BUILD_DIR)/$(PROJECT_NAME).elf

$(BUILD_DIR)/%.elf: $(OBJECTS) Makefile
	@echo "Linking $(notdir $@)..."
	$(Q)$(LD) $(CFLAGS) -o $@ $(OBJECTS) $(LFLAGS)

all: $(BUILD_DIR) $(OUTPUT_FILES)
	$(Q)$(SIZE) $(BUILD_DIR)/$(PROJECT_NAME).elf

run: $(BUILD_DIR)/$(PROJECT_NAME).elf
	$(BUILD_DIR)/$(PROJECT_NAME).elf
