CROSS_COMPILE:=arm-none-eabi-
CC:=$(CROSS_COMPILE)gcc
LD:=$(CROSS_COMPILE)ld
OBJS_APP:=startup.o main.o user_malloc.o


OPT = -O0
CPU = -mcpu=cortex-m4
FLOAT-ABI = -mfloat-abi=hard
FPU = -mfpu=fpv4-sp-d16

MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

LSCRIPT_APP:=app.ld

OBJCOPY:=$(CROSS_COMPILE)objcopy

CFLAGS:=$(MCU) $(OPT) -g -Wall -Wno-main -ffreestanding -nostdlib
LDFLAGS:=-gc-sections -nostdlib -nostartfiles

all: app.bin


##app
app.bin: app.elf
	$(OBJCOPY) -O binary $^ $@

app.elf: $(OBJS_APP) $(LSCRIPT_APP)
	$(LD) $(LDFLAGS) $(OBJS_APP) -o $@ -Map=app.map -T app.ld
	
startup.o: startup.c
	$(CC) -c $(CFLAGS) startup.c

main.o: main.c
	$(CC) -c $(CFLAGS) main.c

malloc.o: user_malloc.c
	$(CC) -c $(CFLAGS) user_malloc.c

#clean
clean:
	rm -f *.bin *.elf *.o *.map
