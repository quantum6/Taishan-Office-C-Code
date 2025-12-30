#ifndef __PRODUCT_CONFIG_H__
#define __PRODUCT_CONFIG_H__

#if defined(PRODUCT_Office)
#include "product_config_office.h"
#elif defined(PRODUCT_Reader)
#include "product_config_reader.h"
#endif



#define CLASS_MAIN        "tso/system/TIOMain"
#define CLASS_COMMAND     "tso/system/ShellMethods"


#ifdef _WINDOWS
#define REG_KEY_ITEM_TAISHAN_PRODUCT            REG_KEY_ITEM_TAISHAN KEY_FILE_PRODUCT
#pragma message("\nProduct: " KEY_FILE_PRODUCT "\n")

#define APPLICATION_MAIN               KEY_FILE_EXE ".exe"
#else
#define APPLICATION_MAIN               KEY_FILE_EXE
#endif

#endif
