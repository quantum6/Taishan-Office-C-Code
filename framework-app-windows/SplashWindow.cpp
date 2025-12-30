 /**
 * 该文件提供处理位图图片的函数
 * 

*/

#include "SplashWindow.h"
#include "office_md.h"
#include "app_message.h"
#include "fw_config.h"
#include "fw_kit.h"
#include "fw_log.h"
#include "product_config.h"

// 定义启动画面大小
#define WELCOMEWIDTH    505
#define WELCOMEHEIGHT   225

extern HWND g_hWelcomeWnd;

BITMAPINFO * PackedDibLoad (LPSTR szFileName)
{
     BITMAPFILEHEADER bmfh;
     BITMAPINFO     * pbmi;
     BOOL             bSuccess;
     DWORD            dwPackedDibSize, dwBytesRead;
     HANDLE           hFile;
     int length = (int)(::strlen(szFileName));
    OLECHAR* mul = new OLECHAR[length+1];
    ZeroMemory(mul,sizeof(mul));
    //MultiByteToWideChar(CP_UTF8, 0, szFileName, -1, mul, length+1);
    MultiByteToWideChar(CP_ACP, 0, szFileName, -1, mul, length+1); //修复Bug:泰山Office安装在中文路径下 启动Office时 无法加载启动画面
    hFile = CreateFileW(mul, GENERIC_READ, FILE_SHARE_READ, NULL, 
                         OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

     if (hFile == INVALID_HANDLE_VALUE)
     {
          return NULL;
     }

     bSuccess = ReadFile (hFile, &bmfh, sizeof (BITMAPFILEHEADER), 
                          &dwBytesRead, NULL);

     if (!bSuccess || (dwBytesRead != sizeof (BITMAPFILEHEADER))         
                   || (bmfh.bfType != * (WORD *) "BM"))
     {
          CloseHandle (hFile);
          return NULL;
     }

     dwPackedDibSize = bmfh.bfSize - sizeof (BITMAPFILEHEADER);

     pbmi = (BITMAPINFO*)malloc (dwPackedDibSize);
     if (pbmi==NULL)
     {
          CloseHandle (hFile);
          return NULL;
     }
     bSuccess = ReadFile (hFile, pbmi, dwPackedDibSize, &dwBytesRead, NULL);
     CloseHandle (hFile);

     if (!bSuccess || (dwBytesRead != dwPackedDibSize))
     {
          free (pbmi);
          return NULL;
     }

     return pbmi;
}

/*----------------------------------------------
   Functions to get information from Packed Dib
  ----------------------------------------------*/

int PackedDibGetWidth (BITMAPINFO * pPackedDib)
{
     if (pPackedDib->bmiHeader.biSize == sizeof (BITMAPCOREHEADER))
          return ((PBITMAPCOREINFO)pPackedDib)->bmciHeader.bcWidth;
     else
          return pPackedDib->bmiHeader.biWidth;
}

int PackedDibGetHeight (BITMAPINFO * pPackedDib)
{
     if (pPackedDib->bmiHeader.biSize == sizeof (BITMAPCOREHEADER))
          return ((PBITMAPCOREINFO)pPackedDib)->bmciHeader.bcHeight;
     else
          return abs (pPackedDib->bmiHeader.biHeight);
}

int PackedDibGetBitCount (BITMAPINFO * pPackedDib)
{
     if (pPackedDib->bmiHeader.biSize == sizeof (BITMAPCOREHEADER))
          return ((PBITMAPCOREINFO)pPackedDib)->bmciHeader.bcBitCount;
     else
          return pPackedDib->bmiHeader.biBitCount;
}

int PackedDibGetRowLength (BITMAPINFO * pPackedDib)
{
     return ((PackedDibGetWidth (pPackedDib) * 
              PackedDibGetBitCount (pPackedDib) + 31) & ~31) >> 3;
}

/*-----------------------------------------------------------
   PackedDibGetInfoHeaderSize includes possible color masks!
  -----------------------------------------------------------*/

int PackedDibGetInfoHeaderSize (BITMAPINFO * pPackedDib)
{
     if (pPackedDib->bmiHeader.biSize == sizeof (BITMAPCOREHEADER))
          return ((PBITMAPCOREINFO)pPackedDib)->bmciHeader.bcSize;

     else if (pPackedDib->bmiHeader.biSize == sizeof (BITMAPINFOHEADER))
          return pPackedDib->bmiHeader.biSize + 
                    (pPackedDib->bmiHeader.biCompression == 
                                        BI_BITFIELDS ? 12 : 0);

     else return pPackedDib->bmiHeader.biSize;
}

/*-------------------------------------------------------------
   PackedDibGetColorsUsed returns value in information header;
          could be 0 to indicate non-truncated color table!
  -------------------------------------------------------------*/

int PackedDibGetColorsUsed (BITMAPINFO * pPackedDib)
{
     if (pPackedDib->bmiHeader.biSize == sizeof (BITMAPCOREHEADER))
          return 0;
     else
          return pPackedDib->bmiHeader.biClrUsed;
}

/*------------------------------------------------------------------
   PackedDibGetNumColors is actual number of entries in color table
  ------------------------------------------------------------------*/

int PackedDibGetNumColors (BITMAPINFO * pPackedDib)
{
     int iNumColors;

     iNumColors = PackedDibGetColorsUsed (pPackedDib);

     if (iNumColors == 0 && PackedDibGetBitCount (pPackedDib) < 16)
          iNumColors = 1 << PackedDibGetBitCount (pPackedDib);

     return iNumColors;
}

int PackedDibGetColorTableSize (BITMAPINFO * pPackedDib)
{
     if (pPackedDib->bmiHeader.biSize == sizeof (BITMAPCOREHEADER))
          return PackedDibGetNumColors (pPackedDib) * sizeof (RGBTRIPLE);
     else
          return PackedDibGetNumColors (pPackedDib) * sizeof (RGBQUAD);
}

RGBQUAD * PackedDibGetColorTablePtr (BITMAPINFO * pPackedDib)
{
     if (PackedDibGetNumColors (pPackedDib) == 0)
          return 0;

     return (RGBQUAD *) (((BYTE *) pPackedDib) + 
                                   PackedDibGetInfoHeaderSize (pPackedDib));
}

RGBQUAD * PackedDibGetColorTableEntry (BITMAPINFO * pPackedDib, int i)
{
     if (PackedDibGetNumColors (pPackedDib) == 0)
          return 0;

     if (pPackedDib->bmiHeader.biSize == sizeof (BITMAPCOREHEADER))
          return (RGBQUAD *) 
               (((RGBTRIPLE *) PackedDibGetColorTablePtr (pPackedDib)) + i);
     else
          return PackedDibGetColorTablePtr (pPackedDib) + i;
}

/*------------------------------
   PackedDibGetBitsPtr finally!
  ------------------------------*/

BYTE * PackedDibGetBitsPtr (BITMAPINFO * pPackedDib)
{
     return ((BYTE *) pPackedDib) + PackedDibGetInfoHeaderSize (pPackedDib) +
                                    PackedDibGetColorTableSize (pPackedDib);
}

/*----------------------------------------------------------------------- 
   PackedDibGetBitsSize can be calculated from the height and row length
          if it's not explicitly in the biSizeImage field
  -----------------------------------------------------------------------*/

int PackedDibGetBitsSize (BITMAPINFO * pPackedDib)
{
     if ((pPackedDib->bmiHeader.biSize != sizeof (BITMAPCOREHEADER)) &&
         (pPackedDib->bmiHeader.biSizeImage != 0))
         return pPackedDib->bmiHeader.biSizeImage;

     return PackedDibGetHeight (pPackedDib) * 
            PackedDibGetRowLength (pPackedDib);
}

/*----------------------------------------------------------------
   PackedDibCreatePalette creates logical palette from Packed DIB
  ----------------------------------------------------------------*/

HPALETTE PackedDibCreatePalette (BITMAPINFO * pPackedDib)
{
     HPALETTE     hPalette;
     int          i, iNumColors;
     LOGPALETTE * plp;
     RGBQUAD    * prgb;

     if (0 == (iNumColors = PackedDibGetNumColors (pPackedDib)))
          return NULL;

     plp = (LOGPALETTE*)malloc (sizeof (LOGPALETTE) * 
                         (iNumColors - 1) * sizeof (PALETTEENTRY));
     if(plp==NULL)
     {
         return NULL;
     }
     plp->palVersion    = 0x0300;
     plp->palNumEntries = iNumColors;

     for (i = 0; i < iNumColors; i++)
     {
          prgb = PackedDibGetColorTableEntry (pPackedDib, i);

          plp->palPalEntry[i].peRed   = prgb->rgbRed;
          plp->palPalEntry[i].peGreen = prgb->rgbGreen;
          plp->palPalEntry[i].peBlue  = prgb->rgbBlue;
          plp->palPalEntry[i].peFlags = 0;
     }

     hPalette = CreatePalette (plp);
     free (plp);

     return hPalette;
}

DWORD WINAPI ShowWelcomeWnd(LPVOID lParam)
{
    TCHAR szAppName[] = TEXT(TS_OFFICE);
    HWND         hwnd , hwnd1;
    MSG          msg;
    WNDCLASS     wndclass;
    
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = WndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = (HINSTANCE)lParam;
    wndclass.hIcon         = NULL;
    wndclass.hCursor       = NULL;
    wndclass.hbrBackground = NULL; //(HBRUSH) GetStockObject (BLACK_BRUSH);
    wndclass.lpszMenuName  = NULL;
    wndclass.lpszClassName = szAppName;
    
    if (!RegisterClass (&wndclass))
    {
        return 0;
    }
    
    int width  = GetDeviceCaps(GetDC(GetDesktopWindow()), HORZRES);
    int height = GetDeviceCaps(GetDC(GetDesktopWindow()), VERTRES);
    
    int welComeWidth  = WELCOMEWIDTH;
    int welComeHeight = WELCOMEHEIGHT;


    char path[MAX_PATH];
    
    if(os_getAppHome(path))
    {
        strcat_s(path, OS_DIR_TEXT);
        strcat_s(path, KEY_DIR_IMAGES); //加上Images路径
        strcat_s(path, OS_DIR_TEXT); 
        strcat_s(path, KEY_FILE_START_BMP);

        BITMAPINFO  *pPackedDib = PackedDibLoad(path);

        if(pPackedDib != NULL)
        {
            welComeWidth = PackedDibGetWidth(pPackedDib);
            welComeHeight = PackedDibGetHeight(pPackedDib);
            free(pPackedDib);
        }    
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }

    hwnd = CreateWindowExW(0,
                          szAppName,                  // window class name                          
                          TEXT(""),
                          WS_POPUP,
                          CW_USEDEFAULT,              // initial x position
                          CW_USEDEFAULT,              // initial y position
                          CW_USEDEFAULT,              // initial x size
                          CW_USEDEFAULT,              // initial y size
                          NULL,                       // parent window handle
                          NULL,                       // window menu handle
                          (HINSTANCE)lParam,                  // program instance handle
                          NULL);                     // creation parameters
    if(hwnd == NULL)
    {
        return 0;
    }

    hwnd1 = CreateWindowExW(0,
                           szAppName,               // window class name                          
                           TEXT(""),
                           WS_POPUP | WS_VISIBLE,
                           (width-welComeWidth)/2,             // initial x position
                           (height-welComeHeight)/2,            // initial y position
                           welComeWidth,                        // initial x size
                           welComeHeight,                        // initial y size
                           hwnd,                       // parent window handle
                           NULL,                       // window menu handle
                           (HINSTANCE)lParam,                  // program instance handle
                           NULL);  
    if(hwnd1 == NULL)
    {
        return 0;
    }

    ShowWindow (hwnd1, SW_SHOW);
    UpdateWindow (hwnd1);
    
    g_hWelcomeWnd = hwnd1;
    
    BOOL bRet;
    int count=0;

    while (bRet = GetMessage (&msg, NULL, 0, 0))
    {
        if(bRet==-1)
        {
            if(count>2)
            {
                g_hWelcomeWnd = NULL;    
                // 退出线程
                ExitThread(0);
                return 0;
            }
            count++;
        }
        TranslateMessage (&msg);
        DispatchMessage (&msg);
    }

    g_hWelcomeWnd = NULL;
    
    // 退出线程
    ExitThread(0);

    return 0;
}

// 以下回调函数用于显示启动画面
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HBITMAP      hBitmap;
    static HPALETTE     hPalette;
    static int          cxClient, cyClient;    
    static int          timeCount = 0;
    static int            welComeWidth  = WELCOMEWIDTH;
    static int            welComeHeight = WELCOMEHEIGHT;

    BITMAP              bitmap;
    HDC                 hdc, hdcMem;
    PAINTSTRUCT         ps;
    BITMAPINFO          *pPackedDib;
    static PBYTE        pBits;    
    
    switch (message)
    {
    case WM_CREATE:
        if (hBitmap)
        {
            DeleteObject (hBitmap);
            hBitmap = NULL;
        }
    
        char path[MAX_PATH];

        if(os_getAppHome(path))
        {
            strcat_s(path, OS_DIR_TEXT);
            strcat_s(path, KEY_DIR_IMAGES); //加上Images路径
            strcat_s(path, OS_DIR_TEXT); 
            strcat_s(path, KEY_FILE_START_BMP);
            pPackedDib = PackedDibLoad(path);
        }

        // 如果没有启动画面文件存在，则不显示启动画面窗口
        if(pPackedDib == NULL)
        {
            PostQuitMessage(0);
        }

        welComeWidth = PackedDibGetWidth(pPackedDib);
        welComeHeight = PackedDibGetHeight(pPackedDib);
        
        if (pPackedDib)
        {
            // Create the DIB section from the DIB
            
            hBitmap = CreateDIBSection (NULL,
                pPackedDib, 
                DIB_RGB_COLORS,
                (void**)&pBits, 
                NULL, 0);
            if(hBitmap==NULL)
            {
                PostQuitMessage(0);
            }
            // Copy the bits
            
            CopyMemory (pBits, PackedDibGetBitsPtr(pPackedDib),
                PackedDibGetBitsSize (pPackedDib));
            
            // Create palette from the DIB
            
            hPalette = PackedDibCreatePalette (pPackedDib);
            
            // Free the packed-DIB memory
            
            free (pPackedDib);
        }

        InvalidateRect (hwnd, NULL, TRUE);
        return 0;

    case WM_PAINT:        
        hdc = BeginPaint (hwnd, &ps);    
        if (hPalette)
        {
            SelectPalette (hdc, hPalette, FALSE);
            RealizePalette (hdc);
        }
        if (hBitmap)
        {
            GetObject (hBitmap, sizeof (BITMAP), &bitmap);
            
            hdcMem = CreateCompatibleDC (hdc);
            if(hdcMem==NULL)
            {
                EndPaint (hwnd, &ps);
                return 0;
            }
            SelectObject (hdcMem, hBitmap);
            
            //拉伸启动画面    
            if(!StretchBlt(hdc,    0, 0, welComeWidth, welComeHeight,    
                       hdcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight,
                       SRCCOPY))
            {
                EndPaint (hwnd, &ps);
                DeleteDC (hdcMem);
                return 0;
            }
        }
        EndPaint (hwnd, &ps);
        return 0;

    case WM_TIMER:    
        timeCount++;        
        if(timeCount == 2)
        {
            if (hBitmap)
                DeleteObject (hBitmap);
            
            KillTimer(hwnd, 1000);//2019年2月15日16:17:48
            //PostQuitMessage (0);
            SendMessage(hwnd, WM_DESTROY, 0, 0);
        }
        return 0;

    case WM_DESTROY:
        if (hBitmap)
            DeleteObject (hBitmap);
        PostQuitMessage (0);
        return 0;
    }
    
    return DefWindowProc (hwnd, message, wParam, lParam);
}
