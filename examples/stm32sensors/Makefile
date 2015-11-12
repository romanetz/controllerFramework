PROJECT_NAME = stm32sensors
PROJECT_DIR = .

SOURCES = $(wildcard $(PROJECT_DIR)/*.cpp)

#PLATFORM ?= stm32f407vg
PLATFORM ?= stm32f103ze

CFLAGS ?= -O2 -ggdb -Wall -Werror

BASE_DIR = ../..
include $(BASE_DIR)/platform/rules.mk
