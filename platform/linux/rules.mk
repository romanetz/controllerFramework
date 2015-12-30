TARGET ?=

INCLUDE_PATH += $(PLATFORM_DIR)
CSOURCES += $(wildcard $(PLATFORM_DIR)/*.c)

BINARIES += $(BUILD_DIR)/$(PROJECT_NAME)

LFLAGS += -lpthread

all: $(BUILD_DIR) $(BINARIES)
	@echo "Done"

$(BUILD_DIR)/$(PROJECT_NAME): $(OBJECTS) Makefile
	@echo "Linking $@..."
	$(Q)$(LD) -o $@ $(OBJECTS) $(LFLAGS)