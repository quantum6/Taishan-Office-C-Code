#ifndef __NPPLUGIN_JS_DATA_H__
#define __NPPLUGIN_JS_DATA_H__

#include <stdlib.h>
#include <stdio.h>
#if defined(_WINDOWS)
#include <process.h>
#else
#include <unistd.h>
#endif

#define JSON_COUNT               "count"

#define JSON_COMMAND_KEY         "command"
#define JSON_COMMAND_FUNCTION    "function"
#define JSON_COMMAND_EXIT        "exit"

#define JSON_FUNCTION_NAME       "name"
#define JSON_FUNCTION_PARAMS     "params"
#define JSON_FUNCTION_PARAM      "param"
#define JSON_FUNCTION_RESULT     "result"

#define JSON_DATA                "data"
#define JSON_DATATYPE            "datatype"
#define JSON_DATATYPE_BOOL       "bool"
#define JSON_DATATYPE_INT        "int"
#define JSON_DATATYPE_DOUBLE     "double"
#define JSON_DATATYPE_STRING     "string"
#define JSON_DATATYPE_LIST       "list"


typedef struct
{
    int dataFlag;

    //当前数据的大小。
    int dataSize;

    //开辟共享内存时的大小。
    int dataShmCapacity;
    int dataShmID;
} JsControlData;

class JsData
{
public:
    JsData(const int nID, const int shmID);
    ~JsData();

    int getControlShmID() {return this->m_nControlShmID;}

    int getDataFlag(){return controlShmAddr->dataFlag;}
    void clearDataFlag(){controlShmAddr->dataFlag = 0;}

    void copyData(const char* pData);
    char* getData();

private: 
    void releaseDataSHM();

    bool isReceiver;
    int  m_nPluginID;
    int  m_nControlShmID;
    JsControlData* controlShmAddr;
    char* dataShmAddr;
    
};

#endif
