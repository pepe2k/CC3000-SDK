################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
board.obj: C:/Users/a0132173/Downloads/Single\ Installer/New\ folder/Source/Simplemail/board.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs5.2/ccsv5/tools/compiler/msp430_4.1.1/bin/cl430" -vmspx --abi=coffabi --opt_for_speed=0 -g --include_path="C:/ti/ccs5.2/ccsv5/ccs_base/msp430/include" --include_path="C:/Users/a0132173/Downloads/Single Installer/New folder/Source/CC3000 Spi" --include_path="C:/Users/a0132173/Downloads/Single Installer/New folder/Source/HyperTerminal Driver" --include_path="C:/Users/a0132173/Downloads/Single Installer/New folder/Source/CC3000HostDriver" --include_path="C:/Users/a0132173/Downloads/Single Installer/New folder/Source/Simplemail/inc" --include_path="C:/ti/ccs5.2/ccsv5/tools/compiler/msp430_4.1.1/include" --advice:power="all,-1,-2,-3,-4,-5,-6,-7,-8,-9,-10,-11,-12,-13,-14,-15" --define=__MSP430FR5739__ --define=__CCS__ --define=CC3000_UNENCRYPTED_SMART_CONFIG --diag_warning=225 --display_error_number --no_warnings --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="board.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

cc3000.obj: C:/Users/a0132173/Downloads/Single\ Installer/New\ folder/Source/Simplemail/cc3000.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs5.2/ccsv5/tools/compiler/msp430_4.1.1/bin/cl430" -vmspx --abi=coffabi --opt_for_speed=0 -g --include_path="C:/ti/ccs5.2/ccsv5/ccs_base/msp430/include" --include_path="C:/Users/a0132173/Downloads/Single Installer/New folder/Source/CC3000 Spi" --include_path="C:/Users/a0132173/Downloads/Single Installer/New folder/Source/HyperTerminal Driver" --include_path="C:/Users/a0132173/Downloads/Single Installer/New folder/Source/CC3000HostDriver" --include_path="C:/Users/a0132173/Downloads/Single Installer/New folder/Source/Simplemail/inc" --include_path="C:/ti/ccs5.2/ccsv5/tools/compiler/msp430_4.1.1/include" --advice:power="all,-1,-2,-3,-4,-5,-6,-7,-8,-9,-10,-11,-12,-13,-14,-15" --define=__MSP430FR5739__ --define=__CCS__ --define=CC3000_UNENCRYPTED_SMART_CONFIG --diag_warning=225 --display_error_number --no_warnings --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="cc3000.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

demo.obj: C:/Users/a0132173/Downloads/Single\ Installer/New\ folder/Source/Simplemail/demo.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs5.2/ccsv5/tools/compiler/msp430_4.1.1/bin/cl430" -vmspx --abi=coffabi --opt_for_speed=0 -g --include_path="C:/ti/ccs5.2/ccsv5/ccs_base/msp430/include" --include_path="C:/Users/a0132173/Downloads/Single Installer/New folder/Source/CC3000 Spi" --include_path="C:/Users/a0132173/Downloads/Single Installer/New folder/Source/HyperTerminal Driver" --include_path="C:/Users/a0132173/Downloads/Single Installer/New folder/Source/CC3000HostDriver" --include_path="C:/Users/a0132173/Downloads/Single Installer/New folder/Source/Simplemail/inc" --include_path="C:/ti/ccs5.2/ccsv5/tools/compiler/msp430_4.1.1/include" --advice:power="all,-1,-2,-3,-4,-5,-6,-7,-8,-9,-10,-11,-12,-13,-14,-15" --define=__MSP430FR5739__ --define=__CCS__ --define=CC3000_UNENCRYPTED_SMART_CONFIG --diag_warning=225 --display_error_number --no_warnings --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="demo.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

smtp.obj: C:/Users/a0132173/Downloads/Single\ Installer/New\ folder/Source/Simplemail/smtp.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs5.2/ccsv5/tools/compiler/msp430_4.1.1/bin/cl430" -vmspx --abi=coffabi --opt_for_speed=0 -g --include_path="C:/ti/ccs5.2/ccsv5/ccs_base/msp430/include" --include_path="C:/Users/a0132173/Downloads/Single Installer/New folder/Source/CC3000 Spi" --include_path="C:/Users/a0132173/Downloads/Single Installer/New folder/Source/HyperTerminal Driver" --include_path="C:/Users/a0132173/Downloads/Single Installer/New folder/Source/CC3000HostDriver" --include_path="C:/Users/a0132173/Downloads/Single Installer/New folder/Source/Simplemail/inc" --include_path="C:/ti/ccs5.2/ccsv5/tools/compiler/msp430_4.1.1/include" --advice:power="all,-1,-2,-3,-4,-5,-6,-7,-8,-9,-10,-11,-12,-13,-14,-15" --define=__MSP430FR5739__ --define=__CCS__ --define=CC3000_UNENCRYPTED_SMART_CONFIG --diag_warning=225 --display_error_number --no_warnings --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="smtp.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


