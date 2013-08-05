## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,em4f linker.cmd \
  package/cfg/mt_wifi_app_multi_thread_pem4f.oem4f \

linker.cmd: package/cfg/mt_wifi_app_multi_thread_pem4f.xdl
	$(SED) 's"^\"\(package/cfg/mt_wifi_app_multi_thread_pem4fcfg.cmd\)\"$""\"C:/00_A0131814/00_Praneet/00_CCxxxx/CCS/MyWorkspace_2/mt_wifi_app_multi_thread/.config/xconfig_mt_wifi_app_multi_thread/\1\""' package/cfg/mt_wifi_app_multi_thread_pem4f.xdl > $@
