################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../lm4f120h5qr.cmd 

CFG_SRCS += \
../mt_wifi_app_multi_thread.cfg 

LIB_SRCS += \
C:/StellarisWare/driverlib/ccs-cm4f/Debug/driverlib-cm4f.lib \
C:/00_A0131814/00_Praneet/00_CCxxxx/Apps/00_ReleaseStructure/Stellaris/Apps/LM4f120H5QR/Multithreaded\ WiFi\ Application\ CCS/mt_cc3000_host_driver/Debug/mt_cc3000_host_driver.lib \
C:/00_A0131814/00_Praneet/00_CCxxxx/Apps/00_ReleaseStructure/Stellaris/Apps/LM4f120H5QR/Multithreaded\ WiFi\ Application\ CCS/mt_cc3000_spi/Debug/mt_cc3000_spi.lib \
C:/00_A0131814/00_Praneet/00_CCxxxx/Apps/00_ReleaseStructure/Stellaris/Apps/LM4f120H5QR/Multithreaded\ WiFi\ Application\ CCS/mt_impl_multi_thread/Debug/mt_impl_multi_thread.lib \
C:/00_A0131814/00_Praneet/00_CCxxxx/Apps/00_ReleaseStructure/Stellaris/Apps/LM4f120H5QR/Multithreaded\ WiFi\ Application\ CCS/mt_osal/Debug/mt_osal.lib 

C_SRCS += \
C:/00_A0131814/00_Praneet/00_CCxxxx/Apps/00_ReleaseStructure/Stellaris/Apps/LM4f120H5QR/Source/wifi_application_multithread/src/board.c \
C:/StellarisWare/utils/uartstdio.c \
C:/00_A0131814/00_Praneet/00_CCxxxx/Apps/00_ReleaseStructure/Stellaris/Apps/LM4f120H5QR/Source/wifi_application_multithread/src/wifi_application_multithread.c 

OBJS += \
./board.obj \
./uartstdio.obj \
./wifi_application_multithread.obj 

GEN_SRCS += \
./configPkg/compiler.opt \
./configPkg/linker.cmd 

C_DEPS += \
./board.pp \
./uartstdio.pp \
./wifi_application_multithread.pp 

GEN_MISC_DIRS += \
./configPkg/ 

GEN_CMDS += \
./configPkg/linker.cmd 

GEN_OPTS += \
./configPkg/compiler.opt 

GEN_SRCS__QUOTED += \
"configPkg\compiler.opt" \
"configPkg\linker.cmd" 

GEN_MISC_DIRS__QUOTED += \
"configPkg\" 

C_DEPS__QUOTED += \
"board.pp" \
"uartstdio.pp" \
"wifi_application_multithread.pp" 

OBJS__QUOTED += \
"board.obj" \
"uartstdio.obj" \
"wifi_application_multithread.obj" 

C_SRCS__QUOTED += \
"C:/00_A0131814/00_Praneet/00_CCxxxx/Apps/00_ReleaseStructure/Stellaris/Apps/LM4f120H5QR/Source/wifi_application_multithread/src/board.c" \
"C:/StellarisWare/utils/uartstdio.c" \
"C:/00_A0131814/00_Praneet/00_CCxxxx/Apps/00_ReleaseStructure/Stellaris/Apps/LM4f120H5QR/Source/wifi_application_multithread/src/wifi_application_multithread.c" 

GEN_OPTS__FLAG += \
--cmd_file="./configPkg/compiler.opt" 

GEN_CMDS__FLAG += \
-l"./configPkg/linker.cmd" 


