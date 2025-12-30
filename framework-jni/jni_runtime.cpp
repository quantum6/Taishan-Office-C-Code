#include <stdio.h>
#include <stdlib.h>

#ifdef _WINDOWS
#include <windows.h>
#else
#include <dirent.h>
#include <dlfcn.h>
#endif

#include "fw_config.h"
#include "fw_log.h"
#include "fw_kit.h"
#include "jni_runtime.h"


// 8K 不够了。
#define CLASSPATH_SIZE (8192*2)


void getJarPathApp(char* pPath)
{
    getAppBinDir(pPath);
    strcat(pPath, OS_DIR_TEXT);
    strcat(pPath, KEY_FILE_TS_OFFICE_JAR);
}


typedef jint (JNICALL *CreateJavaVM_t)(JavaVM **pvm, void **env, void *args);
typedef jint (JNICALL *GetDefaultJavaVMInitArgs_t)(void *args);
static bool convertUTF8ToUnicode(char * utf8StrIn, wchar_t * unicodeBufferOut, int sizeOfBuffer);

#ifdef _WINDOWS
static jstring NewPlatformString(JNIEnv *env, char *s);
#endif

typedef struct
{
    CreateJavaVM_t CreateJavaVM;
    GetDefaultJavaVMInitArgs_t GetDefaultJavaVMInitArgs;
} InvocationFunctions;


static jboolean LoadJavaVM(const char* pJdkPath, InvocationFunctions *ifn);

static int  createJavaVM(JavaVM** ppJvm, JNIEnv** ppEnv, const JavaVMInitArgs* pArgs, const char* pJdkPath);
static int  get_jvm_path(const char* pJdkPath, char* pBuffer);
static int  find_file(const char* pJdkPath, const char* pFile, char* pBuffer);

// --add-exports=java.desktop/sun.awt.image=ALL-UNNAMED
static char* ADD_EXPORTS[] = {
    "java.base/sun.nio.cs",
    "java.base/sun.text",
	"java.base/sun.security.action",
	"java.base/sun.security.x509",
    "java.base/sun.security.tools.keytool",
	
	"java.xml/com.sun.org.apache.xerces.internal.xni",
	"java.xml/com.sun.org.apache.xerces.internal.xni.parser",
	"java.xml/com.sun.org.apache.xerces.internal.util",
	"java.xml/com.sun.org.apache.xerces.internal.impl",
	"java.xml/com.sun.org.apache.xerces.internal.impl",
	"java.xml/com.sun.org.apache.xerces.internal.jaxp.datatype",
	"java.xml/com.sun.org.apache.xerces.internal.dom",
	
	"java.desktop/java.awt.peer",
	"java.desktop/sun.awt.image",
	"java.desktop/sun.print",
    "java.desktop/sun.font",
    "java.desktop/sun.awt",
    "java.desktop/sun.awt.shell",
    "java.desktop/sun.swing",
    "java.desktop/com.sun.imageio.plugins.common",
    "java.desktop/com.sun.imageio.plugins.jpeg",
    "java.desktop/sun.awt.datatransfer",
    "java.desktop/sun.java2d",
    
    NULL
    };

