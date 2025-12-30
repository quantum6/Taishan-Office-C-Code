/**
 * 版权所有 (c) 2018-2021 泰山信息科技有限公司。保留所有权利。
 */
#include "DataExchange.h"
#include "Function.h"
#include "jni.h"
#include "jni_md.h"

/*
 *  函数名称:getMetaSize
 *	函数功能:得到元文件的宽、高、水平DPI和垂直DPI
 *  函数参数:jstring path——元文件的路径
 *  函数返回:jintArray size
 *					   size[0]——宽
 *					   size[1]——高
 *					   size[2]——水平DPI
 *					   size[3]——垂直DPI
 *			 失败时返回null
 */
jintArray getMetaSize(JNIEnv * env,jclass cls,jstring path);
/*
 *  函数名称:drawMetaFile
 *	函数功能:得到元文件的宽、高、水平DPI和垂直DPI
 *  函数参数:jstring path——元文件的路径
 *  函数返回:jintArray jpix——像素数组
 *			 失败时返回null
 */
jintArray drawMetaFile(JNIEnv* env, jclass cls, jstring path,jint cx, jint cy, jint type, jdoubleArray crop);

jintArray drawIcon(JNIEnv* env, jclass cls, jstring path, jint index);

jboolean getFileIconByIndex(JNIEnv *pEnv, jobject obj, jstring imgPath, jstring filePath, jint index, jstring firstLine, jstring sndLine, jstring fontName);

jint getFileIconNum(JNIEnv *pEnv, jobject obj, jstring filePath);
