################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
board.obj: C:/00_A0131814/00_Praneet/00_CCxxxx/Apps/00_ReleaseStructure/Stellaris/Apps/LM4f120H5QR/Source/wifi_application_multithread/src/board.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/arm_5.0.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -g --include_path="C:/ti/ccsv5/tools/compiler/arm_5.0.1/include" --include_path="../../../Source/cc3000_host_driver/include" --include_path="../../../Source/cc3000_host_driver/include/common" --include_path="../../../Source/osal/inc" --include_path="../../../Source/osal/sys_bios/inc" --include_path="../../../Source/wifi_application_multithread/inc" --include_path="../../../Source/cc3000_spi/inc" --include_path="C:/StellarisWare" --include_path="C:/StellarisWare/driverlib/" --include_path="C:/StellarisWare/inc" --define=__CCS__ --define=BUILD_AS_EXE --define=PART_LM4F120H5QR --define=TARGET_IS_BLIZZARD_RA1 --define=MULTITHREAD_SUPPORT --define=__ENABLE_MULTITHREADED_SUPPORT__ --define=ENABLE_SMART_CONFIG --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="board.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

configPkg/compiler.opt: ../mt_wifi_app_multi_thread.cfg
	@echo 'Building file: $<'
	@echo 'Invoking: XDCtools'
	"C:/TI/xdctools_3_24_05_48/xs" --xdcpath="C:/TI/bios_6_34_02_18/packages;C:/TI/tirtos_1_01_00_25/packages;C:/TI/tirtos_1_01_00_25/products/bios_6_34_04_22/packages;C:/TI/tirtos_1_01_00_25/products/ipc_1_25_02_12/packages;C:/TI/tirtos_1_01_00_25/products/ndk_2_22_01_14/packages;C:/TI/tirtos_1_01_00_25/products/uia_1_02_00_07/packages;C:/TI/tirtos_1_01_00_25/products/xdctools_3_24_06_63/packages;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M4F -p ti.platforms.stellaris:LM4F120H5QR -r release -c "C:/ti/ccsv5/tools/compiler/arm_5.0.1" "$<"
	@echo 'Finished building: $<'
	@echo ' '

configPkg/linker.cmd: configPkg/compiler.opt
configPkg/: configPkg/compiler.opt

uartstdio.obj: C:/StellarisWare/utils/uartstdio.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/arm_5.0.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -g --include_path="C:/ti/ccsv5/tools/compiler/arm_5.0.1/include" --include_path="../../../Source/cc3000_host_driver/include" --include_path="../../../Source/cc3000_host_driver/include/common" --include_path="../../../Source/osal/inc" --include_path="../../../Source/osal/sys_bios/inc" --include_path="../../../Source/wifi_application_multithread/inc" --include_path="../../../Source/cc3000_spi/inc" --include_path="C:/StellarisWare" --include_path="C:/StellarisWare/driverlib/" --include_path="C:/StellarisWare/inc" --define=__CCS__ --define=BUILD_AS_EXE --define=PART_LM4F120H5QR --define=TARGET_IS_BLIZZARD_RA1 --define=MULTITHREAD_SUPPORT --define=__ENABLE_MULTITHREADED_SUPPORT__ --define=ENABLE_SMART_CONFIG --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="uartstdio.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

wifi_application_multithread.obj: C:/00_A0131814/00_Praneet/00_CCxxxx/Apps/00_ReleaseStructure/Stellaris/Apps/LM4f120H5QR/Source/wifi_application_multithread/src/wifi_application_multithread.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/arm_5.0.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -g --include_path="C:/ti/ccsv5/tools/compiler/arm_5.0.1/include" --include_path="../../../Source/cc3000_host_driver/include" --include_path="../../../Source/cc3000_host_driver/include/common" --include_path="../../../Source/osal/inc" --include_path="../../../Source/osal/sys_bios/inc" --include_path="../../../Source/wifi_application_multithread/inc" --include_path="../../../Source/cc3000_spi/inc" --include_path="C:/StellarisWare" --include_path="C:/StellarisWare/driverlib/" --include_path="C:/StellarisWare/inc" --define=__CCS__ --define=BUILD_AS_EXE --define=PART_LM4F120H5QR --define=TARGET_IS_BLIZZARD_RA1 --define=MULTITHREAD_SUPPORT --define=__ENABLE_MULTITHREADED_SUPPORT__ --define=ENABLE_SMART_CONFIG --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="wifi_application_multithread.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


