################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
configPkg/compiler.opt: ../mt_impl_multi_thread.cfg
	@echo 'Building file: $<'
	@echo 'Invoking: XDCtools'
	"C:/TI/xdctools_3_24_05_48/xs" --xdcpath="C:/TI/bios_6_34_02_18/packages;C:/TI/tirtos_1_01_00_25/packages;C:/TI/tirtos_1_01_00_25/products/bios_6_34_04_22/packages;C:/TI/tirtos_1_01_00_25/products/ipc_1_25_02_12/packages;C:/TI/tirtos_1_01_00_25/products/ndk_2_22_01_14/packages;C:/TI/tirtos_1_01_00_25/products/uia_1_02_00_07/packages;C:/TI/tirtos_1_01_00_25/products/xdctools_3_24_06_63/packages;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M4F -p ti.platforms.stellaris:LM4F120H5QR -r release -c "C:/ti/ccsv5/tools/compiler/arm_5.0.1" "$<"
	@echo 'Finished building: $<'
	@echo ' '

configPkg/linker.cmd: configPkg/compiler.opt
configPkg/: configPkg/compiler.opt

netapp.obj: C:/00_A0131814/00_Praneet/00_CCxxxx/Apps/00_ReleaseStructure/Stellaris/Apps/LM4f120H5QR/Source/cc3000_host_driver/implementation/multi_threaded/src/netapp.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/arm_5.0.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -g --include_path="C:/ti/ccsv5/tools/compiler/arm_5.0.1/include" --include_path="../../../Source/cc3000_host_driver/" --include_path="../../../Source/osal/inc" --include_path="../../../Source/cc3000_host_driver/include" --include_path="../../../Source/cc3000_host_driver/include/common" --include_path="../../../Source/osal/sys_bios/inc" --define=__CCS__ --define=CC3000_UNENCRYPTED_SMART_CONFIG --define=__ENABLE_MULTITHREADED_SUPPORT__ --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="netapp.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

nvmem.obj: C:/00_A0131814/00_Praneet/00_CCxxxx/Apps/00_ReleaseStructure/Stellaris/Apps/LM4f120H5QR/Source/cc3000_host_driver/implementation/multi_threaded/src/nvmem.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/arm_5.0.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -g --include_path="C:/ti/ccsv5/tools/compiler/arm_5.0.1/include" --include_path="../../../Source/cc3000_host_driver/" --include_path="../../../Source/osal/inc" --include_path="../../../Source/cc3000_host_driver/include" --include_path="../../../Source/cc3000_host_driver/include/common" --include_path="../../../Source/osal/sys_bios/inc" --define=__CCS__ --define=CC3000_UNENCRYPTED_SMART_CONFIG --define=__ENABLE_MULTITHREADED_SUPPORT__ --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="nvmem.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

security.obj: C:/00_A0131814/00_Praneet/00_CCxxxx/Apps/00_ReleaseStructure/Stellaris/Apps/LM4f120H5QR/Source/cc3000_host_driver/implementation/multi_threaded/src/security.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/arm_5.0.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -g --include_path="C:/ti/ccsv5/tools/compiler/arm_5.0.1/include" --include_path="../../../Source/cc3000_host_driver/" --include_path="../../../Source/osal/inc" --include_path="../../../Source/cc3000_host_driver/include" --include_path="../../../Source/cc3000_host_driver/include/common" --include_path="../../../Source/osal/sys_bios/inc" --define=__CCS__ --define=CC3000_UNENCRYPTED_SMART_CONFIG --define=__ENABLE_MULTITHREADED_SUPPORT__ --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="security.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

socket.obj: C:/00_A0131814/00_Praneet/00_CCxxxx/Apps/00_ReleaseStructure/Stellaris/Apps/LM4f120H5QR/Source/cc3000_host_driver/implementation/multi_threaded/src/socket.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/arm_5.0.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -g --include_path="C:/ti/ccsv5/tools/compiler/arm_5.0.1/include" --include_path="../../../Source/cc3000_host_driver/" --include_path="../../../Source/osal/inc" --include_path="../../../Source/cc3000_host_driver/include" --include_path="../../../Source/cc3000_host_driver/include/common" --include_path="../../../Source/osal/sys_bios/inc" --define=__CCS__ --define=CC3000_UNENCRYPTED_SMART_CONFIG --define=__ENABLE_MULTITHREADED_SUPPORT__ --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="socket.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

wlan.obj: C:/00_A0131814/00_Praneet/00_CCxxxx/Apps/00_ReleaseStructure/Stellaris/Apps/LM4f120H5QR/Source/cc3000_host_driver/implementation/multi_threaded/src/wlan.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/arm_5.0.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -g --include_path="C:/ti/ccsv5/tools/compiler/arm_5.0.1/include" --include_path="../../../Source/cc3000_host_driver/" --include_path="../../../Source/osal/inc" --include_path="../../../Source/cc3000_host_driver/include" --include_path="../../../Source/cc3000_host_driver/include/common" --include_path="../../../Source/osal/sys_bios/inc" --define=__CCS__ --define=CC3000_UNENCRYPTED_SMART_CONFIG --define=__ENABLE_MULTITHREADED_SUPPORT__ --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="wlan.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


