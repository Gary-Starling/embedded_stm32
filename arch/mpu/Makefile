CROSS_COMPILE:=arm-none-eabi-
CC:=$(CROSS_COMPILE)gcc
LD:=$(CROSS_COMPILE)ld
OBJCOPY:=$(CROSS_COMPILE)objcopy

CFLAGS:=-mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g -ggdb -Wall -Wno-main -O0
LDFLAGS:=-T app.ld -gc-sections -nostdlib -Map=image.map

image.bin: image.elf
	$(OBJCOPY) -O binary $^ $@

image.elf: main.o startup.o mpu.o $(LSCRIPT)
	$(LD) $(LDFLAGS) startup.o main.o mpu.o -o $@
	
startup.o: startup.c

main.o: main.c

mpu.o: mpu.c

clean:
	rm -f image.bin image.elf *.o image.map
