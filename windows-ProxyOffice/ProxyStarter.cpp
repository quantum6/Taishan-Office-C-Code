
#include "fw_config.h"
#include "fw_kit.h"
#include "jni_runtime.h"
#include "proxy_launcher.h"
#include "app_common.h"
#include "proxy_config.h"
#include "fw_log.h"


int APIENTRY wWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPWSTR     lpCmdLine,
                     int       nCmdShow)
{
	char appParam[BUFFER_SIZE] = {0};
	//strcpy(appParam, GetCommandLineA());

    char jar[BUFFER_SIZE] = {0};
    os_getAppHome(jar);
    sprintf(jar, "%s%c%s%c%s", jar,
        OS_DIR_CHAR, KEY_DIR_PLUGINS,
        OS_DIR_CHAR, KEY_FILE_PLUGIN_JAR);

    ProxyLauncher* pLauncher = new ProxyLauncher();
	pLauncher->launch(
        hInstance, hPrevInstance, lpCmdLine, nCmdShow, 
        appParam, jar);

    delete pLauncher;

	return 0;
}
