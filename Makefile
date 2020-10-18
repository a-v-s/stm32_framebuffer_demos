################################################################################
# Set the MCU
################################################################################
MCU = STM32F103xB

################################################################################
# Set Output Filename 
################################################################################
TARGET = STM32WS2812

################################################################################
# Set the root directory of the ucdev enviorement 
################################################################################
UCDEV_ROOT=../../ucdev

################################################################################
# Sources
################################################################################
C_SOURCES += $(LIBHALGLUE_ROOT)/hal/stm32f1/clocksetup.c
C_SOURCES += $(LIBHALGLUE_ROOT)/common/arm_cpuid.c

C_SOURCES += main.c

#C_SOURCES += ../demo.c
#C_SOURCES += $(LIBHALGLUE_ROOT)/common/serialnumber.c
#C_SOURCES += $(LIBHLUSBD_ROOT)/drivers/usbd_stm.c
#C_SOURCES += $(LIBHLUSBD_ROOT)/usbd.c
#C_SOURCES += $(LIBHLUSBD_ROOT)/usbd_descriptors.c
#C_SOURCES += $(LIBHLUSBD_ROOT)/ConvertUTF/ConvertUTF.c

C_SOURCES += $(UCDEV_ROOT)/lib/libws2812dma/drivers/ws2812_stm32.c
C_SOURCES += $(UCDEV_ROOT)/lib/libws2812dma/ws2812.c
################################################################################
# Includes
################################################################################
INC += -I. -I..
INC += -I $(LIBHLUSBD_ROOT)/drivers/
INC += -I $(LIBHLUSBD_ROOT)/
INC += -I $(LIBHLUSBD_ROOT)/ConvertUTF
INC += -I$(LIBHALGLUE_ROOT)/common/
INC += -I$(LIBHALGLUE_INC) -I$(LIBHALGLUE_INC)/stm32
INC += -I$(CUBEF1_ROOT)/Drivers/CMSIS/Device/ST/STM32F1xx/Include/
INC += -I$(CUBEF1_ROOT)/Drivers/CMSIS/Include/
INC += -I$(CUBEF1_HAL_INC_ROOT)
INC += -I$(UCDEV_ROOT)/lib/libws2812dma/

# C includes
C_INCLUDES =  $(INC) -I. 

# link script
LDSCRIPT = $(shell tr '[:upper:]' '[:lower:]' <<< $(MCU)).ld

MK_DIR=$(UCDEV_ROOT)/build/make
include $(MK_DIR)/all.mk
  
