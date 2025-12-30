LIB_SYSTEM=build_lib_system.sh
if [ -f ./${LIB_SYSTEM} ]; then
    .  ./${LIB_SYSTEM}
else
    . ../${LIB_SYSTEM}
fi

get_os_name
get_cpu_info


function get_gcc_params()
{
    GCC_VERSION=`g++ -v 2>&1>/dev/null | grep "gcc 版本 " | awk '{ print $3 }'`
    if [ "${GCC_VERSION}" == "" ]; then
        GCC_VERSION=`g++ -v 2>&1>/dev/null | grep "gcc version " | awk '{ print $3 }'`
    fi
    GCC_VERSION=`echo ${GCC_VERSION} | awk -F "." '{ print $1 }'`

    GCC_PARAMS="-fPIC"
    if [ "${GCC_VERSION}" == "5" ]; then
        GCC_PARAMS=${GCC_PARAMS}" -fno-pie -no-pie"
    fi
    echo ${GCC_PARAMS}
}

function get_product_home_os()
{
    PRODUCT=${1}
    PRODUCT_L=$(to_lowercase $PRODUCT)

    if [ "${2}" == "${OS_MAC}" ]; then
        TS_PRODUCT_HOME=/Applications/泰山${PRODUCT}.app/Contents/Frameworks/${PRODUCT}
        TS_SUITE_HOME=/Applications/泰山Suite.app/Contents/Frameworks/Suite
    elif [ "${2}" == "${OS_UOS}" ]; then
        TS_PRODUCT_HOME=/opt/apps/cn.ts-it.${PRODUCT_L}/files
        TS_SUITE_HOME=/opt/apps/cn.ts-it.suite/files
    else
        TS_PRODUCT_HOME=/opt/Taishan/${PRODUCT}
        TS_SUITE_HOME=/opt/Taishan/Suite
    fi
    echo ${TS_PRODUCT_HOME}
}

function get_product_home()
{
    get_os_name
    get_product_home_os ${1} ${OS_NAME}
}

function change_file_to_unix()
{
    if [ -f $1 ]; then
        sed -i 's/\r//' $1
        return
    fi

    for item in `ls $1`
    do
        change_file_to_unix $1/$item
    done
}

