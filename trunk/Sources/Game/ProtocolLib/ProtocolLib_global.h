#ifndef PROTOCOLLIB_GLOBAL_H
#define PROTOCOLLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PROTOCOLLIB_LIBRARY)
#  define PROTOCOLLIBSHARED_EXPORT Q_DECL_EXPORT
#else
#  define PROTOCOLLIBSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // PROTOCOLLIB_GLOBAL_H