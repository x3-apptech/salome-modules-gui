#ifdef WNT
#ifdef OCCVIEWER_EXPORTS
#define OCCVIEWER_EXPORT __declspec(dllexport)
#else
#define OCCVIEWER_EXPORT __declspec(dllimport)
#endif
#else
#define OCCVIEWER_EXPORT
#endif

#if defined WNT
#pragma warning ( disable: 4251 )
#endif
