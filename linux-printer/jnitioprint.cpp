#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <jni.h>
#include <jni_md.h>

int TIO_Print_GetNamesOfPrinter(std::vector<std::string> & namesOut);
int TIO_Print_PrintPDFFile(const char * printName, const char * pdfFilePath);
static char * getStringFromJString(JNIEnv *env, jstring jstr);

extern "C" JNIEXPORT jobjectArray JNICALL Java_tso_tnative_TLinuxPrinter_getPrinterNames(JNIEnv * env, jclass) {
    std::vector<std::string> names;
    TIO_Print_GetNamesOfPrinter(names);
    
    int len = names.size();
    if (len == 0) {
        return NULL;
    }

    jobjectArray nameArray = env->NewObjectArray(len, env->FindClass("java/lang/String"), NULL);
    for (int i=0; i<len; i++){
        jstring nameJStr = env->NewStringUTF(names[i].c_str());
    	env->SetObjectArrayElement(nameArray, i, nameJStr);
    }

    return nameArray;
}

/*
 * Class:     tso_tnative_TLinuxPrinter
 * Method:    printPDFFile
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
extern "C" JNIEXPORT jint JNICALL Java_tso_tnative_TLinuxPrinter_printPDFFile(JNIEnv * env, jclass, jstring printerNameJStr, jstring pdfFilePathJStr){
	char * printerName = getStringFromJString(env, printerNameJStr);
	char * filePath = getStringFromJString(env, pdfFilePathJStr);
	int res = TIO_Print_PrintPDFFile(printerName, filePath);
	
	free(printerName);
	free(filePath);	
	return res;
}

char * getStringFromJString(JNIEnv *env, jstring jstr){
	const char *str	= env->GetStringUTFChars(jstr, 0);
	if (str == 0) {
		env->ReleaseStringUTFChars(jstr, str);	
		return 0;	
	}

	int len = strlen(str);
	if (len == 0) {
		env->ReleaseStringUTFChars(jstr, str);
		return 0;	
	}	

	char * strBuffer = (char *)malloc(len+1);
	memset(strBuffer, 0, len+1); 
	strcpy(strBuffer,str);

    	env->ReleaseStringUTFChars(jstr, str);
	return strBuffer;
}
