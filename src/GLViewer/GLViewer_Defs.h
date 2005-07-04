// File:      GLViewer_Defs.h
// Created:   March, 2005
// Author:    OCC team
// Copyright (C) CEA 2005

#ifndef GLVIEWER_DEFS_H
#define GLVIEWER_DEFS_H

#include <qmap.h>
#include <qvaluelist.h>

class GLViewer_Object;
//Selection staus 
enum SelectionChangeStatus
{
    SCS_Invalid,
    SCS_Local,
    SCS_Global
};

//! Fit Selection Rectangle
static const int      SELECTION_RECT_GAP = 50;

//! Display Text Format of Objects
enum DisplayTextFormat
{
  DTF_TEXTURE   = 0,
  DTF_BITMAP    = 1
};

enum SelectionStatus
{
    SS_Invalid,
    SS_LocalChanged,
    SS_GlobalChanged,
    SS_NoChanged
};

typedef QMap<GLViewer_Object*,int> ObjectMap;
typedef QValueList<GLViewer_Object*> ObjList;

#define SEGMENTS   32
#define PI         3.14159265359
#define STEP       ( float )( 2 * PI / SEGMENTS )

#endif// GLVIEWER_DEFS_H
