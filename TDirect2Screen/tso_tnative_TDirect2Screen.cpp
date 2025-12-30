#include <jni.h>
#include "TDirect2Screen.h"
#include "tso_tnative_TDirect2Screen.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 设置要绘制的窗口id，窗口的宽度，高度
 * @param window_id awt窗口的id
 * @param width 窗口的宽度
 * @param height 窗口的高度
 */
JNIEXPORT void JNICALL Java_tso_tnative_TLinuxImagePaint_setXWindow(JNIEnv *, jclass, jlong xid,jint width,jint height)
{ 
    setXWindow(xid,width,height);
}

/*
 * 设置待绘制图片的宽度，高度，和对应的像素字节数据
 * @param imgWidth 图片的宽度
 * @param imgHeight 图片的高度
 * @param pixels 图片像素数组
 */
JNIEXPORT void JNICALL Java_tso_tnative_TLinuxImagePaint_paintImage(JNIEnv *env, jclass,jint imgWidth,jint imgHeight,jintArray pixels)
{
    jbyte *jbytes = (jbyte*)env->GetPrimitiveArrayCritical(pixels, 0);
    paintImage(imgWidth,imgHeight,(char*)jbytes);
    env->ReleasePrimitiveArrayCritical (pixels, jbytes,0);
}

#ifdef __cplusplus
}
#endif
