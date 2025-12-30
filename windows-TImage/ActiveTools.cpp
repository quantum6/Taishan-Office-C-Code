  /**
 * 版权所有 (c) 2018-2021 泰山信息科技有限公司。保留所有权利。
 */ 
#include <windows.h>
#include <SHELLAPI.H>
#include "ActiveTools.h"

#define ULONG_PTR unsigned long*
#include "GdiPlus.h"
using namespace Gdiplus;

#pragma comment(lib, "GdiPlus.lib")


GdiplusStartupInput m_gdiplusStartupInput; 
ULONG_PTR m_gdiplusToken = NULL;

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
jintArray getMetaSize(JNIEnv * env,jclass cls,jstring path)
{
	jintArray size = NULL;

	const jchar *wPath = env->GetStringChars(path, NULL);

	if(wPath == NULL)
		return size;

	//wchar_t *wPath=new wchar_t[MAX_PATH]; 
	//memset(wPath, '\0', MAX_PATH*2);
	//MultiByteToWideChar(CP_ACP,0,name,-1,wPath,strlen(name)*2); 
	//if(longName != NULL)
		//delete[] longName;
	INT dpix;
	INT dpiy;
	UINT width;
	UINT height;
	
	FILE *isExist = _wfopen((const wchar_t *)wPath,L"r");
	if(isExist == NULL)
	{
		return NULL;
	}
	else
	{
		fclose(isExist);
	}

	if(GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL) == Ok)
	{		
		MetafileHeader metafileHeader;
		Metafile::GetMetafileHeader((const wchar_t *)wPath,&metafileHeader);
		dpix = (INT) metafileHeader.GetDpiX();
		dpiy = (INT) metafileHeader.GetDpiY();
		width = metafileHeader.Width;
		height = metafileHeader.Height;

		BOOL isSmf = FALSE;
		WORD Vertion1 = 0x0001;
		WORD Vertion2 = 0x0009;
		WORD buff;
		FILE *smfTest = _wfopen((const wchar_t *)wPath, L"rb");
		buff = getwc(smfTest);
		if (buff == Vertion1) 
		{
			buff = getwc(smfTest);
			if (buff == Vertion2) 
			{
				isSmf = TRUE;
			}
		}
		fclose(smfTest);

		if (isSmf)
		{
			WORD cx ;
			WORD cy ;
			WORD flag = 0x020C;
			WORD s;
			FILE *temp = _wfopen((const wchar_t *)wPath, L"rb");
			if (temp != NULL)
			{
				for(int i = 0;i<128;i++)
				{
					if (feof(temp) != 0) 
					{						
						break;
					}
					s = getwc(temp);
					if (flag == s ) 
					{
						if (feof(temp) == 0) 
						{
							cy = getwc(temp);
						}
						if (feof(temp) == 0) 
						{
							cx = getwc(temp);
						}
						
						if (cx != 0 && cy != 0) 
						{						
							width = getPositive(cx);
							height = getPositive(cy);
							break;
						}
					}
				}
				if (temp != NULL)
				{				
					fclose(temp);
				}
			 }
		}
		GdiplusShutdown(m_gdiplusToken);
	}
		
		int iSize[4];
		iSize[0] = width;
		iSize[1] = height;
		iSize[2] = dpix;
		iSize[3] = dpiy;
		
		size = env->NewIntArray(4);
		if(size != NULL)
		{
			env->SetIntArrayRegion(size, 0, 4 , (jint*)iSize);
		} 


/*
	if(name != NULL)
		delete[] name; */

	if(wPath != NULL)
	{
		env->ReleaseStringChars(path, wPath);
	}
	return size;
}


/*
void log(int i)
{
	FILE *test = fopen("C:\\1.txt","awt");
	if (test != NULL)
	{
		fprintf(test,"%d\n",i);
		fclose(test);
	}
}*/


/*
 *  函数名称:drawMetaFile
 *	函数功能:得到元文件的宽、高、水平DPI和垂直DPI
 *  函数参数:jstring path——元文件的路径
 *  函数返回:jintArray jpix——像素数组
 *			 失败时返回null
 */
