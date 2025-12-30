

/** 不是太明白，BMP要自己处理吗？*/
#include "SplashWindow.h"
#include <process.h>

#include "fw_kit.h"
#include "fw_log.h"

#include "office_md.h"
#include "app_message.h"
#include "fw_config.h"
#include "fw_upgrade.h"
#include "app_common.h"
#include "app_jni_command.h"
#include "app_launcher.h"
#include "..\framework-windows\DataExchange.h"

#include "proxy_launcher.h"
#include "proxy_config.h"



char* ProxyLauncher::getMessageKey()
{
	return "";
}

const char* ProxyLauncher::getClassNameMain()
{
	return CLASS_MAIN;
}

const char* ProxyLauncher::getClassNameCommand()
{
	return CLASS_COMMAND;
}


bool ProxyLauncher::checkRunning(const HINSTANCE hInstance,
                     const HINSTANCE hPrevInstance,
                     const LPWSTR     lpCmdLine,
                     const int       nCmdShow,
					 const char* szAppParam)
{
	return true;
}

bool ProxyLauncher::preAction(const HINSTANCE hInstance,
                     const HINSTANCE hPrevInstance,
                     const LPWSTR     lpCmdLine,
                     const int       nCmdShow)
{

	return false;
}

bool ProxyLauncher::postAction(const AppJniCommand* pAppJni, const JNIEnv* pEnv)
{
	while (true)
	{
		Sleep(1000);
	}

    ::FreeLibrary(NULL);
	return true;
}
