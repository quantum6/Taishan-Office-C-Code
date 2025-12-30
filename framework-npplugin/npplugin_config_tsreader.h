
#ifndef __NPPLUGIN_CONFIG_READER_H__
#define __NPPLUGIN_CONFIG_READER_H__

#define NPPLUGIN_PRODUCT_NAME                 "taishan-reader"
#define NPPLUGIN_PRODUCT_DESCRIPTION          "taishan-reader"

#define NPPLUGIN_PRODUCT_MIME       "application/taishan-reader:ofd:Taishan Reader mimetype"

#define KEY_FILE_PLUGIN_PROXY    PROXY_NAME_READER


static const char* saPluginJsFunctions[] =
{
    //功能由插件实现。
    (char*)"init", 
    (char*)"getPluginVersion", 

    //功能由代理实现
    (char*)"openFile",
    (char*)"saveFile",
    (char*)"printFile",
    (char*)"closeFile",
	
    (char*)"setToolbarAllVisible",
    (char*)"setCompositeVisible", 
    (char*)"setCompositeEnable",
    (char*)"setViewPreference",
    
    (char*)"setZoomRatio",
    (char*)"getZoomRatio",

    (char*)"setUserName",
    (char*)"getUserName",

    (char*)"setSealId",
    
    (char*)"setMetaData",
    (char*)"getMetaData",

    (char*)"setLogSvrURL",
    
    (char*)"addTrackInfo",
    (char*)"clearTrackInfo",

    (char*)"getTaggedText",
    (char*)"getLogFilePath"
};


#endif // __NPPLUGIN_CONFIG_READER_H__
