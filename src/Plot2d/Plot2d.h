#ifdef WNT
#ifdef PLOT2D_EXPORTS
#define PLOT2D_EXPORT __declspec(dllexport)
#else
#define PLOT2D_EXPORT __declspec(dllimport)
#endif
#else
#define PLOT2D_EXPORT
#endif

#if defined WNT
#pragma warning ( disable: 4251 )
#endif
