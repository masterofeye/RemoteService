#ifndef FLASHPLU_GLOBAL_H
#define FLASHPLU_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef FLASHPLU_LIB
# define FLASHPLU_EXPORT Q_DECL_EXPORT
#else
# define FLASHPLU_EXPORT Q_DECL_IMPORT
#endif

#endif // FLASHPLU_GLOBAL_H
