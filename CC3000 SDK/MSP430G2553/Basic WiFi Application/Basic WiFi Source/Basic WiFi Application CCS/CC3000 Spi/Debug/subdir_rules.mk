################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
spi.obj: Z:/ti/SimpleLinkHostFolder/MSP430/Apps/MSP430G2553/Basic\ WiFi\ Application/Source/CC3000\ Spi/spi.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/TI/ccsv5/tools/compiler/msp430_4.1.3/bin/cl430" -vmsp --abi=eabi -O2 --opt_for_speed=0 -g --include_path="C:/TI/ccsv5/ccs_base/msp430/include" --include_path="Z:/ti/SimpleLinkHostFolder/MSP430/Apps/MSP430G2553/Basic WiFi Application/Source/BasicWiFi Application" --include_path="Z:/ti/SimpleLinkHostFolder/MSP430/Apps/MSP430G2553/Basic WiFi Application/Source/inc" --include_path="Z:/ti/SimpleLinkHostFolder/MSP430/Apps/MSP430G2553/Basic WiFi Application/Source/CC3000HostDriver" --include_path="C:/TI/ccsv5/tools/compiler/msp430_4.1.3/include" --advice:power="all" --define=__MSP430G2553__ --define=CC3000_UNENCRYPTED_SMART_CONFIG --define=CC3000_TINY_DRIVER --define=__CCS__ --diag_warning=225 --display_error_number --printf_support=minimal --preproc_with_compile --preproc_dependency="spi.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


