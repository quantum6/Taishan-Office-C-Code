# call lib
source ../build_lib_compile.sh
get_cpu_info
get_gcc_params
get_jdk_path
get_compile_include

kill_process Upgrader
rm_file      Upgrader

g++ ${GCC_PARAMS}  \
    -o Upgrader \
    Upgrader.cpp \
    ../framework-upgrade/fw_upgrade.cpp \
    ../framework-upgrade/UpgradeApi.cpp \
    ../framework-upgrade/UpgradeApiLinux.cpp \
    ../framework-linux/app_message.cpp \
    ${FRAMEWORK_FILES} \
    ${COMPILE_INCLUDE_LINK}
