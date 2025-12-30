#ifndef __PROXY_CONFIG_H__
#define __PROXY_CONFIG_H__

#include "fw_config.h"
#include "proxy_jni_function.h"

#if defined(PROXY_Office)
#include "proxy_config_office.h"
#elif defined(PROXY_Reader)
#include "proxy_config_reader.h"
#endif

#define GET_INSTANCE "getInstance"

#ifdef _WINDOWS
#define REG_KEY_ITEM_TAISHAN_PRODUCT            REG_KEY_ITEM_TAISHAN KEY_FILE_PRODUCT
#pragma message("\nProduct: " KEY_FILE_PRODUCT "\n")

#define APPLICATION_MAIN               KEY_FILE_EXE ".exe"
#else
#define APPLICATION_MAIN               KEY_FILE_EXE
#endif

#endif