bool  JniRuntime::start(const char* pJdkPath)
{
    //ASSERT(pJniRuntime != NULL);
    if (pJvm != NULL)
    {
        return true;
    }

    // 创建虚拟机时虚拟机的参数描述
    const int classpathSize = CLASSPATH_SIZE;
    char classpath[classpathSize] = {0};
    JavaVMOption options[64];
    int  optionNum = 0;
    if(strlen(jarFilePath) == 0)
    {
        TS_LOG_INFO("Info: jar file is not set...");
    }
    addJarsToParamter(classpath, jarFilePath);
    options[optionNum++].optionString = strdup(classpath);

    //sprintf(classpath, "-Djava.home=%s", pJdkPath);
    //options[optionNum++].optionString = strdup(classpath);

    options[optionNum++].optionString = (char*)"-Xms256m";

#ifdef _WINDOWS    
# ifndef _WIN64
    options[optionNum++].optionString = (char*)"-Xmx800m";
# else
    options[optionNum++].optionString = (char*)"-Xmx8192m";        
# endif
#else
    options[optionNum++].optionString = (char*)"-Xmx8192m";
#endif

    options[optionNum++].optionString = (char*)"-Xverify:none";
    options[optionNum++].optionString = (char*)"-XX:SurvivorRatio=6";
    options[optionNum++].optionString = (char*)"-XX:MinHeapFreeRatio=10";
    options[optionNum++].optionString = (char*)"-XX:MaxHeapFreeRatio=40";
    options[optionNum++].optionString = (char*)"-Djava.awt.noerasebackground=true";
    options[optionNum++].optionString = (char*)"-Djava.java2d.noddraw=true";
    options[optionNum++].optionString = (char*)"-Djava.java2d.ddlock=true";
    options[optionNum++].optionString = (char*)"-Djava.util.Arrays.useLegacyMergeSort=true";
    
    int index = 0;
    char binPath[128] = {0};
    while (1)
    {
        if (ADD_EXPORTS[index] == NULL)
        {
            break;
        }
        memset(binPath, 0, 128);
        sprintf(binPath, "--add-exports=%s=ALL-UNNAMED", ADD_EXPORTS[index]);
        options[optionNum++].optionString = strdup(binPath);
        index++;
    }

    memset(binPath, 0, 128);
    getAppBinDir(binPath);

    memset(classpath, 0, classpathSize);
    sprintf(classpath, "-Djava.library.path=%s%s%s%s%s",
        binPath,  OS_SEP,
        pJdkPath, OS_DIR_TEXT, KEY_DIR_BIN);

    options[optionNum++].optionString = (char*)strdup(classpath);
    JavaVMInitArgs vm_args;
    memset(&vm_args, 0, sizeof(vm_args));

    // 该JDK版本变量随JDK的主版本号变化
    // 而变化，如JDK1.5系列则需更改JNI和 
    // 此处的JNI_VERSION_1_4
#if defined(JNI_VERSION_1_8)
    vm_args.version  = JNI_VERSION_1_8;
#else
    vm_args.version  = JNI_VERSION_1_6;
#endif

    vm_args.nOptions = optionNum;
    vm_args.options  = options;
    vm_args.ignoreUnrecognized = JNI_TRUE;
    // create jvm

    int res = createJavaVM(
        &(this->pJvm),
        &(this->pEnv),
        &vm_args,
        pJdkPath);
    // delete options;
    if (res < 0)
    {
        DEBUG_FORMAT("Create Java VM error, code = %d", res);
        exit(-1);
    }
    //TS_LOG_INFO("Create Java VM successful! code = %d\n", res);
    this->jClassString = this->pEnv->FindClass("java/lang/String");
    this->jStringUTF8  = this->pEnv->NewStringUTF(CHARSET_UTF8);
    return true;
}

bool JniRuntime::start()
{
    char jdk[BUFFER_SIZE]  = {0};
    getAppJdkDir(jdk);
    return start(jdk);
}

jclass JniRuntime::load_java_class(const char* className)
{
    int len = (int)strlen(className) + 1;
    char *buf = (char*)ts_malloc(len);

    char *s = buf, *t = (char*)className, c;
    do
    {
        c = *t++;
        *s++ = (c == '.') ? '/' : c;
    } while (c != '\0');

    /*
    int length = strlen(buf)*2;
    wchar_t* temp = new wchar_t[length];
    MultiByteToWideChar(CP_UTF8,0,buf,-1,temp,length);
    //MessageBox(NULL,temp,TEXT("LoadClass测试"),MB_OK);     */
    jclass cls = ((JNIEnv*)pEnv)->FindClass(buf);
    free(buf);

    return cls;
}

