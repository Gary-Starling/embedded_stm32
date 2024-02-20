CROSS_COMPILE:=arm-none-eabi-
CC:=$(CROSS_COMPILE)gcc
LD:=$(CROSS_COMPILE)ld
OBJS_APP:=startup.o main.o testlib.o
OBJS_BOOT:=boot.o testlib.o

OPT = -O0
CPU = -mcpu=cortex-m4
FLOAT-ABI = -mfloat-abi=hard
FPU = -mfpu=fpv4-sp-d16

MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

LSCRIPT_APP:=app.ld
LSCRIPT_BOOT:=boot.ld

OBJCOPY:=$(CROSS_COMPILE)objcopy


CFLAGS:=$(MCU) $(OPT) -g -Wall -Wno-main -ffreestanding -nostdlib
LDFLAGS:=-gc-sections -nostdlib

all: image.bin

image.bin: app.bin boot.bin
	cat boot.bin app.bin > image.bin

##app
app.bin: app.elf
	$(OBJCOPY) -O binary $^ $@

app.elf: $(OBJS_APP) $(LSCRIPT_APP)
	$(LD) $(LDFLAGS) $(OBJS_APP) -o $@ -Map=app.map -T app.ld
	
startup.o: startup.c
	$(CC) -c $(CFLAGS) startup.c

main.o: main.c
	$(CC) -c $(CFLAGS) main.c


#boot
boot.bin: boot.elf
	$(OBJCOPY) -O binary --gap-fill=0xff --pad-to=0x08004000 $< $@

boot.elf: $(OBJS_BOOT) $(LSCRIPT_BOOT)
	$(LD) $(LDFLAGS) $(OBJS_BOOT) -o $@ -Map=boot.map -T boot.ld

boot.o: boot.c
	$(CC) -c $(CFLAGS) boot.c

testlib.o: testlib.c
	$(CC) -c $(CFLAGS) testlib.c
#clean
clean:
	rm -f *.bin *.elf *.o *.map
