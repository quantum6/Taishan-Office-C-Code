# call lib
source ../build_lib_compile.sh
get_cpu_info
get_gcc_params
get_jdk_path
get_compile_include


kill_process Unpackager
rm_file      Unpackager

g++ -fPIC \
    -o Unpackager \
    Unpackager.cpp \
    ../framework-upgrade/fw_upgrade.cpp \
    ${FRAMEWORK_FILES} \
    ${COMPILE_INCLUDE_LINK}
