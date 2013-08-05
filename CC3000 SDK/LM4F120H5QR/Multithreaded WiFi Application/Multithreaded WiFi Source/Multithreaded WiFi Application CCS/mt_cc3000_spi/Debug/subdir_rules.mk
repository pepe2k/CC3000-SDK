################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
spi.obj: C:/00_A0131814/00_Praneet/00_CCxxxx/Apps/00_ReleaseStructure/Stellaris/Apps/LM4f120H5QR/Source/cc3000_spi/src/spi.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/arm_5.0.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -g --include_path="C:/ti/ccsv5/tools/compiler/arm_5.0.1/include" --include_path="../../../Source/cc3000_spi/inc" --include_path="../../../Source/cc3000_host_driver/core_driver/inc" --include_path="../../../Source/cc3000_host_driver/include/common" --include_path="C:/StellarisWare" --include_path="C:/StellarisWare/inc" --include_path="C:/StellarisWare/driverlib/" --define=__CCS__ --define=PART_LM4F120H5QR --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="spi.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


