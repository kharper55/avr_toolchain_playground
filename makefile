##########------------------------------------------------------##########
##########              Project-specific Details                ##########
##########    Check these every time you start a new project    ##########
##########        												##########
##########               Created by Elliot Williams 
##########	   (https://github.com/hexagon5un/AVR-Programming)  ##########     
##########			   and adapted for an alternative			##########
##########						directory tree					##########
##########------------------------------------------------------##########

MCU   = atmega328p
F_CPU = 16000000UL  
BAUD  = 9600UL
PORT = COM6

## Dirs
LIBDIR := lib
SRCDIR := src
OBJDIR := obj
BUILDDIR := build

##########------------------------------------------------------##########
##########                 Programmer Defaults                  ##########
##########          Set up once, then forget about it           ##########
##########        (Can override.  See bottom of file.)          ##########
##########------------------------------------------------------##########

PROGRAMMER_TYPE = arduino
# extra arguments to avrdude: baud rate, chip type, -F flag, etc.
PROGRAMMER_ARGS = -P $(PORT)

##########------------------------------------------------------##########
##########                  Program Locations                   ##########
##########     Won't need to change if they're in your PATH     ##########
##########------------------------------------------------------##########

CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRSIZE = avr-size
AVRDUDE = avrdude

##########------------------------------------------------------##########
##########                   Makefile Magic!                    ##########
##########         Summary:                                     ##########
##########             We want a .hex file                      ##########
##########        Compile source files into .elf                ##########
##########        Convert .elf file into .hex                   ##########
##########        You shouldn't need to edit below.             ##########
##########------------------------------------------------------##########

## The name of your project (without the .c)
# TARGET = blinkLED
## Or name it automatically after the enclosing directory
TARGET = $(lastword $(subst /, ,$(CURDIR)))
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

# Object files: will find all .c/.h files in current directory
#  and in LIBDIR.  If you have any other (sub-)directories with code,
#  you can add them in to SOURCES below in the wildcard statement.
SOURCES:=$(call rwildcard,$(SRCDIR),*.c)
SRCDIRS:=$(dir $(SOURCES))
SRCNDIRS:=$(notdir $(SOURCES))
SRCINCS:=$(patsubst %,-I%,$(subst :, ,$(SRCDIRS)))
HEADERS:=$(SOURCES:.c=.h)
HDRDIRS:=$(dir $(HEADERS))
HDRINCS:=$(patsubst %,-I%,$(subst :, ,$(HDRDIRS)))
OBJECTS:=$(addprefix $(OBJDIR)/, $(patsubst %.c, %.o, $(SRCNDIRS)))
#OBJECTS:=$(patsubst $(SRCDIRS)/%.c, $(OBJ_DIR)/%.o, $(SRCNDIRS))

## Compilation options, type man avr-gcc if you're curious.
CPPFLAGS = -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -I$(LIBDIR) $(SRCINCS) -I./
CFLAGS = -Os -g -std=gnu99 -Wall
## Use short (8-bit) data types 
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums 
## Splits up object files per function
CFLAGS += -ffunction-sections -fdata-sections
LDFLAGS = -Wl,-Map,$(BUILDDIR)/$(TARGET).map 
## Optional, but often ends up with smaller code
LDFLAGS += -Wl,--gc-sections 
TARGET_ARCH = -mmcu=$(MCU)

## Explicit pattern rules:	

# Create ./obj directory if not present
$(OBJDIR): 
	mkdir $(OBJDIR)

# Create ./build directory if not present
$(BUILDDIR): 
	mkdir $(BUILDDIR)

##  To make .o files from .c files located in various SRCDIRS
$(OBJDIR)/%.o: src/adc/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c $< -o $@

$(OBJDIR)/%.o: src/gpio/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c $< -o $@

$(OBJDIR)/%.o: src/uart/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c $< -o $@

$(OBJDIR)/%.o: src/twi/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c $< -o $@

$(OBJDIR)/%.o: src/tmr1/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c $< -o $@

