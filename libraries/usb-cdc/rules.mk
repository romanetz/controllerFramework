ifneq ($(findstring usb-cdc, $(LIBRARIES)),)
	INCLUDE_PATH += $(LIBRARIES_DIR)/usb-cdc
	CSOURCES += $(wildcard $(LIBRARIES_DIR)/usb-cdc/*.c)
endif
