#ifndef __LINUX_TCLIPBOARD_H__
#define __LINUX_TCLIPBOARD_H__

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <X11/Xlib.h>
#include <string>
#include <string.h>
#include <vector>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
using namespace std;

#define CLIPBOARD_FORMAT_PNG_UOS "image/png"
#define CLIPBOARD_SELECTION_NAME "CLIPBOARD"
#define PNG_SUFFIX						 	 "png"
#define TARGETS_NAME						 "TARGETS"
#define BUFFER_SIZE              512

/*
*	bufName: 目前使用"CLIPBOARD" selection
*	fmtName：要获取的剪贴板数据的格式
*	outputFile：将对应格式的剪贴板数据输出到outputFile指定的文件里
*	pvecFormats: 用来存储bufName对应的selection支持的所有数据格式，具体支持什么格式与当前"CLIPBOARD" selection owner有关
*/
Bool getSelectionData(Display *display, const Window window, const char *bufName, const char *fmtName,  const char * outputFile, vector<string>* pvecFormats);

/*
*	获取当前系统剪贴板支持的数据格式列表
*/
Bool getTargets(vector<string>& vecFormats);

/*
*	获取当前系统剪贴板上的数据到文件，并将文件路径存储到outputFile
*  fmtName：指定需要获取的数据格式
*  outputFile：保存文件时使用的路径
*/
bool getSelectionDataToFile(const char* fmtName, char* outputFile);

int  getFileSize(const char* file_name);

#endif //__LINUX_TCLIPBOARD_H__
