#if !defined ( SALOME_PYQT_GUI_H )
#define SALOME_PYQT_GUI_H

// ========================================================
// set dllexport type for Win platform 
#ifdef WNT

#ifdef SALOME_PYQT_EXPORTS
#define SALOME_PYQT_EXPORT __declspec(dllexport)
#else
#define SALOME_PYQT_EXPORT __declspec(dllimport)
#endif

#else   // WNT

#define SALOME_PYQT_EXPORT

#endif  // WNT

// ========================================================
// avoid warning messages
#ifdef WNT
#pragma warning (disable : 4786)
#pragma warning (disable : 4251)
#endif

#endif // SALOME_PYQT_GUI_H
