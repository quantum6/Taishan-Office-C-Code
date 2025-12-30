# call lib
source ../build_lib_compile.sh
get_cpu_info
get_gcc_params
get_browser_plugins_path
get_compile_include
get_npplugin_include

typeset -l PRODUCT_L
PRODUCT_L=${1}
LIB_SO=libnpts${PRODUCT_L}.so

rm_file ${LIB_SO}
rm_file *.o
rm_file ${LIB_SO}
rm_file *.o

BUILD_DATE=`date +%Y%m%d`

#g++ -DXP_UNIX=1 -DMOZ_X11=1 -fPIC -g\
g++ -shared -fPIC -g \
    -DXP_UNIX=1 -DPRODUCT_${PRODUCT_L} -DMOZ_X11=1 \
    -DBUILD_DATE=\"${BUILD_DATE}\" \
    -o ${LIB_SO} \
    ../framework-npplugin/npplugin_commander.cpp  \
    ../cJSON/cJSON.c \
    -I. \
    ${NPPLUGIN_FILES} \
    ${FRAMEWORK_FILES} \
    ${COMPILE_INCLUDE_LINK}

