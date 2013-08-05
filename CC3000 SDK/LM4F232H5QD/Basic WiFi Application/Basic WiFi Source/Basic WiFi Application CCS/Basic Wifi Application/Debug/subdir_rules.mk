################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
basic_wif_ccs.obj: ../basic_wif_ccs.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: TMS470 Compiler'
	"C:/TI/ccsv5/tools/compiler/arm_4.9.7/bin/cl470" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="C:/TI/ccsv5/tools/compiler/arm_4.9.7/include" --include_path="C:/StellarisWare" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4F232H5QD/Basic WiFi Application/Source/inc" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4F232H5QD/Basic WiFi Application/Basic WiFi Application CCS/Basic Wifi Application" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4F232H5QD/Basic WiFi Application/Source/CC3000 Spi" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4F232H5QD/Basic WiFi Application/Source/CC3000HostDriver" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4F232H5QD/Basic WiFi Application/Source/HyperTerminal Driver" --gcc --define=ccs --define=__CCS__ --define=PART_LM4F232H5QD --diag_warning=225 --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="basic_wif_ccs.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

basic_wifi_application.obj: Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4F232H5QD/Basic\ WiFi\ Application/Source/Basic\ Wifi\ Application/basic_wifi_application.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: TMS470 Compiler'
	"C:/TI/ccsv5/tools/compiler/arm_4.9.7/bin/cl470" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="C:/TI/ccsv5/tools/compiler/arm_4.9.7/include" --include_path="C:/StellarisWare" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4F232H5QD/Basic WiFi Application/Source/inc" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4F232H5QD/Basic WiFi Application/Basic WiFi Application CCS/Basic Wifi Application" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4F232H5QD/Basic WiFi Application/Source/CC3000 Spi" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4F232H5QD/Basic WiFi Application/Source/CC3000HostDriver" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4F232H5QD/Basic WiFi Application/Source/HyperTerminal Driver" --gcc --define=ccs --define=__CCS__ --define=PART_LM4F232H5QD --diag_warning=225 --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="basic_wifi_application.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

board.obj: Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4F232H5QD/Basic\ WiFi\ Application/Source/Basic\ Wifi\ Application/board.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: TMS470 Compiler'
	"C:/TI/ccsv5/tools/compiler/arm_4.9.7/bin/cl470" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="C:/TI/ccsv5/tools/compiler/arm_4.9.7/include" --include_path="C:/StellarisWare" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4F232H5QD/Basic WiFi Application/Source/inc" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4F232H5QD/Basic WiFi Application/Basic WiFi Application CCS/Basic Wifi Application" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4F232H5QD/Basic WiFi Application/Source/CC3000 Spi" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4F232H5QD/Basic WiFi Application/Source/CC3000HostDriver" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4F232H5QD/Basic WiFi Application/Source/HyperTerminal Driver" --gcc --define=ccs --define=__CCS__ --define=PART_LM4F232H5QD --diag_warning=225 --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="board.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


