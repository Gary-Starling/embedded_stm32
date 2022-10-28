#  Startup для мк написан на C
#  Шаблон для работы

######################################
# target
######################################
TARGET = test


######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -Og
# WIN?LIN?
LINUX = 0


#######################################
# paths
#######################################
# Build path
BUILD_DIR = build


######################################
# source
######################################

# C sources
# wildcar + маска .C, вытащим все C файлы из 
# в остальном, добавим нужные нам файлы из драйвера
C_SOURCES += $(wildcard Core/Src/*.c)
C_SOURCES += ./Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_tim_ex.c \
Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_gpio.c \
Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_rcc.c \
Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_tim.c \
Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_rcc_ex.c \
Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal.c \
Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_cortex.c \
Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_flash.c \
Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_flash_ex.c \
Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_exti.c \
#Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_i2c.c \
#Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_i2c_ex.c \
#Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_dma.c \
#Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pwr.c \
#Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pwr_ex.c \

OBJ = $(patsubst %.c, %.o, $(C_SOURCES))


# ASM sources
ASM_SOURCES =  \
./startup_stm32f030x6.s


#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-

# GCC_PATH
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif

HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S



#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m0

# fpu
# NONE for Cortex-M0/M0+/M3
# float-abi

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32F030x6

# AS includes
AS_INCLUDES = 

# C includes
# Вставлям папки с помощью директивы -I
# работат так -I<имя папки>
C_INCLUDES =  \
-ICore/Inc \
-IDrivers/STM32F0xx_HAL_Driver/Inc \
-IDrivers/STM32F0xx_HAL_Driver/Inc/Legacy \
-IDrivers/CMSIS/Device/ST/STM32F0xx/Include \
-IDrivers/CMSIS/Include \


# compile gcc flags
CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections



# Добавить отладочную информацию
ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2 
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = STM32F030C6Tx_FLASH.ld

# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(TARGET).map,--cref -Wl,--gc-sections



#######################################
# Make
# keys
# ключ -c, линковщик вызывать не требуется, только компиляция
# ключи -mcpu=cortex-m3 -mthumbб собрать код под ядро cortex-m0 с набором инструкций thumb
# включить отладочную информацию, ключ -g
# показывать все предупреждения, ключ -Wall
# ключ "–O" определяет формат выходного файла binary/hex
# Создание чистого бинарника -O binary
# -o - на выходе мы ходим видеть файл <name>.elf
# -T – имя файла со скриптом линковки
#
# -fdata-sections Функция удаления неиспользуемых разделов компоновщика может затем удалить неиспользуемые функции
# libs
# -lc -lm -lnosys не используем библиотеки при линковке
# -specs=nano.specs использовать newlib-nano
# -Wl,-Map=test.map для линковшика
#######################################
# build the application
#######################################
# list of objects

OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

# $@ - слева от двоеточия  $< - справа
# Пример
# build/main.o : build/main.c									 это <				 это @
# 	arm-none-eabi-gcc -c -mcpu=cortex-m0 -mthumb -ICore/Inc -Og  Core/Src/main.c -o build/main.o 	
$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@		




# Проверить коннект к устройству
connect: 
	openocd -f interface/stlink-v2.cfg -f target/stm32f0x.cfg

# Прошивка
flash:
	openocd -f interface/stlink-v2.cfg -f target/stm32f0x.cfg -c "program ./build/test.elf verify exit reset"

## Отладка
## objdump Позволяет получить дательную информацию по внутренностям объектного файла
info_obj:
	arm-none-eabi-objdump -t -h -w -x ./build/test.elf

info_nm:
	arm-none-eabi-nm --numeric-sort -S --print-size ./build/test.elf	

openocd:
	openocd.exe -c "gdb_port 50001" -c "tcl_port 50002" -c "telnet_port 50003" -s "./MPRT_22_KYT_F0" \
	-f interface/stlink-v2.cfg -f target/stm32f0x.cfg
gdb:
	arm-none-eabi-gdb ./build/test.elf \
	-ex 'target remote localhost:50001' \
	-ex 'monitor reset halt'


#######################################
# clean up Надо бы доделать
#######################################
ifeq ($(LINUX), 0)
CL = -del $(BUILD_DIR)
else
CL = -rm -fR $(BUILD_DIR)
endif

# Удалить всё
clean:
	rm -fR ./*.d *.o *.elf *.map ./build

print:
	@echo $(C_SOURCES)
	@echo $(OBJ)