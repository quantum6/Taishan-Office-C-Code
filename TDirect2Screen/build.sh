# call lib
source ../build_lib_compile.sh
get_cpu_info
get_gcc_params
get_jdk_path
get_tio_home
get_compile_include

OUTPUT=libtdirect2screen.so
if [ -f "${OUTPUT}" ]; then
    rm ${OUTPUT}
fi

g++ -O3 -fPIC -z noexecstack -W -Wall \
    -shared -o ${OUTPUT} \
    *.cpp  ../framework/fw_timestamp.cpp \
    -I${JDK_INCLUDE}/include \
    -I${JDK_INCLUDE}/include/linux \
    -lSDL2
