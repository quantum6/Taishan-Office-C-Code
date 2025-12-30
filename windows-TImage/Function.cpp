  /**
 * 版权所有 (c) 2018-2021 泰山信息科技有限公司。保留所有权利。
 */ 
#include "Function.h"
#include "DataExchange.h"
#define ULONG_PTR unsigned long*
#include "GdiPlus.h"
using namespace Gdiplus;

#include "jni.h"
#include <windows.h>
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



BOOL PaintByGdiPlus(HDC hdc, const wchar_t *wPath, jint width, jint height, double* crop)
{
 	Graphics graphics(hdc);
	graphics.SetSmoothingMode(SmoothingModeHighSpeed);
	graphics.SetPageUnit(UnitPixel);
	Image *image = new Image(wPath); 
	Status isDrawn = WrongState;
	if (crop != NULL)
	{
		
		int oWidth = image->GetWidth();
		int oHeight = image->GetHeight();

	
		float x1 = (float)(oWidth * crop[1] / 100);
		float y1 = (float)(oHeight * crop[0] / 100);

		double w = oWidth - x1 - oWidth * crop[3] / 100;
		double h = oHeight - y1 - oHeight * crop[2] / 100;
		float xscale = (float)(width * 1.0 / w);
		float yscale = (float)(height * 1.0 / h);
		graphics.TranslateTransform(-x1, -y1);
		graphics.ScaleTransform(xscale, yscale, MatrixOrderAppend);
		isDrawn = graphics.DrawImage(image, 0, 0, oWidth, oHeight);
		
	}
	else 
	{
		isDrawn = graphics.DrawImage(image, 0, 0, width, height);
	}
	delete image;
	if (isDrawn == Ok) 
	{
		return TRUE;
	}
	else 
	{
		return FALSE;
	}
}

/*
 *  函数名称:getPositive
 *	函数功能:得到正值
 *  函数参数:origingal——一个WORD值
 *  函数返回:返回origingal的正值
 */
WORD getPositive(WORD original)
{
	WORD flag = 0x8000;
	
	if (original>flag)
	{
		return (~original) + 0x0001;
	}
	else
	{
		return original;
	}
}

WORD getSize(const wchar_t *wPath, bool isHeight)
{
	Image *image = new Image(wPath);
	if (image != NULL) 
	{
		WORD height = (WORD)image->GetHeight();
		WORD width = (WORD)image->GetWidth();
		delete image;
	
		if (isHeight)
		{
			return height;
		}
		return width;
	}
	return NULL;
}

/*
 *  函数名称:getWinOsFontWidth
 *	函数功能:得到字符串绘制后的宽度
 *  函数参数:wchar_t* label——字符串
 *			 jstring fontName——字体名			 
 *  函数返回:字符串绘制后的宽度
 *			 失败时返回null
 */
LONG getWinOsFontWidth(wchar_t* label, jstring fontName, JNIEnv *env)
{
	if (label == NULL)
	{
		return -1L;
	}
	int needsize = StringToMutilBytes(env, fontName,NULL);
	char * szFontName = new char[needsize];
	StringToMutilBytes(env, fontName, szFontName);
	FLOAT fontSize = 12.0f;
	HDC dc = GetDC(NULL);
	LOGFONT m_lf;
	memset (&m_lf, 0, sizeof(m_lf));
	m_lf.lfCharSet	= DEFAULT_CHARSET;
	strcpy(m_lf.lfFaceName, szFontName);
	m_lf.lfHeight = (LONG)- fontSize * GetDeviceCaps(dc, LOGPIXELSY) / 72;
	m_lf.lfItalic = FALSE;
	m_lf.lfUnderline = FALSE;
	m_lf.lfWeight = FW_NORMAL;

	HFONT m_font = CreateFontIndirect(&m_lf);

	HGDIOBJ fontObj = SelectObject(dc,m_font);
	SIZE lpsize;
	memset (&lpsize, 0, sizeof(lpsize));	

	if(GetTextExtentPoint32W(dc,label,wcslen(label),&lpsize)==0)
	{
		return -1L;
	}
	ReleaseDC(NULL,dc);
    DeleteObject(m_font); 
	return lpsize.cx;
}
/*	函数名称:CreateBlankEmf
 *	函数功能:创建一个空白的Emf
 *	函数参数:szPath——路径	
 *	函数返回:成功为true，否则为false
 *			
 */
BOOL CreateBlankEmf(const char* szPath)
{
 // 创建一个空的EMF文件
	HENHMETAFILE hemf ;
    HDC   hdcEMF ;
	RECT  rect;
 
	rect.left = 0;
	rect.right = 160;
	rect.top = 0;
	rect.bottom = 90;
 
	hdcEMF = CreateEnhMetaFile (NULL, szPath, &rect, NULL);
 
	hemf = CloseEnhMetaFile (hdcEMF) ;
 
	return DeleteEnhMetaFile(hemf); 
}