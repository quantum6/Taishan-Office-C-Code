#ifndef __APP_MESSAGE_LINUX_H__
#define __APP_MESSAGE_LINUX_H__

#include <sys/msg.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MESSAGE_DATA_BUFFER_SIZE        256
#define MESSAGE_TIO_COMMAND             69

#define TIO_COMMAND_CLOSE       "?0?"
#define TIO_COMMAND_START       "?1?"
#define TIO_COMMAND_OPEN        "?2?"
#define TIO_COMMAND_SS          "?3?"
#define TIO_COMMAND_WP          "?4?"
#define TIO_COMMAND_PG          "?5?"
#define TIO_COMMAND_END         "?args?"
#define TIO_COMMAND_EXIT        "?exit?"

typedef struct
{
    long  type;  // message type, must be > 0 消息的类型必须>0
    char  data[MESSAGE_DATA_BUFFER_SIZE]; // message data 长度随意
} PipeMessage;

void send_messages(const key_t msgKey, const int count, const char** commands);

void send_message(const key_t msgKey, const char* command);

char* get_lock_file(const char* pExe, const char* pKey, char* pBuffer);

#endif //__APP_MESSAGE_LINUX_H__

