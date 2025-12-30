//#include "com_lruize_tsshading_TSShading.h"
#include "tso_appreader_read_util_TSShading.h"
#include "tsshading.h"

#include <QCoreApplication>
#include <QString>




void Java_tso_appreader_read_util_TSShading_testHello(JNIEnv *, jobject)
{
    printf("this is C++ print\n");
}

jstring Java_tso_appreader_read_util_TSShading_getBas64__(JNIEnv *env, jobject)
{
    QString str = "tsst QString ";
    return env->NewStringUTF(str.toUtf8().data());
}

jstring Java_tso_appreader_read_util_TSShading_getBase64__Ljava_lang_String_2(JNIEnv *env, jobject, jstring path)
{
    char* argv[1];
    int argc = 0;
    printf("in fun ");
    QCoreApplication  app(argc,argv);

    char* rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("utf-8");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray)env->CallObjectMethod(path, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0)
    {
        rtn = (char*)malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
//    LibTest lib;
//    QString str = lib.getBase64();
    printf("read string %s \n",rtn);
    QString str = "undefind function\n";
    if(NULL != rtn){
        free(rtn);
    }
    return env->NewStringUTF(str.toUtf8().data());
}

jstring Java_tso_appreader_read_util_TSShading_getImage(JNIEnv * env, jobject, jobject info)
{
//    ImageInfo rInfo;
    jclass jcs = env->GetObjectClass(info);
    if(jcs == NULL){
        printf("create jclass fail \n");
        return  env->NewStringUTF(QString("create fail").toUtf8().data());
    }
//    jfieldID width = env->GetFieldID(jcs,"width","I");
////    jfieldID height = env->GetFieldID(jcs,"height","D");
////    jfieldID imagePath = env->GetFieldID(jcs,"path","Ljava/lang/String;");
//    int iwidth = env->GetIntField(info,width);
//    printf("read width valeu in java call is %d \n",iwidth);
//    rInfo.height = env->GetDoubleField(info,height);
//    printf("get width %f, height %f\n",rInfo.width,rInfo.height);
//    jstring pathStr = (jstring) env->GetObjectField(info, imagePath);
//    printf("get jstring Success \n");
////    const char *locstr = env->GetStringUTFChars(nameStr, 0);
////    printf("get char* Success \n");
//    rInfo.path = getQString(env,pathStr);
//    printf("get image path %s\n",rInfo.path.toUtf8().data());

    TSShading lib(env,info);
    lib.init();
    QString base64 = lib.getBase64();
//    QString result = lib.getBase64(rInfo);

//    env->ReleaseStringUTFChars(nameStr,locstr);//释放字符串引用

//    return  env->NewStringUTF(result.toUtf8().data());
        return  env->NewStringUTF(base64.toUtf8().data());
}

JNIEXPORT jbyteArray JNICALL Java_tso_appreader_read_util_TSShading_getImageByte
(JNIEnv *env , jobject, jobject info)
{
    jclass jcs = env->GetObjectClass(info);
    if(jcs == NULL){
        printf("create jclass fail \n");
//        return  env->NewStringUTF(QString("create fail").toUtf8().data());


    }
    TSShading lib(env,info);
    lib.init();
    QByteArray by = lib.getBase64Byte();


    jbyteArray firstMacArray = env->NewByteArray( by.size() );
//    jintArray ii = env->NewIntArray()
    jbyte *bytes = env->GetByteArrayElements( firstMacArray, 0);
    printf("\n pring read \n");
    for ( int i = 0; i < by.size(); i++ )
    {
        bytes[ i ] = by.at(i);
        if(i<100){
            printf("%c ",bytes[i]);
        }
    }
    printf("\n end \n");
    env->SetByteArrayRegion(firstMacArray, 0, by.size(), bytes );
    jbyte *bytt =  (jbyte*)env->GetByteArrayElements(firstMacArray,0);
    char* bytChar = (char*)bytt;
    printf("pring byte size is %d \n",by.size());
    printf("\n pring after \n");
    for(int i=0; i< 100 && i< by.size(); ++i){
        printf("[%d] %c ",i,bytChar[i]);
    }
    return firstMacArray;
}
