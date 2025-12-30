CPU_INCLUDE=$1

LIB_CUPS=/usr/lib/${CPU_INCLUDE}-linux-gnu/libcups.so.2
if [ ! -f ${LIB_CUPS} ]; then
    LIB_CUPS=/usr/lib/libcups.so.2
fi

echo ${LIB_CUPS}

INCLUDE_CUPS=`pwd`
JNI_DIR=./include
g++ -c -fPIC \
    tioprint.cpp \
    -I${INCLUDE_CUPS}

g++ -c -fPIC \
    jnitioprint.cpp \
    -I${JNI_DIR} -I${JNI_DIR}/linux 
    
g++ -shared tioprint.o jnitioprint.o ${LIB_CUPS} \
    -o libtlinuxprinter.so

rm *.o
