################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccs5.2/ccsv5/tools/compiler/msp430_4.1.1

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./Board/board.obj" \
"./Board/Wheel.obj" \
"./MD5/md5.obj" \
"./cc3000.obj" \
"./WebServerApp.obj" \
"./HttpString.obj" \
"./HttpStatic.obj" \
"./HttpDynamicHandler.obj" \
"./HttpDynamic.obj" \
"./HttpDebug.obj" \
"./HttpCore.obj" \
"./HttpAuth.obj" \
"./FlashDbContent.obj" \
"./FlashDB.obj" \
-l"libc.a" \
"../lnk_msp430f5529.cmd" \
"C:/Users/a0132173/Downloads/CC3000 sources/MSP430F5529/HTTP server on 1.11/CCS/CC3000HostDriver/Debug/CC3000HostDriver.lib" \
"C:/Users/a0132173/Downloads/CC3000 sources/MSP430F5529/HTTP server on 1.11/CCS/CC3000Spi/Debug/CC3000Spi.lib" \
"C:/Users/a0132173/Downloads/CC3000 sources/MSP430F5529/HTTP server on 1.11/CCS/Hyper Terminal Driver/Debug/Hyper Terminal Driver.lib" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include MD5/subdir_vars.mk
-include Library/subdir_vars.mk
-include Board/subdir_vars.mk
-include subdir_rules.mk
-include MD5/subdir_rules.mk
-include Library/subdir_rules.mk
-include Board/subdir_rules.mk
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
all: HTTP\ Server.out

# Tool invocations
HTTP\ Server.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: MSP430 Linker'
	"C:/ti/ccs5.2/ccsv5/tools/compiler/msp430_4.1.1/bin/cl430" -vmspx --abi=coffabi -O2 -g --advice:power="all" --gcc --define=__MSP430F5529__ --define=__CCS__ --diag_warning=225 --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=minimal -z --stack_size=400 -m"HTTP Server.map" --heap_size=80 --use_hw_mpy=F5 -i"C:/ti/ccs5.2/ccsv5/ccs_base/msp430/include" -i"C:/ti/ccs5.2/ccsv5/tools/compiler/msp430_4.1.1/lib" -i"C:/ti/ccs5.2/ccsv5/tools/compiler/msp430_4.1.1/include" --reread_libs --warn_sections --display_error_number --rom_model -o "HTTP Server.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(MSP430_EXECUTABLE_OUTPUTS__QUOTED) "HTTP Server.out"
	-$(RM) "FlashDB.pp" "FlashDbContent.pp" "HttpAuth.pp" "HttpCore.pp" "HttpDebug.pp" "HttpDynamic.pp" "HttpDynamicHandler.pp" "HttpStatic.pp" "HttpString.pp" "WebServerApp.pp" "cc3000.pp" "MD5\md5.pp" "Board\Wheel.pp" "Board\board.pp" 
	-$(RM) "FlashDB.obj" "FlashDbContent.obj" "HttpAuth.obj" "HttpCore.obj" "HttpDebug.obj" "HttpDynamic.obj" "HttpDynamicHandler.obj" "HttpStatic.obj" "HttpString.obj" "WebServerApp.obj" "cc3000.obj" "MD5\md5.obj" "Board\Wheel.obj" "Board\board.obj" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
