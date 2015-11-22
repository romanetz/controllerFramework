PROJECT_NAME = stm32-usbcdc
PROJECT_DIR = .

SOURCES = $(wildcard $(PROJECT_DIR)/*.cpp)

PLATFORM ?= stm32f103c8

CFLAGS ?= -O2 -ggdb -Wall -Werror

BASE_DIR = ../..
include $(BASE_DIR)/platform/rules.mk
