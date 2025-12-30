
#ifndef __NPPLUGIN_SDK_H__
#define __NPPLUGIN_SDK_H__

#include <string>
#include <sstream>
#include <memory.h>

#include "npapi.h"
#include "npfunctions.h"
#include "npruntime.h"

#define PRUint32 unsigned int
#define PRUint8  unsigned char


typedef enum  {
  DM_DEFAULT,
  DM_SOLID_COLOR
} DrawMode;

typedef enum {
  FUNCTION_NONE,
  FUNCTION_NPP_GETURL,
  FUNCTION_NPP_GETURLNOTIFY,
  FUNCTION_NPP_POSTURL,
  FUNCTION_NPP_POSTURLNOTIFY,
  FUNCTION_NPP_NEWSTREAM,
  FUNCTION_NPP_WRITEREADY,
  FUNCTION_NPP_WRITE,
  FUNCTION_NPP_DESTROYSTREAM,
  FUNCTION_NPP_WRITE_RPC
} TestFunction;

typedef enum {
  ACTIVATION_STATE_UNKNOWN,
  ACTIVATION_STATE_ACTIVATED,
  ACTIVATION_STATE_DEACTIVATED
} ActivationState;

typedef struct FunctionTable {
  TestFunction funcId;
  const char* funcName;
} FunctionTable;

typedef enum {
  POSTMODE_FRAME,
  POSTMODE_STREAM
} PostMode;

typedef struct TestNPObject : NPObject {
  NPP npp;
  DrawMode drawMode;
  PRUint32 drawColor; // 0xAARRGGBB
} TestNPObject;

typedef struct _PlatformData PlatformData;

typedef struct TestRange : NPByteRange {
  bool waiting;
} TestRange;

typedef struct InstanceData {
  NPP npp;
  NPWindow window;
  TestNPObject* scriptableObject;
  PlatformData* platformData;
  int32_t instanceCountWatchGeneration;
  bool lastReportedPrivateModeState;
  bool hasWidget;
  bool npnNewStream;
  bool throwOnNextInvoke;
  bool runScriptOnPaint;
  uint32_t timerID[2];
  bool timerTestResult;
  bool asyncCallbackResult;
  bool invalidateDuringPaint;
  int32_t winX;
  int32_t winY;
  int32_t lastMouseX;
  int32_t lastMouseY;
  int32_t widthAtLastPaint;
  int32_t paintCount;
  int32_t writeCount;
  int32_t writeReadyCount;
  int32_t asyncTestPhase;
  TestFunction testFunction;
  TestFunction functionToFail;
  NPError failureCode;
  NPObject* callOnDestroy;
  PostMode postMode;
  std::string testUrl;
  std::string frame;
  std::string timerTestScriptCallback;
  std::string asyncTestScriptCallback;
  std::ostringstream err;
  uint16_t streamMode;
  int32_t streamChunkSize;
  int32_t streamBufSize;
  int32_t fileBufSize;
  TestRange* testrange;
  void* streamBuf;
  void* fileBuf;
  bool crashOnDestroy;
  bool cleanupWidget;
  ActivationState topLevelWindowActivationState;
  int32_t topLevelWindowActivationEventCount;
  ActivationState focusState;
  int32_t focusEventCount;
  int32_t eventModel;
  bool closeStream;

  //不包含插件类，更方便一些。
  void* pNPPluginApp;

} InstanceData;

void notifyDidPaint(InstanceData* instanceData);


// {{{{{{{{{{ 初始化常量

static bool sIdentifiersInitialized = false;

static const NPUTF8* sPluginPropertyIdentifierNames[] = {
  "propertyAndMethod"
};

#define ARRAY_LENGTH(a) (sizeof(a)/sizeof(a[0]))


static NPIdentifier sPluginPropertyIdentifiers[ARRAY_LENGTH(sPluginPropertyIdentifierNames)];
static NPVariant    sPluginPropertyValues     [ARRAY_LENGTH(sPluginPropertyIdentifierNames)];

// }}}}}}}}}}

// {{{{{{{{{{ 需要插件实现的几个函数。
// 这几个函数实现后，在SDK中调用。所以放在这里。

// 插件信息
const char* getNpPluginName();
const char* getNpPluginVersion();
const char* getNpPluginDescription();

void      clearIdentifiers();
void initializeIdentifiers();

// }}}}}}}}}} 需要插件实现的几个函数。

#endif // __NPPLUGIN_SDK_H__
