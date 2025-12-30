CPU_ARCH=x64
CPU_BITS=64

FFMPEG_DIR=`pwd`/ffmpeg-4.4-full_build-shared
FFMPEG_DIR_INCLUDE=${FFMPEG_DIR}/include
FFMPEG_DIR_LIB=${FFMPEG_DIR}/lib

SDL2_DIR=`pwd`/SDL2-2.0.14
SDL2_DIR_INCLUDE=`pwd`/SDL2-2.0.14/include
# WINDOWS的编译不过去。
SDL2_DIR_INCLUDE=${SDL2_DIR_INCLUDE}-linux
SDL2_DIR_LIB=`pwd`/SDL2-2.0.14/lib-${CPU_ARCH}
echo ${SDL2_DIR_LIB}
ls ${SDL2_DIR_LIB}

TSJDK8_DIR=/cygdrive/d/tsoffice-3.1/tsjdk8-project/tsjdk8-bin-${CPU_BITS}
TSJDK8_DIR_INCLUDE=${TSJDK8_DIR}/include
TSJDK8_DIR_LIB=${TSJDK8_DIR}/lib

rm *.dll
rm *.o

gcc -c  \
    ffplay.c -o ffplay.o \
    -I ${SDL2_DIR_INCLUDE} \
    -I ${FFMPEG_DIR_INCLUDE}

gcc -c  \
    playercore.c -o playercore.o \
    -I ${SDL2_DIR_INCLUDE}
    
gcc -c  \
    playercorejni.c -o playercorejni.o \
    -I ${TSJDK8_DIR_INCLUDE} \
    -I ${TSJDK8_DIR_INCLUDE}/win32  

gcc \
    -shared ffplay.o playercore.o playercorejni.o -o TPlayerCore.dll \
    -L ${TSJDK8_DIR_LIB} -ljawt \
    -L ${SDL2_DIR_LIB} -lSDL2 \
    -L ${FFMPEG_DIR_LIB} -lavcodec -lavfilter -lavformat -lavutil -lswscale -lswresample

rm *.o
