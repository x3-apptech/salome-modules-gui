#ifndef OB_H
#define OB_H

#if defined OB_EXPORTS
#if defined WNT
#define OB_EXPORT __declspec( dllexport )
#else
#define OB_EXPORT
#endif
#else
#if defined WNT
#define OB_EXPORT __declspec( dllimport )
#else
#define OB_EXPORT
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

#endif
