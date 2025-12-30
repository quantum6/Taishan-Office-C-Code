#include "fw_log.h"
#include "fw_kit.h"

#include "cJSON.h"
#include "npplugin_js_function.h"
#if defined(_WINDOWS)
#include <windows.h>
#endif


// {{{{{{{{{{

bool JS_FUNCTION_init(NPVariant* result)
{
      BOOLEAN_TO_NPVARIANT(true, *result);
      return true;
}

bool JS_FUNCTION_getPluginVersion(NPVariant* result)
{
    char* temp = (char*)NPN_MemAlloc(BUFFER_SIZE);
    memset(temp, 0, BUFFER_SIZE);
    strcpy(temp, getNpPluginVersion());

    STRINGZ_TO_NPVARIANT(temp, *result);

    //释放会出错
    //NPN_MemFree(temp);

    return true;
}

static void jsonList(cJSON* pList, InstanceData* instance, NPVariant* pNpResult)
{
    if (pList == NULL || instance == NULL)
    {
        return;
    }
    
    int size = cJSON_GetArraySize(pList);
    if (size == 0)
    {
        return;
    }

    //为什么要这么做，不知道。必须要这样做。
    NPObject* pWindow = NULL;
    NPN_GetValue(instance->npp, NPNVWindowNPObject, &pWindow);
    char buf[16] = "Array()";
    NPString str;
    str.UTF8Characters = buf;
    str.UTF8Length=strlen(buf);
    
    NPVariant arrayVar;
    NPN_Evaluate(instance->npp, pWindow, &str, &arrayVar);

    int total_size = sizeof(NPVariant)*size;
    NPVariant* array = (NPVariant*)NPN_MemAlloc(total_size);
    memset(array, 0, total_size);

    for (int i=0; i<size; i++)
    {
        cJSON* pItem = cJSON_GetArrayItem(pList, i);
        if (pItem == NULL)
        {
            continue;
        }

        pItem = cJSON_GetObjectItem(pItem, JSON_DATA);
        STRINGN_TO_NPVARIANT(pItem->valuestring, strlen(pItem->valuestring), (array[i]));
        NPN_SetProperty(instance->npp, arrayVar.value.objectValue, NPN_GetIntIdentifier(i), &(array[i]));
    }

    OBJECT_TO_NPVARIANT(arrayVar.value.objectValue, *pNpResult);
}

// {{{{{{{{{{

JsCommander::JsCommander(const int pluginID)
{
	this->m_pJsData = NULL;
    this->m_pJsData = new JsData(pluginID, 0);
}

JsCommander::~JsCommander()
{
    sendCommandExit();
    delete this->m_pJsData;
    this->m_pJsData = NULL;
}

void JsCommander::sendCommandExit()
{
    cJSON* pRoot = cJSON_CreateObject();
    cJSON_AddItemToObject(pRoot, JSON_COMMAND_KEY, cJSON_CreateString(JSON_COMMAND_EXIT));

    m_pJsData->copyData(cJSON_Print(pRoot));

    cJSON_Delete(pRoot);
}

void JsCommander::sendCommandFunction(char* function, const NPVariant* args, const uint32_t argCount)
{
    cJSON* pRoot = cJSON_CreateObject();
    cJSON_AddItemToObject(pRoot, JSON_COMMAND_KEY, cJSON_CreateString(JSON_COMMAND_FUNCTION));
    cJSON_AddStringToObject(pRoot, JSON_FUNCTION_NAME, function);

    if (args != NULL && argCount > 0)
    {
      char itemName[BUFFER_SIZE] = {0};

    cJSON* pTemp = cJSON_CreateObject();
    cJSON_AddItemToObject(pRoot, JSON_FUNCTION_PARAMS, pTemp);
    cJSON_AddNumberToObject(pTemp, JSON_COUNT, argCount);

      for (int i=0; i<argCount; i++)
      {
        memset(itemName, 0, BUFFER_SIZE);
         sprintf(itemName, "%s%d", JSON_FUNCTION_PARAM, i);
        switch (args[i].type)
        {
            case NPVariantType_Int32:
                cJSON_AddNumberToObject(pTemp, itemName, NPVARIANT_TO_INT32(args[i]));
                break;

            case NPVariantType_Bool:
                cJSON_AddBoolToObject(pTemp, itemName, NPVARIANT_TO_BOOLEAN(args[i]));
                break;

            case NPVariantType_Double:
                cJSON_AddBoolToObject(pTemp, itemName, NPVARIANT_TO_DOUBLE(args[i]));
                break;

            case NPVariantType_Null:
                cJSON_AddStringToObject(pTemp, itemName, "");
                break;

            case NPVariantType_String:
            {
                NPString url = NPVARIANT_TO_STRING(args[i]);
                if (url.UTF8Length > 0)
                {
                    char* urlstr = (char*) NPN_MemAlloc(url.UTF8Length + 1);
                    strncpy(urlstr, url.UTF8Characters, url.UTF8Length);
                    urlstr[url.UTF8Length] = '\0';
                    cJSON_AddStringToObject(pTemp, itemName, urlstr);
                    NPN_MemFree(urlstr);
                }
                else
                {
                    cJSON_AddStringToObject(pTemp, itemName, "");
                }
                break;
            }

            case NPVariantType_Void:
            case NPVariantType_Object:
            default:
                break;
        }
      }
    }

    m_pJsData->copyData(cJSON_Print(pRoot));

    cJSON_Delete(pRoot);
}

bool JsCommander::receiveResult(InstanceData* instance, NPVariant* pNpResult)
{
    //改用信号量
    while (m_pJsData->getDataFlag())
    {
#if defined(_WINDOWS)
        Sleep(10);
#else
        sleep(10);
#endif
    }

    cJSON* root = cJSON_Parse(m_pJsData->getData());
    cJSON *type = cJSON_GetObjectItem(root, JSON_DATATYPE);
    cJSON *data = cJSON_GetObjectItem(root, JSON_FUNCTION_RESULT);
    if (type == NULL || type->valuestring == NULL | data == NULL)
    {
        return false;
    }

    if (!strcmp(type->valuestring, JSON_DATATYPE_BOOL))
    {
        BOOLEAN_TO_NPVARIANT(data->valueint, *pNpResult);
    }
    else if (!strcmp(type->valuestring, JSON_DATATYPE_INT))
    {
        DOUBLE_TO_NPVARIANT(data->valueint, *pNpResult);
    }
    else if (!strcmp(type->valuestring, JSON_DATATYPE_DOUBLE))
    {
        DOUBLE_TO_NPVARIANT(data->valuedouble, *pNpResult);
    }
    else if (!strcmp(type->valuestring, JSON_DATATYPE_STRING))
    {
        //必须如此，否则会出错。还不能释放。
        int len = strlen(data->valuestring);
        char* buffer = (char*)NPN_MemAlloc(len);
        memcpy(buffer, data->valuestring, len);
        STRINGN_TO_NPVARIANT(buffer, len, *pNpResult);
    }
    else if (!strcmp(type->valuestring, JSON_DATATYPE_LIST))
    {
        jsonList(data, instance, pNpResult);
    }

    cJSON_Delete(root);

    return true;
}

// }}}}}}}}}}