jclass JniRuntime::find_main_class(const char* pMainClass, const char* pJarPath)
{
    jstring jMainClassName = NULL;
    char*   pMainClassName = NULL;

    jclass jStringClass = ((JNIEnv*)pEnv)->FindClass("java/lang/String");
    //jar
    if (strstr(pJarPath, DOT_JAR) != NULL)
    {
        jclass cls    = ((JNIEnv*)pEnv)->FindClass("java/util/jar/JarFile");
        jmethodID mid = ((JNIEnv*)pEnv)->GetMethodID(cls, "<init>","(Ljava/lang/String;)V");
#ifdef _WINDOWS
        // 支持中文路径
        jstring str   =  NewPlatformString(((JNIEnv*)pEnv),(char*)pJarPath);
#else
        jstring str   =  ((JNIEnv*)pEnv)->NewStringUTF(pJarPath);
#endif
        jobject jar   = ((JNIEnv*)pEnv)->NewObject(cls, mid, str);
        mid           = ((JNIEnv*)pEnv)->GetMethodID(cls, "getManifest","()Ljava/util/jar/Manifest;");
        jobject man   = ((JNIEnv*)pEnv)->CallObjectMethod(jar, mid);
        if (man == 0)
        {
            return NULL;
        }

        mid  = ((JNIEnv*)pEnv)->GetMethodID(((JNIEnv*)pEnv)->GetObjectClass(man), "getMainAttributes", "()Ljava/util/jar/Attributes;");
        jobject attr = ((JNIEnv*)pEnv)->CallObjectMethod(man, mid);
        if (attr != 0)
        {
            mid = ((JNIEnv*)pEnv)->GetMethodID(((JNIEnv*)pEnv)->GetObjectClass(attr),
                "getValue",    "(Ljava/lang/String;)Ljava/lang/String;");
            str = ((JNIEnv*)pEnv)->NewStringUTF("Main-Class");

            jMainClassName = (jstring)((JNIEnv*)pEnv)->CallObjectMethod(attr, mid, str);
            pMainClassName = (char *) ((JNIEnv*)pEnv)->GetStringUTFChars(jMainClassName, 0);
        }
    }
    jclass jMainClass = load_java_class(pMainClassName);
    if (jMainClassName != NULL)
    {
        ((JNIEnv*)pEnv)->ReleaseStringUTFChars(jMainClassName, pMainClassName);
    }
    return jMainClass;
}

bool JniRuntime::run_java_class(const jclass jMainClass, jobjectArray jArgs)
{
    // 寻找main方法
    jmethodID jMainID = ((JNIEnv*)pEnv)->GetStaticMethodID(jMainClass, "main",
                       "([Ljava/lang/String;)V");
    if (jMainID == NULL)
    {
        TS_LOG_INFO("Error:could not get the methodID of main...");
        return false;
    }

    // 调用main方法
    ((JNIEnv*)pEnv)->CallStaticVoidMethod(jMainClass, jMainID, jArgs);

    if (((JNIEnv*)pEnv)->ExceptionOccurred())
    {
        ((JNIEnv*)pEnv)->ExceptionDescribe();
        TS_LOG_INFO("Error:call main method, Exception occured...");
        return false;
    }
    return true;
}

jobjectArray JniRuntime::create_jargs(list<char*>& argList)
{
    if (argList.size() == 0)
    {
        return NULL;
    }

    jobjectArray args = ((JNIEnv*)pEnv)->NewObjectArray((jsize)argList.size(), this->jClassString, NULL);
    if (args == NULL)
    {
        return NULL;
    }

    int i=0;
    for (list<char*>::const_iterator iter=argList.begin(); iter != argList.end(); iter++)
    {
        //TIO_LOG_INFO("%d, %s", i, *iter);
        jstring jstr = ((JNIEnv*)pEnv)->NewStringUTF(*iter);
        if (jstr != NULL)
        {
            ((JNIEnv*)pEnv)->SetObjectArrayElement(args, i, jstr);
            i++;
        }
    }

    return args;
}

void  JniRuntime::release()
{
    if (this->nRefCount > 0 || this->pJvm == NULL)
    {
        return;
    }
    
    this->pJvm->DetachCurrentThread();
    this->pJvm->DestroyJavaVM();

    this->pJvm         = NULL;
    this->pEnv         = NULL;

    this->jClassString = NULL;
}

void JniRuntime::addJarsToParamter(char classpath[], const char* pJarPath)
{
    // 取得TSOffice以及Jre的路径并赋值给相应变量
    // 得到根目录
    char path[BUFFER_SIZE] = {0};

    getAppBinDir(path);
    getJarPathApp(path);
    sprintf(classpath, "-Djava.class.path=%s%s", path, OS_SEP); 

    memset(path, 0, BUFFER_SIZE);
    getAppPluginsDir(path);
    getJarsPath(classpath, path);

    //如果TIOffice.jar中的类，与jdk/lib/ext中的类，互相引用，那么就会提示找不到类。
    memset(path, 0, BUFFER_SIZE);
    getAppLibDir(path);

    getJarsPath(classpath, path);
}

jobjectArray JniRuntime::create_jvm_args(const int argc, const char** argv)
{
    return create_jvm_args(argc, argv, NULL);
}

static void add_arg_to_array(JNIEnv* pEnv, const char* arg, jobjectArray mainArgs, const int index)
{
    jstring jstr = pEnv->NewStringUTF(arg);
    if (jstr != NULL)
    {
        pEnv->SetObjectArrayElement(mainArgs, index, jstr);
    }
}

