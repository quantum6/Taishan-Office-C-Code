#ifndef __LINUX_DIRECT2SCREEN_H__
#define __LINUX_DIRECT2SCREEN_H__

#include <unistd.h>
#include <SDL2/SDL.h>
#include <pthread.h>
#include <queue>

using namespace std;

#define CHANNELS 4
#define SLEEP_TIME_MS 10
#define IMAGE_BUFFER_COUNT 5
#define IMAGE_BUFFER_SIZE (1920 * 1080)

struct image_data
{
    // 图像总字节数
    int totalBytes;
    
    // 图像数据
    int data[IMAGE_BUFFER_SIZE];
};
    
void setXWindow(long window_id,int width,int height);
void paintImage(int imgWidth,int imgHeight, char* pixels);

#endif // end of __LINUX_DIRECT2SCREEN_H__

