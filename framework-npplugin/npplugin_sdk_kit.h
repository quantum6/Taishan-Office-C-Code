
#ifndef __NPPLUGIN_SDK_KIT_H__
#define __NPPLUGIN_SDK_KIT_H__

#include "npplugin_sdk.h"

NPUTF8* createCStringFromNPVariant(const NPVariant* variant);

NPIdentifier variantToIdentifier(NPVariant variant);
NPIdentifier stringVariantToIdentifier(NPVariant variant);
NPIdentifier int32VariantToIdentifier(NPVariant variant);
NPIdentifier doubleVariantToIdentifier(NPVariant variant);

PRUint32 parseHexColor(const char* color, int len);

#endif // __NPPLUGIN_SDK_KIT_H__
