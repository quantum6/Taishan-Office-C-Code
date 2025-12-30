/**
 * 版权所有 (c) 2018-2021 泰山信息科技有限公司。保留所有权利。
 */ 
#include <windows.h>
#include "jni.h"
#include "jni_md.h"
#ifdef __cplusplus
extern "C" {
#endif
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
JNIEXPORT jintArray JNICALL Java_tso_tnative_TImage_getMetaSize
(JNIEnv * env,jclass cls,jstring path);

/*
 *  函数名称:drawMetaFile
 *	函数功能:得到元文件的宽、高、水平DPI和垂直DPI
 *  函数参数:jstring path——元文件的路径
 *  函数返回:jintArray jpix——像素数组
 *			 失败时返回null
 */
JNIEXPORT jintArray JNICALL Java_tso_tnative_TImage_drawMetaFile
  (JNIEnv* env, jclass cls, jstring path, jint width, jint height, jint type ,jdoubleArray crop);

/*
 *  函数名称:drawIcon
 *	函数功能:绘制文件的图标
 *  函数参数:jstring path——文件的路径
 *  函数返回:jintArray jpix——像素数组
 *			 失败时返回null
 */
JNIEXPORT jintArray JNICALL Java_tso_tnative_TImage_drawIcon
(JNIEnv* env, jclass cls, jstring path, jint index);

/*
 *	函数功能：将指定的含有Icon文件中的指定索引号的Icon取出，绘制到临时的EMF文件中去
 *  函数参数：pEnv － 本地方法运行环境
 *			  obj  － 本地方法所在的类对象
 *			  imgPath － 临时EMF文件名
 *            filePath － 要取得图标的文件，文件类型可为*.dll,*.exe
 *			  index － 图标的索引
 *			  fisrtLine － 第一行的Label内容
 *			  sndLine － 第二行的Label内容
 *			  fontName － Label的字体名
 *  返回：	 成功返回true
 *           失败返回NULL
 */
JNIEXPORT jboolean JNICALL Java_tso_tnative_TImage_getFileIconByIndex
(JNIEnv *pEnv, jobject obj, jstring imgPath, jstring filePath, jint index, jstring fisrtLine, jstring sndLine, jstring fontName);

/*
 *	函数功能：从文件中取得该文件所包含的图标的数量
 *  函数参数：pEnv － 本地方法运行环境
 *			  obj  － 本地方法所在的类对象
 *            filePath － 要取得图标的文件，文件类型可为*.dll,*.exe
 *  返回：   返回该文件中包含的图标的数量
 *           失败返回NULL
 */
JNIEXPORT jint JNICALL Java_tso_tnative_TImage_getFileIconNum
(JNIEnv *pEnv, jobject obj, jstring filePath);

#ifdef __cplusplus
}
#endif