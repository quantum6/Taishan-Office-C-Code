#ifndef __PROXY_CONFIG_READER_H__
#define __PROXY_CONFIG_READER_H__

#include "fw_config.h"

#define PROXY_STARTER            APPLICATION_TIO

#define KEY_FILE_PLUGIN_JAR      "ReaderPlugin.jar"

#define CLASS_MAIN               "tso/plugin/reader/ReaderPlugin"
#define CLASS_COMMAND            CLASS_MAIN
#define GET_INSTANCE_SIGNATURE   "()Ltso/plugin/reader/ReaderPluginable;"

static const JniFunctionPair oJniFunctions[] =
{
/*
这里不实现，由调用方（如libtsreader.so）实现。
    { (char*)"init",                init              },
    { (char*)"getPluginVersion",    getPluginVersion  },
*/

    { (char*)"openFile",            JNI_FUNCTION_StrZ_Z   },
    { (char*)"saveFile",            JNI_FUNCTION_Str_Z    },
    { (char*)"printFile",           JNI_FUNCTION_StrZ_Z   },
    { (char*)"closeFile",           JNI_FUNCTION_V_Z      },
	
    { (char*)"setToolbarAllVisible",JNI_FUNCTION_Z_Z      },
    { (char*)"setCompositeVisible", JNI_FUNCTION_StrZ_Z   },
    { (char*)"setCompositeEnable",  JNI_FUNCTION_StrZ_Z   },
    { (char*)"setViewPreference",   JNI_FUNCTION_StrStr_Z },
    
    { (char*)"setZoomRatio",        JNI_FUNCTION_F_Z      },
    { (char*)"getZoomRatio",        JNI_FUNCTION_V_F      },

    { (char*)"setUserName",         JNI_FUNCTION_Str_Z    },
    { (char*)"getUserName",         JNI_FUNCTION_V_Str    },

    { (char*)"setSealId",           JNI_FUNCTION_Str_Z    },
    
    { (char*)"setMetaData",         JNI_FUNCTION_StrStr_Z },
    { (char*)"getMetaData",         JNI_FUNCTION_Str_Str  },

    { (char*)"setLogSvrURL",        JNI_FUNCTION_Str_Z    },
    
    { (char*)"addTrackInfo",        JNI_FUNCTION_Str_Z    },
    { (char*)"clearTrackInfo",      JNI_FUNCTION_V_Z      },

    { (char*)"getTaggedText",       JNI_FUNCTION_Str_Str  },
    { (char*)"getLogFilePath",      JNI_FUNCTION_V_Str    }

};

#endif
