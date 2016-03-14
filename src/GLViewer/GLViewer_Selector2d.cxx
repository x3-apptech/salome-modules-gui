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
// File:      GLViewer_Selector2d.cxx
// Created:   November, 2004
//#include <GLViewerAfx.h>
//
#include "GLViewer_Selector2d.h"
#include "GLViewer_Viewer2d.h"
#include "GLViewer_Context.h"
#include "GLViewer_ViewPort.h"
#include "GLViewer_ViewFrame.h"

/*!
  Constructor
*/
GLViewer_Selector2d::GLViewer_Selector2d( GLViewer_Viewer2d* v2d, GLViewer_Context* glc ) :
GLViewer_Selector( v2d ),
myGLContext( glc )
{   
//  myGLContext->SetHighlightColor( Quantity_NOC_CYAN1 );
//  myGLContext->SetSelectionColor( Quantity_NOC_RED );
}

/*!
  Destructor
*/
GLViewer_Selector2d::~GLViewer_Selector2d()
{
}

/*!
  Changes hilight color of context
  \param color - new hilight color
*/
void GLViewer_Selector2d::setHilightColor( Quantity_NameOfColor color )
{
  myGLContext->SetHighlightColor( color );
}

/*!
  Changes selection color of context
  \param color - new selection color
*/
void GLViewer_Selector2d::setSelectColor( Quantity_NameOfColor color )
{
  myGLContext->SetSelectionColor( color );
}

/*!
  Detects object at point
  \param x, y - point co-ordinates
*/
void GLViewer_Selector2d::detect( int x, int y )
{
  //cout << "GLViewer_Selector2d    : detect ( " << x << " , " << y << " )" << endl;
  if ( myLocked || !myGLContext || !myViewer || !myViewer->getActiveView() || 
       myViewer->getSelectionMode() == GLViewer_Viewer::NoSelection )
    return;

  GLViewer_ViewPort* vp = myViewer->getActiveView()->getViewPort();
  if( !vp->inherits( "GLViewer_ViewPort2d" ) )
    return;

  myGLContext->MoveTo( x, y );
}

/*!
  Undetects all objects
*/
void GLViewer_Selector2d::undetectAll()
{
  if ( myLocked || !myGLContext || !myViewer || !myViewer->getActiveView() || 
       myViewer->getSelectionMode() == GLViewer_Viewer::NoSelection )
    return;

  GLViewer_ViewPort* vp = myViewer->getActiveView()->getViewPort();
  if( !vp->inherits( "GLViewer_ViewPort2d" ) )
    return;

  myGLContext->clearHighlighted( true );
}

/*!
  Selects previously hilighted objects
  \param append - append objects to selection
*/
void GLViewer_Selector2d::select( bool append )
{
  //cout << "GLViewer_Selector2d    : select ( " << (int)append << " )" << endl;
  GLViewer_Viewer::SelectionMode selMode = myViewer->getSelectionMode();
  if ( myLocked || !myGLContext || !myViewer || !myViewer->getActiveView() ||
       selMode == GLViewer_Viewer::NoSelection ) 
    return;

  int selBefore = numSelected();
  if ( selBefore && append && selMode != GLViewer_Viewer::Multiple )
    return;    

  GLViewer_ViewPort* vp = myViewer->getActiveView()->getViewPort();
  if( !vp->inherits( "GLViewer_ViewPort2d" ) )
      return;

  int status = myGLContext->Select( append );
  checkSelection( selBefore, append, status );
}

/*!
  Selects objects in rectangle
  \param selRect - selection rectangle
  \param append - append objects to selection
*/
void GLViewer_Selector2d::select( const QRect& selRect, bool append )
{
    GLViewer_Viewer::SelectionMode selMode = myViewer->getSelectionMode();
    if ( myLocked || !myGLContext || !myViewer || !myViewer->getActiveView() ||
       selMode == GLViewer_Viewer::NoSelection ) 
    return;

    int selBefore = numSelected();
    if ( selBefore && append && selMode != GLViewer_Viewer::Multiple )
        return;    

    GLViewer_ViewPort* vp = myViewer->getActiveView()->getViewPort();
    if( !vp->inherits( "GLViewer_ViewPort2d" ) )
        return;

    int aStatus = myGLContext->SelectByRect( selRect, append );
    checkSelection( selBefore, append, aStatus );
}

/*!
  Unselects all objects
*/
void GLViewer_Selector2d::unselectAll()
{
  if ( myLocked || !myViewer ) 
    return;

//  bool updateViewer = true;
  bool hadSelection = ( numSelected() > 0 );
     
//     bool lcOpen = ( myAISContext->IndexOfCurrentLocal() != -1 );
//     lcOpen ? myAISContext->ClearSelected( updateViewer ) :    
//              myAISContext->ClearCurrent( updateViewer );     
  if ( hadSelection ) emit selSelectionCancel();
}

/*!
  Checks selection state and emits  'selSelectionDone' or 'selSelectionCancel'     
  Should be called by after non-interactive selection.
*/
void GLViewer_Selector2d::checkSelection( int selBefore, bool append, int aStatus )
{
    int selAfter = numSelected();
    if ( selBefore > 0 && selAfter < 1 )     
        emit selSelectionCancel();
    else if ( selAfter > 0 )
    {
        switch( aStatus )
        {
        case SS_LocalChanged:
            emit selSelectionDone( selAfter > 1, SCS_Local );
            break;
        case SS_GlobalChanged:
            emit selSelectionDone( selAfter > 1, SCS_Global );
            break;
        }
    }
}

/*!
  \return number of selected objects
*/
int GLViewer_Selector2d::numSelected() const
{
  return myGLContext->NbSelected();
}

