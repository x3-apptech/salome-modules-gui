#ifndef CAM_H
#define CAM_H

#if defined CAM_EXPORTS
#if defined WNT
#define CAM_EXPORT __declspec( dllexport )
#else
#define CAM_EXPORT
#endif
#else
#if defined WNT
#define CAM_EXPORT __declspec( dllimport )
#else
#define CAM_EXPORT
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
