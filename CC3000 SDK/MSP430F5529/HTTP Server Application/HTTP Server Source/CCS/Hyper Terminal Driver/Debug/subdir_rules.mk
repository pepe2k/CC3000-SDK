################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
dispatcher.obj: C:/Users/a0132173/Downloads/CC3000\ sources/MSP430F5529/HTTP\ server\ on\ 1.11/Source/Hyper\ Terminal\ Driver/dispatcher.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs5.2/ccsv5/tools/compiler/msp430_4.1.1/bin/cl430" -vmspx --abi=coffabi -O2 -g --include_path="C:/ti/ccs5.2/ccsv5/ccs_base/msp430/include" --include_path="C:/ti/ccs5.2/ccsv5/tools/compiler/msp430_4.1.1/include" --advice:power=all --define=__MSP430F5529__ --define=__CCS__ --diag_warning=225 --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="dispatcher.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


