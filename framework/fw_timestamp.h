#ifndef __FW_TIMESTAMP_H__
#define __FW_TIMESTAMP_H__

//纯独立


#ifndef TIMESTAMP
#define TIMESTAMP()                 timestampMilliSeconds()
#endif

#ifdef __cplusplus
extern "C" {
#endif

long      timestampValue();
long      timestampMilliSeconds();
long long timestampMicroSeconds();
int       timestampText(char* text);


#ifdef __cplusplus
}
#endif

#endif //__FW_TIMESTAMP_H__

