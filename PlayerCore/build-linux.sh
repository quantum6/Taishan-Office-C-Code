FFMPEG_DIR=${HOME}/lib-ffmpeg
SDL2_INCLUDE_DIR=${FFMPEG_DIR}/include/SDL2
TSJDK8_DIR=${HOME}/tsjdk8


gcc -c -g -fPIC \
     ffplay.c -o ffplay.o \
    -I ${SDL2_INCLUDE_DIR} \
    -I ${FFMPEG_DIR}/include

gcc -c -g -fPIC \
     playercore.c -o playercore.o \
    -I ${SDL2_INCLUDE_DIR}

gcc -c -g -fPIC \
     playercorejni.c -o playercorejni.o \
    -I ${TSJDK8_DIR}/include \
    -I ${TSJDK8_DIR}/include/linux

TSJDK8_LIB_DIR=${TSJDK8_DIR}/jre/lib/loongarch64
gcc -shared \
    ffplay.o playercore.o playercorejni.o -o libtplayercore.so  \
    -L${FFMPEG_DIR}/lib \
    -lSDL2 -lavformat -lswscale -lswresample -lavcodec -lavutil \
    -lfdk-aac -lmp3lame -lvpx -lx264 \
    -L${TSJDK8_LIB_DIR} \
    -L${TSJDK8_LIB_DIR}/server \
    -ljava -ljvm -ljawt -lawt -lawt_xawt \
    -L/usr/local/lib  \
    -lX11 -lm -lpthread -lverify -lz

rm *.o
