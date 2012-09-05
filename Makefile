### Project name (also used for output file name)
PROJECT	= mobile_usb_battery

### Source files and search directories
CSRC	= main.c iic.c adc.c timer32.c BQ29312A.c startup1100.c
ASRC	=
CSRCARM	=
ASRCARM	=
VPATH   =

### Optimization level (0, 1, 2, 3, 4 or s)
OPTIMIZE = 0

### C Standard level (c89, gnu89, c99 or gnu99)
CSTD = gnu89

### Processor type and use Thumb(-2) mode for CSRC/ASRC files (YES or NO)
CPU   = cortex-m0
THUMB = YES

### Linker script for the target MCU
LINKSCRIPT = LPC1114.ld

### Output file type (hex, bin or both) and debugger type
OUTPUT	= hex
HEXFMT  = ihex
DEBUG	= dwarf-2

### Include dirs, library dirs and definitions
MATHLIB	= -lm
LIBS	=
LIBDIRS	=
INCDIRS	=
DEFS	=
ADEFS	=

### Warning contorls
WARNINGS = all extra

### Object output directory
OBJDIR = obj


### Programs to build porject
CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE    = arm-none-eabi-size
NM      = arm-none-eabi-nm


# Define all object files
COBJ      = $(CSRC:.c=.o) 
AOBJ      = $(ASRC:.S=.o)
COBJARM   = $(CSRCARM:.c=.o)
AOBJARM   = $(ASRCARM:.S=.o)
COBJ      := $(addprefix $(OBJDIR)/,$(COBJ))
AOBJ      := $(addprefix $(OBJDIR)/,$(AOBJ))
COBJARM   := $(addprefix $(OBJDIR)/,$(COBJARM))
AOBJARM   := $(addprefix $(OBJDIR)/,$(AOBJARM))
PROJECT   := $(OBJDIR)/$(PROJECT)


### Compiler flags
ifeq ($(THUMB),YES)
THUMBFLAG = -mthumb
THUMBIW = -mthumb-interwork
else
THUMBFLAG =
THUMBIW =
endif


# Flags for C files
CFLAGS += -std=$(CSTD)
CFLAGS += -g$(DEBUG)
CFLAGS += -O$(OPTIMIZE)
CFLAGS += $(addprefix -W,$(WARNINGS))
CFLAGS += $(addprefix -I,$(INCDIRS))
CFLAGS += $(addprefix -D,$(DEFS))
CFLAGS += -Wp,-M,-MP,-MT,$(OBJDIR)/$(*F).o,-MF,$(OBJDIR)/$(*F).d


# Assembler flags
ASFLAGS += $(addprefix -D,$(ADEFS)) -Wa,-g$(DEBUG)


# Linker flags
LDFLAGS += -nostartfiles -Wl,-Map=$(PROJECT).map,--cref,--gc-sections
LDFLAGS += -lc -lgcc
LDFLAGS += $(patsubst %,-L%,$(LIBDIRS)) $(patsubst %,-l%,$(LIBS))
LDFLAGS += $(MATHLIB)
LDFLAGS += -T$(LINKSCRIPT)


# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS  = -mcpu=$(CPU) $(THUMBIW) -I. $(CFLAGS)
ALL_ASFLAGS = -mcpu=$(CPU) $(THUMBIW) -I. -x assembler-with-cpp $(ASFLAGS)


# Debug Variables
CURRENTDIR=$(shell pwd)
OCD = openocd
OCD_CMD = -s $(CURRENTDIR)/tcl \
          -f interface/vsllink_swd.cfg \
          -f target/lpc1114_swd_flash.cfg

# Default target.
all: version build size

ifeq ($(OUTPUT),hex)
build: elf hex lst sym
hex: $(PROJECT).hex
else
ifeq ($(OUTPUT),bin)
build: elf bin lst sym
bin: $(PROJECT).bin
else
ifeq ($(OUTPUT),both)
build: elf hex bin lst sym
hex: $(PROJECT).hex
bin: $(PROJECT).bin
else
$(error "Invalid format: $(OUTPUT)")
endif
endif
endif

elf: $(PROJECT).elf
lst: $(PROJECT).lst 
sym: $(PROJECT).sym


# Display compiler version information.
version :
	@$(CC) --version

# Create final output file from ELF output file.
%.hex: %.elf
	@echo
	$(OBJCOPY) -O $(HEXFMT) $< $@

%.bin: %.elf
	@echo
	$(OBJCOPY) -O binary $< $@

# Create extended listing file from ELF output file.
%.lst: %.elf
	@echo
	$(OBJDUMP) -h -S -C $< > $@

# Create a symbol table from ELF output file.
%.sym: %.elf
	@echo
	$(NM) -n $< > $@

# Display size of file.
size:
	@echo
	$(SIZE) -A $(PROJECT).elf


# Link: create ELF output file from object files.
%.elf:  $(AOBJARM) $(AOBJ) $(COBJARM) $(COBJ)
	@echo
	@echo Linking...
	$(CC) $(THUMBFLAG) $(ALL_CFLAGS) $(AOBJARM) $(AOBJ) $(COBJARM) $(COBJ) --output $@ $(LDFLAGS)

# Compile: create object files from C source files. ARM or Thumb(-2)
$(COBJ) : $(OBJDIR)/%.o : %.c
	@echo
	@echo $< :
	$(CC) -c $(THUMBFLAG) $(ALL_CFLAGS) $< -o $@

# Compile: create object files from C source files. ARM-only
$(COBJARM) : $(OBJDIR)/%.o : %.c
	@echo
	@echo $< :
	$(CC) -c $(ALL_CFLAGS) $< -o $@ 

# Assemble: create object files from assembler source files. ARM or Thumb(-2)
$(AOBJ) : $(OBJDIR)/%.o : %.S
	@echo
	@echo $< :
	$(CC) -c $(THUMBFLAG) $(ALL_ASFLAGS) $< -o $@

# Assemble: create object files from assembler source files. ARM-only
$(AOBJARM) : $(OBJDIR)/%.o : %.S
	@echo
	@echo $< :
	$(CC) -c $(ALL_ASFLAGS) $< -o $@

# Debug with openocd
ocd:
	$(OCD) $(OCD_CMD) -c "soft_reset_halt"

# Target: clean project.
clean:
	@echo
	rm -f -r $(OBJDIR) | exit 0


# Include the dependency files.
-include $(shell mkdir $(OBJDIR) 2>/dev/null) $(wildcard $(OBJDIR)/*.d)

