################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
basic_wifi_application.obj: Z:/ti/SimpleLinkHostFolder/MSP430/Apps/MSP430FR5739/Basic\ WiFi\ Application/Source/BasicWiFi\ Application/basic_wifi_application.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/TI/ccsv5/tools/compiler/msp430_4.1.3/bin/cl430" -vmspx --abi=coffabi -O4 --opt_for_speed=0 -g --include_path="C:/TI/ccsv5/ccs_base/msp430/include" --include_path="Z:/ti/SimpleLinkHostFolder/MSP430/Apps/MSP430FR5739/Basic WiFi Application/Source/CC3000 Spi" --include_path="Z:/ti/SimpleLinkHostFolder/MSP430/Apps/MSP430FR5739/Basic WiFi Application/Source/inc" --include_path="Z:/ti/SimpleLinkHostFolder/MSP430/Apps/MSP430FR5739/Basic WiFi Application/Source/HyperTerminal Driver" --include_path="Z:/ti/SimpleLinkHostFolder/MSP430/Apps/MSP430FR5739/Basic WiFi Application/Source/CC3000HostDriver" --include_path="C:/TI/ccsv5/tools/compiler/msp430_4.1.3/include" --gcc --define=__CCS__ --define=__MSP430FR5739__ --diag_warning=225 --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="basic_wifi_application.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

board.obj: Z:/ti/SimpleLinkHostFolder/MSP430/Apps/MSP430FR5739/Basic\ WiFi\ Application/Source/BasicWiFi\ Application/board.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/TI/ccsv5/tools/compiler/msp430_4.1.3/bin/cl430" -vmspx --abi=coffabi -O4 --opt_for_speed=0 -g --include_path="C:/TI/ccsv5/ccs_base/msp430/include" --include_path="Z:/ti/SimpleLinkHostFolder/MSP430/Apps/MSP430FR5739/Basic WiFi Application/Source/CC3000 Spi" --include_path="Z:/ti/SimpleLinkHostFolder/MSP430/Apps/MSP430FR5739/Basic WiFi Application/Source/inc" --include_path="Z:/ti/SimpleLinkHostFolder/MSP430/Apps/MSP430FR5739/Basic WiFi Application/Source/HyperTerminal Driver" --include_path="Z:/ti/SimpleLinkHostFolder/MSP430/Apps/MSP430FR5739/Basic WiFi Application/Source/CC3000HostDriver" --include_path="C:/TI/ccsv5/tools/compiler/msp430_4.1.3/include" --gcc --define=__CCS__ --define=__MSP430FR5739__ --diag_warning=225 --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="board.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


