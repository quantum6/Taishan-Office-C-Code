# deb [trusted=yes] https://test-professional-loongarch.chinauos.com/desktop-professional eagle main
# sudo apt -y install libgtk2.0-dev
# sudo apt -y install libxt-dev
# sudo apt -y install g++

source build_lib_compile.sh
get_os_name
get_cpu_info

ROOT=`pwd`
reset

find . -name ".svn" | xargs rm -rf

chown -R ${USER} *
find . -name "*.sh" | xargs chmod +x

SO_DIR=lib_${OS_NAME}_${CPU_INSTALL_BUILD}
rm_dir ${SO_DIR}
mk_dir ${SO_DIR}

echo

TS_PRODUCTS=(  "Office"    "Reader")

# for product in ${TS_PRODUCTS[*]}
for index in {0..1}
do
    for project in linux-Product \
                   linux-Proxy \
                   linux-NpPlugin
    do
        if [ ! -d ${project} ]; then
            echo ${project} not exist
            continue;
        fi

        echo ----------------- build ${TS_PRODUCTS[index]} ${project} ------------------
        cd ${project}
        ./build.sh ${TS_PRODUCTS[index]} ${SO_DIR}
	      sleep 3s
        cd ${ROOT}
        echo
    done
    echo
done
# exit

for app in linux-RapidMenu \
           linux-Upgrader linux-Downloader linux-Unpackager \
           linux-printer linux-tclipboard \
           linux-Reader-pattern \
           PlayerCore
#  TDirect2Screen
do
    if [ ! -d ${app} ]; then
        echo ${app} not exist
        continue;
    fi

    echo ----------------- build ${app} ------------------
    cd ${app}
    if [ -f build-linux.sh ]; then
        SUFFIX=-linux
    fi
    
    ./build${SUFFIX}.sh ${CPU_INCLUDE}
    cd ${ROOT}
    echo
done

get_office_home
get_reader_home

# {{{{{{{{{{ Upgrader

for so_file in linux-Upgrader/Upgrader \
               linux-Downloader/Downloader \
               linux-Unpackager/Unpackager
do
    if [ ! -f ${so_file} ]; then
        echo ${so_file} not exist
        continue;
    fi

    for dest_dir in ${TS_OFFICE_HOME} ${TS_READER_HOME}
    do
        if [ -d ${dest_dir} ]; then
            sudo cp ${so_file} ${dest_dir}/Upgrader
        fi
    done
    mv ${so_file} ${SO_DIR}
done

# }}}}}}}}}} Upgrader

for so_file in linux-printer/libtlinuxprinter.so \
               linux-tclipboard/libtclipboard.so \
               PlayerCore/libtplayercore.so
#              TDirect2Screen/libtdirect2screen.so
do
    if [ ! -f ${so_file} ]; then
        echo ${so_file} not exist
        continue;
    fi

    for dest_dir in ${TS_OFFICE_HOME} ${TS_READER_HOME}
    do
        if [ -d ${dest_dir} ]; then
            sudo cp ${so_file} ${dest_dir}/bin
        fi
    done
    mv ${so_file} ${SO_DIR}
done

# {{{{{{{{{{ Office

for so_file in linux-Product/Office \
               linux-RapidMenu/RapidMenu
do
    if [ ! -f ${so_file} ]; then
        echo ${so_file} not exist
        continue;
    fi

    if [ -d ${TS_OFFICE_HOME} ]; then
        sudo cp  ${so_file} ${TS_OFFICE_HOME}/bin
    fi
    mv ${so_file} ${SO_DIR}
done

PROXY_OFFICE=ProxyOffice
killall ${PROXY_OFFICE}
if [ -d ${TS_OFFICE_HOME} ]; then
    sudo rm ${TS_OFFICE_HOME}/Plugins/${PROXY_OFFICE}
fi
for so_file in linux-NpPlugin/libnptsoffice.so \
               linux-Proxy/${PROXY_OFFICE}
do
    if [ ! -f ${so_file} ]; then
        echo ${so_file} not exist
        continue;
    fi

    if [ -d ${TS_OFFICE_HOME} ]; then
        sudo cp  ${so_file} ${TS_OFFICE_HOME}/Plugins
    fi

    mv ${so_file} ${SO_DIR}
done

UOS_PLUGINS=/opt/apps/cn.ts-it.office/entries/plugins/browser
if [ -d ${UOS_PLUGINS} ]; then
    sudo cp  ${SO_DIR}/libnptsoffice.so ${UOS_PLUGINS}
fi

# }}}}}}}}}} Office

# {{{{{{{{{{ Reader

so_file=linux-Product/Reader
if [ -d ${TS_READER_HOME} ]; then
    sudo cp  ${so_file} ${TS_READER_HOME}/bin
fi
mv ${so_file} ${SO_DIR}

PROXY_READER=ProxyReader
killall ${PROXY_READER}
if [ -d ${TS_READER_HOME} ]; then
    sudo rm ${TS_READER_HOME}/Plugins/${PROXY_READER}
fi
for so_file in linux-NpPlugin/libnptsreader.so \
               linux-Proxy/${PROXY_READER}
do
    if [ ! -f ${so_file} ]; then
        echo ${so_file} not exist
        continue;
    fi

    if [ -d ${TS_READER_HOME} ]; then
        sudo cp  ${so_file} ${TS_READER_HOME}/Plugins
    fi
    mv ${so_file} ${SO_DIR}
done

UOS_PLUGINS=/opt/apps/cn.ts-it.reader/entries/plugins/browser
if [ -d ${UOS_PLUGINS} ]; then
    sudo cp  ${SO_DIR}/libnptsreader.so ${UOS_PLUGINS}
fi

# }}}}}}}}}} Reader