#ifdef _WINDOWS
static void add_arg_to_array_WINDOWS(JNIEnv* pEnv, const wchar_t* arg, jobjectArray mainArgs, const int index)
{
    jstring jstr = pEnv->NewString((const jchar*)(arg), (jsize)(wcslen(arg)) );
    if (jstr != NULL)
    {
        pEnv->SetObjectArrayElement(mainArgs, index, jstr);
    }
}
#endif

jobjectArray JniRuntime::create_jvm_args(const int argc, const char** argv, const char* extend)
{
    int count = argc;
    if (strlen(extend) > 0)
    {
        count++;
    }
    // 构建参数列表 
    if (count <= 1 || argv == NULL)
    {
        return NULL;
    }
    jobjectArray mainArgs = this->pEnv->NewObjectArray(count-1, this->jClassString, NULL);
    if (mainArgs == NULL)
    {
        return NULL;
    }

    int i=1;
    //argv中，第0是程序名，忽略。
    for (; i < argc; i++)
    {
#ifdef _WINDOWS
        add_arg_to_array_WINDOWS(this->pEnv, (wchar_t*)argv[i], mainArgs, i-1);
#else
        add_arg_to_array(this->pEnv, argv[i], mainArgs, i-1);
#endif
    }
    if (strlen(extend) > 0)
    {
        add_arg_to_array(this->pEnv, extend, mainArgs, argc-1);
    }

    return mainArgs;
}

//前提:cmdLine为非空字符串
jobjectArray JniRuntime::createJvmArgs(const char * cmdLine)
{
    jobjectArray mainArgs = this->pEnv->NewObjectArray(1, this->jClassString, NULL);
    if (mainArgs == NULL) 
        return NULL;
    //convertUTF8ToUnicode
#ifdef _WINDOWS
    wchar_t cmdLineW[1024]={0};
    if (convertUTF8ToUnicode((char *)cmdLine, cmdLineW, sizeof(cmdLineW)) == false) {
        return NULL;
    }
    jstring jstr=this->pEnv->NewString((const jchar *)cmdLineW, wcslen(cmdLineW));
#else
        jstring jstr = this->pEnv->NewStringUTF(cmdLine);
#endif
    pEnv->SetObjectArrayElement(mainArgs, 0, jstr);
    return mainArgs;
}

int   createJavaVM(JavaVM** ppJvm, JNIEnv** ppEnv, const JavaVMInitArgs* pArgs, const char* pJdkPath)
{
    InvocationFunctions ifn;
    ifn.CreateJavaVM             = NULL;
    ifn.GetDefaultJavaVMInitArgs = NULL;

    if (!LoadJavaVM(pJdkPath, &ifn))
    {
        DEBUG_TEXT("Error:load Java VM failed...");
        return -1;
    }

    if (ifn.CreateJavaVM(ppJvm, (void **)ppEnv, (void*)pArgs) != JNI_OK)  //-1
    {
        TS_LOG_TEXT("Error:create java VM failed...");
        return -1;
    }
    return 0;
}


 /*
 *  函数功能：装载虚拟机并取得"创建虚拟机"及"取得虚拟机默认的初始化参数"的入口    
 *  函数参数：jvmpath 虚拟机的路径
 *            ifn 返回的包含取得的入口的结构指针
 *  返回：TRUE  装载虚拟机成功
 *        FALSE 装载虚拟机失败
 */
