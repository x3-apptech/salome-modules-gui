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
// File:      GLViewer_Selector.cxx
// Created:   November, 2004
//#include <GLViewerAfx.h>
//
#include "GLViewer_Selector.h"

#include "GLViewer_Viewer.h"

int GLViewer_Selector::apppendKey = Qt::ShiftModifier;

/*!
    Constructor
*/
GLViewer_Selector::GLViewer_Selector( GLViewer_Viewer* viewer )
: QObject( 0 ),
myViewer( viewer ),
myLocked( false ),
myMinRectSize( 1, 1 )
{
}

/*!
    Destructor
*/
GLViewer_Selector::~GLViewer_Selector()
{
}

/*!
    Sets the min size of rectangle to treat it as a rectangle for multiple
    selection( sensitivity ). If a rectangle size is less than that min size,
    the right-bottom point of the rectangle will be used for single selection.
    The default min size is ( 1,1 ). [ public ]
*/
void GLViewer_Selector::setMinRectSize( const QSize& minSize )
{
    myMinRectSize = minSize;
}

/*!
    Locks / unlocks the selector. If locked, nothing can be selected
    regadless of the selection mode. [ public ]
*/
void GLViewer_Selector::lock( bool locked )
{
    myLocked = locked;
}
