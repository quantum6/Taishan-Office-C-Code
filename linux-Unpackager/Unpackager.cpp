#include <unistd.h>
#include <pwd.h>
#include <algorithm>
#include <pwd.h>
#include <dirent.h>

#include "fw_kit.h"
#include "fw_config.h"
#include "fw_log.h"
#include "fw_upgrade.h"
#include "UpgradeApi.h"
#include "UpgradeApiLinux.h"

    const char* LOCK_NAME = "TIO_UPGRADE_RUN";
    const char* SUFFIX_RPM = ".rpm";
    const char* SUFFIX_DEB = ".deb";
    const char* CMD_UPGRADE_INSTALL_RPM = "rpm -ivh -U --force ";
    const char* CMD_UPGRADE_INSTALL_DEB = "dpkg --force-overwrite -i ";
    

// 在指定目录中查找具有指定后缀的文件并通过pOutFileName返回找到的文件名，不递归子目录
bool  searchInstallFile(const char* pPath, const char* pExt, char* pOutFileName)
{    
    bool bfound = false;
    struct dirent* filename = NULL;
    DIR* dir = opendir(pPath);
    if(dir == NULL)
        return bfound;
    
    while((filename = readdir(dir))!=NULL)
    {
        if(filename->d_type == DT_DIR)
            continue;
        if(filename->d_type == DT_REG && string(filename->d_name).find(pExt) != string::npos)
        {
            string sFile = string(filename->d_name);
            string sExt = sFile.substr(sFile.length()-strlen(pExt),strlen(pExt));
            if(sExt != pExt)
                continue;
            
            bfound = true;
            strcat(pOutFileName, filename->d_name);
            break;
        }
    }
    closedir(dir);
    
    return bfound;
}

int main(const int argc, const char** argv)
{
    sleep(1);
    os_initAppHome(argv[0]);

    char appName[BUFFER_SIZE] = {0};
    strcpy(appName, getProductName());

    char queryUpgraderCommand[BUFFER_SIZE] = {0};
    char upgraderFile[BUFFER_SIZE] = {0};
    char buffer[BUFFER_SIZE] = {0};
    char runPath[BUFFER_SIZE] = {0};
    
    strcat(runPath, argv[0]);
    strrchr(runPath, OS_DIR_CHAR)[0] = 0;
    // 将工作目录改为当前run所在目录
    chdir(runPath);
    // 在Upgrader目录创建log，避免升级结束后log文件被删除无法查看
    FILE* fLog = fopen("../unPackage.log", "a+");
    
    char bufName[BUFFER_SIZE] = {0};
    os_getLoginName(bufName);
    sprintf(buffer, "%s/%s.%s.%s", runPath, LOCK_NAME, APP_LOCK, bufName);
    
    //如果已经运行，退出
    if (os_isFileLocked(buffer))
    {
        TS_LOG_INFO("File %s is locked.", buffer);
        if(fLog)
        {
            fprintf(fLog, "File %s is locked.\n", buffer);
            fclose(fLog);
        }
        return -1;
    }
    
    const char* pSuffix = NULL;
    const char* pInstallCommand = NULL;
    char result[BUFFER_SIZE] = {0};
    os_executeProcess("arch", result);
    if(strlen(result) == 0)
    {
        TS_LOG_INFO("Failed to get arch.");
        if(fLog)
        {
            fprintf(fLog, "%s\n", "Failed to get arch.");
            fclose(fLog);
        }
        return -1;
    }
    
    string strRes = result;
    strRes.erase(std::remove(strRes.begin(), strRes.end(), '\r'), strRes.end());
    strRes.erase(std::remove(strRes.begin(), strRes.end(), '\n'), strRes.end());
    if(strRes == "mips64")  // loongson
    {
        pSuffix = SUFFIX_RPM;
        pInstallCommand = CMD_UPGRADE_INSTALL_RPM;
        strcat(queryUpgraderCommand, "rpm -ql");
    }
    else if(strRes == "aarch64") // feiteng
    {
        pSuffix = SUFFIX_DEB;
        pInstallCommand = CMD_UPGRADE_INSTALL_DEB;
        strcat(queryUpgraderCommand, "dpkg -L");
    }
    else
    {
        TS_LOG_INFO("Unsupported arch: \"%s\".", strRes.c_str());
        if(fLog)
        {
            fprintf(fLog, "Unsupported arch: \"%s\"\n", strRes.c_str());
            fclose(fLog);
        }
        return -1;
    }
    strcat(queryUpgraderCommand, " tsoffice | grep -i Upgrader/Upgrader$");

    char installFile[BUFFER_SIZE] = {0};
    char installFileFullPath[BUFFER_SIZE] = {0};
    // 在run所在目录下查找指定后缀的安装文件
    searchInstallFile(runPath, pSuffix, installFile);
    strcat(installFileFullPath, runPath);
    if(runPath[strlen(runPath)-1] != OS_DIR_CHAR)
        strcat(installFileFullPath,OS_DIR_TEXT);
    strcat(installFileFullPath, installFile);
    
    if(strlen(installFile) == 0)
    {
        TS_LOG_INFO("Not found %s install file in %s.", pSuffix, runPath);
        if(fLog)
        {
            fprintf(fLog, "Not found %s install file in %s\n", pSuffix, runPath);
            fclose(fLog);
        }
        return -1;
    }
    
    char command[BUFFER_SIZE] = {0};
    // 确保泰山软件已经退出，正常运行到此处应该没有泰山进程存在了
    sprintf(command, "killall %s", appName);
    system(command);
    system("killall RapidMenu");
    
    memset(command, 0, BUFFER_SIZE);
    // 执行安装命令，并在Upgrader目录创建log，避免升级结束后log文件被删除无法查看
    sprintf(command, "%s %s 2>&1 | tee ../fullInstall.log", pInstallCommand, installFileFullPath);
    TS_LOG_INFO("Run install command: %s.", command);
    // 等待安装完成
    system(command);
    
    // 查找Upgrader程序所在路径
    os_executeProcess(queryUpgraderCommand, upgraderFile);
    if(strlen(upgraderFile) == 0)
    {
        TS_LOG_INFO("Failed to get Upgrader file path.");
        if(fLog)
        {
            fprintf(fLog, "%s\n", "Failed to get Upgrader file path.");
            fclose(fLog);
        }
        return -1;
    }    
    strRes = upgraderFile;
    strRes.erase(std::find(strRes.begin(), strRes.end(), '\r'), strRes.end());
    strRes.erase(std::find(strRes.begin(), strRes.end(), '\n'), strRes.end());
    
    //启动安装目录下的upgrader程，并在Upgrader目录创建log，避免升级结束后log文件被删除无法查看
    const char* pTitle = "泰山Office软件升级";
    sprintf(command,"%s -t %s -x sh -c '%s install\\;%s 2>&1 | tee -a ../fullInstall.log'", checkOSType(OS_UOS) ? TERMINAL_UOS : TERMINAL_MATE, pTitle, strRes.c_str(), installFile);
    
    TS_LOG_INFO("Run upgrade command: \"%s\"\n",command);             
    if(fLog)
    {
        fprintf(fLog, "Run upgrade command: \"%s\"\n",command);
        fclose(fLog);
    }
    system(command);
    
    return 0;
}