jintArray drawMetaFile(JNIEnv* env, jclass cls, jstring path,jint width, jint height, jint type, jdoubleArray crop)
{
	if(!path)
	{
		return NULL;
	}

	int size = 0;
	double* crop1 = NULL;
	if (crop != NULL)
	{
		size = env->GetArrayLength(crop);
	}
	
	if (size > 0)
	{
		crop1 = (double*)malloc(size * sizeof(double));
		crop1 = env->GetDoubleArrayElements(crop, NULL);
	}

	
	
	//WMF句柄
	HMETAFILE hWmf = NULL;	
	//EMF句柄
	HENHMETAFILE hEmf = NULL;
	//返回的java中的象素数组数据
	jintArray jpix = NULL;
	//显示模式颜色位数
	int bits = 0;
	//底色
	DWORD	initColor = 0;
	//象素整形数据
	DWORD * pp		= NULL;
	HBITMAP memBM	= NULL;
	HDC		memDC	= NULL;
	HDC		ddc		= NULL;
	//处理图片路径信息//
	/*
	int needSize = StringToMutilBytes(env,path,NULL);
		LPSTR longName = new char[needSize];
		StringToMutilBytes(env,path,longName);
		LPTSTR name = new char [MAX_PATH];
		//处理长文件名，98下面不支持
		GetShortPathName(longName, name, MAX_PATH);
	
		wchar_t *wPath = new wchar_t[strlen(name)*2];				
		MultiByteToWideChar(CP_ACP,0,name,-1,wPath,strlen(name)*2);		
		if (longName != NULL) 
		{
			delete[] longName;
		}*/
	const wchar_t *wPath = (const wchar_t *)env->GetStringChars(path, NULL);
	
	//初始化
	int len = width * height, i;
	RECT rt;
	rt.left = 0;
	rt.top = 0;
	rt.right = width;
	rt.bottom = height;
	
	BITMAPINFOHEADER bih;
	
	bih.biSize			= sizeof(BITMAPINFOHEADER);
	bih.biWidth			= width;
	//应代码审查而修改 by user271 2004-07-26
	//在此处为负值是为了得到正确的图片，如果不为负值，则图片是倒的
	bih.biHeight		= -height;
	bih.biPlanes		= 1;
	bih.biBitCount		= 32;
	bih.biCompression	= BI_RGB;
	bih.biSizeImage		= 0;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed		= 0;
	bih.biClrImportant	= 0;
				
	BITMAPINFO bi;
	bi.bmiHeader = bih;
	
	PBYTE pBits = new BYTE[len*4];//(PBYTE)malloc(len * 4);
	if (pBits == NULL)
	{	
		//delete[] name;
		//delete[] wPath;
		if (crop1 != NULL) 
		{
			env->ReleaseDoubleArrayElements(crop, crop1, NULL);
		}
		env->ReleaseStringChars(path, (const jchar *)wPath);
		return jpix;
	}
	(DWORD*)pp =(DWORD*)pBits;
	//bug:57161 ,90174,97402,102346,94115  by user271 2004-12-28
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	memDC = CreateCompatibleDC (NULL);
	//设置透明色
	bits = GetDeviceCaps (memDC, BITSPIXEL);
	if (::GetVersionEx(&osvi))
	{
		if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			switch(bits)
			{
				case 8:
					initColor = 0xc0c0c0;
					break;
				case 16:
					initColor = 0xFFFFF7;
					break;
				default: 
				//24位和32位处理相同
				initColor = 0xFFFFFE;
			}
		}
		else if ((osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
			((osvi.dwMajorVersion > 4) || ((osvi.dwMajorVersion == 4) && (osvi.dwMinorVersion > 0))))
		{
			switch(bits)
			{
				case 8:
					initColor = 0xc0c0c0;
					break;
				case 16:
					initColor = 0xFFFFF0;
					break;
				case 24:
					initColor = 0xFEFDFE;
					break;
				default:
					//32位
					initColor = 0xFFFFFE;
			}
		}
	}
	for (i = 0, len = width * height; i < len; i ++)
	{
		pp[i] = initColor;
	}
	//创建绘制环境
	ddc = CreateDC("DISPLAY", NULL, NULL, NULL);
	memBM = CreateDIBitmap(ddc, &bih, CBM_INIT, pBits, &bi, DIB_RGB_COLORS);
	DeleteDC(ddc);
	
	if (memBM == NULL)
	{
		DeleteDC(memDC);
		//delete[] name;
		//delete[] wPath;
		if (crop1 != NULL) 
		{
			env->ReleaseDoubleArrayElements(crop, crop1, NULL);
		}
		env->ReleaseStringChars(path, (const jchar *)wPath);
		delete []pBits;		
		return jpix;
	}
	SelectObject (memDC, memBM);	
	BOOL isGDIplusPaint = FALSE;
	BOOL isException = FALSE;
	BOOL isdrawn = FALSE;	//GDI+是否成功
	//初始化GDI+
	//BOOL test = FALSE;
	//把标准型Wmf转换成PlaceableWmf，用GDI+显示，add by user649 2007-04-03
	if (type == 1 || type == 257)
	{
		WORD mHeight ;
		WORD mWidth ;
		if(GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL) == Ok)
		{	
			_try
			{			
				mHeight = getSize(wPath, true);
				mWidth = getSize(wPath, false);
			}
			_except(EXCEPTION_EXECUTE_HANDLER)
			{
				mHeight = 768;
				mWidth = 1024;
			}
			if (mHeight == 0 || mWidth == 0)
			{
				mHeight = (WORD)width;
				mWidth = (WORD)height;
			}
			FILE *temp = _wfopen(wPath, L"rb");
			WORD flag = 0x020c;
			WORD s;
			if(temp != NULL)
			{
				for(int j = 0;j<128;j++)
				{
					if (feof(temp) != 0) 
					{					
						break;
					}
					s = getwc(temp);
					if (flag == s && feof(temp) == 0) 
					{
						
						mHeight = getwc(temp);
						if (feof(temp) == 0)
						{
							mWidth = getwc(temp);
						}																	
						if (mWidth != 0  && mHeight != 0) 
						{
							break;
						}
						
					}
					
					
				}
				if ( temp != NULL)
				{
					fclose(temp);
				}
			}
			WORD placeablehead[11] = {0xCDD7,0x9AC6,0x0000,0x0000,0x0000,
									  0x0000,0x0000,0x0060,0x0000,0x0000,
									  0x0000};
			placeablehead[5] = mWidth;
			placeablehead[6] = mHeight;		
			for(int l = 0;l<10;l++)
			{
				placeablehead[10] = placeablehead[10]^placeablehead[l];
			}
			char tempPath2 [MAX_PATH];
			GetTempPath(MAX_PATH, tempPath2);
			strcat(tempPath2, "TSOWH.wmf");
			FILE * temp1 = _wfopen (wPath, L"rb");
			FILE * temp2 = fopen (tempPath2, "wb");
			fwrite(placeablehead, sizeof(WORD), 11, temp2);
			//	fclose(temp2);
			//	temp2 = fopen(tempPath2,"ab");
				
			DWORD tsize = 1024;
			BYTE tbuff[1024];
			while(tsize == 1024)
			{
				
				tsize = fread(tbuff, sizeof(BYTE), 1024, temp1);
				fwrite(tbuff, sizeof(BYTE), tsize, temp2);
				fflush(temp2);
			}
			fclose(temp2);
			fclose(temp1);
		
			wchar_t *wPath2 = new wchar_t[strlen(tempPath2)*2];
			MultiByteToWideChar(CP_ACP,0,tempPath2,-1,wPath2,strlen(tempPath2)*2);
			//Bitmap bmp(mWidth, mHeight, PixelFormat24bppRGB);
			//Graphics graphics(&bmp);
			_try
   			{	
				isdrawn = PaintByGdiPlus(memDC, wPath2, width, height, crop1);
				isGDIplusPaint = TRUE;
   			}
   			_except(EXCEPTION_EXECUTE_HANDLER)
   			{
				isException = TRUE;
   			}
		 	GdiplusShutdown(m_gdiplusToken);
			
			delete[] wPath2;
		}
	}

	else if(GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL) == Ok)
	{
		_try
   		{

			isdrawn = PaintByGdiPlus(memDC, wPath, width, height, crop1);
			isGDIplusPaint = TRUE;
   		}
   		_except(EXCEPTION_EXECUTE_HANDLER)
   		{
			isException = TRUE;
   		}

				
		//关闭GDI+函数
		//user397 怀疑与此方法有关，暂时注释掉
		GdiplusShutdown(m_gdiplusToken);
	}
	if (crop1 != NULL)
	{
		env->ReleaseDoubleArrayElements(crop, crop1, NULL);
	}
	if(!isdrawn || isException)
	{
		if(type > 256)
		{
			type -= 256;
		}		
		//GDI+失败仍然用老办法    	
		//delete[] wPath;
		char tempPath [MAX_PATH];
		if(type == 1) //placeable
		{		
			hWmf = GetMetaFileW(wPath);
			if (hWmf==NULL)
			{
				DeleteObject(memBM);
				DeleteDC(memDC);
				//delete[] name;
				if (crop1 != NULL) 
				{
					env->ReleaseDoubleArrayElements(crop, crop1, NULL);
				}
				env->ReleaseStringChars(path,(const jchar *) wPath);
				delete []pBits;				
				return jpix;
			}
			SetMapMode (memDC, MM_ANISOTROPIC) ;
			SetViewportExtEx (memDC, width, height, NULL) ;
			_try
			{
				PlayMetaFile(memDC, hWmf);
			}
			_except(EXCEPTION_EXECUTE_HANDLER)
			{
			}
			
		}
		else if (type == 2)
		{
			//应代码审查而修改 by user271 2004-07-26
			//char tempPath [MAX_PATH];
			GetTempPath(MAX_PATH, tempPath);
			//char randFile[64];
			//sprintf(randFile,"tempWH2%d",rand());
			//strcat(randFile, ".wmf");
			//strcat(tempPath, randFile);
			strcat(tempPath, "TSOTempWH2.wmf");
			FILE * temp = _wfopen (wPath, L"rb");
			FILE * temp2 = fopen (tempPath, "wb");
			if(!temp || !temp2)
			{
				DeleteObject(memBM);
				DeleteDC(memDC);
//				DeleteMetaFile(hWmf);
				delete []pBits;
				//delete []name;
				env->ReleaseStringChars(path, (const jchar *)wPath);
				return jpix;
			}
			fseek(temp, 0x16, SEEK_SET);
			DWORD size = 1024;
			BYTE buff[1024];
			while(size == 1024)
			{
				size = fread(buff, sizeof(BYTE), 1024, temp);
				fwrite(buff, sizeof(BYTE), size, temp2);
				fflush(temp2);
			}
			fflush(temp2);
			fclose(temp);
			fclose(temp2);
			hWmf = GetMetaFile(tempPath);
			if (hWmf == NULL)
			{
				DeleteObject(memBM);
				DeleteDC(memDC);
				DeleteFile(tempPath);
				delete []pBits;
				//delete []name;
				env->ReleaseStringChars(path,(const jchar *) wPath);
				return jpix;
			}
			SetMapMode (memDC, MM_ANISOTROPIC) ;
			SetViewportExtEx (memDC, width, height, NULL) ;
			PlayMetaFile(memDC, hWmf);
			
		}
		else if (type == 3)
		{	
			hEmf = GetEnhMetaFileW(wPath);
			if (hEmf == NULL)
			{
				DeleteObject(memBM);
				DeleteDC(memDC);
				DeleteMetaFile(hWmf);
				delete []pBits;
				//delete []name;
				env->ReleaseStringChars(path,(const jchar *) wPath);
				return jpix;
			}
			PlayEnhMetaFile(memDC, hEmf, &rt);
		}
		DeleteFile(tempPath);
	}	
	
	
	GetDIBits(memDC, memBM, 0, height, pBits, &bi, DIB_RGB_COLORS);
	//处理透明色的颜色
	if (initColor == 0xFEFDFE)
	{
		for (i = 0, len = width * height; i < len; i ++)
		{
			*pp = (*pp &0xFFFFFF) ;
			*pp = *pp != initColor ? (*pp | 0xFF000000) : *pp;
			pp++;
		}
	}
	else
	{
		for (i = 0, len = width * height; i < len; i ++)
		{
			*pp = *pp != initColor ? (*pp | 0xFF000000) : *pp;
			pp++;
		}
	}
	//
	jpix = env->NewIntArray(width * height);
	if(jpix != NULL)
	{
		env->SetIntArrayRegion(jpix, 0, width * height , (jint*)pBits);
	}	
	 
	DeleteObject(memBM);
	DeleteDC(memDC);
	//delete []name;
	env->ReleaseStringChars(path,(const jchar *) wPath);			
	delete []pBits;
	if(hWmf != NULL)
	{
		DeleteMetaFile(hWmf);	
	}
	if(hEmf != NULL)
	{	
		DeleteEnhMetaFile(hEmf);
	}
	return jpix;
}

