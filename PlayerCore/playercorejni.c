//#define __int64 long long
#include <stdint.h>
#define __int64 int64_t

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <jni.h>
#include <memory.h>
#ifndef linux
#include <windows.h>
#endif
#include "JAWTInfo.h"

#ifdef linux
int PlayerCore_Create(void * parentHWND, const char * inputFileName);
#endif
#ifndef linux
int PlayerCore_Create(HWND parentHWND, const char * inputFileName);
#endif

#ifdef linux
int PlayerCore_CreateWithSize(void * parentHWND, const char * inputFileName, int winX, int winY, int winW, int winH);
#endif
#ifndef linux
int PlayerCore_CreateWithSize(HWND parentHWND, const char * inputFileName, int winX, int winY, int winW, int winH);
#endif

void PlayerCore_DisplayOneFrame(int vsID);
int PlayerCore_getStatus(int vsID);
void PlayerCore_Pause(int vsID);
void PlayerCore_Continue(int vsID);
void PlayerCore_Reset(int vsID);
double PlayerCore_GetCurrentPlayTime(int vsID);
int64_t PlayerCore_GetDuration(int vsID);
int PlayerCore_SetPosition(int vsID, int64_t position);
void PlayerCore_IncreaseVolume(int vsID);
void PlayerCore_DecreaseVolume(int vsID);
void PlayerCore_Silence(int vsID);
void PlayerCore_ChangeToMaxVolume(int vsID);
void PlayerCore_Destroy(int vsID);
int PlayerCore_isOver(int vsID);
int PlyaerCore_setPositionAndSize(int vsID, int x, int y, int w, int h);
int PlyaerCore_setPositionAndSizeForWP(int vsID, int x, int y, int w, int h);

/*
 *  函数名称:StringToWindowChars
 *	函数功能:将jstring转成UTFchar
 *  函数参数:jstr jstring字符串  winchar UTFchar字符串返回值
 *  函数返回:成功时 返回转换后的字符串 编码格式为UTF-9  0为失败            
 */
static char * StringToWindowChars(JNIEnv * env, jstring jstr){	
	const char * str = (*env)->GetStringUTFChars(env, jstr, 0);
	if(str == NULL)
        return 0;

    char * winchar = malloc(strlen(str)+1);
	strcpy(winchar, str);
	(*env)->ReleaseStringUTFChars(env, jstr, str);
    return winchar;	
}

static void releaseJAWTInfo(struct JAWTInfo * info){
    if(info->ds != NULL){
        // Free the drawing surface info
        info->ds->FreeDrawingSurfaceInfo(info->dsi);
        // Unlock the drawing surface
        info->ds->Unlock(info->ds);
        // Free the drawing surface
        info->awt.FreeDrawingSurface(info->ds);
    }

    free(info);
}