$(OBJDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c $< -o $@


##  To make .elf file from .o files
$(BUILDDIR)/$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $(TARGET_ARCH) $^ $(LDLIBS) -o $@

$(BUILDDIR)/%.hex: $(BUILDDIR)/%.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.eeprom: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@ 

%.lst: %.elf
	$(OBJDUMP) -S $< > $@

## These targets don't have files named after them
.PHONY: all disassemble disasm eeprom size clean squeaky_clean flash fuses

all: $(OBJDIR) $(BUILDDIR) $(BUILDDIR)/$(TARGET).hex 

debug:
	@echo
	@echo "Source files:"      $(SOURCES)
	@echo "Header files:"      $(HEADERS)
	@echo "Object dir:"        $(OBJDIR)
	@echo "Object files:"      $(OBJECTS)
	@echo "MCU, F_CPU, BAUD:"  $(MCU), $(F_CPU), $(BAUD)
	@echo
	@echo "SRCNDIRS"		   $(SRCNDIRS)
	@echo "SRCDIRS"			   $(SRCDIRS)
	@echo "SRCINCS"			   $(SRCINCS)
	@echo

# Optionally create listing file from .elf
# This creates approximate assembly-language equivalent of your code.
# Useful for debugging time-sensitive bits, 
# or making sure the compiler does what you want.
disassemble: $(TARGET).lst

disasm: disassemble

# Optionally show how big the resulting program is 
size:  $(BUILDDIR)/$(TARGET).elf
	$(AVRSIZE) -C --mcu=$(MCU) $(BUILDDIR)/$(TARGET).elf

# Need to be adapted for change in directories
#clean:
#	rm -f $(TARGET).elf $(TARGET).hex $(TARGET).obj \
	$(TARGET).o $(TARGET).d $(TARGET).eep $(TARGET).lst \
	$(TARGET).lss $(TARGET).sym $(TARGET).map $(TARGET)~ \
	$(TARGET).eeprom

clean: 
	rm -f $(OBJECTS) $(BUILDDIR)/*.hex $(BUILDDIR)/*.elf $(BUILDDIR)/*.map

squeaky_clean:
	rm -f $(OBJECTS) $(BUILDDIR)/*.hex $(BUILDDIR)/*.elf $(BUILDDIR)/*.map \
	$(BUILDDIR)/*.eeprom $(BUILDDIR)/*.lst \
	$(BUILDDIR)/*.sym $(BUILDDIR)/*.lss \
	$(BUILDDIR)/*.eep

##########------------------------------------------------------##########
##########              Programmer-specific details             ##########
##########           Flashing code to AVR using avrdude         ##########
##########------------------------------------------------------##########

flash: $(BUILDDIR)/$(TARGET).hex 
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -U flash:w:$<

## An alias
program: flash

flash_eeprom: $(BUILDDIR)/$(TARGET).eeprom
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -U eeprom:w:$<

avrdude_terminal:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -nt

## If you've got multiple programmers that you use, 
## you can define them here so that it's easy to switch.
## To invoke, use something like `make flash_arduinoISP`
flash_usbtiny: PROGRAMMER_TYPE = usbtiny
flash_usbtiny: PROGRAMMER_ARGS =  # USBTiny works with no further arguments
flash_usbtiny: flash

flash_usbasp: PROGRAMMER_TYPE = usbasp
flash_usbasp: PROGRAMMER_ARGS =  # USBasp works with no further arguments
flash_usbasp: flash

flash_arduinoISP: PROGRAMMER_TYPE = avrisp
flash_arduinoISP: PROGRAMMER_ARGS = -b 19200 -P /dev/ttyACM0 
## (for windows) flash_arduinoISP: PROGRAMMER_ARGS = -b 19200 -P com5
flash_arduinoISP: flash

flash_109: PROGRAMMER_TYPE = avr109
flash_109: PROGRAMMER_ARGS = -b 9600 -P /dev/ttyUSB0
flash_109: flash

##########------------------------------------------------------##########
##########       Fuse settings and suitable defaults            ##########
##########------------------------------------------------------##########

## Mega 48, 88, 168, 328 default values
LFUSE = 0x62
HFUSE = 0xdf
EFUSE = 0x00

## Generic 
FUSE_STRING = -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m 

fuses: 
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) \
	           $(PROGRAMMER_ARGS) $(FUSE_STRING)
show_fuses:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -nv	

## Called with no extra definitions, sets to defaults
set_default_fuses:  FUSE_STRING = -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m 
set_default_fuses:  fuses

## Set the fuse byte for full-speed mode
## Note: can also be set in firmware for modern chips
set_fast_fuse: LFUSE = 0xE2
set_fast_fuse: FUSE_STRING = -U lfuse:w:$(LFUSE):m 
set_fast_fuse: fuses

## Set the EESAVE fuse byte to preserve EEPROM across flashes
set_eeprom_save_fuse: HFUSE = 0xD7
set_eeprom_save_fuse: FUSE_STRING = -U hfuse:w:$(HFUSE):m
set_eeprom_save_fuse: fuses

## Clear the EESAVE fuse byte
clear_eeprom_save_fuse: FUSE_STRING = -U hfuse:w:$(HFUSE):m
clear_eeprom_save_fuse: fuses


#https://stackoverflow.com/questions/19571391/remove-prefix-with-make
#https://stackoverflow.com/questions/53697862/makefile-how-to-put-all-the-o-file-in-a-directory
#https://stackoverflow.com/questions/40621451/makefile-automatically-compile-all-c-files-keeping-o-files-in-separate-folde
#https://stackoverflow.com/questions/8654548/using-make-to-move-o-files-to-a-separate-directory
#https://stackoverflow.com/questions/33923586/make-remove-directory-from-every-path-in-array