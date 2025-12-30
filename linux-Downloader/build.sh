# call lib
source ../build_lib_compile.sh
get_cpu_info
get_gcc_params
get_jdk_path
get_compile_include

kill_process Downloader
rm_file      Downloader

g++ ${GCC_PARAMS}  \
    -o Downloader \
    Downloader.cpp \
    ../framework-linux/app_message.cpp \
    ../framework-upgrade/UpgradeApi.cpp \
    ../framework-upgrade/UpgradeApiLinux.cpp \
    -I../framework-upgrade \
    ${FRAMEWORK_FILES} \
    ${COMPILE_INCLUDE_LINK}
	
# UPGRADER_PATH=${TIO_HOME}/Upgrader
# mk_dir ${UPGRADER_PATH}
# cp_file_to_dir Downloader ${UPGRADER_PATH}
