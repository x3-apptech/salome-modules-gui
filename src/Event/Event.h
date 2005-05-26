// File:      SalomeApp.h
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004


// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SalomeApp_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SalomeApp_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef WNT

#ifdef EVENT_EXPORTS
#define EVENT_EXPORT __declspec(dllexport)
#else
#define EVENT_EXPORT __declspec(dllimport)
#endif

#else
#define EVENT_EXPORT
#endif               //WNT

#define APP_VERSION "0.1"

#if defined WNT
#pragma warning ( disable: 4251 )
#endif

