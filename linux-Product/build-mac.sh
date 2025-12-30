# call lib
source ../build_lib_compile.sh
get_cpu_info
get_gcc_params
get_jdk_path
get_compile_include

# CLANG_COMP = /usr/local/opt/llvm/bin/clang++

PRODUCT=Office
typeset -l PRODUCT_L
PRODUCT_L=${PRODUCT}

kill_process ${PRODUCT}
rm_file      ${PRODUCT}

g++ ${GCC_PARAMS}  -D__APPLE__ \
    -o ${PRODUCT} -DPRODUCT_${PRODUCT} -DPRODUCT_APP_SUFFIX=${PRODUCT_L} \
    ProductStarter.cpp product_launcher.cpp \
    ../framework-linux/app_message.cpp \
    ../framework-upgrade/fw_upgrade.cpp \
    ../framework-app-linux/app_launcher.cpp \
    ../framework-app/app_common.cpp \
    ../framework-app/app_jni_command.cpp \
../framework/fw_kit_mac.cpp \
../framework/inih/INIReader.cpp ../framework/inih/ini.c \
-I. \
${FRAMEWORK_FILES} \
${COMPILE_INCLUDE_LINK}
