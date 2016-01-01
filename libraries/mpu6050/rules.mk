ifneq ($(findstring mpu6050, $(LIBRARIES)),)
	INCLUDE_PATH += $(LIBRARIES_DIR)/mpu6050
	CSOURCES += $(wildcard $(LIBRARIES_DIR)/mpu6050/*.c)
endif
