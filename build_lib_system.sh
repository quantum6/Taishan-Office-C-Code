#!/bin/sh

function to_lowercase()
{
    LOWER_CASE=$(echo ${1} | tr '[A-Z]' '[a-z]')
    echo ${LOWER_CASE}
}

function to_uppercase()
{
    UPPER_CASE=$(echo ${1} | tr '[a-z]' '[A-Z]')
    echo ${UPPER_CASE}
}

function get_os_name()
{
    OS_WINDOWS="windows"
    # uos
    OS_UOS="uos"
    # Zhongbiao Kylin
    OS_NEOKYLIN="neokylin"
    # Yinhe Kylin
    OS_KYLIN="kylin"
    
    OS_ARCH="arch"
    
    OS_MAC="macosx"
    
    if [ -d /cygdrive/d ]; then
        OS_NAME=${OS_WINDOWS}
        echo ${OS_NAME}
        return
    fi
    
    OS_RESULT=`uname`
    OS_RESULT=$(to_lowercase $OS_RESULT)

    if [ "${OS_RESULT}" == "darwin" ]; then
        OS_NAME=${OS_MAC}
        echo ${OS_NAME}
        return
    fi
    
    OS_RESULT=`cat  /etc/issue | awk '{print $1}' `
    OS_NAME=`echo ${OS_RESULT} | awk '{print $1}' `
    OS_NAME=$(to_lowercase $OS_NAME)
    if [ "${OS_NAME}" == "uniontech" ]; then
        OS_NAME=${OS_UOS}
    fi

    echo ${OS_NAME}
}

function get_cpu_info()
{
    get_os_name

    # x86/amd64
    CPU_NAME_INTEL="intel"
    CPU_NAME_AMD="amd"
    CPU_NAME_ZHAOXIN="zhaoxin"
    
    # arm64: CPU_PHYTIUM/CPU_FT is same manufactory
    CPU_NAME_PHYTIUM="phytium"
    CPU_NAME_FT="ft"

    # arm64
    CPU_NAME_KUNPENG="kunpeng"

    # mips64el
    CPU_NAME_LOONGSON="loongson"

    # loongarch64
    CPU_ARCH_LOONG="loongarch"

    # sw_64
    CPU_NAME_SUNWAY="sunway"
    
    # riscv64
    CPU_NAME_RISCV="riscv64"

    if [ -f /proc/cpuinfo ]; then
        CPU_INFO=`cat /proc/cpuinfo | grep name | cut -f2 -d: | uniq -c `
        if [[ "${CPU_INFO}" == "" ]]; then
            CPU_INFO=`cat /proc/cpuinfo | grep model | cut -f2 -d: | uniq -c `
        fi
    else
        CPU_INFO=`sysctl machdep.cpu.brand_string`
    fi
    CPU_INFO=$(to_lowercase "$CPU_INFO")

    if [[ ${CPU_INFO} == *${CPU_NAME_LOONGSON}* ]]; then
        CPU_NAME=loongson
        CPU_INFO=`cat /proc/cpuinfo | grep isa | cut -f2 -d: | uniq -c `
        if [[ ${CPU_INFO} == *${CPU_ARCH_LOONG}* ]]; then

            CPU_JDK_USR=loongarch64
            CPU_JDK_LIB=loongarch64
            CPU_JDK_BUILD=loongarch64
            CPU_INSTALL_BUILD=loongarch64
            CPU_INCLUDE=loongarch64
            CPU_RES=loongarch64
            CPU=__LOONGARCH64__
        else 
            CPU_JDK_USR=mips64el
            CPU_JDK_LIB=mips64el
            CPU_JDK_BUILD=mips64
            CPU_INSTALL_BUILD=mips64el
            CPU_INCLUDE=mips64el
            CPU_RES=mips64el
            CPU=__MIPS64EL__
        fi
    elif [[ ${CPU_INFO} == *${CPU_NAME_PHYTIUM}* || ${CPU_INFO} == *${CPU_NAME_FT}* ]]; then
        CPU_NAME=phytium
        CPU_JDK_USR=arm64
        CPU_JDK_LIB=aarch64
        CPU_JDK_BUILD=aarch64
        CPU_INSTALL_BUILD=arm64
        CPU_INCLUDE=aarch64
        CPU_RES=arm64
        CPU=__AARCH64__
    elif [[ ${CPU_INFO} == *${CPU_NAME_KUNPENG}* ]]; then
        CPU_NAME=kunpeng
        CPU_JDK_USR=arm64
        CPU_JDK_LIB=aarch64
        CPU_JDK_BUILD=aarch64
        CPU_INSTALL_BUILD=arm64
        CPU_INCLUDE=aarch64
        CPU_RES=arm64
        CPU=__AARCH64__
    elif [[ ${CPU_INFO} == *${CPU_NAME_ZHAOXIN}* ]]; then
        CPU_NAME=zhaoxin
        CPU_JDK_USR=amd64
        CPU_JDK_LIB=amd64
        CPU_JDK_BUILD=amd64
        CPU_INSTALL_BUILD=amd64
        CPU_INCLUDE=x86_64
        CPU_RES=amd64
        CPU=__AMD64__
    elif [[ ${CPU_INFO} == *${CPU_NAME_SUNWAY}* ]]; then
        CPU_NAME=sunway
        CPU_JDK_USR=sw64
        CPU_JDK_LIB=sw64
        CPU_JDK_BUILD=sw64
        CPU_INSTALL_BUILD=sw_64
        CPU_INCLUDE=sw_64
        CPU_RES=sw_64
        CPU=__SW64__
    elif [[ `uname -m` == *${CPU_NAME_RISCV}* ]]; then
        CPU_NAME=riscv
        CPU_JDK_USR=riscv64
        CPU_JDK_LIB=riscv64
        CPU_JDK_BUILD=riscv64
        CPU_INSTALL_BUILD=riscv64
        CPU_INCLUDE=riscv64
        CPU_RES=riscv64
        CPU=__RISCV64__
    else
        CPU_NAME=intel
        CPU_JDK_USR=amd64
        CPU_JDK_LIB=amd64
        CPU_JDK_BUILD=x86_64
        CPU_INSTALL_BUILD=amd64
        CPU_INCLUDE=x86_64
        CPU_RES=amd64
        CPU=__AMD64__
    fi

    # CPU_INCLUDE=`uname -m`
    # CPU_INCLUDE=`/bin/arch`

    echo   ${CPU_NAME} ${CPU_JDK_USR} ${CPU_JDK_LIB} ${CPU_JDK_BUILD} ${CPU}
    # return ${CPU_NAME} ${CPU_JDK_LIB} ${CPU_JDK_BUILD}
}

