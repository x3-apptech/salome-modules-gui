// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

//  Author : OPEN CASCADE
// File:      GLViewer_Defs.h
// Created:   March, 2005
//
#ifndef GLVIEWER_DEFS_H
#define GLVIEWER_DEFS_H

#include <QMap>
#include <QList>

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
  DTF_TEXTURE          = 0,
  DTF_TEXTURE_SCALABLE = 1,
  DTF_BITMAP           = 2
};

enum SelectionStatus
{
    SS_Invalid,
    SS_LocalChanged,
    SS_GlobalChanged,
    SS_NoChanged
};

typedef QMap<GLViewer_Object*,int> ObjectMap;
typedef QList<GLViewer_Object*> ObjList;

#define SEGMENTS   32
#define PI         3.14159265359
#define STEP       ( float )( 2 * PI / SEGMENTS )

#endif// GLVIEWER_DEFS_H
