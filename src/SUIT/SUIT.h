#ifndef SUIT_H
#define SUIT_H

#if defined SUIT_EXPORTS
#if defined WNT
#define SUIT_EXPORT __declspec( dllexport )
#else
#define SUIT_EXPORT
#endif
#else
#if defined WNT
#define SUIT_EXPORT __declspec( dllimport )
#else
#define SUIT_EXPORT
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
#define SUIT_VERIFY(x) assert( x );
#define SUIT_ASSERT(x) assert( x );
#else
#define SUIT_VERIFY(x) x
#define SUIT_ASSERT(x)
#endif

#endif
