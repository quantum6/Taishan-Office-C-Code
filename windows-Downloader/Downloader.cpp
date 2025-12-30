#include <windows.h>
#include "app_message.h"
#include "UpgradeApiWindows.h"
#include "UpgradeApi.h"


int APIENTRY wWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPWSTR     lpCmdLine,
                     int       nCmdShow)
{
	char lockName[MAX_PATH] = {0};
	sprintf(lockName,"%sDownload",getProductName());
    return start_jni_app(CLASS_NAME_DOWNLOADER, lockName, lpCmdLine);
}