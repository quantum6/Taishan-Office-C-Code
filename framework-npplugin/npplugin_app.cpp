#include "fw_kit.h"
#include "fw_log.h"
#include "npplugin_sdk.h"
#include "npplugin_app.h"
#include "npplugin_config.h"


NPPluginApp::NPPluginApp(const int plugID)
{
    this->pluginID        = -1;
    this->bIsValid        = false;
    this->bIsProxyStarted = false;

    this->pluginID   = plugID;
    this->pCommander = new JsCommander(this->pluginID);
}

NPPluginApp::~NPPluginApp()
{
	this->bIsValid = false;
    this->bIsProxyStarted = false;
    this->pluginID = 0;

    //jsListeners
    delete pCommander;
    pCommander = NULL;
}

bool NPPluginApp::start()
{
    this->bIsValid = true;

	return true;
}

void  NPPluginApp::window(const int handle,
      const int x, const int y, const int width, const int height)
{
    if (this->bIsProxyStarted)
    {
        //send command
        return;
    }

    this->bIsProxyStarted     = true;
    char command[BUFFER_SIZE] = {0};
    char buffer[BUFFER_SIZE]  = {0};
    getAppPluginsDir(command);
    sprintf(command, "%s%c%s ",
            command, OS_DIR_CHAR, KEY_FILE_PLUGIN_PROXY);
    sprintf(command, "%s %d %d ",
            command, getPluginID(),  this->pCommander->getControlShmID());
    sprintf(command, "%s %d %d %d %d %d",
            command,
            handle, x, y, width, height);
#if !defined(_WINDOWS)
    sprintf(command, "%s &", command);
#endif
    os_executeProcess(command, buffer);
}

void  NPPluginApp::refresh()
{
    //
}

