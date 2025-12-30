# call lib
source ../build_lib_compile.sh
get_cpu_info
get_gcc_params
get_jdk_path
get_compile_include

kill_process ${1}
rm_file      ${1}

PRODUCT=${1}
typeset -l PRODUCT_L
PRODUCT_L=${PRODUCT}

get_tsfreetype_link ${PRODUCT}

g++ ${GCC_PARAMS}  \
    -o ${1} -DPRODUCT_${PRODUCT} -DPRODUCT_APP_SUFFIX=${PRODUCT_L} \
    ProductStarter.cpp product_launcher.cpp \
    ../framework-linux/app_message.cpp \
    ../framework-upgrade/fw_upgrade.cpp \
    ../framework-app-linux/app_launcher.cpp \
    ../framework-app/app_common.cpp \
    ../framework-app/app_jni_command.cpp \
    -I. \
    ${TSFREETYPE_LINK} \
    ${FRAMEWORK_FILES} \
    ${COMPILE_INCLUDE_LINK}

sudo rm ${CPU_LINUX_GNU_PATH}/libtsfreetype*

cp_file_to_dir ${1} ${TAISHAN_HOME}/${1}/bin
if [ "${2}" != "" ]; then
    cp_file_to_dir ${1} ${2}
fi

