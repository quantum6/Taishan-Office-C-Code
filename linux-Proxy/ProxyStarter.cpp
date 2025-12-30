
#include "fw_config.h"
#include "fw_kit.h"
#include "jni_runtime.h"
#include "app_common.h"
#include "proxy_launcher.h"
#include "proxy_config.h"
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

    char jar[BUFFER_SIZE] = {0};
    os_getAppHome(jar);
    sprintf(jar, "%s%c%s%c%s", jar,
        OS_DIR_CHAR, KEY_DIR_PLUGINS,
        OS_DIR_CHAR, KEY_FILE_PLUGIN_JAR);

    ProxyLauncher* pLauncher = new ProxyLauncher();
    pLauncher->launch(
        argc, argv, 
        appParam, jar);

    delete pLauncher;

    return 0;
}
