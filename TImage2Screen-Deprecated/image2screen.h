#ifndef __LINUX_IMAGE2SCREEN_H__
#define __LINUX_IMAGE2SCREEN_H__

int image2screen_init(long xid, int frame_width, int frame_height);

int image2screen_update(char* pFrameBuffer);

int image2screen_release();

#endif //__LINUX_IMAGE2SCREEN_H__
