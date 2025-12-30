#include <stdio.h>
#include <string.h>
#include "fw_config.h"
#include "app_message.h"

void getAppParam(char* appParam, const char* szApplication)
{    
	if (appParam != NULL && szApplication != NULL && strlen(szApplication) > 0)
	{
		sprintf(appParam, "/%s=%s", APPLICATION_KEY, szApplication);
	}
}

void getAppNameFromParam(char* szApplication, const char* appParam)
{
    if(appParam != NULL && strstr(appParam, APPLICATION_KEY))
    {
		const char* pApp = strstr(appParam, "=");
    
		if(pApp)
			sprintf(szApplication,"%s", pApp+1);
    }
}

