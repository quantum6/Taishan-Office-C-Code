#include "app_message.h"
#include "fw_log.h"
#include "fw_kit.h"
#include "fw_config.h"


static void send_message_text(const int msg_q_id, const int type, const char* pData)
{
    PipeMessage msg = {0};
    msg.type = type;
    strcpy(msg.data, pData);

    msgsnd(msg_q_id, &msg, MESSAGE_DATA_BUFFER_SIZE, 0);
}

void send_messages(const key_t msgKey, const int count, const char** commands)
{
    int msg_q_id = msgget(msgKey, O_WRONLY|0777);
    if (msg_q_id < 0)
    {
        TS_LOG_INFO("msgget error!");
        return;
    }

    if (count > 1)
    {
        for (int i=1; i<count; i++)
        {
            //TS_LOG_INFO("send %i=%s", i, commands[i]);
            send_message_text(msg_q_id, MESSAGE_TIO_COMMAND, commands[i]);
        }
    }
    else
    {
        //把office提到最前显示
        send_message_text(msg_q_id, MESSAGE_TIO_COMMAND, TIO_COMMAND_START);
    }

    //表示本次发送结束
    send_message_text(msg_q_id, MESSAGE_TIO_COMMAND, TIO_COMMAND_END);

    //注意不能删除，否则主进程就会出错
    //msgctl(msg_q_id, IPC_RMID, NULL);
}


void send_message(const key_t msgKey, const char* command)
{
    const char* commands[2] = {NULL, command};
    send_messages(msgKey, 2, commands);
}


/**
 某些情况下，目录权限可能不对。所以要多加判断。
 */
char* get_lock_file(const char* pExe, const char* pKey, char* pBuffer)
{
    os_getAppHome(pBuffer);
    char bufName[BUFFER_SIZE] = {0};
    os_getLoginName(bufName);
    sprintf(pBuffer, "%s/%s", pBuffer, KEY_DIR_CONFIG);
    if (access(pBuffer, W_OK) != -1)
    {
        sprintf(pBuffer, "%s/%s.%s.%s", pBuffer, pKey, APP_LOCK, bufName);
        return pBuffer;
    }

    memset(pBuffer, 0, sizeof(pBuffer)+1);
    getProductWorkDir(pBuffer);
    sprintf(pBuffer, "%s/%s/%s.%s.%s", pBuffer, KEY_DIR_CONFIG, pKey, APP_LOCK, bufName);
    make_dirs(pBuffer, 1);
    return pBuffer;
}

