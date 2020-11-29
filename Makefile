################################################################################
# Set the MCU
################################################################################
MCU = STM32F411xE

################################################################################
# Set Output Filename 
################################################################################
TARGET = STM32Screen

################################################################################
# Set the root directory of the ucdev enviorement 
################################################################################
UCDEV_ROOT=../../ucdev

################################################################################
# Sources
################################################################################
C_SOURCES += $(LIBHALGLUE_ROOT)/hal/stm32f4/clocksetup.c
C_SOURCES += $(LIBHALGLUE_ROOT)/common/arm_cpuid.c

C_SOURCES += main.c
C_SOURCES += ucglib_hal.c
C_SOURCES += dis.c
C_SOURCES += sheep.c
C_SOURCES += itph.c



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

INC += -I$(UCDEV_ROOT)/ext/ucglib/csrc

C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_bitmap.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_box.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_ccs.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_circle.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_clip.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_com_msg_api.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_dev_default_cb.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_dev_ic_hx8352c.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_dev_ic_ili9163.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_dev_ic_ili9325.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_dev_ic_ili9325_spi.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_dev_ic_ili9341.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_dev_ic_ili9486.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_dev_ic_ld50t6160.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_dev_ic_pcf8833.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_dev_ic_seps225.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_dev_ic_ssd1289.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_dev_ic_ssd1331.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_dev_ic_ssd1351.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_dev_ic_st7735.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_dev_msg_api.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_dev_oled_128x128_ft.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_dev_oled_128x128_ilsoft.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_dev_oled_128x128_univision.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_dev_oled_160x128_samsung.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_dev_oled_96x64_univision.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_dev_tft_128x128_ili9163.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_dev_tft_128x160_st7735.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_dev_tft_132x132_pcf8833.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_dev_tft_240x320_ili9325_spi.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_dev_tft_240x320_ili9341.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_dev_tft_240x320_itdb02.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_dev_tft_240x320_ssd1289.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_dev_tft_240x400_hx8352c.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_dev_tft_320x480_ili9486.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_font.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_init.c
#C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_line.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_pixel.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_pixel_font_data.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_polygon.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_rotate.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_scale.c
C_SOURCES += $(UCDEV_ROOT)/ext/ucglib/csrc/ucg_vector_font_data.c


# C includes
C_INCLUDES =  $(INC) -I. 

# link script
LDSCRIPT = $(shell tr '[:upper:]' '[:lower:]' <<< $(MCU)).ld

# SEMIHOSTING
#LDFLAGS += --specs=rdimon.specs -lc -lrdimon



MK_DIR=$(UCDEV_ROOT)/build/make
include $(MK_DIR)/all.mk
  
