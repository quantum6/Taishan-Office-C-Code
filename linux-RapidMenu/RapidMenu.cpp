#include <gtk/gtk.h>
#include <sys/msg.h>
#include <sys/prctl.h>

#include "fw_log.h"
#include "fw_kit.h"
#include "app_message.h"
#include "fw_resource.h"
#include "product_config_office.h"


#define STATUS_MENU_INDEX_CMD     0
#define STATUS_MENU_INDEX_ICON    1
#define STATUS_MENU_INDEX_TEXT    2
#define STATUS_MENU_INDEX_PARAM   3

extern int   os_executeProcessBackground(const char* psProcessName);

const char* STATUS_MENU_INFOS[][4] =
{
    {STATUS_MENU_CMD_START, STATUS_MENU_ICON_START, STATUS_MENU_TEXT_START, TIO_COMMAND_START},
    {STATUS_MENU_CMD_EXIT,  STATUS_MENU_ICON_EXIT,  STATUS_MENU_TEXT_EXIT,  TIO_COMMAND_CLOSE},

    //注意，数组必须这样写。
    {NULL, NULL, NULL}
};

static void start_app_with_param(const char* param)
{
    char buffer[BUFFER_SIZE] = {0};
    getAppBinDir(buffer);
    sprintf(buffer, "%s%s%s %s", buffer, OS_DIR_TEXT, KEY_FILE_PRODUCT, param);
    //TS_LOG_TEXT(buffer);
	
    // 解决因阻塞导致小敏菜单无法弹出、无响应、不能退出主界面的问题
    os_executeProcessBackground(buffer);
}

static void tray_icon_menu_action(GtkMenuItem *item, gpointer user_data)
{
    char* action = (char*)user_data;
    int i=0;
    while (TRUE)
    {
        char* command = (char*)(STATUS_MENU_INFOS[i][STATUS_MENU_INDEX_CMD]);
        if (command == NULL)
        {
            break;
        }
        if (strcmp(command, action) == 0)
        {
            //TS_LOG_INFO("i=%d, %s, %s, ", i, action, command);
            start_app_with_param(STATUS_MENU_INFOS[i][STATUS_MENU_INDEX_PARAM]);
            break;
        }
        i++;
    }

    //退出
    if (strcmp(action, STATUS_MENU_CMD_EXIT) == 0)
    {
        gtk_main_quit();
    }
}

static void tray_icon_activated(GObject *trayIcon, gpointer window)
{
	start_app_with_param(TIO_COMMAND_START);
}

static void tray_icon_popup(GtkStatusIcon *status_icon, guint button, guint32 activate_time, gpointer popUpMenu)
{
    gtk_menu_popup(GTK_MENU(popUpMenu), NULL, NULL, gtk_status_icon_position_menu, status_icon, button, activate_time);
}

void show_status_icon()
{
    char buffer[BUFFER_SIZE] = {0};

    gtk_init(NULL, NULL);

    //set popup menu for tray icon
    GtkWidget *menu = gtk_menu_new();

    int i=0;
    while (TRUE)
    {
        char* cmd = (char*)(STATUS_MENU_INFOS[i][STATUS_MENU_INDEX_CMD]);
        if (cmd == NULL)
        {
            break;
        }
        //TS_LOG_INFO("i=%d, %s", i, STATUS_MENU_INFOS[i][STATUS_MENU_INDEX_TEXT]);
        GtkWidget* menuItem = gtk_image_menu_item_new_with_label(STATUS_MENU_INFOS[i][STATUS_MENU_INDEX_TEXT]);

        memset(buffer, 0, BUFFER_SIZE);
		
        concatDirFile(getAppImagesDir(buffer), STATUS_MENU_INFOS[i][STATUS_MENU_INDEX_ICON]);
        GtkWidget* image = gtk_image_new_from_file(buffer);
        gtk_image_menu_item_set_image(            (GtkImageMenuItem*)menuItem, image);
        gtk_image_menu_item_set_always_show_image((GtkImageMenuItem*)menuItem, TRUE);

        g_signal_connect(G_OBJECT(menuItem), "activate", G_CALLBACK(tray_icon_menu_action), (gpointer)cmd);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
        i++;
    }

    gtk_widget_show_all(menu);

    //set try icon file
    memset(buffer, 0, BUFFER_SIZE);
    concatDirFile(getAppImagesDir(buffer), KEY_FILE_RAPID_MENU_ICON);

    GtkStatusIcon *trayIcon  = gtk_status_icon_new_from_file(buffer);
    //set tooltip
    gtk_status_icon_set_tooltip_text(trayIcon, TASKBAR_PRODUCT_TEXT);
    //connect handlers for mouse events
    g_signal_connect(GTK_STATUS_ICON(trayIcon), "activate",   G_CALLBACK(tray_icon_activated), NULL);
    g_signal_connect(GTK_STATUS_ICON(trayIcon), "popup-menu", G_CALLBACK(tray_icon_popup),     menu);

    gtk_status_icon_set_visible(trayIcon, TRUE);
    gtk_main();
}

int main(const int argc, const char** argv)
{
    os_initAppHome(argv[0]);

    /**
     * 为了保证最快速的相应，所以这里尽可能的省略其他操作。
     */
    char buffer[BUFFER_SIZE] = {0};
    get_lock_file(argv[0], KEY_FILE_RAPID_MENU, buffer);

    if (os_isFileLocked(buffer))
    {
        TS_LOG_INFO("EXIT");
        return 0;
    }

    //设置进程名，这样更好看一些
    prctl(PR_SET_NAME, KEY_FILE_RAPID_MENU);

    //启动窗口
    show_status_icon();
}
