#ifdef WNT
#ifdef VTKVIEWER_EXPORTS
#define VTKVIEWER_EXPORT __declspec( dllexport )
#else
#define VTKVIEWER_EXPORT __declspec( dllimport )
#endif
#else
#define VTKVIEWER_EXPORT
#endif
