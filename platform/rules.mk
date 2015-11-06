PLATFORM_NAME = $(strip $(foreach PLATFORM_NAME, $(wildcard $(BASE_DIR)/platform/*), $(findstring $(notdir $(PLATFORM_NAME)), $(PLATFORM))))
PLATFORM_DIR = $(BASE_DIR)/platform/$(PLATFORM_NAME)

BUILD_DIR ?= $(PROJECT_DIR)/build

CC = $(TARGET)gcc
LD = $(TARGET)gcc
SIZE = $(TARGET)size
OBJCOPY = $(TARGET)objcopy

SOURCES += $(wildcard $(BASE_DIR)/platform/*.cpp)
HEADERS += $(wildcard $(BASE_DIR)/platform/*.h)
INCLUDE_PATH += $(BASE_DIR)/platform

CFLAGS += -fno-exceptions -fno-rtti
CFLAGS += $(foreach PATH, $(INCLUDE_PATH), -I$(PATH))

OBJECTS += $(addprefix $(BUILD_DIR)/, $(notdir $(SOURCES:.cpp=.o)))

VPATH = $(sort $(dir $(SOURCES)))

include $(PLATFORM_DIR)/rules.mk

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(OBJECTS) : $(BUILD_DIR)/%.o : %.cpp $(HEADERS)
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -rfv $(BUILD_DIR)

.PHONY: clean

.SECONDARY:

