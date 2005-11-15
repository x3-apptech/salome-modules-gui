// File:      LightApp.h
// Created:   June, 2005
// Author:    OCC team
// Copyright (C) CEA 2005


// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LightApp_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LightApp_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef WNT

#ifdef LIGHTAPP_EXPORTS
#define LIGHTAPP_EXPORT __declspec(dllexport)
#else
#define LIGHTAPP_EXPORT __declspec(dllimport)
#endif

#pragma warning ( disable:4251 )
#pragma warning ( disable:4786 )
#pragma warning ( disable:4503 )

#else
#define LIGHTAPP_EXPORT
#endif               //WNT

#define APP_VERSION "0.1"
