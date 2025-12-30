
#ifndef __NPPLUGIN_CONFIG_H__
#define __NPPLUGIN_CONFIG_H__

#include "fw_config.h"
#include "npplugin_sdk.h"
#include "npplugin_js_function.h"
#include "npplugin_js_listener.h"


#if defined(PRODUCT_office)
#include "npplugin_config_tsoffice.h"
#else
#include "npplugin_config_tsreader.h"
#endif

#define NPPLUGIN_PRODUCT_VERSION              "3.3.0.293"

#ifdef WIN32
#define NPPLUGIN_PRODUCT_BUILD_VERSION        NPPLUGIN_PRODUCT_VERSION
#else
#define NPPLUGIN_PRODUCT_BUILD_VERSION        NPPLUGIN_PRODUCT_VERSION "-" BUILD_DATE
#endif

#endif // __NPPLUGIN_CONFIG_H__
