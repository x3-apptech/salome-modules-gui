#if !defined ( _PYINTERP_H )
#define _PYINTERP_H

// ========================================================
// set dllexport type for Win platform 
#ifdef WNT

#ifdef PYINTERP_EXPORTS
#define PYINTERP_EXPORT __declspec(dllexport)
#else
#define PYINTERP_EXPORT __declspec(dllimport)
#endif

#else   // WNT

#define PYINTERP_EXPORT

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

#endif // _PYINTERP_H
