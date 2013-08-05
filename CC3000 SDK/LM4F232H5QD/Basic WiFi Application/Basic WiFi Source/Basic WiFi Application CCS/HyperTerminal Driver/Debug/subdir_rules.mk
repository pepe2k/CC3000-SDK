################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
dispatcher.obj: Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4F232H5QD/Basic\ WiFi\ Application/Source/HyperTerminal\ Driver/dispatcher.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: TMS470 Compiler'
	"C:/TI/ccsv5/tools/compiler/tms470/bin/cl470" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="C:/TI/ccsv5/tools/compiler/tms470/include" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4F232H5QD/Basic WiFi Application/Source/CC3000HostDriver" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4F232H5QD/Basic WiFi Application/Source/inc" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4F232H5QD/Basic WiFi Application/Source/HyperTerminal Driver" --include_path="C:/StellarisWare" --gcc --define=ccs --define=__CCS__=1 --define=PART_LM4F232H5QD=1 --define=TARGET_IS_BLIZZARD_RA1=1 --diag_warning=225 --display_error_number --ual --preproc_with_compile --preproc_dependency="dispatcher.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


