// File:      GLViewer_Selector.cxx
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

/****************************************************************************
**  Class:   GLViewer_Selector
**  Descr:   Base class for object selection in QAD-based application
**  Module:  GLViewer
**  Created: UI team, 22.09.00
*****************************************************************************/

#include "GLViewer_Selector.h"

#include "GLViewer_Viewer.h"

int GLViewer_Selector::apppendKey = Qt::ShiftButton;

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
