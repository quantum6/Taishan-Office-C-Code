#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL_events.h>
#ifdef __WINDOWS__
#include <windows.h>
#endif

struct VideoState;

static struct VideoState ** vsArr = NULL;
static int arrSize = 0;
static int current = -1;

struct VideoState * initPlayerCore(void * parentWindowInfo, const char * inputFileName, int winX, int winY, int winW, int winH);
void event_loop(struct VideoState *cur_stream);
void toggle_pause(struct VideoState *is);
void stream_seek_start(struct VideoState *is);
int64_t getDurationInFFPlay(struct VideoState *is);
double getCurrentTimeInFFPlay(struct VideoState *is);
void stream_seek(struct VideoState *is, int64_t pos, int64_t rel, int seek_by_bytes);
int getPauseStatus(struct VideoState *is);
void increaseVolume(struct VideoState *is);
void decreaseVolume(struct VideoState *is);
void silence(struct VideoState * is);
void changeToMaxVolume(struct VideoState *is);
void do_exit(struct VideoState *is);
int isPlayedOver(struct VideoState *is);
int changeWindowPositionAndSize(struct VideoState *is, int x, int y, int w, int h);
void processEventOfFFPlay(struct VideoState *is);
static int isSupported(const char * inputFileName);
int changeWindowPositionAndSizeForWP(struct VideoState * is, int x, int y, int w, int h);

