#ifndef __UPGRADE_API_WINDOWS_H__
#define __UPGRADE_API_WINDOWS_H__

#include <windows.h>

int start_jni_app(const char* className, const char* lock_name, LPWSTR lpCmdLine);

#endif