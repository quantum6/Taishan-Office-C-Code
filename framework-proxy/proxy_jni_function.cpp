
#include "fw_kit.h"
#include "fw_log.h"
#include "npplugin_js_data.h"
#include "proxy_jni_function.h"
#include "app_jni_command.h"


#define RELEASE_JSTRING(jStr, pJNIEnv) if (jStr != NULL) pJNIEnv->DeleteLocalRef(jStr);

// {{{{{{{{{{

#define CHECK_JSON_PARAM_COUNT(pJson,want)                     if (!checkJsonParamCount(pJson, want)) return false;
#define GET_JSON_PARAM_INDEX_JSTRING(pJson,index)              cJSON* param = getParamN(pJson, index); \
                                                                                                                              if (param==NULL) return NULL;
#define GET_JSON_PARAM_INDEX_JINT(pJson,index)                 cJSON* param = getParamN(pJson, index); \
                                                                                                                              if (param==NULL) return 0;
#define GET_JSON_PARAM_INDEX_JDOUBLE(pJson,index)              cJSON* param = getParamN(pJson, index); \
                                                                                                                              if (param==NULL) return 0;
#define GET_JSON_PARAM_INDEX_JBOOLEAN(pJson,index)             cJSON* param = getParamN(pJson, index); \
                                                                                                                              if (param==NULL) return false;


static bool checkJsonParamCount(cJSON* pJson, const int want)
{
    if (want <= 0)
    {
        return true;
    }

    cJSON* pItem = cJSON_GetObjectItem(pJson, JSON_COUNT);
    if (pItem == NULL || pItem->valueint != want)
    {
        return false;
    }
    return true;
}

static cJSON* getParamN(cJSON* pJson, const int index)
{
      char param[BUFFER_SIZE] = {0};
      sprintf(param, "%s%d", JSON_FUNCTION_PARAM, index);
      return cJSON_GetObjectItem(pJson, param);
}

static jstring jsonParamToJstring(cJSON* pJson, const int index, const JNIEnv* pEnv)
{
    GET_JSON_PARAM_INDEX_JSTRING(pJson, index);
    jstring jstr = ((JNIEnv*)pEnv)->NewStringUTF(param->valuestring);
    //释放会出错
    //NPN_MemFree(urlstr);

    return jstr;
}


static jboolean jsonParamToJboolean(cJSON* pJson, const int index, const JNIEnv* pEnv)
{
    GET_JSON_PARAM_INDEX_JBOOLEAN(pJson, index);
    return (jboolean)(param->valueint);
}

static jint jsonParamToJint(cJSON* pJson, const int index, const JNIEnv* pEnv)
{
    GET_JSON_PARAM_INDEX_JINT(pJson, index);
    return (jint)(param->valueint);
}

static jshort jsonParamToJshort(cJSON* pJson, const int index, const JNIEnv* pEnv)
{
    GET_JSON_PARAM_INDEX_JINT(pJson, index);
    return (jshort)(param->valueint);
}

static jdouble jsonParamToJdouble(cJSON* pJson, const int index, const JNIEnv* pEnv)
{
    GET_JSON_PARAM_INDEX_JDOUBLE(pJson, index);
    return (jdouble)(param->valuedouble);
}

static jfloat jsonParamToJfloat(cJSON* pJson, const int index, const JNIEnv* pEnv)
{
    GET_JSON_PARAM_INDEX_JDOUBLE(pJson, index);
    return (jfloat)(param->valuedouble);
}

// }}}}}}}}}}


// {{{{{{{{{{

static void jbooleanToJson(cJSON* pResult, jboolean jresult)
{
    cJSON_AddStringToObject(pResult, JSON_DATATYPE, JSON_DATATYPE_BOOL);
    cJSON_AddBoolToObject(pResult, JSON_FUNCTION_RESULT,  (bool)jresult);
}

static void jintToJson(cJSON* pResult, jint jresult)
{
    cJSON_AddStringToObject(pResult, JSON_DATATYPE, JSON_DATATYPE_INT);
    cJSON_AddNumberToObject(pResult, JSON_FUNCTION_RESULT,  (int)jresult);
}

