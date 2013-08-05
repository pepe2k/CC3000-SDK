################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccs5.2/ccsv5/tools/compiler/msp430_4.1.1

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./uart.obj" \
"./terminal.obj" \
"./strlib.obj" \
"./server.obj" \
"./sensors.obj" \
"./demo.obj" \
"./cc3000.obj" \
"./board.obj" \
-l"libc.a" \
"../lnk_msp430fr5739.cmd" \
"C:/Users/a0132173/Downloads/CC3000 sources/Releases/Home Automation/Home Automation Updated/Home Automation Application/Automation App/HomeAutomationAppCCS/CC3000 Host Driver/Debug/CC3000 Host Driver.lib" \
"C:/Users/a0132173/Downloads/CC3000 sources/Releases/Home Automation/Home Automation Updated/Home Automation Application/Automation App/HomeAutomationAppCCS/CC3000 Spi/Debug/CC3000 Spi.lib" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include Library/subdir_vars.mk
-include subdir_rules.mk
-include Library/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: CC3000\ Automation\ App.out

# Tool invocations
CC3000\ Automation\ App.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: MSP430 Linker'
	"C:/ti/ccs5.2/ccsv5/tools/compiler/msp430_4.1.1/bin/cl430" -vmspx --abi=coffabi -g --advice:power="all" --define=__MSP430FR5739__ --define=__CCS__ --diag_warning=225 --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=minimal -z --stack_size=160 -m"CC3000 Automation App.map" --heap_size=160 --use_hw_mpy=F5 -i"C:/ti/ccs5.2/ccsv5/ccs_base/msp430/include" -i"C:/ti/ccs5.2/ccsv5/tools/compiler/msp430_4.1.1/lib" -i"C:/ti/ccs5.2/ccsv5/tools/compiler/msp430_4.1.1/include" --reread_libs --warn_sections --display_error_number --rom_model -o "CC3000 Automation App.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(MSP430_EXECUTABLE_OUTPUTS__QUOTED) "CC3000 Automation App.out"
	-$(RM) "board.pp" "cc3000.pp" "demo.pp" "sensors.pp" "server.pp" "strlib.pp" "terminal.pp" "uart.pp" 
	-$(RM) "board.obj" "cc3000.obj" "demo.obj" "sensors.obj" "server.obj" "strlib.obj" "terminal.obj" "uart.obj" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