function get_date_text()
{
    DATE_TEXT=`date +%Y-%m-%d`
    echo ${DATE_TEXT}
}

function get_time_text()
{
    TIME_TEXT=`date +%H:%M:%S`
    echo ${TIME_TEXT}
}

function kill_process()
{
    #ps aux | grep -i $1 | grep -v grep | awk -e '{print $2}' | xargs -r sudo kill -9 1>/dev/null 2>&1
    SEARCH=`ps -A | grep $1`
    # echo ${SEARCH}
    if [ "${SEARCH}" == "" ]; then
        return
    fi
    sudo killall $1
}

function link_src_to_dest()
{
    if [ ! -f $1 ]; then
        return
    fi
    if [ -L $2 -o -f $2 ]; then
        sudo rm $2
    fi
    sudo ln -snf $1 $2
}

function mk_dir()
{
    if [ ! -d $1 ]; then
        mkdir $1
    fi
}

function rm_file()
{
    if [ -f "$1" -o -L "$1" ]; then
        sudo rm "$1"
    fi
}

function rm_files()
{
    for file in "$@"
    do
        rm_file "$file"
    done
}

function rm_dir()
{
    if [ -d "$1" ]; then
        sudo rm -rf "$1"
    fi
}

function cp_file_to()
{
    if [ ! -f $1 ]; then
        return
    fi

    sudo cp $1 $2
}

function cp_file_to_dir()
{
    if [ ! -f $1 ]; then
        return
    fi

    if [ ! -d $2 ]; then
        return
    fi
    
    sudo cp $1 $2
}

function cp_dir_to()
{
    if [ ! -d $1 ]; then
        return
    fi

    rm_dir $2

    sudo cp -rf $1 $2
}

function get_build_type()
{
    if [ -f /usr/bin/rpm ]; then
        BUILD_TYPE=rpm
    else
        BUILD_TYPE=deb
    fi
    echo ${BUILD_TYPE}
}

