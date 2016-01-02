ifneq ($(findstring hmc5883, $(LIBRARIES)),)
	INCLUDE_PATH += $(LIBRARIES_DIR)/hmc5883
	CSOURCES += $(wildcard $(LIBRARIES_DIR)/hmc5883/*.c)
endif
