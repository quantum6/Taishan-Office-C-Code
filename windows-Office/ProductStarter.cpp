
#include "fw_config.h"
#include "fw_kit.h"
#include "jni_runtime.h"
#include "product_launcher.h"
#include "app_common.h"
#include "product_config.h"
#include "fw_log.h"


int APIENTRY wWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPWSTR     lpCmdLine,
                     int       nCmdShow)
{
    char appParam[BUFFER_SIZE]    = {0};
	getAppParam(appParam, APPLICATION_STARTER);

	char jar[BUFFER_SIZE] = {0};
    getJarPathApp(jar);

    ProductLauncher* pLauncher = new ProductLauncher();
	pLauncher->launch(
        hInstance, hPrevInstance, lpCmdLine, nCmdShow, 
        appParam, jar);

    delete pLauncher;

	return 0;
}
