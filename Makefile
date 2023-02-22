ifdef $(GENDEV)
ROOTDIR = $(GENDEV)
else
ROOTDIR = /opt/toolchains/sega
endif

LDSCRIPTSDIR = $(ROOTDIR)/ldscripts

LIBPATH = -L$(ROOTDIR)/sh-elf/lib -L$(ROOTDIR)/sh-elf/lib/gcc/sh-elf/4.5.2 -L$(ROOTDIR)/sh-elf/sh-elf/lib
INCPATH = -I. -I$(ROOTDIR)/sh-elf/include -I$(ROOTDIR)/sh-elf/sh-elf/include

CCFLAGS = -m2 -mb -Ofast -flto -Wall -c -fomit-frame-pointer -fipa-pta
CCFLAGS += -D__32X__ -DUSE_VOL_ENVELOPE
HWFLAGS = -m2 -mb -O1 -Wall -c -fomit-frame-pointer
LDFLAGS = -T $(LDSCRIPTSDIR)/mars.ld -Wl,-Map=output.map -nostdlib -flto -Wl,--as-needed
ASFLAGS = --big --defsym LINEAR_CROSSFADE=1

PREFIX = $(ROOTDIR)/sh-elf/bin/sh-elf-
CC = $(PREFIX)gcc
AS = $(PREFIX)as
LD = $(PREFIX)ld
OBJC = $(PREFIX)objcopy

DD = dd
RM = rm -f

TARGET = ANARCH32X
LIBS = $(LIBPATH) -lxmp -lc -lgcc -lgcc-Os-4-200 -lnosys
OBJS = \
	crt0.o \
	src/main_32x.o \
	src/sound.o \
	src/hw_32x.o \
	src/32x_images.o \
	image.o files.o \
	music/module_structure.o \
	music/module_data.o 

all: m68k.bin $(TARGET).bin

m68k.bin:
	make -C src-md

$(TARGET).bin: $(TARGET).elf
	$(OBJC) -O binary $< temp.bin
	$(DD) if=temp.bin of=$@ bs=128K conv=sync

$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $(TARGET).elf

hw_32x.o: hw_32x.c
	$(CC) $(HWFLAGS) $(INCPATH) $< -o $@

%.o: %.c
	$(CC) $(CCFLAGS) $(INCPATH) $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $(INCPATH) $< -o $@

clean:
	make clean -C src-md
	$(RM) music/*.o *.o *.bin *.elf output.map src/*.o
