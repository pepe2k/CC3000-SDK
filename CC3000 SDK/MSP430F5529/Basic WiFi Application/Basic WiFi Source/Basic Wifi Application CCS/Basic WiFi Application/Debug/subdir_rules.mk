################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
basic_wifi_application.obj: C:/SimpleLink_1.0.0.0.33/MSP4305529/Original/Original/Basic\ Wifi\ Application/Source/Basic\ Wifi\ Application/basic_wifi_application.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/TI/ccsv5/tools/compiler/msp430_4.1.3/bin/cl430" -vmspx --abi=eabi -g --include_path="C:/SimpleLink_1.0.0.0.33/MSP4305529/Original/Original/Basic Wifi Application/Source/inc" --include_path="C:/SimpleLink_1.0.0.0.33/MSP4305529/Original/Original/Basic Wifi Application/Source/CC3000 Spi" --include_path="C:/SimpleLink_1.0.0.0.33/MSP4305529/Original/Original/Basic Wifi Application/Source/Hyper Terminal Driver" --include_path="C:/TI/ccsv5/ccs_base/msp430/include" --include_path="C:/SimpleLink_1.0.0.0.33/MSP4305529/Original/Original/Basic Wifi Application/Source/CC3000HostDriver" --include_path="C:/TI/ccsv5/tools/compiler/msp430_4.1.3/include" --advice:power="all" --define=__MSP430F5529__ --define=__CCS__ --diag_warning=225 --display_error_number --diag_wrap=off --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="basic_wifi_application.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

board.obj: C:/SimpleLink_1.0.0.0.33/MSP4305529/Original/Original/Basic\ Wifi\ Application/Source/Basic\ Wifi\ Application/board.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/TI/ccsv5/tools/compiler/msp430_4.1.3/bin/cl430" -vmspx --abi=eabi -g --include_path="C:/SimpleLink_1.0.0.0.33/MSP4305529/Original/Original/Basic Wifi Application/Source/inc" --include_path="C:/SimpleLink_1.0.0.0.33/MSP4305529/Original/Original/Basic Wifi Application/Source/CC3000 Spi" --include_path="C:/SimpleLink_1.0.0.0.33/MSP4305529/Original/Original/Basic Wifi Application/Source/Hyper Terminal Driver" --include_path="C:/TI/ccsv5/ccs_base/msp430/include" --include_path="C:/SimpleLink_1.0.0.0.33/MSP4305529/Original/Original/Basic Wifi Application/Source/CC3000HostDriver" --include_path="C:/TI/ccsv5/tools/compiler/msp430_4.1.3/include" --advice:power="all" --define=__MSP430F5529__ --define=__CCS__ --diag_warning=225 --display_error_number --diag_wrap=off --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="board.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


