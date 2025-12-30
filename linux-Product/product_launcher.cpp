#include <gtk/gtk.h>
#include <sys/msg.h>

#include "fw_log.h"
#include "fw_kit.h"

#include "fw_config.h"
#include "fw_resource.h"
#include "fw_config.h"
#include "fw_upgrade.h"
#include "app_common.h"
#include "app_message.h"
#include "app_jni_command.h"
#include "product_launcher.h"
#include "product_config.h"


#define SHOW_WINDOW_TIME 3
#define MSG_WAIT_TIME_US 1000* 100  // 100 ms
#define MSG_WAIT_TIMES 100


static GtkWidget* g_pWindow    = NULL;
static GdkPixbuf* g_pIcon      = NULL;

static GdkPixbuf* create_pixbuf(const char* pImage)
{
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(pImage, NULL);
    return pixbuf;
}


static void exit_window(int sig)
{
    /**
     在有些系统上，如果用gtk_widget_destroy()，就会导致msgrcv收到4号中断，还没法恢复。
     */
    if (g_pWindow != NULL)
    {
        gtk_widget_hide(g_pWindow);
        g_pWindow = NULL;

        g_object_unref(g_pIcon);
        g_pIcon = NULL;
#ifndef __APPLE__
        gtk_main_quit();
#endif
    }
}

static void show_destroy(GtkWidget *wiget, gpointer *data)/*定义回调函数destroy，关闭窗口时系统自动调用*/
{
    exit_window(0);
}

static void* show_action(void* arg)
{
    gtk_init(NULL, NULL);
    //GTK_WINDOW_TOPLEVEL, GTK_WINDOW_POPUP
    g_pWindow=gtk_window_new(GTK_WINDOW_POPUP);
    GtkWidget* window = g_pWindow;
    gtk_window_set_decorated(   GTK_WINDOW(window), FALSE);
    gtk_window_set_position(    GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 505, 225);
    gtk_window_set_title(       GTK_WINDOW(window), TASKBAR_PRODUCT_TEXT);
    /*设置当关闭窗口时，要执行的回调函数*/
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(show_destroy), NULL);

    char buffer[BUFFER_SIZE] = {0};

    concatDirFile(getAppImagesDir(buffer), KEY_FILE_TSE_ICO);
    g_pIcon = create_pixbuf(buffer);
    gtk_window_set_icon(GTK_WINDOW(window), g_pIcon);

    memset(buffer, 0, BUFFER_SIZE);
    concatDirFile(getAppImagesDir(buffer), KEY_FILE_START_BMP);

    GtkWidget* image = gtk_image_new_from_file(buffer);
    gtk_container_add(GTK_CONTAINER(window), image);

    gtk_widget_show_all(window);
    
#ifndef __APPLE__
    /*进入消息处理循环*/
    gtk_main();
#endif
    
    return NULL;
}

static void show_splash_window()
{
    if(g_pWindow != NULL)
    {
        return;
    }

    pthread_t tid;
    pthread_create(&tid, NULL, show_action, NULL);

    signal(SIGALRM, exit_window);
    alarm(SHOW_WINDOW_TIME);
}

static void startup_rapidmenu(const int argc, const char** argv)
{
    char menuCmd[BUFFER_SIZE] = {0};
    //启动快捷菜单
    getAppBinDir(menuCmd);
    sprintf(menuCmd, "%s%s%s", menuCmd, OS_DIR_TEXT, KEY_FILE_RAPID_MENU);

    if (strcmp(KEY_FILE_PRODUCT, PRODUCT_NAME_OFFICE) != 0)
    {        
        show_splash_window();
    }
    else
    {
        char psCmd[256] = {0};
        sprintf(psCmd, "ps ax | grep %s/%s/RapidMenu | grep -v grep | wc -l", os_initAppHome(NULL), KEY_DIR_BIN);
        FILE* fp = popen(psCmd, "r");
        memset(psCmd, 0, 255);
        fread(psCmd, 1, 1, fp);
        pclose(fp);
        // Office仅在RapidMenu不存在时才显示splash图像
        if(strcmp(psCmd, "1") != 0)
        {
            if (!((argc > 2) && ( 
                            strcmp(TIO_COMMAND_CLOSE, argv[1]) == 0
                            ||  strcmp("???",  argv[1]) == 0
                            ||  strcmp("?q?",  argv[1]) == 0))
                )
                show_splash_window();
        }
    }

    char cmd2[BUFFER_SIZE] = {0};
    sprintf(cmd2, "%s &", menuCmd);
    system(cmd2);
}


