##############################################################################
# examples of invoking this Makefile:
# building configurations: Debug (default), and Release
# make
# make CONF=rel
# make CONF=spy
#
# cleaning configurations: Debug (default), and Release
# make clean
# make CONF=rel clean

#-----------------------------------------------------------------------------
# general utilities
#
RM = del

#-----------------------------------------------------------------------------
# tools
#
ifeq ($(GNU_MSP430),)
GNU_MSP430 = /opt/msp430
endif

CC    := $(GNU_MSP430)/bin/msp430-gcc
CPP   := $(GNU_MSP430)/bin/msp430-g++
AS    := $(GNU_MSP430)/bin/msp430-as
LINK  := $(GNU_MSP430)/bin/msp430-gcc
BIN   := $(GNU_MSP430)/bin/msp430-objcopy
RM    := rm -rf
MKDIR := mkdir

#-----------------------------------------------------------------------------
# directories
#
APP_DIR := .
LIB_DIR :=

# source directories
VPATH = $(APP_DIR)/BasicWiFi \
	$(APP_DIR)/CC3000_Spi \
	$(APP_DIR)/../Source/CC3000HostDriver \
	$(APP_DIR)/HyperTerminal_Driver \

# Output file basename
OUTPUT := BasicWiFiApp

# include directories
INCLUDES = -I$(APP_DIR)/BasicWiFi \
	-I$(APP_DIR)/CC3000_Spi \
	-I$(APP_DIR)/../Source/CC3000HostDriver \
	-I$(APP_DIR)/HyperTerminal_Driver \
	-I$(APP_DIR)/../Source/inc

# defines
DEFINES = -D__CCS__ -DCC3000_TINY_DRIVER -DCC3000_UNENCRYPTED_SMART_CONFIG

MSP430_CORE = 430
MSP430_MCU = msp430fr5739

#-----------------------------------------------------------------------------
# files
#

# assembler source files
ASM_SRCS :=

# C source files
C_SRCS := $(wildcard $(APP_DIR)/BasicWiFi/*.c) \
	$(wildcard $(APP_DIR)/CC3000_Spi/*.c) \
	$(wildcard $(APP_DIR)/../Source/CC3000HostDriver/*.c) \
	$(wildcard $(APP_DIR)/HyperTerminal_Driver/*.c)

LIBS :=

#-----------------------------------------------------------------------------
# build options for various configurations
#

ifeq (rel, $(CONF))       # Release configuration ............................

BIN_DIR := rel

ASFLAGS = -mmcu=$(MSP430_MCU) -mcpu=$(MSP430_CORE)

CFLAGS = -mmcu=$(MSP430_MCU) -mcpu=$(MSP430_CORE) -Wall \
	-Os $(INCLUDES) $(DEFINES) -DNDEBUG

CPPFLAGS = -mmcu=$(MSP430_MCU) -mcpu=$(MSP430_CORE) \
	-Wall -fno-rtti -fno-exceptions \
	-Os $(INCLUDES) $(DEFINES) -DNDEBUG

LINKFLAGS = -mmcu=$(MSP430_MCU) -mcpu=$(MSP430_CORE) \
  -Xlinker \
	-Map=$(BIN_DIR)/$(OUTPUT).map \
	-L$(QP_PORT_DIR)/$(BIN_DIR) \
	-L$(GNU_MSP430)/msp430/lib/ldscripts/$(MSP430_MCU)

else                     # default Debug configuration .......................

BIN_DIR := dbg

ASFLAGS = -g -mcpu=$(MSP430_CORE)

CFLAGS = -mmcu=$(MSP430_MCU) -mcpu=$(MSP430_CORE) -Wall \
	-O -g -O $(INCLUDES) $(DEFINES)

CPPFLAGS = -mmcu=$(MSP430_MCU) -mcpu=$(MSP430_CORE) \
	-Wall -fno-rtti -fno-exceptions \
	-g -O $(INCLUDES) $(DEFINES)

LINKFLAGS = -mmcu=$(MSP430_MCU) -mcpu=$(MSP430_CORE) \
  -Xlinker \
	-Map=$(BIN_DIR)/$(OUTPUT).map \
	-L$(QP_PORT_DIR)/$(BIN_DIR) \
	-L$(GNU_MSP430)/msp430/lib/ldscripts/$(MSP430_MCU)

endif


ASM_OBJS     := $(patsubst %.s,%.o,$(ASM_SRCS))
C_OBJS       := $(patsubst %.c,%.o,$(notdir $(C_SRCS)))
CPP_OBJS     := $(patsubst %.cpp,%.o,$(CPP_SRCS))

TARGET_BIN   := $(BIN_DIR)/$(OUTPUT).hex
TARGET_ELF   := $(BIN_DIR)/$(OUTPUT).elf
ASM_OBJS_EXT := $(addprefix $(BIN_DIR)/, $(ASM_OBJS))
C_OBJS_EXT   := $(addprefix $(BIN_DIR)/, $(C_OBJS))
C_DEPS_EXT   := $(patsubst %.o, %.d, $(C_OBJS_EXT))
CPP_OBJS_EXT := $(addprefix $(BIN_DIR)/, $(CPP_OBJS))
CPP_DEPS_EXT := $(patsubst %.o, %.d, $(CPP_OBJS_EXT))


#-----------------------------------------------------------------------------
# rules
#

all: $(BIN_DIR) $(TARGET_BIN)

$(BIN_DIR):
	@echo
	mkdir -p $@

$(TARGET_BIN): $(TARGET_ELF)
	$(BIN) -O ihex $< $@

$(TARGET_ELF) : $(ASM_OBJS_EXT) $(C_OBJS_EXT) $(CPP_OBJS_EXT)
	$(LINK) $(LINKFLAGS) -o $@ $^ $(LIBS)

$(BIN_DIR)/%.d : %.c
	$(CC) -MM -MT $(@:.d=.o) $(CFLAGS) $< > $(BIN_DIR)/$(@F)

$(BIN_DIR)/%.d : %.cpp
	$(CPP) -MM -MT $(@:.d=.o) $(CPPFLAGS) $< > $(BIN_DIR)/$(@F)

$(BIN_DIR)/%.o : %.s
	$(AS) $(ASFLAGS) $< -o $(BIN_DIR)/$(@F)

$(BIN_DIR)/%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $(BIN_DIR)/$(@F)

-include $(C_DEPS_EXT) $(CPP_DEPS_EXT)


.PHONY : clean
clean:
	-$(RM) $(BIN_DIR)/*.o \
	$(BIN_DIR)/*.d \
	$(BIN_DIR)/*.hex \
	$(BIN_DIR)/*.elf \
	$(BIN_DIR)/*.map
	
	
show:
	@echo CONF = $(CONF)
	@echo ASM_SRCS = $(ASM_SRCS)
	@echo C_SRCS = $(C_SRCS)
	@echo CPP_SRCS = $(CPP_SRCS)
	@echo ASM_OBJS_EXT = $(ASM_OBJS_EXT)
	@echo C_OBJS_EXT = $(C_OBJS_EXT)
	@echo C_DEPS_EXT = $(C_DEPS_EXT)
	@echo CPP_DEPS_EXT = $(CPP_DEPS_EXT)
	@echo TARGET_ELF = $(TARGET_ELF)

burn:
	mspdebug rf2500 "prog $(BIN_DIR)/$(OUTPUT).elf"
