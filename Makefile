### Makefile for AVR #########################################################
TARGET		= main.elf
TARGOBJS		= main.o
TARGHEX		= $(TARGET:.elf=.hex)

# Procyon used parts
# General, used by others
# PROCOBJS		+= timer.o
# Net
PROCOBJS		+= net/arp.o net/dhcp.o net/icmp.o net/ip.o net/net.o net/netstack.o
# Net driver
# PROCOBJS	+= net/enc28j60.o
# Real time clock
PROCOBJS		+= rtc.o
# MMC-functionality
# PROCOBJS		+= mmc.o
# SPI
PROCOBJS		+= spi.o
# UART, 2 because we have two!
# PROCOBJS		+= uart2.o
# Telnet
PROCOBJS		+= vt100.o xmodem.o

DEVICE		= atmega644
CLOCK			= 1000000
# -B 8.68us = 115.2kHz Needs to be atmost 1/4 of dev clockspeed
# PROGRAMMER = -c stk500 -P /dev/tty.PL2303-00* -B 8.68
PROGRAMMER	= -c avrisp2 -P usb -B 8.68
FUSES			= -U lfuse:w:0x62:m -U efuse:w:0xff:m -U hfuse:w:0xd9:m

FORMAT		= ihex

SOURCES	 	= $(shell ls *.c)

### Paths ####################################################################
CFLAGS		= -Wall -Os -I$(AVRLIB)
LFLAGS		= -Llib -lprocyon

AVRLIB		= avrlib

AVR-PATH		:= /usr/local/CrossPack-AVR/bin/
PATH			:= $(PATH):$(AVR-PATH)
AVR-CC		:= sh avr-gcc
AVR-OBJCOPY	:= avr-objcopy
AVR-OBJDUMP	:= avr-objdump
AVR-SIZE		:= avr-size
AVR-AR		:= avr-ar

### Commands #################################################################

AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)
COMPILE = $(AVR-CC) $(CFLAGS) -mmcu=$(DEVICE)

# default: $(TARGET)
default: lib/libprocyon.a

# Including generated dependencies
ifneq "$(MAKECMDGOALS)" "clean"
-include $(subst .o,.d,$(TARGOBJS))
-include $(patsubst %,lib/%,$(subst .o,.d,$(PROCOBJS)))
endif

### Functions ################################################################

# $(call make-depend,source-file,object-file,depend-file)
define make-depend
	@# Include -MM to exclude "system headers"
	$(AVR-CC) -I$(AVRLIB) -mmcu=$(DEVICE) -MM -MP -MF $3 -MT $2 $1
endef

### Project Rules ############################################################

$(TARGET): $(TARGOBJS)
	$(COMPILE) $(LFLAGS) -o $@ $(TARGOBJS)

#### Procyon #################################################################

lib/libprocyon.a: $(patsubst %,lib/%,$(PROCOBJS))
	$(AVR-AR) cr $@ $(patsubst %,lib/%,$(PROCOBJS))

lib/net:
	mkdir -p $@

lib/%.o: avrlib/%.c lib/net
	$(call make-depend,$<,$@,$(subst .o,.d,$@))
	$(COMPILE) -c -o $@ $<

clean_procyon:
	-rm -r lib/libprocyon.a

### Rules ####################################################################

disasm: $(TARGET)
	@$(AVR-OBJDUMP) -d $<

sizes: $(TARGOBJS)
	@$(AVR-SIZE) --totals $(sort $^)

flash: $(TARGHEX)
	$(AVRDUDE) -U flash:w:$<:i

fuse:
	$(AVRDUDE) $(FUSES)

readfuse:
	$(AVRDUDE) -qq -U lfuse:r:/dev/stdout:h -U efuse:r:/dev/stdout:h -U hfuse:r:/dev/stdout:h

%.hex: %.elf
	$(AVR-OBJCOPY) -j .text -j .data -O $(FORMAT) $< $@

%.eep: %.elf
	$(AVR-OBJCOPY) \
		-j .eeprom \
		--set-section-flags=.eeprom="alloc,load" \
		--change-section-lma .eeprom=0 \
		-O $(FORMAT) $< $@

# Make them all depend on changes in this file
$(TARGOBJS) $(TARGET) lib/libprocyon.a: Makefile

%.o: %.c
	$(call make-depend,$<,$@,$(subst .o,.d,$@))
	$(COMPILE) -c -o $@ $<

clean:
	-rm -f *.o **/*.o *.hex *.elf *.d **/*.d *.tmp

clean_all: clean clean_procyon

.PHONY: clean clean_all default

### Written mostly late at night by Emil Eriksson 2010 #######################
