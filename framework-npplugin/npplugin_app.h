#ifndef __NPPLUGIN_APP_H__
#define __NPPLUGIN_APP_H__

#include "fw_log.h"
#include "npplugin_js_listener.h"

class NPPluginApp
{
public:

	NPPluginApp(const int plugID);
    ~NPPluginApp();

    int getPluginID()
    {
        return this->pluginID;
    }
    
    bool isValid()
    {
        return this->bIsValid;
    }

	bool start();
	void window(const int handle,
      const int x, const int y, const int width, const int height);

	void refresh();

    /*
    每个插件都有自己的列表，要么放在这里，要么放在InstanceData。
    所以放在这里。
    自然的，npplugin_js_function/npplugin_js_listener也跟着放在同一个目录。
    */
    std::map<std::string, JsListener*> jsListeners;
    JsCommander* pCommander;

private:

    int  pluginID;
    bool bIsValid;
    bool bIsProxyStarted;

};

#endif
