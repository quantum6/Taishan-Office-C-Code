# call lib
source ../build_lib_compile.sh
get_cpu_info
get_gcc_params
get_jdk_path
get_compile_include

kill_process RapidMenu
rm_file      RapidMenu

g++ ${GCC_PARAMS}  \
    -o RapidMenu \
    RapidMenu.cpp \
    -I../linux-Product \
    ../framework-linux/app_message.cpp \
    ${FRAMEWORK_FILES} \
    ${COMPILE_INCLUDE_LINK}

# cp_file_to_dir RapidMenu ${TIO_BIN_PATH}

