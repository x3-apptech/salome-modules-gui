#if !defined ( _PYTHONCONSOLE_H )
#define _PYTHONCONSOLE_H

// ========================================================
// set dllexport type for Win platform 
#ifdef WNT

#ifdef PYTHONCONSOLE_EXPORTS
#define PYCONSOLE_EXPORT __declspec(dllexport)
#else
#define PYCONSOLE_EXPORT __declspec(dllimport)
#endif

#else   // WNT

#define PYCONSOLE_EXPORT

#endif  // WNT

// ========================================================
// little trick - we do not have debug python libraries
#ifdef _DEBUG

#undef _DEBUG
#include <Python.h>
#define _DEBUG

#else  // _DEBUG

#include <Python.h>

#endif // _DEBUG

// ========================================================
// avoid warning messages
#ifdef WNT
#pragma warning (disable : 4786)
#pragma warning (disable : 4251)
#endif

#endif // _PYTHONCONSOLE_H
