ifneq ($(findstring ms5611, $(LIBRARIES)),)
	INCLUDE_PATH += $(LIBRARIES_DIR)/ms5611
	CSOURCES += $(wildcard $(LIBRARIES_DIR)/ms5611/*.c)
endif