#ifndef linux
static HWND getHWNDFromJavaComponent(JNIEnv * env, jobject comp){
#endif

#ifdef linux
static void * getHWNDFromJavaComponent(JNIEnv * env, jobject comp){
#endif
    struct JAWTInfo * info = (struct JAWTInfo *)malloc(sizeof(struct JAWTInfo));
    memset(info, 0, sizeof(struct JAWTInfo));

    jboolean result = JNI_FALSE;
    jint lock = 0;

    // Get the AWT
    info->awt.version = JAWT_VERSION_1_7;
    result = JAWT_GetAWT(env, &info->awt);
    if (result == JNI_FALSE){
        releaseJAWTInfo(info);
        //MessageBox(NULL, "step 1", "step 1", MB_OK);
        return 0;
    }
   
    info->ds = info->awt.GetDrawingSurface(env, comp); //Get the drawing surface
    if(info->ds == NULL){
        //MessageBox(NULL, "step 2", "step 2", MB_OK);
        return 0; //TODO:release info
    }

    lock = info->ds->Lock(info->ds); //Lock the drawing surface  
    if(!(lock & JAWT_LOCK_ERROR) == 0){
        //MessageBox(NULL, "step 3", "step3", MB_OK);
        return 0; //TODO:release info
    }

    info->dsi = info->ds->GetDrawingSurfaceInfo(info->ds); //Get the drawing surface info
    if(info->dsi == NULL){
        //MessageBox(NULL, "step 4", "step 4", MB_OK);
        return 0;
    }

#ifndef linux
    info->dsi_win = (JAWT_Win32DrawingSurfaceInfo*)info->dsi->platformInfo; //Get the platform-specific drawing info
#endif
#ifdef linux
    info->dsi_win = (JAWT_X11DrawingSurfaceInfo*)info->dsi->platformInfo; //Get the platform-specific drawing info
#endif
    if(info->dsi_win == NULL) {
       // MessageBox(NULL, "step 5", "step 5", MB_OK);
        return 0;
    }

#ifndef linux
    HWND hwnd = info->dsi_win->hwnd;
#endif

#ifdef linux
    void * hwnd = (void *)info->dsi_win->drawable;
#endif
    
    releaseJAWTInfo(info);
    return hwnd;
}

//emo.enative.EPlayerCore.create
jint JNICALL Java_tso_tnative_TPlayerCore_create(JNIEnv *env, jclass cls, jobject comp, jstring jstrFilePath){
#ifndef linux
    HWND hwnd = getHWNDFromJavaComponent(env, comp);
#endif
#ifdef linux
    void * hwnd = getHWNDFromJavaComponent(env, comp);
#endif

    if(hwnd == 0)
        return -10;

    char * szFilePath = StringToWindowChars(env, jstrFilePath);
    if(szFilePath == 0) {
        return -11;
    }

    int ret = PlayerCore_Create(hwnd, szFilePath);
    //int ret = PlayerCore_Create(0, szFilePath);
    free(szFilePath);

    return ret;
}

jint JNICALL Java_tso_tnative_TPlayerCore_createWithSize(JNIEnv *env, jclass cls, jobject comp, jstring jstrFilePath, jint winX, jint winY, jint winW, jint winH){
#ifndef linux
    HWND hwnd = getHWNDFromJavaComponent(env, comp);
#endif
#ifdef linux
    void * hwnd = getHWNDFromJavaComponent(env, comp);
#endif

    if(hwnd == 0)
        return -10;

    char * szFilePath = StringToWindowChars(env, jstrFilePath);
    if(szFilePath == 0) {
        return -11;
    }

    int ret = PlayerCore_CreateWithSize(hwnd, szFilePath, winX, winY, winW, winH);
    free(szFilePath);

    return ret;
}


//emo.enative.EPlayerCore.create
jint JNICALL Java_tso_tnative_TPlayerCore_initSound(JNIEnv *env, jclass cls, jstring jstrFilePath){
    char * szFilePath = StringToWindowChars(env, jstrFilePath);
    if(szFilePath == 0) {
        return -11;
    }

    int ret = PlayerCore_Create(0, szFilePath);
    free(szFilePath);

    return ret;
}

void JNICALL Java_tso_tnative_TPlayerCore_displayOneFrame(JNIEnv *env, jclass cls, jint playerID){
    PlayerCore_DisplayOneFrame((int)playerID);
}

jint JNICALL Java_tso_tnative_TPlayerCore_getStatus(JNIEnv *env, jclass cls, jint playerID){
    return (jint)PlayerCore_getStatus(playerID);
}

void JNICALL Java_tso_tnative_TPlayerCore_pause(JNIEnv *env, jclass cls, jint playerID){
    PlayerCore_Pause(playerID);
}

void JNICALL Java_tso_tnative_TPlayerCore_play(JNIEnv *env, jclass cls, jint playerID){
    PlayerCore_Continue(playerID);
}

void JNICALL Java_tso_tnative_TPlayerCore_stop(JNIEnv *env, jclass cls, jint playerID){
    PlayerCore_Reset((int)playerID);
}

double JNICALL Java_tso_tnative_TPlayerCore_getCurrentPlayTime(JNIEnv *env, jclass cls, jint playerID){
    return PlayerCore_GetCurrentPlayTime(playerID);
}

int64_t JNICALL Java_tso_tnative_TPlayerCore_getDuration(JNIEnv *env, jclass cls, jint playerID){
    return PlayerCore_GetDuration(playerID)/1000; //millisecond
}

//time millisecond
void JNICALL Java_tso_tnative_TPlayerCore_seekTo(JNIEnv *env, jclass cls, jint playerID, jlong time){
    PlayerCore_SetPosition(playerID, time*1000);
}

void JNICALL Java_tso_tnative_TPlayerCore_increaseVolume(JNIEnv *env, jclass cls, jint playerID){
    PlayerCore_IncreaseVolume(playerID);
}

void JNICALL Java_tso_tnative_TPlayerCore_decreaseVolume(JNIEnv *env, jclass cls, jint playerID){
    PlayerCore_DecreaseVolume(playerID);
}

void JNICALL Java_tso_tnative_TPlayerCore_silence(JNIEnv *env, jclass cls, jint playerID){
    PlayerCore_Silence(playerID);
}

void JNICALL Java_tso_tnative_TPlayerCore_changeToMaxVolume(JNIEnv *env, jclass cls, jint playerID){
    PlayerCore_ChangeToMaxVolume(playerID);
}

void JNICALL Java_tso_tnative_TPlayerCore_destroy(JNIEnv *env, jclass cls, jint playerID){
    PlayerCore_Destroy(playerID);
}

int JNICALL Java_tso_tnative_TPlayerCore_isOver(JNIEnv *env, jclass cls, jint playerID)
{
    return PlayerCore_isOver(playerID);
}

int JNICALL Java_tso_tnative_TPlayerCore_setPositionAndSize(JNIEnv *env, jclass cls, jint playerID, jint x, jint y, jint w, jint h) {
    return PlyaerCore_setPositionAndSize(playerID, x, y, w, h);
}

int JNICALL Java_tso_tnative_TPlayerCore_setPositionAndSizeForWP(JNIEnv *env, jclass cls, jint playerID, jint x, jint y, jint w, jint h) {
    return PlyaerCore_setPositionAndSizeForWP(playerID, x, y, w, h);
}
