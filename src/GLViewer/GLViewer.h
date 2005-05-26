// File:      GLViewer.h
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

// Macro for exports
#ifdef WNT

#ifdef GLVIEWER_EXPORTS
#define GLVIEWER_EXPORT __declspec(dllexport)
#else
#define GLVIEWER_EXPORT __declspec(dllimport)
#endif
#else
#define GLVIEWER_EXPORT
#endif // WNT

#ifndef PI
#define PI         3.14159265359
#endif
