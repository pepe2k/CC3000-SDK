################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv5/tools/compiler/arm_5.0.1

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./wifi_application_multithread.obj" \
"./uartstdio.obj" \
"./board.obj" \
-l"libc.a" \
"C:/StellarisWare/driverlib/ccs-cm4f/Debug/driverlib-cm4f.lib" \
"../lm4f120h5qr.cmd" \
"C:/00_A0131814/00_Praneet/00_CCxxxx/Apps/00_ReleaseStructure/Stellaris/Apps/LM4f120H5QR/Multithreaded WiFi Application CCS/mt_cc3000_host_driver/Debug/mt_cc3000_host_driver.lib" \
"C:/00_A0131814/00_Praneet/00_CCxxxx/Apps/00_ReleaseStructure/Stellaris/Apps/LM4f120H5QR/Multithreaded WiFi Application CCS/mt_cc3000_spi/Debug/mt_cc3000_spi.lib" \
"C:/00_A0131814/00_Praneet/00_CCxxxx/Apps/00_ReleaseStructure/Stellaris/Apps/LM4f120H5QR/Multithreaded WiFi Application CCS/mt_impl_multi_thread/Debug/mt_impl_multi_thread.lib" \
"C:/00_A0131814/00_Praneet/00_CCxxxx/Apps/00_ReleaseStructure/Stellaris/Apps/LM4f120H5QR/Multithreaded WiFi Application CCS/mt_osal/Debug/mt_osal.lib" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include subdir_rules.mk
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
all: mt_wifi_app_multi_thread.out

# Tool invocations
mt_wifi_app_multi_thread.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Linker'
	"C:/ti/ccsv5/tools/compiler/arm_5.0.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -g --define=__CCS__ --define=BUILD_AS_EXE --define=PART_LM4F120H5QR --define=TARGET_IS_BLIZZARD_RA1 --define=MULTITHREAD_SUPPORT --define=__ENABLE_MULTITHREADED_SUPPORT__ --define=ENABLE_SMART_CONFIG --diag_warning=225 --display_error_number -z --stack_size=256 -m"mt_wifi_app_multi_thread.map" --heap_size=0 -i"C:/ti/ccsv5/tools/compiler/arm_5.0.1/lib" -i"C:/ti/ccsv5/tools/compiler/arm_5.0.1/include" --reread_libs --warn_sections --display_error_number --rom_model -o "mt_wifi_app_multi_thread.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(TMS470_EXECUTABLE_OUTPUTS__QUOTED)$(GEN_SRCS__QUOTED)$(GEN_MISC_FILES__QUOTED)$(GEN_OPTS__QUOTED)$(GEN_CMDS__QUOTED) "mt_wifi_app_multi_thread.out"
	-$(RMDIR) $(GEN_MISC_DIRS__QUOTED)
	-$(RM) "board.pp" "uartstdio.pp" "wifi_application_multithread.pp" 
	-$(RM) "board.obj" "uartstdio.obj" "wifi_application_multithread.obj" 
	-@echo 'Finished clean'
	-@echo ' '

post-build:
	-@echo 'Create flash image: TI-TXT'
	-"C:/ti/ccsv5/tools/compiler/arm_5.0.1/bin/armhex" --ti_txt "mt_wifi_app_multi_thread.out" -o "mt_wifi_app_multi_thread.txt" -order MS -romwidth 16
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
