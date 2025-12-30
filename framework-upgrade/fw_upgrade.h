#ifndef __FW_UPGRADE_H__
#define __FW_UPGRADE_H__

#define OS_LINUX          "linux"
#define OS_UOS            "uos"
#define OS_KYLIN          "kylin"

#define OS_ISSUE_PATH     "/etc/issue"
#define TERMINAL_MATE     "mate-terminal"
#define TERMINAL_UOS      "x-terminal-emulator"


// 升级失败后，下次在弹出升级需要的等待时间
#define SECONDS_WAIT_WHEN_FAIL    (4 * 60 * 60) 

char* getTempProductVersionFile(char* pBuffer);

bool searchExtFileInPath(const char* pPath, const char* pExt);
void get_ini_string(const char* iniFilePath, const char* sectionName, const char* keyName, char* value);
void getInstalledVersion(char* pBuffer);
bool checkOSType(const char* osType);

/*
 *  函数功能：判断升级包文件名里的版本是否比当前版本新
 *  函数参数： pFileName  升级包文件名
 *  返回：    如果比当前版本新返回true
 */
bool checkUpgradeFileVersion(char* pFileName);

// 获取升级包存储路径
int getProductUpdatePackagePath(char* pPath);

// 判断升级包存储路径下是否存在升级包
bool  isUpdatePackageExist();

// 启动 Downloader或Upgrader 程序
bool  startUpgrader(const char* exeName, const char* pCmdline = NULL);

// 更新控制面板-应用程序中的版本号
bool updateUninstallDisplayVersion();

void getRegUninstallPath(char *result);
bool is64bitSystem();
bool isWow64();

#endif  // __FW_UPGRADE_H__
