#include <stdio.h>
#include <string.h>

#define _DARWIN_BETTER_REALPATH
#include <mach-o/dyld.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include "fw_log.h"
#include "fw_kit.h"
#include "fw_config.h"
#include "product_config.h"

// return the MacOS path under .app folder
void getAppMacOSPath(char* pPath)
{
    char buffer1[BUFFER_SIZE] = {'\0'};
    char buffer2[BUFFER_SIZE] = {'\0'};
    char* resolved = NULL;
    int length = -1;

    uint32_t size = (uint32_t)sizeof(buffer1);
    if (_NSGetExecutablePath(buffer1, &size) != -1)
    {
         resolved = realpath(buffer1, buffer2);
    }
    
    if(strlen(buffer2) > 0)
    {
        sprintf(pPath, "%s", buffer2);
    }
}

// return the Office path under Contents/Frameworks
void os_initMacAppHome(char* pPath)
{
    char path[BUFFER_SIZE] = {'\0'};
    getAppMacOSPath(path);
    
    strchr(strstr(path, "Contents")+strlen("Contents"), '/')[0] = '\0';
    strcat(path, "/Frameworks/");
    strcat(path, KEY_FILE_PRODUCT);
    
    sprintf(pPath,"%s" ,path);
}
