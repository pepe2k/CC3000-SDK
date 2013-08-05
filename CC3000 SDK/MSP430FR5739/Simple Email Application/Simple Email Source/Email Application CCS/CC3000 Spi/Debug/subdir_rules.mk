################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
spi.obj: C:/Users/a0132173/Downloads/Single\ Installer/New\ folder/Source/CC3000\ Spi/spi.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs5.2/ccsv5/tools/compiler/msp430_4.1.1/bin/cl430" -vmspx --abi=coffabi -O4 --opt_for_speed=0 -g --include_path="C:/ti/ccs5.2/ccsv5/ccs_base/msp430/include" --include_path="C:/Users/a0132173/Downloads/Single Installer/New folder/Source/inc" --include_path="C:/ti/ccs5.2/ccsv5/tools/compiler/msp430_4.1.1/include" --include_path="C:/Users/a0132173/Downloads/Single Installer/New folder/Source/CC3000HostDriver" --include_path="C:/Users/a0132173/Downloads/Single Installer/New folder/Source/Simplemail/inc" --define=__CCS__ --define=CC3000_UNENCRYPTED_SMART_CONFIG --define=__MSP430FR5739__ --diag_warning=225 --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="spi.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


