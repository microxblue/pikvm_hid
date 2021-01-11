OUTNAME     = pikvm_hid

IN_FLASH   ?= 1
DEBUG_LEVEL = 3

PROJROOT = .
PREFIX   = arm-none-eabi-
CC       = $(PREFIX)gcc
LD       = $(PREFIX)ld
AS       = $(PREFIX)gcc -x assembler-with-cpp
AR       = $(PREFIX)ar
GDB      = $(PREFIX)gdb
OC       = $(PREFIX)objcopy
OD       = $(PREFIX)objdump
ELFSIZE  = $(PREFIX)size

ifeq ($(IN_FLASH), 1)
VECT_TAB_BASE = 0x08000000
LINKER_SCRIPT = $(PRJ_HOME)/flash.ld
else
VECT_TAB_BASE = 0x20005000
LINKER_SCRIPT = $(PRJ_HOME)/ram.ld
endif

MCPU = cortex-m3
ARMTHMODE = thumb
ARM_ARCH = cortexM

SOC = stm32f10c8

USR_DEFINE = -DSTM32F103xB


PRJ_HOME = .
OUT_DIR  = out
OUTPUT_DIR = $(PRJ_HOME)/$(OUT_DIR)

MAP_FILE = $(OUTPUT_DIR)/$(OUTNAME).map
SYM_FILE = $(OUTPUT_DIR)/$(OUTNAME).sym

VPATH = hal	           	  \
        lib							  \
        lib/$(ARM_ARCH)	  \
        usb/Device        \
        usb/Class/CDC     \
        usb/Class/HID     \
        usb/Xpd           \
        kernel            \
        app               \
        brd               \


ASM_SRCS = $(wildcard $(PRJ_HOME)/brd/*.S)
ASM_OBJS = $(patsubst $(PRJ_HOME)/brd/%.S, $(OUTPUT_DIR)/%.os, $(ASM_SRCS))

# NOTE: Keep board at the beginning
C_SRCS  = $(notdir $(wildcard $(PRJ_HOME)/brd/*.c))
C_SRCS += $(notdir $(wildcard $(PRJ_HOME)/hal/*.c))
C_SRCS += $(notdir $(wildcard $(PRJ_HOME)/lib/*.c))
C_SRCS += $(notdir $(wildcard $(PRJ_HOME)/lib/$(ARM_ARCH)/*.c))
C_SRCS += $(notdir $(wildcard $(PRJ_HOME)/usb/Device/*.c))
C_SRCS += $(notdir $(wildcard $(PRJ_HOME)/usb/Class/CDC/*.c))
C_SRCS += $(notdir $(wildcard $(PRJ_HOME)/usb/Class/HID/*.c))
C_SRCS += $(notdir $(wildcard $(PRJ_HOME)/usb/Xpd/*.c))
C_SRCS += $(notdir $(wildcard $(PRJ_HOME)/kernel/*.c))
C_SRCS += $(notdir $(wildcard $(PRJ_HOME)/app/*.c))

C_OBJS = $(patsubst %.c, $(OUTPUT_DIR)/%.o, $(C_SRCS))

INC_DIRS  = -I include 					    \
            -I hal	   					    \
            -I lib						      \
            -I lib/$(ARM_ARCH)	    \
            -I kernel               \
            -I usb/Include	        \
            -I usb/Xpd              \
            -I brd          		    \
            -I app						      \
            -I .                    \


COMMON_FLG = -mcpu=$(MCPU) -m$(ARMTHMODE)

CCFLAGS  = -g
CCFLAGS += -Os -Wall -fdata-sections -ffunction-sections
CCFLAGS += $(USR_DEFINE) $(INC_DIRS)
CCFLAGS += $(COMMON_FLG)

CFLAGS   = -MMD -MP -c -g -std=c11
CFLAGS  += -Os -Wall -fdata-sections -ffunction-sections -nostdlib -nostartfiles
CFLAGS  += $(USR_DEFINE) $(INC_DIRS)
CFLAGS  += $(COMMON_FLG)
CFLAGS  += -DVECT_TAB_BASE=$(VECT_TAB_BASE) -DDEBUG_LEVEL=$(DEBUG_LEVEL)

LDFLAGS  = $(COMMON_FLG) -Wl,--cref,--gc-sections

navilos_base = $(OUTPUT_DIR)/$(OUTNAME)
navilos = $(navilos_base).axf
navilos_bin = $(navilos_base).bin

.PHONY: all clean run prebuild

all: $(navilos)

prebuild:
	@echo Create outout direcotry..
	@mkdir -p $(OUT_DIR)

clean:
	@echo Clean..
	@rm $(OUT_DIR) -Rf

$(navilos): prebuild $(ASM_OBJS) $(CC_OBJS) $(C_OBJS) Makefile
	@echo Linking..
	@echo Generate binary.. $(navilos_bin)
	@$(CC) $(COMMON_FLG) $(ASM_OBJS) $(CC_OBJS) $(C_OBJS) -o $(navilos) -n -T $(LINKER_SCRIPT) -Wl,-Map=$(MAP_FILE) $(LDFLAGS)
	@$(OD) -h -S -C -r $(navilos) > $(navilos_base).s
	@$(OC) -O binary $(navilos) $(navilos_bin)
	@$(ELFSIZE) -B $(navilos)

$(OUTPUT_DIR)/%.os: %.S
	@echo Compile $<
	@$(CC) $(CFLAGS) -o $@ $<

$(OUTPUT_DIR)/%.o: %.c
	@echo Compile $<
	@$(CC) $(CFLAGS) -o $@ $<

-include $(patsubst %.c,$(OUTPUT_DIR)/%.d,$(C_SRCS))

