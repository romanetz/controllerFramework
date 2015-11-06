PROJECT_NAME = test
PROJECT_DIR = .

SOURCES = $(wildcard $(PROJECT_DIR)/*.cpp)
HEADERS = $(wildcard $(PROJECT_DIR)/*.h)

#PLATFORM ?= stm32f407vg
PLATFORM ?= stm32f103ze

CFLAGS ?= -O2 -ggdb -Wall -Werror

BASE_DIR = ..
include $(BASE_DIR)/platform/rules.mk
