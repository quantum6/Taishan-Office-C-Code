
#ifndef __PROXY_JNI_FUNCTION_H__
#define __PROXY_JNI_FUNCTION_H__

#include "cJSON.h"
#include "jni.h"
#include "app_jni_command.h"


typedef bool (* JniFunction)
  (const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult);

bool JNI_FUNCTION_F_Z(      const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult);
bool JNI_FUNCTION_StrFFFF_Z(const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult);
bool JNI_FUNCTION_StrIF_Z(  const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult);
bool JNI_FUNCTION_StrIIZ_Z( const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult);
bool JNI_FUNCTION_StrII_Z(  const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult);
bool JNI_FUNCTION_StrIStr_Z(const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult);
bool JNI_FUNCTION_StrStr_Z( const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult);

bool JNI_FUNCTION_Str4_Z(   const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult);
bool JNI_FUNCTION_StrS_Z(   const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult);
bool JNI_FUNCTION_StrZ_Z(   const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult);

bool JNI_FUNCTION_Str_Str(  const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult);
bool JNI_FUNCTION_Str_Z(    const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult);

bool JNI_FUNCTION_S_Z(      const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult);

bool JNI_FUNCTION_V_List(   const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult);
bool JNI_FUNCTION_V_Str(    const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult);
bool JNI_FUNCTION_V_F(      const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult);

bool JNI_FUNCTION_V_Z(      const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult);
bool JNI_FUNCTION_Z_Z(      const AppJniCommand* pJniCommand, JNIEnv* pEnv, cJSON* pJson, const char* method, cJSON* pResult);

typedef struct
{
    char*              funcName;
    JniFunction funcJava;
} JniFunctionPair;

#endif // __PROXY_JNI_FUNCTION_H__