static jboolean LoadJavaVM(const char* pJdkPath, InvocationFunctions *ifn)
{
    char jvmpath[1024] = {0};
    get_jvm_path(pJdkPath, jvmpath);
    //DEBUG_TEXT(pJdkPath);
    //DEBUG_TEXT(jvmpath);

#ifdef _WINDOWS
#define GET_FUNC_FROM_LIB GetProcAddress
    HINSTANCE handle = LoadLibraryExA(jvmpath, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
    DWORD err = GetLastError();
#else
#define GET_FUNC_FROM_LIB dlsym
    void* handle = dlopen(jvmpath, RTLD_NOW | RTLD_GLOBAL);
#endif

    if (handle == NULL)
    {
#ifdef _WINDOWS
        DEBUG_FORMAT("LoadLibrary() |||%s|||", getErrorString("LoadLibrary"));
#else
        DEBUG_FORMAT("dlopen() |||%s|||", dlerror());
#endif
        return JNI_FALSE;
    }
    //MessageBox(NULL,TEXT("LoadJavaVM获得函数地址前"),TEXT("测试"),MB_OK);
    // 取得函数地址
    ifn->CreateJavaVM             = (CreateJavaVM_t)            GET_FUNC_FROM_LIB(handle, "JNI_CreateJavaVM");
    if (ifn->CreateJavaVM == NULL) 
    {
        DEBUG_TEXT("ifn->CreateJavaVM == NULL");
        return JNI_FALSE;
    }

    //why = null ? 
    ifn->GetDefaultJavaVMInitArgs = (GetDefaultJavaVMInitArgs_t)GET_FUNC_FROM_LIB(handle, "JNI_GetDefaultJavaVMInitArgs");
    if (ifn->GetDefaultJavaVMInitArgs == NULL) 
    {
        DEBUG_TEXT("ifn->GetDefaultJavaVMInitArgs == NULL");
        //return JNI_FALSE;
    }
    return JNI_TRUE;
}


#ifdef _WINDOWS

int JniRuntime::getJarsPath(char* pBuffer, const char* pDir)
{
    WIN32_FIND_DATAA FindFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    char DirSpec[MAX_PATH];
    strcpy(DirSpec, pDir);
    strcat(DirSpec, OS_DIR_TEXT);
    strcat(DirSpec, STAR_DOT_JAR);

    hFind = FindFirstFileA(DirSpec, &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        FindClose(hFind);
        return -1;
    }

    do
    {
        if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0
            || strstr(FindFileData.cFileName, DOT_JAR) == NULL)
        {
            continue;
        }

        sprintf(pBuffer, "%s%s%s%s%s", pBuffer, pDir, OS_DIR_TEXT, FindFileData.cFileName, OS_SEP);
    } while(FindNextFileA(hFind, &FindFileData) != 0);

    
    FindClose(hFind);
    return 0;
}

int get_jvm_path(const char* pJdkPath, char* pBuffer)
{
    sprintf(pBuffer, "%s%sbin%sserver%s%s", pJdkPath, OS_DIR_TEXT,
        OS_DIR_TEXT, 
        OS_DIR_TEXT, JVM_FILE);

    return 0;
}

static bool convertUTF8ToUnicode(char * utf8StrIn, wchar_t * unicodeBufferOut, int sizeOfBuffer)
{
    int sizeRequired = MultiByteToWideChar(CP_UTF8, 0, utf8StrIn, -1, 0, 0);
    if (sizeRequired <= 0 || sizeRequired >= (sizeOfBuffer-1))
        return false;
    
    if(MultiByteToWideChar(CP_UTF8, 0, utf8StrIn, -1, unicodeBufferOut, sizeRequired) == 0)
        return false;

    return true;
}

static jstring NewPlatformString(JNIEnv *env, char *s)
{
    if(s == NULL)
    {
        return NULL;
    }
    int size = MultiByteToWideChar(CP_ACP, 0, s, -1, NULL, 0);
    wchar_t *widechars = new wchar_t[size];
    MultiByteToWideChar(CP_ACP,0,s,-1,widechars,size);
    jstring jstr = env->NewString((const jchar *)widechars, wcslen(widechars));
    delete[] widechars;
    return jstr;
}
#else

int find_file(const char* pJdkPath, const char* pFile, char* pBuffer)
{
    char command[BUFFER_SIZE] = {0};

    memset(command, 0, BUFFER_SIZE);
    sprintf(command, "find %s -name %s", pJdkPath, pFile);
    os_executeProcess(command, pBuffer);

    if (strstr(pBuffer, pFile) == NULL)
    {
        return -1;
    }

    //on kylin/phytium, this is must. It there better code ? 
    int lastPos = strlen(pBuffer)-1;
    if (pBuffer[lastPos] == '\n')
    {
        pBuffer[lastPos] = 0;
    }

    return 0;
}


int get_jvm_path(const char* pJdkPath, char* pBuffer)
{
    char command[BUFFER_SIZE] = {0};
    return find_file(pJdkPath, JVM_FILE, pBuffer);
}

int  JniRuntime::getJarsPath(char* pBuffer, const char* pJarPath)
{
    DIR* pDir = opendir(pJarPath);
    if (pDir == NULL)
    {
        return 0;
    }

    struct dirent* ptr;
    while ((ptr = readdir(pDir)) != NULL)
    {
        char* pName = ptr->d_name;
        if (ptr->d_type == DT_DIR || pName == NULL || strstr(pName, DOT_JAR) == NULL)
        {
            continue;
        }
        sprintf(pBuffer, "%s%s%s%s%s", pBuffer, pJarPath, OS_DIR_TEXT, pName, OS_SEP);
    }
    return 0;
}


#endif

