/**
 * 版权所有 (c) 2018-2021 泰山信息科技有限公司。保留所有权利。
 */

#include "DataExchange.h"
#include <windows.h>

/*
 *  函数名称:NewPlatformString
 *	函数功能:.......
 *  函数参数:.......
 *  函数返回:            
 */
/*
jstring NewPlatformString(JNIEnv *env, char *s)
{    
	if(s == NULL)
	{
		return NULL;
	}
    int len = (int)strlen(s);
    jclass cls;
    jmethodID mid;
    jbyteArray ary;
	
    cls = env->FindClass("java/lang/String");
    mid = env->GetMethodID(cls, "<init>", "([B)V");
    ary = env->NewByteArray(len);
    if (ary != NULL) 
	{
		jstring str = NULL;
		env->SetByteArrayRegion(ary, 0, len, (jbyte *)s);
		if (!env->ExceptionOccurred()) 
		{
			str = (jstring)env->NewObject(cls, mid, ary);			
		}
		env->DeleteLocalRef(ary);
		return str;
    }
    return NULL;
}*/

/*
 *  函数名称:NewPlatformString
 *	函数功能:将char转成jstring
 *  函数参数:s char字符串
 *  函数返回:jstring字符串            
 */
jstring NewPlatformString(JNIEnv *env, char *s)
{
	if(s == NULL)
	{
		return NULL;
	}
	int size = MultiByteToWideChar(CP_ACP, 0, s, -1, NULL, 0);
	wchar_t *widechars = new wchar_t[size];
	MultiByteToWideChar(CP_ACP,0,s,-1,widechars,size);
	jstring jstr = env->NewString((const jchar *)widechars, wcslen(widechars));
	delete[] widechars;
	return jstr;
}

/*
 *  函数名称:StringToMutilBytes
 *	函数功能:将jstring转成多字节char
 *  函数参数:jstr jstring字符串  mutilBytes 多字节char字符串返回值
 *  函数返回:多字节char字符串mutilBytes长度            
 */
int StringToMutilBytes(JNIEnv *env,jstring jstr,char* mutilBytes)
{
	const jchar* jc  = env->GetStringChars(jstr, NULL);
	int count = env->GetStringLength(jstr);	
	BSTR wc = SysAllocStringLen((const wchar_t *)jc, count);
	int totalSize = WideCharToMultiByte(CP_OEMCP, 0, wc, -1, NULL, 0, NULL, NULL);
	//假如传进的是空，则返回所需要申请的空间字节数。
	if(!mutilBytes)
	{
		env->ReleaseStringChars(jstr, jc);
		SysFreeString(wc);
		return totalSize;
	}
	totalSize = WideCharToMultiByte(CP_OEMCP, 0, wc, -1, mutilBytes, totalSize, NULL, NULL);
	env->ReleaseStringChars(jstr, jc);
	SysFreeString(wc);
	return totalSize;
}

BSTR StringToWidechar(JNIEnv *env,jstring jniString)
{
	const jchar* jc  = env->GetStringChars(jniString, NULL);
	int length = env->GetStringLength(jniString);
	BSTR wc = SysAllocStringLen((const wchar_t *)jc, length);
	env->ReleaseStringChars(jniString, jc);
	return wc;
}

/*
 *  函数名称:StringToWidechar
 *	函数功能:将jstring转成wchar_t*
 *  函数参数:jstr jstring字符串  wChar wchar_t*字符串返回值
 *  函数返回:wchar_t字符串wChar长度            
 */
int StringToWidechar(JNIEnv *env,jstring jniString, wchar_t* wChar)
{
	const jchar* jc  = env->GetStringChars(jniString, NULL);
	int length = env->GetStringLength(jniString);
	if (wChar)
	{
		wcscpy(wChar, (const wchar_t *)jc);
	}
	env->ReleaseStringChars(jniString, jc);
	return length;
}

