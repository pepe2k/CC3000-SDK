################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
basic_wif_ccs.obj: ../basic_wif_ccs.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/TI/ccsv5/tools/compiler/arm_4.9.7/bin/cl470" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -g --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4f120H5QR/Basic WiFi Application/Source/cc3000_spi/inc" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4f120H5QR/Basic WiFi Application/Source/cc3000_host_driver/core_driver/inc" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4f120H5QR/Basic WiFi Application/Source/cc3000_host_driver/include/common" --include_path="C:/TI/ccsv5/tools/compiler/arm_4.9.7/include" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4f120H5QR/Basic WiFi Application/Basic WiFi Application CCS/Basic Wifi Application" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4f120H5QR/Basic WiFi Application/Source/HyperTerminal Driver" --include_path="C:/StellarisWare" --define=__CCS__ --define=PART_LM4F232H5QD=1 --define=TARGET_IS_BLIZZARD_RA1=1 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="basic_wif_ccs.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

basic_wifi_application.obj: Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4f120H5QR/Basic\ WiFi\ Application/Source/Basic\ Wifi\ Application/basic_wifi_application.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/TI/ccsv5/tools/compiler/arm_4.9.7/bin/cl470" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -g --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4f120H5QR/Basic WiFi Application/Source/cc3000_spi/inc" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4f120H5QR/Basic WiFi Application/Source/cc3000_host_driver/core_driver/inc" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4f120H5QR/Basic WiFi Application/Source/cc3000_host_driver/include/common" --include_path="C:/TI/ccsv5/tools/compiler/arm_4.9.7/include" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4f120H5QR/Basic WiFi Application/Basic WiFi Application CCS/Basic Wifi Application" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4f120H5QR/Basic WiFi Application/Source/HyperTerminal Driver" --include_path="C:/StellarisWare" --define=__CCS__ --define=PART_LM4F232H5QD=1 --define=TARGET_IS_BLIZZARD_RA1=1 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="basic_wifi_application.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

board.obj: Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4f120H5QR/Basic\ WiFi\ Application/Source/Basic\ Wifi\ Application/board.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/TI/ccsv5/tools/compiler/arm_4.9.7/bin/cl470" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -g --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4f120H5QR/Basic WiFi Application/Source/cc3000_spi/inc" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4f120H5QR/Basic WiFi Application/Source/cc3000_host_driver/core_driver/inc" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4f120H5QR/Basic WiFi Application/Source/cc3000_host_driver/include/common" --include_path="C:/TI/ccsv5/tools/compiler/arm_4.9.7/include" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4f120H5QR/Basic WiFi Application/Basic WiFi Application CCS/Basic Wifi Application" --include_path="Z:/ti/SimpleLinkHostFolder/Stellaris/Apps/LM4f120H5QR/Basic WiFi Application/Source/HyperTerminal Driver" --include_path="C:/StellarisWare" --define=__CCS__ --define=PART_LM4F232H5QD=1 --define=TARGET_IS_BLIZZARD_RA1=1 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="board.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