function get_build_code()
{
    INI_FILES=`find . -path ./config-build -prune -o -name InstallInfo.ini -print`
    if [ "${INI_FILES}" == "" ]; then
        INI_FILES=`find .. -path ./config-build -prune -o -name InstallInfo.ini -print`
    fi

    for file in ${INI_FILES}
    do
        BUILD_VERSION_CODE=`grep  version_code ${file} | tr -d "\r"`
        BUILD_VERSION_CODE=${BUILD_VERSION_CODE##*=}
        BUILD_RELEASE_CODE=${BUILD_VERSION_CODE:0:${#BUILD_VERSION_CODE}-6}
        BUILD_RELEASE_CODE=${BUILD_RELEASE_CODE##*.}
        break;
    done
   
    echo ${BUILD_VERSION_CODE} ${BUILD_RELEASE_CODE}
}

function get_browser_plugins_path()
{
    get_os_name
    if [ "${OS_NAME}" == "${OS_NEOKYLIN}" ]; then
        BROWSER_PLUGIN_PATH=/usr/lib64
    else
        BROWSER_PLUGIN_PATH=/usr/lib
    fi
    
    BROWSER_PLUGIN_PATH=${BROWSER_PLUGIN_PATH}/mozilla
    if [ ! -d ${BROWSER_PLUGIN_PATH} ]; then
        sudo mkdir ${BROWSER_PLUGIN_PATH}
    fi
    
    BROWSER_PLUGIN_PATH=${BROWSER_PLUGIN_PATH}/plugins
    if [ ! -d ${BROWSER_PLUGIN_PATH} ]; then
        sudo mkdir ${BROWSER_PLUGIN_PATH}
    fi
    echo ${BROWSER_PLUGIN_PATH}
}

function get_product_plugin()
{
    NPPLUGIN_SO=libnp${1}.so
}

function get_jdk_path()
{
    JDK_ROOT=`find /usr/lib/jvm -name java*openjdk* | grep ${CPU_JDK_USR} `
    # how to get one value ?
    for jdk in ${JDK_ROOT}
    do
        JDK_ROOT=${jdk}
        break
    done
}

function get_build_jdk()
{
    get_os_name
    if [ "${OS_NAME}" == "windows" ]; then
        BUILD_JDK=/cygdrive/d/tsjdk8-windows/openjdk-8u-bin-64
        return
    fi
    
    JDK_FINDS=`find ~ -name tiojdk8-${OS_NAME}* `
    for jdk in ${JDK_FINDS}
    do
        BUILD_JDK=${jdk}
        return
    done
        
    JDK_FINDS=`find ~ -name jdk1.8* `
    for jdk in ${JDK_FINDS}
    do
        BUILD_JDK=${jdk}
        return
    done

    get_jdk_path
    BUILD_JDK=${JDK_ROOT}
}

function parse_args()
{
    while [ $# -gt 0 ]
    do
         item=`echo $1 | awk -F '=' '{print $1}'`
        value=`echo $1 | awk -F '=' '{print $2}'`
        if [ "$item" == "build" ]; then
            BUILD_NUMBER=$value
        elif [ "$item" == "number" ]; then
            BUILD_NUMBER=$value
        elif [ "$item" == "debug" ]; then
            BUILD_DEBUG=$value
            if [ "${BUILD_DEBUG}" = "debug" ]; then
                BUILD_DEBUG=fastdebug
            else
                BUILD_DEBUG=release
            fi
        elif [ "$item" == "clean" ]; then
            BUILD_CLEAN=$value
        elif [ "$item" == "clear" ]; then
            BUILD_CLEAN=$value
        elif [ "$item" == "freetype" ]; then
            BUILD_FREETYPE=$value
        elif [ "$item" == "jdk" ]; then
            BUILD_JDK=$value
        elif [ "$item" == "tar" ]; then
            BUILD_TAR=$value
        elif [ "$item" == "bits" ]; then
            BUILD_BITS=$value
        elif [ "$item" == "os" ]; then
            BUILD_OS=$value
        elif [ "$item" == "cpu" ]; then
            BUILD_CPU=$value
        elif [ "$item" == "product" ]; then
            BUILD_PRODUCT=$value
        fi
        shift
    done

    if [ "${BUILD_BITS}" == "" ]; then
        BUILD_BITS=64
    fi

    if [ "${BUILD_DEBUG}" == "" ]; then
        BUILD_DEBUG=release
    fi
    
    if [ "${BUILD_OS}" == "" ]; then
        get_os_name
        BUILD_OS=${OS_NAME}
    fi

    if [ "${BUILD_CPU}" == "" ]; then
        get_cpu_info
        BUILD_CPU=${CPU_INSTALL_BUILD}
    fi

    echo ${BUILD_NUMBER} ${BUILD_DEBUG} ${BUILD_CLEAN}
}

function check_user_exist()
{
	typeset -i COUNT
	COUNT=`cat /etc/passwd | cut -f1 -d':' | grep -w "$1" -c`

	if [ $COUNT -le 0 ]; then
		echo "User $1 is not in /etc/passwd file"
		return 0
	else
		return 1
	fi	
}

function clean_product_files()
{
    for dir in resource-taishan-linux resource-${1}
    do
        ITEMS=`ls ${dir}`
        for item in ${ITEMS}
        do
        if [ -d ${item} ]; then
            rm -rf ${item}
        else
            rm     ${item}
        fi
        done
    done
}

#result=`get_cpu_info`
#echo 1=$result

#result=$(get_cpu_info)
#echo 2=$result
