#include "tso_tnative_TImage2Screen.h"

#include "image2screen.h"

/*
 * 将系统剪贴板里的dataFormat格式的数据保存到fileName指定的文件
 */
JNIEXPORT jboolean JNICALL Java_tso_tnative_TImage2Screen_init(JNIEnv *env, jclass cls, jlong xid, jint width, jint height)
{
    image2screen_init(xid, width, height);
	return JNI_TRUE;
}

/*
 * 获取系统剪贴板当前数据的格式列表
 */
JNIEXPORT jboolean JNICALL Java_tso_tnative_TImage2Screen_update(JNIEnv *env, jclass cls, jbyteArray data)
{
    jbyte *jbytes = (jbyte*)env->GetPrimitiveArrayCritical(data, 0);
	image2screen_update((char*)jbytes);
    env->ReleasePrimitiveArrayCritical(data, jbytes, 0);

    return JNI_TRUE;
}
