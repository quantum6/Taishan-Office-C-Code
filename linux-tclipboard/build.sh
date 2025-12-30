# call lib
source ../build_lib_compile.sh
get_cpu_info
get_gcc_params
get_jdk_path
get_compile_include

if [ -f "libtclipboard.so" ]; then
    rm libtclipboard.so
fi

if [ "${JDK_INCLUDE}" = "" -o ! -d ${JDK_INCLUDE}/include -o ! -d ${JDK_INCLUDE}/include/linux ];then
    JDK_PATH=`find /usr/lib/jvm -name *jni.h* | grep ${CPU_INSTALL_BUILD} |sed 's/\/includ.*$//' |awk '{if(NR==1) print}'`
    if [ "${JDK_PATH}" != "" ]; then
        JDK_INCLUDE2="-I${JDK_PATH}/include -I${JDK_PATH}/include/linux"
    else
        JDK_PATH=`sudo find /data -name *jni.h* | sed 's/\/includ.*$//' |awk '{if(NR==1) print}'`
        if [ "${JDK_PATH}" != "" ]; then
            JDK_INCLUDE2="-I${JDK_PATH}/include -I${JDK_PATH}/include/linux"
        fi
	fi
fi

g++ -fPIC -z noexecstack \
-shared -o libtclipboard.so \
tso_tnative_TClipBoard.cpp tclipboard.cpp \
${COMPILE_INCLUDE_LINK} \
${JDK_INCLUDE2}

if [ -f "libtclipboard.so" ]; then
    cp libtclipboard.so ${TIO_HOME}/bin
else
    echo "failed to compile libtclipboard.so"
fi 
