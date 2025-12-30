/**
 * 版权所有 (c) 2018-2021 泰山信息科技有限公司。保留所有权利。
 */ 

#include <windows.h>


/*
 *	函数功能:将宽字节字符转成UTF-8字符
 *  函数返回:UTF-8字符串，保存在utf8BufferOut指向的内存中
 */
 bool convertUnicodeToUTF8(wchar_t * utf16StrIn, char * utf8BufferOut, int sizeOfBuffer);
 
 /*
 *	函数功能:将宽字节字符转成多字节char
 *  函数返回:多字节char字符串，返回值需手动用delete释放
 */
 char* convertUnicodeToMultiBytes(wchar_t * utf16StrIn);