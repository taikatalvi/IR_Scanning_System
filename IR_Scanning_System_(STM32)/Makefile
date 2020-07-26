TARGET := firmware

DEFINES += STM32F10X_MD_VL
DEFINES += GCC_ARMCM3
DEFINES += VECT_TAB_FLASH

AS = arm-none-eabi-gcc
CC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
CP = arm-none-eabi-objcopy
SZ = arm-none-eabi-size
RM = rm

CMSIS_INC_PATH = CMSIS/Include
CMSIS_SRC_PATH = CMSIS/Source
LIBS_INC_PATH = Libs/Include
LIBS_SRC_PATH = Libs/Source
USBLIB_INC_PATH = Libs/USB_Lib/Include
USBLIB_SRC_PATH = Libs/USB_Lib/Source

STARTUP = startup_stm32f103xb.s
LDSCRIPT = STM32F103XB_FLASH.ld

INCLUDES := ./
INCLUDES += $(SOURCEDIRS)
INCLUDES += $(CMSIS_INC_PATH)
INCLUDES += $(LIBS_INC_PATH)
INCLUDES += $(USBLIB_INC_PATH)

SOURCEDIRS := ./
SOURCEDIRS += $(LIBS_SRC_PATH)
SOURCEDIRS += $(USBLIB_SRC_PATH)
SOURCEDIRS += $(CMSIS_SRC_PATH)

MCFLAGS := -mcpu=cortex-m3 -mthumb -mlittle-endian --specs=nosys.specs \
		-ffunction-sections -fdata-sections -Wl,--gc-sections 

CFLAGS := $(MCFLAGS) -Os -g -gdwarf-2 -Wall -c
CFLAGS += $(addprefix -I, $(INCLUDES))
CFLAGS += $(addprefix -D, $(DEFINES))

LDFLAGS := $(MCFLAGS) -T$(LDSCRIPT) -L"$(CURDIR)"

OBJ_DIR = obj

AFLAGS += -ahls -mapcs-32

SOURCES := $(notdir $(wildcard $(addsuffix /*.c, $(SOURCEDIRS))))

OBJECTS := $(patsubst %.c, $(OBJ_DIR)/%.o, $(SOURCES))
OBJECTS += $(patsubst %.s, $(OBJ_DIR)/%.o, $(STARTUP))

VPATH := $(SOURCEDIRS)

TOREMOVE := *.elf *.hex *.bin
TOREMOVE += $(TARGET)
TOREMOVE += $(OBJ_DIR)


all: obj-dir $(TARGET).bin size clean-obj


$(TARGET).bin: $(TARGET).elf
	@$(CP) -Obinary $(TARGET).elf $(TARGET).bin

$(TARGET).hex: $(TARGET).elf
	@$(CP) -Oihex $(TARGET).elf $(TARGET).hex

$(TARGET).elf: $(OBJECTS)
	@echo "Linking"
	$(LD) $(LDFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: %.c
	@echo "Compilling C source =>" $<
	$(CC) $(CFLAGS) $< -o "$@"
	
$(OBJ_DIR)/%.o: %.s
	@echo "Compilling ASM source =>" $<
	$(AS) $(CFLAGS) $< -o "$@"

size:
	@echo "---------------------------------------------------"
	@$(SZ) $(TARGET).elf

obj-dir:
	@mkdir -p $(OBJ_DIR)

clean-obj:
	@rm -rf $(OBJ_DIR) *.elf

clean:
	@rm -rf $(TOREMOVE)

write:
	st-flash write $(TARGET).bin 0x08000000

read:
	st-flash read ./default.bin 0x08000000 0x10000

include $(wildcart *.d)