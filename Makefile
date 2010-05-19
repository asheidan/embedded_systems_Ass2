### Makefile for AVR #########################################################
TARGET     = main.elf
TARGOBJS   = main.o uart.o
TARGHEX    = $(TARGET:.elf=.hex)

DEVICE     = atmega644
CLOCK      = 1000000
# -B 8.68us = 115.2kHz Needs to be atmost 1/4 of dev clockspeed
# PROGRAMMER = -c stk500 -P /dev/tty.PL2303-00* -B 8.68
PROGRAMMER = -c avrisp2 -P usb -B 8.68
FUSES      = -U lfuse:w:0x62:m -U efuse:w:0xff:m -U hfuse:w:0xd9:m

SOURCES = $(shell ls *.c)

### Paths ####################################################################
CFLAGS		:= -Wall -Os
# -D__DEBUG__
AVR-PATH    := /usr/local/CrossPack-AVR/bin/
PATH			:= $(PATH):$(AVR-PATH)
AVR-CC      := sh avr-gcc
AVR-OBJCOPY := avr-objcopy
AVR-OBJDUMP := avr-objdump
AVR-SIZE    := avr-size

### Commands #################################################################

AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)
COMPILE = $(AVR-CC) $(CFLAGS) -mmcu=$(DEVICE)

default: $(TARGET)

# Including generated dependencies
ifneq "$(MAKECMDGOALS)" "clean"
-include $(subst .o,.d,$(TARGOBJS))
endif

### Functions ################################################################

# $(call make-depend,source-file,object-file,depend-file)
define make-depend
	@# Include -MM to exclude "system headers"
	$(AVR-CC) -mmcu=$(DEVICE) -MM -MP -MF $3 -MT $2 $1
endef

### Project Rules ############################################################

$(TARGET): $(TARGOBJS)
	$(COMPILE) -o $@ $(TARGOBJS)

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
	$(AVR-OBJCOPY) -j .text -j .data -O ihex $< $@

# Make them all depend on changes in this file
$(TARGOBJS) $(TARGET): Makefile

%.o: %.c
	# $^
	$(call make-depend,$<,$@,$(subst .o,.d,$@))
	$(COMPILE) -c -o $@ $<

clean:
	-rm -f *.o *.hex *.elf *.d *.tmp

.PHONY: clean clean_all default

### Written mostly late at night by Emil Eriksson 2010 #######################
