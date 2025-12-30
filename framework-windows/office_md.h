#ifndef __TIOFFICE_MD_H__
#define __TIOFFICE_MD_H__


#define TAISHAN_OFFICE_KEY	"Taishan/Office"

#include "jni.h"

/*
 *	函数功能：申请指定大小的内存空间
 *  函数参数：size 要申请的内存空间的大小
 *  返回：指向申请到的内存空间的指针
 */
void *MemAlloc(size_t size);

/*
 *	函数功能：判断小敏菜单程序是否在运行
 *  函数参数：无
 *  返回：TRUE  小敏菜单程序正在运行
 *        FALSE 小敏菜单程序没有在运行
 */ 
BOOL IsRapidMenuExist();

/*
 *  函数功能：将传入的虚拟机启动参数转变为虚拟机可识别的参数	
 *  函数参数：arg  传入的虚拟机启动参数
 *  返回：虚拟机可识别的参数
 *
 */

char* GetArguments(char* arg);

#endif