function get_tsjdk_src()
{
    get_tsjdk_name
    get_cpu_info
    TSJDK_SRC=${TSJDK8}
    if [ -d cpu-${CPU_JDK_SRC} ]; then
        cp -rf cpu-${CPU_JDK_SRC}/* ${TSJDK_SRC}
    fi
    echo get_tsjdk_src=${TSJDK_SRC}
}

function get_tsjdk_build_number()
{
    # shoud be tsjdk8 path
    TSJDK_VERSION_FILE=hotspot/make/hotspot_version
    if [ ! -f ${TSJDK_VERSION_FILE} ]; then
        get_tsjdk_src
        TSJDK_VERSION_FILE=${TSJDK_SRC}/${TSJDK_VERSION_FILE}
    fi
    
    if [ "${BUILD_NUMBER}" == "" ]; then
        HS_BUILD_NUMBER=HS_BUILD_NUMBER
        BUILD_NUMBER=`grep -rn "${HS_BUILD_NUMBER}" ${TSJDK_VERSION_FILE} | awk -F '=' '{print $2}'`
    fi
}

function get_tsfreetype_path()
{
    get_lib_suffix
    LIBTSFREETYPE=libtsfreetype
    LIBTSFREETYPE_SO=${LIBTSFREETYPE}.${LIB_SUFFIX}
    if [ $OS_NAME == macosx ]; then
        LIBTSFREETYPE_SO_FULL=${LIBTSFREETYPE}.6.${LIB_SUFFIX}
    else
        LIBTSFREETYPE_SO_FULL=${LIBTSFREETYPE_SO}.6.19.0
    fi

    TSFREETYPE_PATH=`find . -name ts-freetype2`
    TSFREETYPE_PATH=`pwd`/${TSFREETYPE_PATH}
    echo ${TSFREETYPE_PATH}
}

function get_cups_dir()
{
    CUPS_PATH=`pwd`/cups-2.3.3
    echo ${CUPS_PATH}
}

function get_vs_name_windows()
{
    VS_NAME_WINDOWS="Microsoft Visual Studio 11.0"
}

function get_cpu_linux_gnu_path()
{
    get_cpu_info
    CPU_LINUX_GNU_PATH=/usr/lib/${CPU_INCLUDE}-linux-gnu

    if [ ! -d ${CPU_LINUX_GNU_PATH} ]; then
        CPU_LINUX_GNU_PATH=/usr/lib/${CPU_INCLUDE}-linux-gnuabi64
    fi

    if [ ! -d ${CPU_LINUX_GNU_PATH} ]; then
        CPU_LINUX_GNU_PATH=/usr/lib64
    fi

    if [ ! -d ${CPU_LINUX_GNU_PATH} ]; then
        CPU_LINUX_GNU_PATH=/usr/lib
    fi
}

function get_npplugin_include()
{
    FW_NPPLUGIN="../framework-npplugin"

    NPPLUGIN_FILES=" -I${FW_NPPLUGIN} "
    NPPLUGIN_FILES=${NPPLUGIN_FILES}" -I${FW_NPPLUGIN}/xulrunner-sdk-42/include "

    NPPLUGIN_FILES=${NPPLUGIN_FILES}" ${FW_NPPLUGIN}/npplugin_js_data.cpp "
    NPPLUGIN_FILES=${NPPLUGIN_FILES}" ${FW_NPPLUGIN}/npplugin_js_function.cpp "
    NPPLUGIN_FILES=${NPPLUGIN_FILES}" ${FW_NPPLUGIN}/npplugin_js_listener.cpp "
    NPPLUGIN_FILES=${NPPLUGIN_FILES}" ${FW_NPPLUGIN}/npplugin_app.cpp "
    NPPLUGIN_FILES=${NPPLUGIN_FILES}" ${FW_NPPLUGIN}/npplugin_sdk.cpp "
    NPPLUGIN_FILES=${NPPLUGIN_FILES}" ${FW_NPPLUGIN}/npplugin_sdk_kit.cpp "
    NPPLUGIN_FILES=${NPPLUGIN_FILES}" ${FW_NPPLUGIN}/npplugin_sdk_gui_gtk2.cpp "
}

function get_tsfreetype_link()
{
    get_cpu_info
    get_os_name
    
    get_product_home_os ${1} ${OS_UOS}
    TSFREETYPE_RPATH=${TS_PRODUCT_HOME}/bin:${TS_PRODUCT_HOME}/jdk/lib/${CPU_JDK_LIB}
    get_product_home_os ${1} ${OS_KYLIN}
    TSFREETYPE_RPATH=${TSFREETYPE_RPATH}:${TS_PRODUCT_HOME}/bin:${TS_PRODUCT_HOME}/jdk/lib/${CPU_JDK_LIB}

    LIBTSFREETYPE_SO=../linux-link-libs/${CPU_JDK_LIB}
    if [ -d ${LIBTSFREETYPE_SO} ]; then
        sudo cp ${LIBTSFREETYPE_SO}/* ${CPU_LINUX_GNU_PATH}
    fi

    TSFREETYPE_LINK=" -Wl,--rpath=${TSFREETYPE_RPATH} -Wl,--no-as-needed -ltsfreetype -Wl,--as-needed "
    echo get_tsfreetype_link=${TSFREETYPE_LINK}
}

function get_compile_include()
{
    FRAMEWORK_FILES=" -I../framework -I../framework-jni -I../framework-app -I../framework-app-linux -I../framework-linux -I../framework-npplugin -I../framework -I../framework-upgrade -I../cJSON "
    FRAMEWORK_FILES=${FRAMEWORK_FILES}" ../framework/fw_log.cpp"
    FRAMEWORK_FILES=${FRAMEWORK_FILES}" ../framework/fw_timestamp.cpp"
    FRAMEWORK_FILES=${FRAMEWORK_FILES}" ../framework/fw_kit.cpp"
    FRAMEWORK_FILES=${FRAMEWORK_FILES}" ../framework/fw_kit_linux.cpp"
    FRAMEWORK_FILES=${FRAMEWORK_FILES}" ../framework-jni/jni_runtime.cpp"

    COMPILE_INCLUDE_LINK=-D${CPU}
    COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -I../framework"
    COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -I../framework-product"
    COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -I../framework-proxy"

    OSNAME=`uname  -s`
    if [ ${OSNAME} == "Darwin" ] ; then
        export MACOSX_DEPLOYMENT_TARGET=10.12
        COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -I/usr/local/Cellar/gtk+/2.24.32_3/include/gtk-2.0"
        COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -I/usr/local/Cellar/glib/2.64.5/include/glib-2.0"

        COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -I/usr/local/Cellar/glib/2.64.5/lib/glib-2.0/include/"
        COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -I/usr/local/Cellar/cairo/1.16.0_3/include/cairo/"
        COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -I/usr/local/Cellar/pango/1.46.1/include/pango-1.0/"
        COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -I/usr/local/Cellar/harfbuzz/2.7.2/include/harfbuzz"
        COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -I/usr/local/Cellar/gtk+/2.24.32_3/lib/gtk-2.0/include/"
        COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -I/usr/local/Cellar/gdk-pixbuf/2.40.0_1/include/gdk-pixbuf-2.0"
        COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -I/usr/local/Cellar/atk/2.36.0/include/atk-1.0"
        COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -L/usr/local/Cellar/gdk-pixbuf/2.40.0_1/lib"
        COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -L/usr/local/Cellar/gtk+/2.24.32_3/lib/"
        COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -I/opt/X11/include"
        COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -framework CoreFoundation -mmacosx-version-min=10.12"
    fi

    COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -I/usr/include/gtk-2.0"
    COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -I/usr/include/glib-2.0"
    COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -I/usr/include/pango-1.0"
    COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -I/usr/include/cairo"
    COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -I/usr/include/gdk-pixbuf-2.0"
    COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -I/usr/include/atk-1.0"
    COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -I/usr/include/harfbuzz"

    get_cpu_linux_gnu_path
    COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -I"${CPU_LINUX_GNU_PATH}/glib-2.0/include
    COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -I"${CPU_LINUX_GNU_PATH}/gtk-2.0/include

    JDK_INCLUDE="../framework-jni"
    COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -I${JDK_INCLUDE}/include"
    COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -I${JDK_INCLUDE}/include/linux"
    
    COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -pthread -ldl"
    
    if [ ${OSNAME} == "Darwin" ] ; then
        COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" `pkg-config --libs --cflags gtk+-2.0`"
    else
        COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -lglib-2.0 -lgobject-2.0"
        COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -lgtk-x11-2.0 -lgdk_pixbuf-2.0 "
    fi

    # COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -lglib-2.0 -lgobject-2.0"
    # COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -lgtk-x11-2.0 -lgdk-x11-2.0 -lgdk_pixbuf-2.0 -lX11 -lcairo"
    # COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" -lXt -lICE -lXext -lX11"
    # -latk-1.0 -lfontconfig -lfreetype -lgio-2.0 -lpangoft2-1.0 -lpango-1.0 -lpangocairo-1.0 -lcairo -lglib-2.0
    # COMPILE_INCLUDE_LINK=${COMPILE_INCLUDE_LINK}" `pkg-config --libs --cflags gtk+-2.0`"

    echo get_compile_include=${COMPILE_INCLUDE_LINK}
}
