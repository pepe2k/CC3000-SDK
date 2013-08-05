################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
dispatcher.obj: Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4f120H5QR/Basic\ WiFi\ Application/Source/HyperTerminal\ Driver/dispatcher.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/TI/ccsv5/tools/compiler/arm_4.9.7/bin/cl470" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -g --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4f120H5QR/Basic WiFi Application/Source/cc3000_host_driver/include/common" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4f120H5QR/Basic WiFi Application/Source/cc3000_host_driver/core_driver/inc" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4f120H5QR/Basic WiFi Application/Source/cc3000_spi/inc" --include_path="C:/TI/ccsv5/tools/compiler/arm_4.9.7/include" --include_path="C:/StellarisWare" --define=__CCS__ --define=TARGET_IS_BLIZZARD_RA1=1 --define=PART_LM4F232H5QD=1 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="dispatcher.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


