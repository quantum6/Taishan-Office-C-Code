
#include "fw_config.h"
#include "fw_kit.h"
#include "jni_runtime.h"
#include "product_launcher.h"
#include "app_common.h"
#include "product_config.h"
#include "fw_log.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

static void sig_handler(int signo){
    pid_t   pid; 
    int     status; 
    while((pid = waitpid(-1, &status, WNOHANG)) > 0) {}
    return; 
}

int main(const int argc, const char** argv)
{
    signal(SIGCHLD,sig_handler);

    char appParam[BUFFER_SIZE]    = {0};
    getAppParam(appParam, APPLICATION_STARTER);

	  char jar[BUFFER_SIZE] = {0};
    getJarPathApp(jar);

    ProductLauncher* pLauncher = new ProductLauncher();
    pLauncher->launch(
        argc, argv, 
        appParam, jar);

    delete pLauncher;

    return 0;
}
