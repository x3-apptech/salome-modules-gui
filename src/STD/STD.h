#ifndef STD_H
#define STD_H

#if defined STD_EXPORTS
#if defined WNT
#define STD_EXPORT		__declspec( dllexport )
#else
#define STD_EXPORT
#endif
#else
#if defined WNT
#define STD_EXPORT		__declspec( dllimport )
#else
#define STD_EXPORT
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
#define STD_VERIFY(x)				(assert(x))
#define STD_ASSERT(x)	            (assert(x))
#else
#define STD_VERIFY(x)				(x)
#define STD_ASSERT(x)
#endif


#endif
