#ifndef CAF_H
#define CAF_H

#if defined CAF_EXPORTS
#if defined WNT
#define CAF_EXPORT __declspec( dllexport )
#else
#define CAF_EXPORT
#endif
#else
#if defined WNT
#define CAF_EXPORT __declspec( dllimport )
#else
#define CAF_EXPORT
#endif
#endif

#if defined SOLARIS
#define bool  int
#define false 0
#define true  1
#endif

#if defined WNT
#pragma warning ( disable: 4251 )
#endif

#if defined ( _DEBUG ) || defined ( DEBUG )
#include <assert.h>
#define CAF_VERIFY(x) assert(x);
#define CAF_ASSERT(x) assert(x);
#else
#define CAF_VERIFY(x) x
#define CAF_ASSERT(x)
#endif

#endif
