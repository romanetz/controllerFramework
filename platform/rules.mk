BUILD_DIR ?= $(PROJECT_DIR)/build

PLATFORM_NAME = $(strip $(foreach PLATFORM_NAME, $(wildcard $(FRAMEWORK_DIR)/platform/*), $(findstring $(notdir $(PLATFORM_NAME)), $(PLATFORM))))
ifeq ($(PLATFORM_NAME),)
$(error Could not determine desired platform)
endif
PLATFORM_DIR = $(FRAMEWORK_DIR)/platform/$(PLATFORM_NAME)

CFLAGS ?= -Wall -Werror -O2 -ggdb

CC = $(TARGET)gcc
LD = $(TARGET)gcc
STRIP = $(TARGET)strip
OBJCOPY = $(TARGET)objcopy
SIZE = $(TARGET)size
Q ?= @

INCLUDE_PATH += $(BUILD_DIR)/include

ifneq ($(USE_CLASSES),no)
CFLAGS += -DUSE_CLASSES
endif

LIBRARIES_DIR = $(FRAMEWORK_DIR)/libraries
include $(LIBRARIES_DIR)/*/rules.mk

CFLAGS += $(addprefix -I, $(INCLUDE_PATH))

COBJECTS = $(addprefix $(BUILD_DIR)/obj/, $(notdir $(CSOURCES:.c=.o)))
OBJECTS = $(COBJECTS)

HEADERS += $(BUILD_DIR)/include/platform.h

VPATH = $(sort $(dir $(CSOURCES)))

include $(FRAMEWORK_DIR)/platform/generic/rules.mk
include $(PLATFORM_DIR)/rules.mk

$(BUILD_DIR):
	@echo "Creating $(BUILD_DIR) directory..."
	$(Q)mkdir -p $(BUILD_DIR)/include
	$(Q)mkdir -p $(BUILD_DIR)/obj
	$(Q)mkdir -p $(BUILD_DIR)/deps

$(BUILD_DIR)/include/platform.h: Makefile
	@echo "Generating $*.h..."
	$(Q)echo -e "#ifndef __PLATFORM_H__\n#define __PLATFORM_H__\n\n#include <$(PLATFORM_NAME).h>\n\n#endif\n" > $@

$(COBJECTS): $(BUILD_DIR)/obj/%.o: %.c $(HEADERS) Makefile
	@echo "Compiling $*.c..."
	$(Q)$(CC) -c $(CFLAGS) -o $@ $<
	$(Q)$(CC) -MM $(CFLAGS) $< | sed "s/$*\.o/$(shell echo $@ | sed -e 's/[\/&]/\\&/g')/" > $(BUILD_DIR)/deps/$*.d

clean:
	rm -rfv $(BUILD_DIR)

.PHONY: clean

.SECONDARY:

-include $(BUILD_DIR)/deps/*.d
