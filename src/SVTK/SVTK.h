#ifdef WNT
#ifdef SVTK_EXPORTS
#define SVTK_EXPORT __declspec(dllexport)
#else
#define SVTK_EXPORT __declspec(dllimport)
#endif
#else
#define SVTK_EXPORT
#endif

#if defined WNT
#pragma warning ( disable: 4251 )
#endif
