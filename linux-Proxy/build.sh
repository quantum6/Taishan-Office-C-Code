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

EXE_FILE=Proxy${1}

get_tsfreetype_link ${PRODUCT}

g++ ${GCC_PARAMS}  \
    -o ${EXE_FILE} -DPROXY_${PRODUCT} \
    ProxyStarter.cpp \
    ../framework-proxy/proxy_launcher.cpp \
    ../framework-proxy/proxy_jni_function.cpp \
    ../framework-app-linux/app_launcher.cpp \
    ../framework-app/app_common.cpp \
    ../framework-app/app_jni_command.cpp \
    ../framework-npplugin/npplugin_js_data.cpp \
    ../cJSON/cJSON.c \
    -I. \
    ${TSFREETYPE_LINK} \
    ${FRAMEWORK_FILES} \
    ${COMPILE_INCLUDE_LINK}

cp_file_to_dir ${1} ${TAISHAN_HOME}/${1}/Plugins
if [ "${2}" != "" ]; then
    cp_file_to_dir ${EXE_FILE} ${2}
fi

