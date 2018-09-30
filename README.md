# eos_notify_plugin

at pulgins/CMakeLists.txt 

add_subdirectory(notify_plugin) 


at programs/nodeos/CMakeLists.txt

PRIVATE -Wl,${whole_archive_flag} notify_plugin               -Wl,${no_whole_archive_flag}


notify-filter-account-name 过滤的账号
tx-notify-shell-cmd 通知交易id
