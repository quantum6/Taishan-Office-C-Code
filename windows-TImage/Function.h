/**
 * 版权所有 (c) 2018-2021 泰山信息科技有限公司。保留所有权利。
 */

#include <windows.h>

#include "jni.h"
#include "jni_md.h"
/*
 *  函数名称:PaintByGdiPlus
 *	函数功能:用GDI+绘制元文件
 *  函数参数:hdc——设备环境句柄
 *			 wPath——元文件路径
 *			 cx——宽
 *			 cy——高
 *			 isTruesize——是否取真实尺寸
 *  函数返回:成功绘制返回TRUE
 *			 否则返回FALSE
 */
BOOL PaintByGdiPlus(HDC hdc, const wchar_t *wPath, jint width, jint height, double* crop);

/*
 *  函数名称:getPositive
 *	函数功能:得到正值
 *  函数参数:origingal——一个WORD值
 *  函数返回:返回origingal的正值
 */
WORD getPositive(WORD original);

WORD getSize(const wchar_t *wPath, bool isHeight);

LONG getWinOsFontWidth(wchar_t* label, jstring fontName, JNIEnv *env);

BOOL CreateBlankEmf(const char* szPath);