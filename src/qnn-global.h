#ifndef QNN_GLOBAL_H
#define QNN_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QNN_LIBRARY)
#  define QNNSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QNNSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QNN_GLOBAL_H