jintArray drawIcon(JNIEnv* env, jclass cls, jstring path, jint index)
{
	if (path == NULL)
	{
		return NULL;
	}
	//返回的java中的象素数组数据
	jintArray jpix = NULL;
	//显示模式颜色位数
	int bits = 0;
	//底色
	DWORD initColor = 0;
	HICON phiconLarge;
	WORD ind = NULL;
	DWORD * pp		= NULL;
	HBITMAP memBM	= NULL;
	HDC		memDC	= NULL;
	HDC		ddc		= NULL;
	int needSize = StringToWidechar(env,path,NULL);
	LPWSTR wName = new wchar_t[needSize];
	StringToWidechar(env,path,wName);

	BITMAPINFOHEADER bih;
	
	bih.biSize			= sizeof(BITMAPINFOHEADER);
	bih.biWidth			= 32;
	//应代码审查而修改 by user271 2004-07-26
	//在此处为负值是为了得到正确的图片，如果不为负值，则图片是倒的
	bih.biHeight		= -32;
	bih.biPlanes		= 1;
	bih.biBitCount		= 32;
	bih.biCompression	= BI_RGB;
	bih.biSizeImage		= 0;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed		= 0;
	bih.biClrImportant	= 0;
	
	int leng = 32 * 32;
	BITMAPINFO bi;
	bi.bmiHeader = bih;
	
	PBYTE pBits = new BYTE[leng*4];//(PBYTE)malloc(len * 4);
	if (pBits == NULL)
	{	
		delete[] wName;
		return jpix;
	}
	(DWORD*)pp =(DWORD*)pBits;
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	memDC = CreateCompatibleDC (NULL);
	//设置透明色
	bits = GetDeviceCaps (memDC, BITSPIXEL);
	if (::GetVersionEx(&osvi))
	{
		if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			switch(bits)
			{
			case 8:
				initColor = 0xc0c0c0;
				break;
			case 16:
				initColor = 0xFFFFF7;
				break;
			default:
				//24位和32位处理相同
				initColor = 0xFFFFFE;
			}
		}
		else if ((osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
			((osvi.dwMajorVersion > 4) || ((osvi.dwMajorVersion == 4) && (osvi.dwMinorVersion > 0))))
		{
			switch(bits)
			{
			case 8:
				initColor = 0xc0c0c0;
				break;
			case 16:
				initColor = 0xFFFFF0;
				break;
			case 24:
				initColor = 0xFEFDFE;
				break;
			default:
				//32位
				initColor = 0xFFFFFE;
			}
		}
	}
	//
	int j = 0;
	for ( j = 0, leng = 32 * 32; j < leng; j ++)
	{
		pp[j] = initColor;
	}
	//创建绘制环境
	ddc = CreateDC("DISPLAY", NULL, NULL, NULL);
	memBM = CreateDIBitmap(ddc, &bih, CBM_INIT, pBits, &bi, DIB_RGB_COLORS);
	DeleteDC(ddc);
	
	if (memBM == NULL)
	{
		DeleteDC(memDC);
		delete []pBits;		
		return jpix;
	}
	SelectObject (memDC, memBM);
	if (index == 0x1000000f) 
	{
	
		phiconLarge = ExtractAssociatedIconW(NULL,wName,&ind);
		if (phiconLarge == NULL && ind == NULL)
		{
			return NULL;//ExtractIconExA(longName,index,&phiconLarge,NULL,1);
		}
	
	}
	else 
	{
		ExtractIconExW(wName,index,&phiconLarge,NULL,1);
	}
	if (phiconLarge == NULL) 
	{
		return NULL;
	}
	if (!DrawIconEx(memDC , 0 , 0 , phiconLarge,32,32,NULL,NULL,DI_NORMAL | DI_COMPAT))
	{
		
		return jpix;
	}
	
	GetDIBits(memDC, memBM, 0, 32, pBits, &bi, DIB_RGB_COLORS);
	//处理透明色的颜色
	if (initColor == 0xFEFDFE)
	{
		for (j = 0; j < leng; j ++)
		{
			*pp = (*pp &0xFFFFFF) ;
			*pp = *pp != initColor ? (*pp | 0xFF000000) : *pp;
			pp++;
		}
	}
	else
	{
		for (j = 0; j < leng; j ++)
		{
			*pp = *pp != initColor ? (*pp | 0xFF000000) : *pp;
			
			pp++;
		}
	}
	jpix = env->NewIntArray(32*32);
	if(jpix != NULL)
	{
		env->SetIntArrayRegion(jpix, 0, leng , (jint*)pBits);
	}
	if (memBM != NULL) 
	{
		DeleteObject(memBM);
	}
	if (memDC != NULL) 
	{
		DeleteDC(memDC);
	}
	if (pBits != NULL)
	{
		delete []pBits;
	}			
	
	if (phiconLarge != NULL)
	{
		DestroyIcon(phiconLarge);
	}

	return jpix;
	
}
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
jboolean getFileIconByIndex(JNIEnv *pEnv, jobject obj, jstring imgPath, jstring filePath, jint index, jstring firstLine, jstring sndLine, jstring fontName)
{
	if(imgPath == NULL )
	{
		return JNI_FALSE;
	}
	
	if(GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL) == Ok)
	{
		int needSize = StringToMutilBytes(pEnv, imgPath, NULL);
		LPSTR imgName = new char[needSize];
		StringToMutilBytes(pEnv, imgPath, imgName);
		wchar_t *wImgPath = new wchar_t[strlen(imgName)*2];
					
		MultiByteToWideChar(CP_ACP, 0, imgName, -1, wImgPath, strlen(imgName)*2);

		HICON  hIcon = NULL;
		
		if (filePath == NULL)
		{
			char szOleDllPath[256];
 
			// 将Java中传入的Ole对象数据文件路径转变为windows下的字符串
 
			HGLOBAL  hGlobal = NULL;
			BOOL     isSuccess = FALSE;
 
			// 取得系统目录的字符串长度
			UINT  uLen = GetSystemDirectory(NULL, 0);
			if(uLen == 0)
			{
			// 如果取系统目录字符串长度失败，则创建一个空的EMF文件
				isSuccess = CreateBlankEmf(imgName);
				return isSuccess;
			}
 
			// 取得系统目录字符串
			TCHAR* szValue = new TCHAR[uLen+1];
			uLen = GetSystemDirectory(szValue, uLen+1);
 
			if(szValue == NULL)
			{
			// 如果取系统目录失败，则创建一个空的EMF文件
			  delete[] szValue;
			  isSuccess = CreateBlankEmf(imgName);
			  return isSuccess;
			}
 
			sprintf(szOleDllPath, "%s\\ole32.dll", szValue);
 
		 // 取得Ole32.dll中的OLE默认图标
			hIcon = ExtractIcon(NULL, szOleDllPath, 0);
		
		}
	
		if (filePath != NULL) 
		{
		
			needSize = StringToMutilBytes(pEnv, filePath, NULL);
			LPSTR fileName = new char[needSize];
			StringToMutilBytes(pEnv, filePath, fileName);
			if (hIcon == NULL) 
			{
				ExtractIconExA(fileName, (int)index, &hIcon,NULL,1);
			}
		}	
		BSTR wfontName = StringToWidechar(pEnv, fontName);

		HDC hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
		
		Metafile metafile(wImgPath, hdc);
		
		Graphics graphics(&metafile);
		//HICON hicon;
		
		if (hIcon == NULL)
		{
			return JNI_FALSE;
		}
		
		Bitmap bitmap(hIcon);
		if (firstLine == NULL)
		{
			graphics.DrawImage(&bitmap,0,0,32,32);
			return JNI_TRUE;
		}
		Pen pen(Color(0,0,0,0),0);
		graphics.DrawRectangle(&pen,0,0,110,64);
		graphics.DrawImage(&bitmap,39,0,32,32);
		FontFamily fontFamily(wfontName);
		Font font(&fontFamily, 12, FontStyleRegular, UnitPixel);	
		StringFormat stringFormat;
		stringFormat.SetAlignment(StringAlignmentCenter);

		SolidBrush solidBrush(Color(255, 0, 0, 0));
		
		if (firstLine != NULL)
		{
		
			needSize = StringToMutilBytes(pEnv, firstLine, NULL);
			LPSTR fstLabel = new char[needSize];
			StringToMutilBytes(pEnv, firstLine,fstLabel);
			//writeLog(fstLabel);
			WCHAR *wLabel = new WCHAR[needSize];
			MultiByteToWideChar(CP_ACP, 0, fstLabel, -1,wLabel, needSize);
			RectF        rectF(0.0f, 34.0f, 110.0f, 48.0f);
			/*
				LONG width = getWinOsFontWidth(wLabel, fontName, pEnv);
							float x = (110.0f - (float)width)/2;
							//x = (16.0f-x)>=0 ? 16.0f-x : (16.0f - x)/2;
							graphics.DrawString(wLabel, wcslen(wLabel), &font, 
								PointF(x, 34.0f), &solidBrush) ;*/
			graphics.DrawString(wLabel, -1, &font, rectF, &stringFormat, &solidBrush);	
			if (fstLabel != NULL)
			{	
				delete[] fstLabel;
			}
			if (wLabel != NULL)
			{
				delete[] wLabel;
			}

		}		
		
		if (sndLine != NULL)
		{
			needSize = StringToMutilBytes(pEnv, sndLine, NULL);
			LPSTR sndLabel = new char[needSize];
			StringToMutilBytes(pEnv, sndLine,sndLabel);	
			//writeLog(sndLabel);
			wchar_t *wLabel = new wchar_t[needSize];
			MultiByteToWideChar(CP_ACP, 0, sndLabel, -1,wLabel, needSize);
			/*
			LONG width = getWinOsFontWidth(wLabel, fontName, pEnv);
						float x = (110.0f - (float)width)/2;
						//x = (16.0f-x)>=0 ? 16.0f-x : (16.0f - x)/2;
						graphics.DrawString(wLabel, wcslen(wLabel), &font, 
						PointF(x, 48.0f), &solidBrush);*/
			RectF        rectF(0.0f, 48.0f, 110.0f, 64.0f);
			graphics.DrawString(wLabel, -1, &font, rectF, &stringFormat, &solidBrush);
			if (sndLabel != NULL)
			{
				delete[] sndLabel;
			}
			if (wLabel != NULL)
			{
				delete[] wLabel;
			}
			
		}
		if (wImgPath != NULL) 
		{
			delete[] wImgPath;
		}
		
		if (hIcon != NULL)
		{
			DestroyIcon(hIcon);
		}
				
		
		ReleaseDC(NULL,hdc);
		
		//GdiplusShutdown(m_gdiplusToken);
	}
	return JNI_TRUE;
}


jint getFileIconNum(JNIEnv *pEnv, jobject obj, jstring filePath)
{
	if(filePath == NULL)
	{
		return NULL;
	}

	char szPath[512];

	const jchar* wcFilePath = pEnv->GetStringChars(filePath, 0);
	
    //WTOA(wcFilePath, szPath, 512);
	WideCharToMultiByte(CP_ACP, 0, (WCHAR *)wcFilePath, -1, szPath, 512, NULL, NULL);
		
	UINT nNum = ExtractIconEx(szPath, -1, NULL, NULL, 0);			

	pEnv->ReleaseStringChars(filePath, wcFilePath);

	return nNum;
}





