static void jdoubleToJson(cJSON* pResult, jdouble jresult)
{
    cJSON_AddStringToObject(pResult, JSON_DATATYPE, JSON_DATATYPE_DOUBLE);
    cJSON_AddNumberToObject(pResult, JSON_FUNCTION_RESULT,  (double)jresult);
}

static void jfloatToJson(cJSON* pResult, jfloat jresult)
{
    jdoubleToJson(pResult, (jdouble)jresult);
}

static void jstringToCharBuffer(JNIEnv* pEnv, jclass jClassString, jstring jresult, char** ppBuffer)
{
    jmethodID  jmethod = pEnv->GetMethodID(jClassString, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray jarr    = (jbyteArray)pEnv->CallObjectMethod(jresult, jmethod, pEnv->NewStringUTF(CHARSET_UTF8));
    jsize      jlen    = pEnv->GetArrayLength(jarr);
    jbyte*     jdata   = pEnv->GetByteArrayElements(jarr, JNI_FALSE);

    if (*ppBuffer == NULL)
    {
        *ppBuffer = (char*)malloc((int)jlen+1);
        memset(*ppBuffer, 0, (int)jlen +1);
    }
    strncpy(*ppBuffer, (char*)jdata, (int)jlen);

    pEnv->ReleaseByteArrayElements(jarr, jdata, 0);
}

static void jstringToJson(cJSON* pResult, jstring jresult, AppJniCommand* pJniCommand, JNIEnv* pEnv)
{
    cJSON_AddStringToObject(pResult, JSON_DATATYPE, JSON_DATATYPE_STRING);

    char* buffer = NULL;
	jstringToCharBuffer(pEnv, pJniCommand->getJniRuntime()->jClassString, jresult, &buffer);
    cJSON_AddStringToObject(pResult, JSON_FUNCTION_RESULT,  buffer);

    if (buffer != NULL)
    {
        free(buffer);
    }
}

static void jlistToJson(cJSON* pResult, jobject jlist, AppJniCommand* pJniCommand, JNIEnv* pEnv)
{
    cJSON_AddStringToObject(pResult, JSON_DATATYPE, "list");

    jclass    list_cls  = pEnv->GetObjectClass(jlist);
    jmethodID list_get  = pEnv->GetMethodID(list_cls, "get",  "(I)Ljava/lang/Object;");
    jmethodID list_size = pEnv->GetMethodID(list_cls, "size", "()I");
    
    int size = static_cast<int>(pEnv->CallIntMethod(jlist, list_size));
    if (size == 0)
    {
        return;
    }

    cJSON* listJson = cJSON_CreateArray();
    cJSON_AddItemToObject(pResult, JSON_FUNCTION_RESULT, listJson);

    char* buffer = NULL;
    for (int i=0; i<size; i++)
    {
        jstring text = static_cast<jstring>(pEnv->CallObjectMethod(jlist, list_get, i));
        if (text == NULL)
        {
            continue;
        }
		jstringToCharBuffer(pEnv, pJniCommand->getJniRuntime()->jClassString, text, &buffer);

        cJSON *item=cJSON_CreateObject();
        cJSON_AddStringToObject(item, JSON_DATATYPE, JSON_DATATYPE_STRING);
        cJSON_AddStringToObject(item, JSON_DATA,  buffer);
        cJSON_AddItemToArray(listJson, item);
        if (buffer != NULL)
        {
            free(buffer);
            buffer = NULL;
        }
    }
}

// }}}}}}}}}}


// {{{{{{{{{{

bool
JNI_FUNCTION_F_Z(const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult)
{
    CHECK_JSON_PARAM_COUNT(pJson, 1);

	jfloat   param1   = jsonParamToJfloat( pJson, 0, pEnv);

	jmethodID jmethod = pEnv->GetMethodID(      pJniCommand->jCommandClass,   method, "(F)Z");
	jboolean  jresult = pEnv->CallBooleanMethod(pJniCommand->jCommandObject, jmethod, param1);
    jbooleanToJson(pResult, jresult);

    return true;
}

bool
JNI_FUNCTION_StrFFFF_Z(const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult)
{
    CHECK_JSON_PARAM_COUNT(pJson, 5);

    jstring  param1   = jsonParamToJstring(pJson, 0, pEnv);
    jfloat   param2   = jsonParamToJfloat( pJson, 1, pEnv);
    jfloat   param3   = jsonParamToJfloat( pJson, 2, pEnv);
    jfloat   param4   = jsonParamToJfloat( pJson, 3, pEnv);
    jfloat   param5   = jsonParamToJfloat( pJson, 4, pEnv);
    
	jmethodID jmethod = pEnv->GetMethodID(      pJniCommand->jCommandClass,   method, "(Ljava/lang/String;FFFF)Z");
	jboolean  jresult = pEnv->CallBooleanMethod(pJniCommand->jCommandObject, jmethod, param1, param2, param3, param4, param5);
    jbooleanToJson(pResult, jresult);

    RELEASE_JSTRING(param1, pEnv);

    return true;
}

bool
JNI_FUNCTION_StrIF_Z(const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult)
{
    CHECK_JSON_PARAM_COUNT(pJson, 3);

    jstring  param1   = jsonParamToJstring(pJson, 0, pEnv);
    jint     param2   = jsonParamToJint(   pJson, 1, pEnv);
    jfloat   param3   = jsonParamToJfloat( pJson, 2, pEnv);
    
	jmethodID jmethod = pEnv->GetMethodID(      pJniCommand->jCommandClass,   method, "(Ljava/lang/String;IF)Z");
    jboolean  jresult = pEnv->CallBooleanMethod(pJniCommand->jCommandObject, jmethod, param1, param2, param3);
    jbooleanToJson(pResult, jresult);

    RELEASE_JSTRING(param1, pEnv);

    return true;
}

bool
JNI_FUNCTION_StrIIZ_Z(const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult)
{
    CHECK_JSON_PARAM_COUNT(pJson, 4);

	jstring  param1   = jsonParamToJstring( pJson, 0, pEnv);
    jint     param2   = jsonParamToJint(    pJson, 1, pEnv);
    jint     param3   = jsonParamToJint(    pJson, 2, pEnv);
    jboolean param4   = jsonParamToJboolean(pJson, 3, pEnv);
    
    jmethodID jmethod = pEnv->GetMethodID(      pJniCommand->jCommandClass,   method, "(Ljava/lang/String;IIZ)Z");
    jboolean  jresult = pEnv->CallBooleanMethod(pJniCommand->jCommandObject, jmethod, param1, param2, param3, param4);
    jbooleanToJson(pResult, jresult);

    RELEASE_JSTRING(param1, pEnv);

    return true;
}

bool
JNI_FUNCTION_StrII_Z(const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult)
{
    CHECK_JSON_PARAM_COUNT(pJson, 3);

	jstring  param1   = jsonParamToJstring( pJson, 0, pEnv);
    jint     param2   = jsonParamToJint(    pJson, 1, pEnv);
    jint     param3   = jsonParamToJint(    pJson, 2, pEnv);
    
    jmethodID jmethod = pEnv->GetMethodID(                pJniCommand->jCommandClass,   method, "(Ljava/lang/String;II)Z");
    jboolean  jresult = (jboolean)pEnv->CallBooleanMethod(pJniCommand->jCommandObject, jmethod, param1, param2, param3);
    jbooleanToJson(pResult, jresult);

    RELEASE_JSTRING(param1, pEnv);

    return true;
}

bool
JNI_FUNCTION_StrIStr_Z(const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult)
{
    CHECK_JSON_PARAM_COUNT(pJson, 3);

	jstring  param1   = jsonParamToJstring( pJson, 0, pEnv);
    jint     param2   = jsonParamToJint(    pJson, 1, pEnv);
	jstring  param3   = jsonParamToJstring( pJson, 2, pEnv);
    
    jmethodID jmethod = pEnv->GetMethodID(      pJniCommand->jCommandClass,   method, "(Ljava/lang/String;ILjava/lang/String;)Z");
    jboolean  jresult = pEnv->CallBooleanMethod(pJniCommand->jCommandObject, jmethod, param1, param2, param3);
    jbooleanToJson(pResult, jresult);

    RELEASE_JSTRING(param1, pEnv);
    RELEASE_JSTRING(param3, pEnv);

    return true;
}

bool
JNI_FUNCTION_StrStr_Z(const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult)
{
    CHECK_JSON_PARAM_COUNT(pJson, 2);

    jstring  param1   = jsonParamToJstring( pJson, 0, pEnv);
    jstring  param2   = jsonParamToJstring( pJson, 1, pEnv);

    jmethodID jmethod = pEnv->GetMethodID(      pJniCommand->jCommandClass,   method, "(Ljava/lang/String;Ljava/lang/String;)Z");
    jboolean  jresult = pEnv->CallBooleanMethod(pJniCommand->jCommandObject, jmethod, param1, param2);
    jbooleanToJson(pResult, jresult);

    RELEASE_JSTRING(param1, pEnv);
    RELEASE_JSTRING(param2, pEnv);

    return true;
}

bool
JNI_FUNCTION_Str4_Z(const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult)
{
    CHECK_JSON_PARAM_COUNT(pJson, 4);

    jstring  param1   = jsonParamToJstring( pJson, 0, pEnv);
    jstring  param2   = jsonParamToJstring( pJson, 1, pEnv);
    jstring  param3   = jsonParamToJstring( pJson, 2, pEnv);
    jstring  param4   = jsonParamToJstring( pJson, 3, pEnv);

    jmethodID jmethod = pEnv->GetMethodID(      pJniCommand->jCommandClass,   method, "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z");
    jboolean  jresult = pEnv->CallBooleanMethod(pJniCommand->jCommandObject, jmethod, param1, param2, param3, param4);
    jbooleanToJson(pResult, jresult);

    RELEASE_JSTRING(param1, pEnv);
    RELEASE_JSTRING(param2, pEnv);
    RELEASE_JSTRING(param3, pEnv);
    RELEASE_JSTRING(param4, pEnv);

    return true;
}

bool
JNI_FUNCTION_StrS_Z(const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult)
{
    CHECK_JSON_PARAM_COUNT(pJson, 2);

    jstring  param1   = jsonParamToJstring(pJson, 0, pEnv);
    jshort   param2   = jsonParamToJshort( pJson, 1, pEnv);
      
    jmethodID jmethod = pEnv->GetMethodID(      pJniCommand->jCommandClass,   method, "(Ljava/lang/String;S)Z");
    jboolean  jresult = pEnv->CallBooleanMethod(pJniCommand->jCommandObject, jmethod, param1, param2);
    jbooleanToJson(pResult, jresult);

    RELEASE_JSTRING(param1, pEnv);

    return true;
}

bool
JNI_FUNCTION_StrZ_Z(const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult)
{
    CHECK_JSON_PARAM_COUNT(pJson, 2);

    jstring  param1   = jsonParamToJstring( pJson, 0, pEnv);
    jboolean param2   = jsonParamToJboolean(pJson, 1, pEnv);

    jmethodID jmethod = pEnv->GetMethodID(      pJniCommand->jCommandClass,   method, "(Ljava/lang/String;Z)Z");
    jboolean  jresult = pEnv->CallBooleanMethod(pJniCommand->jCommandObject, jmethod, param1, param2);
    jbooleanToJson(pResult, jresult);

    RELEASE_JSTRING(param1, pEnv);

    return true;
}

bool
JNI_FUNCTION_Str_Str(const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult)
{
    CHECK_JSON_PARAM_COUNT(pJson, 1);

    jstring  param1   = jsonParamToJstring( pJson, 0, pEnv);
    
    jmethodID jmethod = pEnv->GetMethodID(              pJniCommand->jCommandClass,   method, "(Ljava/lang/String;)Ljava/lang/String;");
    jstring   jresult = (jstring)pEnv->CallObjectMethod(pJniCommand->jCommandObject, jmethod, param1);
    jstringToJson(pResult, jresult, (AppJniCommand*)pJniCommand, pEnv);

    RELEASE_JSTRING(param1, pEnv);

    return true;
}

bool
JNI_FUNCTION_Str_Z(const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult)
{
    CHECK_JSON_PARAM_COUNT(pJson, 1);

    jstring param1    = jsonParamToJstring(pJson, 0, pEnv);
    
    jmethodID jmethod = pEnv->GetMethodID(      pJniCommand->jCommandClass,   method, "(Ljava/lang/String;)Z");
    jboolean  jresult = pEnv->CallBooleanMethod(pJniCommand->jCommandObject, jmethod, param1);
    jbooleanToJson(pResult, jresult);

    RELEASE_JSTRING(param1, pEnv);

    return true;
}

bool
JNI_FUNCTION_S_Z(const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult)
{
    CHECK_JSON_PARAM_COUNT(pJson, 1);

    jshort   param1   = jsonParamToJshort(pJson, 0, pEnv);
    
    jmethodID jmethod = pEnv->GetMethodID(      pJniCommand->jCommandClass,   method, "(S)Z");
    jboolean  jresult = pEnv->CallBooleanMethod(pJniCommand->jCommandObject, jmethod, param1);
    jbooleanToJson(pResult, jresult);

    return true;
}

bool
JNI_FUNCTION_V_F(const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult)
{
    CHECK_JSON_PARAM_COUNT(pJson, 0);

    jmethodID jmethod = pEnv->GetMethodID(    pJniCommand->jCommandClass,   method, "()F");
    jfloat    jresult = pEnv->CallFloatMethod(pJniCommand->jCommandObject, jmethod);
    jfloatToJson(pResult, jresult);

    return true;
}

bool
JNI_FUNCTION_V_List(const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult)
{
    CHECK_JSON_PARAM_COUNT(pJson, 0);

    jmethodID jmethod = pEnv->GetMethodID(     pJniCommand->jCommandClass,   method, "()Ljava/util/List;");
    jobject   jresult = pEnv->CallObjectMethod(pJniCommand->jCommandObject, jmethod);
    jlistToJson(pResult, jresult, (AppJniCommand*)pJniCommand, pEnv);

    return true;
}

bool
JNI_FUNCTION_V_Str(const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult)
{
    CHECK_JSON_PARAM_COUNT(pJson, 0);

    jmethodID jmethod =          pEnv->GetMethodID(     pJniCommand->jCommandClass,   method, "()Ljava/lang/String;");
    jstring   jresult = (jstring)pEnv->CallObjectMethod(pJniCommand->jCommandObject, jmethod);
    jstringToJson(pResult, jresult, (AppJniCommand*)pJniCommand, pEnv);

    return true;
}

bool
JNI_FUNCTION_V_Z(const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult)
{
    CHECK_JSON_PARAM_COUNT(pJson, 0);

    jmethodID jmethod = pEnv->GetMethodID(      pJniCommand->jCommandClass,   method, "()Z");
    jboolean  jresult = pEnv->CallBooleanMethod(pJniCommand->jCommandObject, jmethod);
    jbooleanToJson(pResult, jresult);

    return true;
}

bool
JNI_FUNCTION_Z_Z(const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult)
{
    CHECK_JSON_PARAM_COUNT(pJson, 1);

    jboolean  param1  = jsonParamToJboolean(pJson, 0, pEnv);

    jmethodID jmethod = pEnv->GetMethodID(      pJniCommand->jCommandClass,   method, "(Z)Z");
	jboolean  jresult = pEnv->CallBooleanMethod(pJniCommand->jCommandObject, jmethod, param1);
    jbooleanToJson(pResult, jresult);

    return true;
}

// }}}}}}}}}}
