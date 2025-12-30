
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

#include <SDL.h>
#include <SDL_thread.h>

#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <sys/msg.h>
#include <sys/prctl.h>

#include "libavformat/avformat.h"
#include "fw_log.h"

static const struct TextureFormatEntry {
    enum AVPixelFormat format;
    int texture_fmt;
} sdl_texture_format_map[] = {
    { AV_PIX_FMT_RGB8,           SDL_PIXELFORMAT_RGB332 },
    { AV_PIX_FMT_RGB444,         SDL_PIXELFORMAT_RGB444 },
    { AV_PIX_FMT_RGB555,         SDL_PIXELFORMAT_RGB555 },
    { AV_PIX_FMT_BGR555,         SDL_PIXELFORMAT_BGR555 },
    { AV_PIX_FMT_RGB565,         SDL_PIXELFORMAT_RGB565 },
    { AV_PIX_FMT_BGR565,         SDL_PIXELFORMAT_BGR565 },
    { AV_PIX_FMT_RGB24,          SDL_PIXELFORMAT_RGB24 },
    { AV_PIX_FMT_BGR24,          SDL_PIXELFORMAT_BGR24 },
    { AV_PIX_FMT_0RGB32,         SDL_PIXELFORMAT_RGB888 },
    { AV_PIX_FMT_0BGR32,         SDL_PIXELFORMAT_BGR888 },
    { AV_PIX_FMT_NE(RGB0, 0BGR), SDL_PIXELFORMAT_RGBX8888 },
    { AV_PIX_FMT_NE(BGR0, 0RGB), SDL_PIXELFORMAT_BGRX8888 },
    { AV_PIX_FMT_RGB32,          SDL_PIXELFORMAT_ARGB8888 },
    { AV_PIX_FMT_RGB32_1,        SDL_PIXELFORMAT_RGBA8888 },
    { AV_PIX_FMT_BGR32,          SDL_PIXELFORMAT_ABGR8888 },
    { AV_PIX_FMT_BGR32_1,        SDL_PIXELFORMAT_BGRA8888 },
    { AV_PIX_FMT_YUV420P,        SDL_PIXELFORMAT_IYUV },
    { AV_PIX_FMT_YUYV422,        SDL_PIXELFORMAT_YUY2 },
    { AV_PIX_FMT_UYVY422,        SDL_PIXELFORMAT_UYVY },
    { AV_PIX_FMT_NONE,           SDL_PIXELFORMAT_UNKNOWN },
};

static int frame_width   = 1280;
static int frame_height  = 720;
static int color_counter = 0;
static SDL_Rect displayRect = {0, 0, 1280, 720};

static SDL_Window   *pSdlWindow   = NULL;
static SDL_Renderer *pSdlRenderer = NULL;
static SDL_Texture  *pSdlTexture  = NULL;

static GtkWidget*   pNativeWindow = NULL;


static void get_sdl_pix_fmt_and_blendmode(int format, Uint32 *sdl_pix_fmt, SDL_BlendMode *sdl_blendmode)
{
    int i;
    if (format == AV_PIX_FMT_RGB32   ||
        format == AV_PIX_FMT_RGB32_1 ||
        format == AV_PIX_FMT_BGR32   ||
        format == AV_PIX_FMT_BGR32_1)
    {
        *sdl_blendmode = SDL_BLENDMODE_BLEND;
    }
    else
    {
        *sdl_blendmode = SDL_BLENDMODE_NONE;
    }

    // 20
    //printf("FF_ARRAY_ELEMS(sdl_texture_format_map)=%d\n", FF_ARRAY_ELEMS(sdl_texture_format_map));
    *sdl_pix_fmt = SDL_PIXELFORMAT_UNKNOWN;
    for (i = 0; i < FF_ARRAY_ELEMS(sdl_texture_format_map) - 1; i++)
    {
        if (format == sdl_texture_format_map[i].format)
        {
            *sdl_pix_fmt = sdl_texture_format_map[i].texture_fmt;
            return;
        }
    }
}

