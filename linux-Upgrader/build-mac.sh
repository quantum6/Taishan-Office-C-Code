# call lib
source ../build_lib_compile.sh
get_cpu_info
get_gcc_params
get_jdk_path
get_compile_include

kill_process Upgrader
rm_file      Upgrader

g++ ${GCC_PARAMS}  -DPRODUCT_$1 \
    -o Upgrader \
    Upgrader.cpp \
    ../framework-upgrade/UpgradeApi.cpp \
    ../framework-upgrade/UpgradeApiLinux.cpp \
    ../framework-linux/app_message.cpp \
    ../framework/fw_kit_mac.cpp \
    ../framework/inih/INIReader.cpp ../framework/inih/ini.c \
    ${FRAMEWORK_FILES} \
    ${COMPILE_INCLUDE_LINK}

