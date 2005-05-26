#ifdef WNT
#ifdef SPLOT2D_EXPORTS
#define SPLOT2D_EXPORT __declspec(dllexport)
#else
#define SPLOT2D_EXPORT __declspec(dllimport)
#endif
#else
#define SPLOT2D_EXPORT
#endif

#if defined WNT
#pragma warning ( disable: 4251 )
#endif