static void waiting_and_receive(const key_t msgKey, const AppJniCommand* pAppJni)
{
    int msg_q_id = msgget(msgKey, IPC_CREAT|O_RDONLY|0777);
    if (msg_q_id < 0)
    {
        TS_LOG_INFO("msgget error!");
        return;
    }

    PipeMessage msg = {0};
    list<char*> messageList;
    while (true)
    {
        memset(&msg, 0, sizeof(PipeMessage));
        msgrcv(msg_q_id, &msg, MESSAGE_DATA_BUFFER_SIZE,
               MESSAGE_TIO_COMMAND,
               0);

        char* pData = &(msg.data[0]);
        if (strlen(msg.data) == 0)
        {
            continue;
        }

        if (   strcmp(pData, TIO_COMMAND_CLOSE) == 0
            || strcmp(pData, TIO_COMMAND_EXIT)  == 0)
        {
            break;
        }
        //表示参数的结束
        else if (   strcmp(pData, TIO_COMMAND_END)  == 0)
        {
            if (messageList.size() > 0)
            {
                ((AppJniCommand*)pAppJni)->open_file_in_office(messageList);
                free_list_all(messageList);
            }
        }
        else if (strlen(msg.data) > 0)
        {
            messageList.push_back(strdup(pData));
        }
    }

    //删除消息队列
    msgctl(msg_q_id, IPC_RMID, NULL);
}

key_t  ProductLauncher::getMessageKey()
{
    return (key_t)MESSAGE_QUEUE_KEY;
}

const char* ProductLauncher::getClassNameMain()
{
    return CLASS_MAIN;
}

const char* ProductLauncher::getClassNameCommand()
{
    return CLASS_COMMAND;
}

bool ProductLauncher::checkRunning(const int argc, const char** argv, const char* szAppParam)
{

    os_initAppHome(argv[0]);
    
    /**
     * 为了保证最快速的相应，所以这里尽可能的省略其他操作。
     */
    char buffer[BUFFER_SIZE] = {0};
    char appName[128] = {0};
    getAppNameFromParam(appName, szAppParam);
    get_lock_file(argv[0], appName, buffer);
    
    key_t msgKey = getMessageKey();
    //如果启动器已经运行，退出
    if (os_isFileLocked(buffer))
    {
        int count = 0;
        while (true)
        {
            ++count;
            int msg_q_id = msgget(msgKey, O_WRONLY|0777);
            if (msg_q_id >= 0 || count > MSG_WAIT_TIMES)
            {
                break;
            }
            
            usleep(MSG_WAIT_TIME_US);
        }
        //TS_LOG_INFO(argv[1]);
        send_messages(msgKey, argc, argv);
        TS_LOG_INFO("EXIT");
        return false;
    }
    //如果锁定成功，收到的是退出命令，直接退出。
    else if (argc > 1
       && (strcmp(TIO_COMMAND_CLOSE, argv[1]) == 0
       ||  strcmp(TIO_COMMAND_EXIT,  argv[1]) == 0))
    {
        return false;
    }
    return true;
}

bool ProductLauncher::preAction(const int argc, const char** argv)
{
#ifndef __APPLE__
    if (isUpdatePackageExist())
    {
        // 启动更新程序后退出
        if (startUpgrader(FILE_UPGRADER_EXE))
        {
            exit(0);
        }
    }
    else
    {
        // 启动下载更新包程序，继续运行
        startUpgrader(FILE_DOWNLOADER_EXE);
    }

    startup_rapidmenu(argc, argv);
#endif

    return false;
}

bool ProductLauncher::postAction(const AppJniCommand* pAppJni,  const JNIEnv* pEnv)
{
    //接受控制命令，向OFFICE传递
    waiting_and_receive(getMessageKey(), pAppJni);
}
