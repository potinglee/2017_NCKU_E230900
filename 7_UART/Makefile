PROJ_NAME=7_UART

DEVICE = Library/CMSIS/Device/ST/STM32F4xx/Include
CORE = Library/CMSIS/Include
PERIPH = Library/STM32F4xx_StdPeriph_Driver
DISCOVERY = Library

################################################################################
#                   SETUP TOOLS                                                #
################################################################################

CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
GDB     = arm-none-eabi-gdb
AS      = arm-none-eabi-as

##### Preprocessor options

# #defines needed when working with the STM peripherals library
DEFS    = -DUSE_STDPERIPH_DRIVER
# DEFS   += -DUSE_FULL_ASSERT

##### Assembler options

AFLAGS  = -mcpu=cortex-m4
AFLAGS += -mthumb
AFLAGS += -mthumb-interwork
AFLAGS += -mlittle-endian
AFLAGS += -mfloat-abi=hard
AFLAGS += -mfpu=fpv4-sp-d16

##### Compiler options

CFLAGS  = -ggdb
CFLAGS += -O0
CFLAGS += -Wall -Wextra -Warray-bounds
CFLAGS += $(AFLAGS)
CFLAGS += -DSTM32F40XX

##### Linker options

# tell ld which linker file to use
# (this file is in the current directory)
LFLAGS  = -Tstm32_flash.ld

################################################################################
#                   HEADER FILES DIRECTORIES                                   #
################################################################################

# The header files we use are located here
INCLUDES  = -I$(DEVICE)
INCLUDES += -I$(CORE)
INCLUDES += -I$(PERIPH)/inc
INCLUDES += -I.

################################################################################
#                   SOURCE FILES TO COMPILE                                    #
################################################################################

SOURCES  += \
			$(PERIPH)/src/stm32f4xx_rcc.c \
			$(PERIPH)/src/stm32f4xx_i2c.c \
			$(PERIPH)/src/stm32f4xx_gpio.c \
			$(PERIPH)/src/stm32f4xx_spi.c \
			$(PERIPH)/src/stm32f4xx_exti.c \
			$(PERIPH)/src/stm32f4xx_syscfg.c \
			$(PERIPH)/src/stm32f4xx_usart.c \
			$(PERIPH)/src/misc.c

# startup file, calls main
SOURCES += startup_stm32f40xx.s
SOURCES += system_stm32f4xx.c
SOURCES += main.c

OBJS  = $(SOURCES:.c=.o)

######################################################################
#                         SETUP TARGETS                              #
######################################################################

.PHONY: all

all: $(PROJ_NAME).elf

%.o : %.c
	@echo "[Compiling  ]  $^"
	@$(CC) -c -o $@ $(INCLUDES) $(CFLAGS) $^

$(PROJ_NAME).elf: $(OBJS)
	@echo "[Linking    ]  $@"
	@$(CC) $(CFLAGS) $(LFLAGS) $^ -o $@
	@$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin

clean:
	rm -f *.o $(PROJ_NAME).elf $(PROJ_NAME).bin

flash:
	st-flash write $(PROJ_NAME).bin 0x8000000
