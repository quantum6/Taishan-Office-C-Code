
#if defined(_WINDOWS)
#else
#include <gtk/gtk.h>
#include <sys/msg.h>
#endif

#include "cJSON.h"
#include "fw_log.h"
#include "fw_kit.h"
#include "fw_config.h"

#include "npplugin_js_data.h"

#include "app_common.h"
#include "app_message.h"
#include "app_jni_command.h"

#include "proxy_launcher.h"
#include "proxy_config.h"

const char* ProxyLauncher::getClassNameMain()
{
    return CLASS_MAIN;
}

const char* ProxyLauncher::getClassNameCommand()
{
    return CLASS_COMMAND;
}

bool ProxyLauncher::checkRunning(const int argc, const char** argv,  const char* szAppParam)
{
    if (argc > 2)
    {
       m_nPluginID = atoi(argv[1]);
       m_pJsData = new JsData(this->m_nPluginID, atoi(argv[2]));
    }
    
    return true;
}

bool ProxyLauncher::preAction(const int argc, const char** argv)
{
    return false;
}

bool ProxyLauncher::processCommandFunction(cJSON* pRoot, const AppJniCommand* pJniCommand, const JNIEnv* pEnv)
{
    cJSON* pItem = cJSON_GetObjectItem(pRoot, JSON_FUNCTION_NAME);
    char*  name = cJSON_GetStringValue(pItem);
    if (name == NULL)
    {
        return false;
    }

    cJSON* pResult = NULL;
    for (int i=0; i<sizeof(oJniFunctions)/sizeof(JniFunctionPair); i++)
    {
        JniFunctionPair pair = oJniFunctions[i];
        if (!strcmp(pair.funcName, name))
        {
            pResult = cJSON_CreateObject();
            pItem = cJSON_GetObjectItem(pRoot,  JSON_FUNCTION_PARAMS);
            pair.funcJava(pJniCommand, (JNIEnv*)pEnv, pItem, name, pResult);

            m_pJsData->copyData(cJSON_Print(pResult));
            cJSON_Delete(pResult);
            pResult = NULL;
            break;
        }
    }

    return false;
}

bool ProxyLauncher::postAction(const AppJniCommand* pJniCommand, const JNIEnv* pEnv)
{
    ((AppJniCommand*)pJniCommand)->initCommandObject(GET_INSTANCE, GET_INSTANCE_SIGNATURE);

    while (true)
    {
        int flag = m_pJsData->getDataFlag();
        if (flag == -1)
        {
            delete m_pJsData;
            m_pJsData = NULL;
            break;
        }
        if (flag)
        {
            cJSON* pRoot = cJSON_Parse(m_pJsData->getData());
            if (pRoot == NULL)
            {
                break;
            }

            cJSON* pCommand = cJSON_GetObjectItem(pRoot, JSON_COMMAND_KEY);
            if (pCommand == NULL || !strcmp(pCommand->valuestring, JSON_COMMAND_EXIT))
            {
                delete m_pJsData;
                m_pJsData = NULL;
                break;
            }
            else if (!strcmp(pCommand->valuestring, JSON_COMMAND_FUNCTION))
            {
                processCommandFunction(pRoot, pJniCommand, pEnv);
            }

            m_pJsData->clearDataFlag();
            cJSON_Delete(pRoot);
        }
#if defined(_WINDOWS)
		Sleep(10);
#else
        usleep(1000);
#endif
    }

    return false;
}
