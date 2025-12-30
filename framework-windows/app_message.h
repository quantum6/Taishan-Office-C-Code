#ifndef __APP_MESSAGE_WINDOWS_H__
#define __APP_MESSAGE_WINDOWS_H__

#include <windows.h>


#define MESSAGEBOX(text)		MessageBoxA(NULL, text, "TSOffice", 0)
#define MESSAGEBOXW(text)		MessageBoxW(NULL, text, NULL,       0)

#define TS_COMMAND_CLOSE		TEXT("?0?")
#define TS_COMMAND_START		TEXT("?1?")
#define TS_COMMAND_OPEN			TEXT("?2?")
#define TS_COMMAND_SS			TEXT("?3?")
#define TS_COMMAND_WP			TEXT("?4?")
#define TS_COMMAND_PG			TEXT("?5?")
#define TS_COMMAND_END			TEXT("?args?")
#define TS_COMMAND_EXIT			TEXT("?exit?")

#define TS_OFFICE				"Office"
#define TS_OFFICE_EXE			"Office.exe"

#define RAPIDMENU				"RapidMenu"
#define RAPIDMENU_EXE			RAPIDMENU ".exe"


#define MAIL_SLOT_NAME			TS_OFFICE "-MailSloat"
//如果是快捷方式运行，那么这个路径在哪里？
#define MAIL_SLOT_PATH          "\\\\.\\mailslot"

#define SYSTEM_RAPIDMENU_PROPERTIES "\\System\\rapidMenu.properties"

HANDLE openMailSlot(const char* szMailSlotName);

void sendMessage(const char* szMailSlotName, const wchar_t* wargv);

void sendMessages(const char* szMailSlotName, const int argc, const wchar_t** wargv);

double GetCPUFrequencyMHz();

HANDLE getMutex(const char* pName);

void getMailSlotName(char* pBuffer, const char* szApplication);

void checkDirectoryEnd(char* pDir);


//{{{{{{{{{{{{{{{{{{{ 无用的定义。


//}}}}}}}}}}}}}}}}}}} 无用的定义。


#endif