static int realloc_texture(SDL_Renderer *renderer, SDL_Texture **texture, Uint32 new_format, int new_width, int new_height, SDL_BlendMode blendmode, int init_texture)
{
    Uint32 format;
    int access, w, h;
    if (!*texture || SDL_QueryTexture(*texture, &format, &access, &w, &h) < 0 || new_width != w || new_height != h || new_format != format) {
        void *pixels;
        int pitch;
        if (*texture)
            SDL_DestroyTexture(*texture);

        if (!(*texture = SDL_CreateTexture(renderer, new_format, SDL_TEXTUREACCESS_STREAMING, new_width, new_height)))
            return -1;
        if (SDL_SetTextureBlendMode(*texture, blendmode) < 0)
            return -1;
        if (init_texture) {
            if (SDL_LockTexture(*texture, NULL, &pixels, &pitch) < 0)
                return -1;
            memset(pixels, 0, pitch * new_height);
            SDL_UnlockTexture(*texture);
        }
        printf("Created %dx%d texture with %s.\n", new_width, new_height, SDL_GetPixelFormatName(new_format));
    }
    return 0;
}

int image2screen_update(char* pFrameBuffer)
{
    int ret = 0;
    ret = SDL_UpdateTexture(pSdlTexture, NULL, pFrameBuffer, 4);
    ret = SDL_RenderCopyEx(pSdlRenderer, pSdlTexture, NULL, &displayRect, 0, NULL, SDL_FLIP_NONE);

    //paint to screen
    SDL_RenderPresent(pSdlRenderer);

    return ret;
}

int image2screen_init(long xid, int frame_width, int frame_height)
{
    atexit(SDL_Quit);
    //usleep(1000*1000);
    SDL_Init (SDL_INIT_VIDEO);
    //SDL_EventState(SDL_SYSWMEVENT, SDL_IGNORE);
    //SDL_EventState(SDL_USEREVENT,  SDL_IGNORE);
    //SDL_SetVideoMode(frame_width, frame_height, 0);//SDL_HWSURFACE | SDL_ANYFORMAT);

    if (xid == 0)
    {
        pSdlWindow = SDL_CreateWindow("TSRJ", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, frame_width, frame_height, SDL_WINDOW_HIDDEN);
        SDL_SetWindowSize(pSdlWindow, frame_width, frame_height);
        SDL_SetWindowPosition(pSdlWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_ShowWindow(pSdlWindow);
    }
    else
    {
        pSdlWindow = SDL_CreateWindowFrom((void*)xid);
        //pSdlWindow = SDL_CreateWindowFrom((pWindow->window));
    }

    pSdlRenderer = SDL_CreateRenderer(pSdlWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    //TIO_LOG_INFO("pSdlRenderer=0x%04X", pSdlRenderer);
    realloc_texture(pSdlRenderer, &pSdlTexture, SDL_PIXELFORMAT_ARGB8888,
         frame_width, frame_height, SDL_BLENDMODE_BLEND, 0);

    displayRect.x = 0;
    displayRect.y = 0;
    displayRect.w = frame_width;
    displayRect.h = frame_height;

    return 0;
}

static void sigterm_handler(int sig)
{
    exit(123);
}

void* paint_thread(void* pData)
{
    char* pFrameBuffer;
    int screen_size;
    screen_size = frame_width*frame_height*4;
    pFrameBuffer = (char*)malloc(screen_size);

    image2screen_init(
#if 0
             0,
#else
             GDK_WINDOW_XID(pNativeWindow->window),
#endif
             frame_width, frame_height);

    for (;;)
    {
        color_counter += 8;
        memset(pFrameBuffer, color_counter, screen_size);
        image2screen_update(pFrameBuffer);

        usleep(20*1000);
    }

    return NULL;
}

int main(int argc, char **argv)
{
    pthread_t tid;

    signal(SIGINT , sigterm_handler);
    signal(SIGTERM, sigterm_handler);

    gtk_init(NULL, NULL);
    //GTK_WINDOW_TOPLEVEL, GTK_WINDOW_POPUP
    pNativeWindow=gtk_window_new(GTK_WINDOW_POPUP);
    GtkWidget* window = pNativeWindow;
    gtk_window_set_decorated(   GTK_WINDOW(window), FALSE);

    frame_width /= 4;
    frame_height/= 4;
    //gtk_window_set_position(    GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), frame_width, frame_height);
    gtk_window_set_title(       GTK_WINDOW(window), "OFFICE");

    gtk_widget_show_all(window);

    sleep(1);
    pthread_create(&tid, NULL, paint_thread, NULL);

    gtk_main();


    return 0;
}
