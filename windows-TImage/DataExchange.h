/**
 * 版权所有 (c) 2018-2021 泰山信息科技有限公司。保留所有权利。
 */ 

#include "jni.h"
#include "jni_md.h"
#include <WINDOWS.H>

/*
 *  函数名称:NewPlatformString
 *	函数功能:将char转成jstring
 *  函数参数:s char字符串
 *  函数返回:jstring字符串            
 */
jstring NewPlatformString(JNIEnv *env, char *s);
/*
 *  函数名称:StringToMutilBytes
 *	函数功能:将jstring转成char
 *  函数参数:jstr jstring字符串  mutilBytes char字符串返回值
 *  函数返回:char字符串mutilBytes长度            
 */
int StringToMutilBytes(JNIEnv *env,jstring jstr,char* mutilBytes);

BSTR StringToWidechar(JNIEnv *env,jstring jniString);

/*
 *  函数名称:StringToWidechar
 *	函数功能:将jstring转成wchar_t*
 *  函数参数:jstr jstring字符串  wChar wchar_t*字符串返回值
 *  函数返回:wchar_t字符串wChar长度            
 */
int StringToWidechar(JNIEnv *env,jstring jniString, wchar_t* wChar);