
#include <sys/types.h>
#include <string.h>
#if defined(_WINDOWS)
#include <windows.h>
#include <iostream>
#include <string>
#else
#include <sys/shm.h>
#endif

#include "fw_kit.h"
#include "fw_log.h"
#include "npplugin_js_data.h"

static bool initSHM(const int size, const int plugID, int* pID, void** ppAddr)
{
    int shmBase = (getpid() << 8) + (plugID << 4); 
    int counter = 0;

#if defined(_WINDOWS)
	char name[BUFFER_SIZE] = {0};
	while ((*pID) <= 0)
    {
        shmBase += counter;
        counter ++;
		memset(name, 0, BUFFER_SIZE);
		sprintf(name, "Taishan-%d", shmBase);
		(*pID) = (int)::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,0,
			size, (LPCWSTR)name);
    }
	(*ppAddr) = ::MapViewOfFile((HANDLE)(*pID), FILE_MAP_ALL_ACCESS, 0, 0, 0);
#else
    while ((*pID) <= 0)
    {
        shmBase += counter;
        counter ++;
        (*pID) = shmget(shmBase, size, 0666 | IPC_CREAT);
    }

    (*ppAddr) = shmat((*pID), NULL, 0);
    if ((*ppAddr)  == (void*)-1)
    {
        shmctl((*pID), IPC_RMID, 0);
        (*ppAddr) = NULL;
        return false;
    }
#endif
    return true;
}

static void deleteSHM(const int shmID, void* pShmAddr)
{
#if defined(_WINDOWS)
	::UnmapViewOfFile(pShmAddr);//停止指针到共享内存的映射
    ::CloseHandle((HANDLE)shmID);//关闭共享内存的句柄
#else
	shmctl(shmID, IPC_RMID, 0);
#endif
}

JsData::JsData(const int pluginID, const int shmID)
{
	
    this->isReceiver      = false;
    this->m_nPluginID     = 0;
    this->m_nControlShmID = 0;
    this->controlShmAddr  = NULL;
    this->dataShmAddr     = NULL;

    this->m_nPluginID     = pluginID;
    this->m_nControlShmID = shmID;
    this->isReceiver      = (this->m_nControlShmID > 0);
    initSHM(sizeof(JsControlData), pluginID, &(this->m_nControlShmID), (void**)&controlShmAddr);
    if (!isReceiver)
    {
        memset(controlShmAddr, 0, sizeof(JsControlData));
    }
}

JsData::~JsData()
{
    releaseDataSHM();
    if (this->isReceiver)
    {
		deleteSHM(this->m_nControlShmID, controlShmAddr);
        m_nControlShmID = 0;
		controlShmAddr  = NULL;
    }
}

void JsData::copyData(const char* pData)
{
    int dataSize = strlen(pData);
    if (dataSize >= controlShmAddr->dataShmCapacity)
    {
        releaseDataSHM();
        initSHM(dataSize*2, this->m_nPluginID, &(controlShmAddr->dataShmID), (void**)(&dataShmAddr));
        controlShmAddr->dataShmCapacity= dataSize*2;
    }

    controlShmAddr->dataSize = dataSize;
    strcpy(dataShmAddr, pData);
    dataShmAddr[dataSize] = 0;
    controlShmAddr->dataFlag = !isReceiver;
}

char* JsData::getData()
{
	if (dataShmAddr == NULL)
	{
#if defined(_WINDOWS)
	dataShmAddr = (char*)::MapViewOfFile((HANDLE)(controlShmAddr->dataShmID), FILE_MAP_ALL_ACCESS, 0, 0, 0);
#else
    dataShmAddr = (char*)shmat(controlShmAddr->dataShmID, NULL, 0);
#endif
	}
    return dataShmAddr;
}

void JsData::releaseDataSHM()
{
    if (controlShmAddr->dataShmID > 0)
    {
        deleteSHM(controlShmAddr->dataShmID, dataShmAddr);
        controlShmAddr->dataShmID       = 0;
        controlShmAddr->dataSize        = 0;
        controlShmAddr->dataShmCapacity = 0;
        dataShmAddr = NULL;
    }

}

