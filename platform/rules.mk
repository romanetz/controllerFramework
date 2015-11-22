PLATFORM_NAME = $(strip $(foreach PLATFORM_NAME, $(wildcard $(BASE_DIR)/platform/*), $(findstring $(notdir $(PLATFORM_NAME)), $(PLATFORM))))
PLATFORM_DIR = $(BASE_DIR)/platform/$(PLATFORM_NAME)

BUILD_DIR ?= $(PROJECT_DIR)/build

CC = $(TARGET)gcc
LD = $(TARGET)gcc
SIZE = $(TARGET)size
OBJCOPY = $(TARGET)objcopy
Q ?= @

SOURCES += $(wildcard $(BASE_DIR)/platform/common/*.cpp)
HEADERS += $(BUILD_DIR)/platform.h
INCLUDE_PATH += $(BASE_DIR)/platform/common $(BUILD_DIR)

CFLAGS += -fno-exceptions -fno-rtti -std=c++11
CFLAGS += $(foreach PATH, $(INCLUDE_PATH), -I$(PATH))

OBJECTS += $(addprefix $(BUILD_DIR)/obj/, $(notdir $(SOURCES:.cpp=.o)))

VPATH = $(sort $(dir $(SOURCES)))

SOURCES += $(wildcard $(BASE_DIR)/libraries/*/*.cpp)
INCLUDE_PATH += $(wildcard $(BASE_DIR)/libraries/*)

include $(PLATFORM_DIR)/rules.mk
include $(BASE_DIR)/platform/common/rules.mk

$(BUILD_DIR):
	@echo "Creating $(BUILD_DIR) directory..."
	$(Q)mkdir -p $(BUILD_DIR)/obj

$(BUILD_DIR)/platform.h: Makefile
	@echo "Creating $*.h..."
	$(Q)echo -e "#ifndef __PLATFORM_H__\n#define __PLATFORM_H__\n\n#include <$(PLATFORM_NAME).h>\n\n#endif\n" > $@

$(OBJECTS) : $(BUILD_DIR)/obj/%.o : %.cpp $(HEADERS) Makefile
	@echo "Compiling $*.cpp..."
	$(Q)$(CC) -c $(CFLAGS) -o $@ $<
	$(Q)$(CC) -MM $(CFLAGS) $< | sed "s/$*\.o/$(shell echo $@ | sed -e 's/[\/&]/\\&/g')/" > $(BUILD_DIR)/obj/$*.d

clean:
	rm -rfv $(BUILD_DIR)

.PHONY: clean

.SECONDARY:

-include $(BUILD_DIR)/obj/*.d
