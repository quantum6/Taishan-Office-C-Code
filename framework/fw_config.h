#ifndef __FW_CONFIG_H__
#define __FW_CONFIG_H__

#define CHARSET_UTF8    "utf8"

#define APPLICATION_KEY         "app"

#define APPLICATION_TIO         "tio"
#define APPLICATION_READER      "ofd"
#define APPLICATION_WP          "wp"
#define APPLICATION_SS          "ss"
#define APPLICATION_PG          "pg"

#define PRODUCT_NAME_OFFICE     "Office"
#define PRODUCT_NAME_READER     "Reader"

#define PROXY_NAME_OFFICE "ProxyOffice"
#define PROXY_NAME_READER "ProxyReader"


#define APP_NAME_PREFIX_KYLIN       "ts-"
#define APP_NAME_PREFIX_UOS         "cn.ts-it."

#define TAISHAN_ROOT                "Taishan"
#ifdef _WINDOWS
#define REG_KEY_ITEM_TAISHAN        "SOFTWARE\\Classes\\Taishan\\"
#define OS_DIR_TEXT                 "\\"
#define OS_DIR_CHAR                 '\\'
#define OS_SEP                      ";"
#define JVM_FILE                    "jvm.dll"
#else

#ifdef __APPLE__
#define TAISHAN_ROOT                "Frameworks"
#define JVM_FILE                    "libjvm.dylib"
#else
#define JVM_FILE                    "libjvm.so"
#endif

#define OS_DIR_TEXT                 "/"
#define OS_DIR_CHAR                 '/'
#define OS_SEP                      ":"
#define TEMP_PATH                   "/tmp"
#endif

#define KEY_DIR_FILES               "files"
#define KEY_DIR_CONFIG              "Config"
#define KEY_DIR_IMAGES              "Images"

#define KEY_FILE_TS_OFFICE_JAR      "TIOffice.jar"
#define FILE_PRODUCT_VERSION_INFO   "ProductVersion.info"

#define KEY_FILE_RAPID_MENU         "RapidMenu"
#define KEY_FILE_RAPID_MENU_ICON    "rapid_menu.ico"

#define KEY_DIR_LIB                 "lib"
#define KEY_DIR_BIN                 "bin"
#define KEY_DIR_PLUGINS             "Plugins"
#define KEY_DIR_UPGRADER            "Upgrader"

#define KEY_DIR_JDK                 "jdk"

#define FILE_UPDATE_EXT             ".zip"
#define FILE_UPGRADE_EXT            ".install"
#define FILE_UPGRADER_EXT           ".update"
#define FILE_NETWORK_INI            "network.ini"
#define FILE_UPGRADER_EXE           "Upgrader"
#define FILE_DOWNLOADER_EXE         "Downloader"

#define      DOT_JAR                ".jar"
#define STAR_DOT_JAR                "*" DOT_JAR

#define APP_LOCK                    "lock"

#endif // __FW_CONFIG_H__

