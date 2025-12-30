#include <ctime>
#include <time.h>
#include <sys/time.h>
#include "TDirect2Screen.h"
#include "../framework/fw_log.h"

static long g_x11_win_id;
static int g_win_width;
static int g_win_height;

static SDL_Window* g_sdl_win;
static SDL_Surface* g_win_surface;
static SDL_Surface* g_draw_surface;

static pthread_t paint_thread;
static pthread_mutex_t g_mutex;
static queue<short> g_que_buf_idle;
static queue<short> g_que_buf_display;
static bool g_x11_winid_changed;
static struct image_data g_image_buff[IMAGE_BUFFER_COUNT];
static uint8_t g_draw_surface_buff[IMAGE_BUFFER_SIZE * CHANNELS];

static void init()
{
    (void)g_eDefaultLogEvel; // 避免unused warning
    g_sdl_win = NULL;
    g_win_surface = NULL;
    g_draw_surface = NULL;

    g_win_width = 0;
    g_win_height = 0;
    
    while(!g_que_buf_display.empty())
        g_que_buf_display.pop();
    
    for(int i=0;  i<IMAGE_BUFFER_COUNT;  ++i)
    {
        g_image_buff[i].totalBytes = 0;
        g_que_buf_idle.push(i);
    }
    
    static bool bMutexInit = false;
    if (!bMutexInit)
    {
        if (pthread_mutex_init(&g_mutex, NULL) != 0)
            DEBUG_TO_FILE("g_mutex init failed\n");
        bMutexInit = true;
    }
}

static void release_resource()
{    
    if (g_win_surface)
    {
        SDL_FreeSurface(g_win_surface);
        g_win_surface = NULL;
    }

    if (g_sdl_win)
    {
        SDL_DestroyWindow(g_sdl_win);
        g_sdl_win = NULL;
    }

    if (g_draw_surface)
    {
        SDL_FreeSurface(g_draw_surface);
        g_draw_surface = NULL;
    }
}

static void* paint_func(void* data)
{
    (void)data;
    int totalImages = 0;
    int index = -1;
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        DEBUG_TO_FILE("SDL_Init failed: %s\n", SDL_GetError());
        return 0;
    }
    
    if (g_x11_win_id == 0)
    {
        return 0;
    }
    
    g_x11_winid_changed = false;
    release_resource();
    g_sdl_win = SDL_CreateWindowFrom((const void*)g_x11_win_id); 

    if (g_sdl_win == NULL) 
    {
        DEBUG_TO_FILE("\nCreate frome window failed: %s\n", SDL_GetError());
        return  0;
    }

    g_win_surface = SDL_GetWindowSurface(g_sdl_win);    
    if (g_win_surface == NULL) 
    {
        DEBUG_TO_FILE("Get window surface failed: %s\n", SDL_GetError());
        return  0;
    }

    Uint32 rMask, gMask, bMask;
    rMask = 0x00ff0000;
    gMask = 0x0000ff00;
    bMask = 0x000000ff;
    
    g_draw_surface = SDL_CreateRGBSurfaceFrom(g_draw_surface_buff, g_win_width, g_win_height, 
            CHANNELS * 8, g_win_width * CHANNELS, rMask, gMask, bMask, 0);

#ifdef PERF_TEST
    timespec beg_, end_;
    clock_gettime(CLOCK_REALTIME, &beg_);
#endif

    while(true)
    {             
        index = -1;
        pthread_mutex_lock(&g_mutex);
        if (g_que_buf_display.size()>0)
        {   
            index = g_que_buf_display.front(); 
            g_que_buf_display.pop();
            pthread_mutex_unlock(&g_mutex);
        } 
        else
        {
            pthread_mutex_unlock(&g_mutex);
            if (g_x11_winid_changed == true)
                break;
            
            usleep(SLEEP_TIME_MS * 1000);
            continue;
        }

        memcpy(g_draw_surface_buff,g_image_buff[index].data,g_image_buff[index].totalBytes);

        if (SDL_BlitSurface(g_draw_surface, 0, g_win_surface, 0) == 0)
        {
            if (0!=SDL_UpdateWindowSurface(g_sdl_win))
            {
                DEBUG_TO_FILE("Update window surface failed: %s\n", SDL_GetError()); 
            }
        }
        else
        {
            DEBUG_TO_FILE("Blit surface failed: %s\n", SDL_GetError()); 
        }

        ++totalImages;
        pthread_mutex_lock(&g_mutex);
        g_que_buf_idle.push(index);
        pthread_mutex_unlock(&g_mutex);
    }       

#ifdef PERF_TEST
     clock_gettime(CLOCK_REALTIME, &end_);
     double timeSeconds = ((end_.tv_sec - beg_.tv_sec) * 1000 +(end_.tv_nsec - beg_.tv_nsec) / 1000000) / 1000.0;
     printf("totalImages: %d, timeSeconds: %.2f, FPS: %.2f\n",totalImages, timeSeconds, totalImages/timeSeconds);
#endif    
       
    release_resource();
    pthread_exit(0);
}

void setXWindow(long xid,int winwidth,int winheight)
{
    static bool bInit =false;
    if (!bInit)
    {
        init();
        bInit = true;
    }

    if (winwidth != 0)
        g_win_width = winwidth;
    if (winheight != 0)
        g_win_height = winheight;

    if (g_x11_win_id == 0 || (g_x11_win_id != 0 && g_x11_win_id != xid)) 
    {        
        g_x11_win_id = xid;
        g_x11_winid_changed = true;
            
        // 等待线程退出
        if(paint_thread != 0)
        {
            void* retVal = NULL;
            pthread_join(paint_thread, (void**)&retVal);            
        }        
        
        if(xid == 0)
            return ;
        
        // 重置queue
        while(!g_que_buf_display.empty())
            g_que_buf_display.pop();
    
        for(int i=0;  i<IMAGE_BUFFER_COUNT;  ++i)
        {
            g_image_buff[i].totalBytes = 0;
            g_que_buf_idle.push(i);
        }
    
        if(pthread_create(&paint_thread,0,paint_func,0) != 0)
            DEBUG_TO_FILE("Create paint thread failed.");    
    }
}

void paintImage(int imgwidth,int imgheight, char* img_pixels)
{
    int index = 0;
    while(true) 
    {
        pthread_mutex_lock(&g_mutex);
        if (!g_que_buf_idle.empty())
         {      
            index = g_que_buf_idle.front(); 
            g_que_buf_idle.pop();
            
            g_image_buff[index].totalBytes = imgwidth * imgheight * CHANNELS;
            memcpy(g_image_buff[index].data,img_pixels,imgwidth * imgheight * CHANNELS);
            
            g_que_buf_display.push(index);
            pthread_mutex_unlock(&g_mutex);
            break;
        }
        else
        {
          pthread_mutex_unlock(&g_mutex);
          usleep(SLEEP_TIME_MS * 1000);
        }
    }
}
