/**
 * 版权所有 (c) 2018-2021 泰山信息科技有限公司。保留所有权利。
 */ 

#include "DataExchange.h"

 bool convertUnicodeToUTF8(wchar_t * utf16StrIn, char * utf8BufferOut, int sizeOfBuffer)
{ 
    if (wcslen(utf16StrIn) == 0)
        return false;

    int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, utf16StrIn, -1, 0, 0, 0, 0);
    if (sizeRequired <= 0 || sizeRequired >= (sizeOfBuffer-1))
        return false;
    
    if (WideCharToMultiByte(CP_UTF8, 0, utf16StrIn, -1, utf8BufferOut, sizeRequired, 0, 0) == 0)
        return false;

    return true;
}

char* convertUnicodeToMultiBytes(wchar_t * utf16StrIn)
{
    if ((utf16StrIn == NULL) || (wcslen(utf16StrIn) == 0))
        return NULL;
	int totalSize = WideCharToMultiByte(CP_OEMCP, 0, utf16StrIn, -1, NULL, 0, NULL, NULL);
	char* buf = new char[totalSize+1];
	memset(buf,0,totalSize+1);
	totalSize = WideCharToMultiByte(CP_OEMCP, 0, utf16StrIn, -1, buf, totalSize, NULL, NULL);
	return buf;
}