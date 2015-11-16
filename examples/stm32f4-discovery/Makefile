PROJECT_NAME = stm32f4-discovery-example
PROJECT_DIR = .

SOURCES = $(wildcard $(PROJECT_DIR)/*.cpp)

PLATFORM ?= stm32f407vg

CFLAGS ?= -O2 -ggdb -Wall -Werror

BASE_DIR = ../..
include $(BASE_DIR)/platform/rules.mk