#ifdef linux
int PlayerCore_Create(void * parentHWND, const char * inputFileName) {
#endif
#ifdef __WINDOWS__
int PlayerCore_Create(HWND parentHWND, const char * inputFileName) {
#endif
    if (isSupported(inputFileName) != 1 )
        return -1;

    if (vsArr == NULL) {
        arrSize = 2;
        vsArr = malloc(arrSize*sizeof(void *));
    }

    current++;
    if(current >= arrSize) {
        struct VideoState ** vsArrTemp = (struct VideoState **)malloc(sizeof(void *)*arrSize*2);
        memcpy(vsArrTemp, vsArr, sizeof(void *)*arrSize);
        arrSize *= 2;
        free(vsArr);
        vsArr = vsArrTemp;
    }
    
    struct VideoState * vs = initPlayerCore(parentHWND, inputFileName, 0, 0, 0, 0);
    if(vs == NULL){
        return -1;
    }

    vsArr[current] = vs;

    return current;
}

#ifdef linux
int PlayerCore_CreateWithSize(void * parentHWND, const char * inputFileName, int winX, int winY, int winW, int winH) {
#endif
#ifdef __WINDOWS__
int PlayerCore_CreateWithSize(HWND parentHWND, const char * inputFileName, int winX, int winY, int winW, int winH) {
#endif
    if (isSupported(inputFileName) != 1 )
        return -1;

    if (vsArr == NULL) {
        arrSize = 2;
        vsArr = malloc(arrSize*sizeof(void *));
    }

    current++;
    if(current >= arrSize) {
        struct VideoState ** vsArrTemp = (struct VideoState **)malloc(sizeof(void *)*arrSize*2);
        memcpy(vsArrTemp, vsArr, sizeof(void *)*arrSize);
        arrSize *= 2;
        free(vsArr);
        vsArr = vsArrTemp;
    }

    struct VideoState * vs = initPlayerCore(parentHWND, inputFileName, winX, winY, winW, winH);
    if(vs == NULL){
        return -1;
    }

    vsArr[current] = vs;

    return current;
}


void PlayerCore_DisplayOneFrame(int vsID)
{
    if(vsID<0 || vsID>current)
        return ;

    if(!vsArr[vsID])
        return ;

    event_loop(vsArr[vsID]);
    processEventOfFFPlay(vsArr[vsID]);
}

void PlayerCore_PauseOrContinue(int vsID)
{
    if(vsID<0 || vsID>current)
        return ;

    if(!vsArr[vsID])
        return ;

    toggle_pause(vsArr[vsID]);
}

void PlayerCore_Pause(int vsID)
{
    if(vsID<0 || vsID>current)
        return ;

    if(!vsArr[vsID])
        return ;

    if (getPauseStatus(vsArr[vsID]))
        return ;

    toggle_pause(vsArr[vsID]);
}

void PlayerCore_Continue(int vsID)
{
    if(vsID<0 || vsID>current)
        return ;
    
    if(!vsArr[vsID])
        return ;

    if (!getPauseStatus(vsArr[vsID]))
        return ;

    toggle_pause(vsArr[vsID]);
}

int PlayerCore_getStatus(int vsID)
{
    if(vsID<0 || vsID>current)
        return -1;

    if(!vsArr[vsID])
        return -1;

    return getPauseStatus(vsArr[vsID]);
}

void PlayerCore_Reset(int vsID)
{
    if(vsID<0 || vsID>current)
        return ;

    if(!vsArr[vsID])
        return ;

    stream_seek(vsArr[vsID], 0, 0, 0);
    PlayerCore_Pause(vsID);
}

int64_t PlayerCore_GetDuration(int vsID)
{
    if(vsID<0 || vsID>current)
        return 0;

    if(!vsArr[vsID])
        return 0;

    return getDurationInFFPlay(vsArr[vsID]);
}

double PlayerCore_GetCurrentPlayTime(int vsID)
{
    if(vsID<0 || vsID>current)
        return 0;

    if(!vsArr[vsID])
        return 0;

    return getCurrentTimeInFFPlay(vsArr[vsID]);
}

int PlayerCore_SetPosition(int vsID, int64_t position)
{
    if(vsID<0 || vsID>current)
        return 0;

    if(!vsArr[vsID])
        return 0;

    stream_seek(vsArr[vsID], position, 0, 0);
    return 1;
}

void PlayerCore_IncreaseVolume(int vsID)
{
    if(vsID<0 || vsID>current)
        return ;

    if(!vsArr[vsID])
        return ;

    increaseVolume(vsArr[vsID]);
}

void PlayerCore_DecreaseVolume(int vsID)
{
    if(vsID<0 || vsID>current)
        return ;

    if(!vsArr[vsID])
        return ;

    decreaseVolume(vsArr[vsID]);
}

void PlayerCore_Silence(int vsID)
{
    if(vsID<0 || vsID>current)
        return ;

    if(!vsArr[vsID])
        return ;

    silence(vsArr[vsID]);    
}

void PlayerCore_ChangeToMaxVolume(int vsID)
{
    if(vsID<0 || vsID>current)
        return ;

    if(!vsArr[vsID])
        return ;

    changeToMaxVolume(vsArr[vsID]);
}

void PlayerCore_Destroy(int vsID)
{
    //puts("PlayerCore_Destroy:entry");
    //fflush(stdout);
    if(vsID<0 || vsID>current)
        return ;

    if(!vsArr[vsID])
        return ;
        
    //puts("PlayerCore_Destroy:destroy");
    //fflush(stdout);

    do_exit(vsArr[vsID]);
    vsArr[vsID] = 0;
}

int PlayerCore_isOver(int vsID)
{
    if(vsID<0 || vsID>current)
        return -1;

    if(!vsArr[vsID])
        return -2;
    return isPlayedOver(vsArr[vsID]);
}

int PlyaerCore_setPositionAndSize(int vsID, int x, int y, int w, int h)
{
    if(vsID<0 || vsID>current)
        return -1;

    if(!vsArr[vsID])
        return -2;

    return changeWindowPositionAndSize(vsArr[vsID], x, y, w, h);
}

int PlyaerCore_setPositionAndSizeForWP(int vsID, int x, int y, int w, int h)
{
    if(vsID<0 || vsID>current)
        return -1;

    if(!vsArr[vsID])
        return -2;

    return changeWindowPositionAndSizeForWP(vsArr[vsID], x, y, w, h);
}


static int isSupported(const char * inputFileName)
{
    if (inputFileName == NULL)
    {
        return 0;
    }

    char * pFind = strrchr(inputFileName, '.');
    if (pFind == NULL)
        return 0;

    //<支持的音频>
    if (strcasecmp(pFind, ".mp3") == 0)
        return 1;
    else if(strcasecmp(pFind, ".wav") == 0)
        return 1;
    else if(strcasecmp(pFind, ".wma") == 0)
        return 1;
    else if(strcasecmp(pFind, ".m4a") == 0)
        return 1;
    //</支持的音频>

    //<支持的视频格式>
    if(strcasecmp(pFind, ".mp4") == 0)
        return 1;
    else if (strcasecmp(pFind, ".avi") == 0)
        return 1;
    else if(strcasecmp(pFind, ".wmv") == 0)
        return 1;
    else if(strcasecmp(pFind, ".mpeg") == 0)
        return 1;
    else if(strcasecmp(pFind, ".mpg") == 0)
        return 1;
    else if(strcasecmp(pFind, ".mov") == 0)
        return 1;
    else if(strcasecmp(pFind, ".swf") == 0)
        return 1;
    else if (strcasecmp(pFind, ".asf") == 0)
        return 1;
    //</支持的视频格式>


    return 0;
}
