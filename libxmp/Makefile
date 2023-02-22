ifdef $(GENDEV)
ROOTDIR = $(GENDEV)
else
ROOTDIR = /opt/toolchains/sega
endif

INSTALLDIR = $(ROOTDIR)/sh-elf

LIBPATH = -L$(ROOTDIR)/sh-elf/lib -L$(ROOTDIR)/sh-elf/lib/gcc/sh-elf/4.6.2 -L$(ROOTDIR)/sh-elf/sh-elf/lib
INCPATH = -I. -I$(ROOTDIR)/sh-elf/include -I$(ROOTDIR)/sh-elf/sh-elf/include

SHCCFLAGS = -m2 -mb -Ofast -Wall -g -c -fomit-frame-pointer -D__32X__ -DUSE_VOL_ENVELOPE
SHASFLAGS = --big

SHPREFIX = $(ROOTDIR)/sh-elf/bin/sh-elf-
SHCC = $(SHPREFIX)gcc
SHAS = $(SHPREFIX)as
SHAR = $(SHPREFIX)ar

CP = cp
RM = rm -f

TARGET = libxmp
OBJS = xmp.o

all: $(TARGET).a

$(TARGET).a: $(OBJS)
	$(SHAR) -rcs $@ $(OBJS)

%.o: %.c
	$(SHCC) $(SHCCFLAGS) $(INCPATH) $< -o $@

clean:
	$(RM) *.o *.a

install: all
	$(CP) xmp.h $(INSTALLDIR)/include
	$(CP) libxmp.a $(INSTALLDIR)/lib
