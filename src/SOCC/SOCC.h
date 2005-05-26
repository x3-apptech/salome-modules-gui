#ifdef WNT
#ifdef SOCC_EXPORTS
#define SOCC_EXPORT __declspec(dllexport)
#else
#define SOCC_EXPORT __declspec(dllimport)
#endif
#else
#define SOCC_EXPORT
#endif

#if defined WNT
#pragma warning ( disable: 4251 )
#endif
