#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/TI/bios_6_34_02_18/packages;C:/TI/tirtos_1_01_00_25/packages;C:/TI/tirtos_1_01_00_25/products/bios_6_34_04_22/packages;C:/TI/tirtos_1_01_00_25/products/ipc_1_25_02_12/packages;C:/TI/tirtos_1_01_00_25/products/ndk_2_22_01_14/packages;C:/TI/tirtos_1_01_00_25/products/uia_1_02_00_07/packages;C:/TI/tirtos_1_01_00_25/products/xdctools_3_24_06_63/packages;C:/00_A0131814/00_Praneet/00_CCxxxx/CCS/MyWorkspace_2/mt_wifi_app_multi_thread/.config
override XDCROOT = C:/TI/xdctools_3_24_05_48
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/TI/bios_6_34_02_18/packages;C:/TI/tirtos_1_01_00_25/packages;C:/TI/tirtos_1_01_00_25/products/bios_6_34_04_22/packages;C:/TI/tirtos_1_01_00_25/products/ipc_1_25_02_12/packages;C:/TI/tirtos_1_01_00_25/products/ndk_2_22_01_14/packages;C:/TI/tirtos_1_01_00_25/products/uia_1_02_00_07/packages;C:/TI/tirtos_1_01_00_25/products/xdctools_3_24_06_63/packages;C:/00_A0131814/00_Praneet/00_CCxxxx/CCS/MyWorkspace_2/mt_wifi_app_multi_thread/.config;C:/TI/xdctools_3_24_05_48/packages;..
HOSTOS = Windows
endif
