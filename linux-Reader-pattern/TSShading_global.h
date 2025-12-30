#ifndef TSSHADING_GLOBAL_H
#define TSSHADING_GLOBAL_H

#include <QtCore/qglobal.h>
#include "jni.h"

#if defined(TSSHADING_LIBRARY)
#  define TSSHADING_EXPORT Q_DECL_EXPORT
#else
#  define TSSHADING_EXPORT Q_DECL_IMPORT
#endif

#endif // TSSHADING_GLOBAL_H
