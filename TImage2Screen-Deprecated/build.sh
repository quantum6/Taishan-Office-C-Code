# call lib
source ../build_lib_compile.sh
get_cpu_info
get_gcc_params
get_jdk_path
get_compile_include
get_tio_bin_path

BUILD_DIR=${HOME}/ts-framework4/lib-ffmpeg


LIB_NAME=libimage2screen.so
rm ${LIB_NAME}
g++ -shared -DXP_UNIX=1 -DMOZ_X11=1 -fPIC -g \
    image2screen.c tso_tnative_TImage2Screen.cpp -o ${LIB_NAME} \
    -I${BUILD_DIR}/include \
    -I${BUILD_DIR}/include/SDL2 \
    -L${BUILD_DIR}/lib -lSDL2 \
    ${FRAMEWORK_FILES} \
    ${COMPILE_INCLUDE_LINK}

cp ${LIB_NAME} ${HOME}/eclipse-workspace/ts-office4/src/bin
cp ${LIB_NAME} ${HOME}/eclipse-workspace/ts-office4/bin/bin

rm a.out
#g++ -shared -DXP_UNIX=1 -DMOZ_X11=1 -fPIC -g \
g++  \
    image2screen.c \
    -o test \
    -I${BUILD_DIR}/include \
    -I${BUILD_DIR}/include/SDL2 \
    -L${BUILD_DIR}/lib -lSDL2 \
    ${FRAMEWORK_FILES} \
    ${COMPILE_INCLUDE_LINK}

# echo build OK. Now run test
# ./test
