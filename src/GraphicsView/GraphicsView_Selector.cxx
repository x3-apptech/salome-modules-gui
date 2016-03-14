// Copyright (C) 2013-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "GraphicsView_Selector.h"
#include "GraphicsView_Viewer.h"
#include "GraphicsView_ViewPort.h"
#include "GraphicsView_ViewFrame.h"

int GraphicsView_Selector::appendKey = Qt::ShiftModifier;

//=======================================================================
// Name    : GraphicsView_Selector
// Purpose : Constructor
//=======================================================================
GraphicsView_Selector::GraphicsView_Selector( GraphicsView_Viewer* theViewer )
: QObject( 0 ),
  myViewer( theViewer ),
  myLocked( false )
{   
}

//=======================================================================
// Name    : GraphicsView_Selector
// Purpose : Destructor
//=======================================================================
GraphicsView_Selector::~GraphicsView_Selector()
{
}

//================================================================
// Function : detect
// Purpose  : 
//================================================================
void GraphicsView_Selector::detect( double x, double y )
{
  if ( myLocked )
    return;

  if( GraphicsView_ViewPort* aViewPort = myViewer->getActiveViewPort() )
    aViewPort->highlight( x, y );
}

//================================================================
// Function : undetectAll
// Purpose  : 
//================================================================
void GraphicsView_Selector::undetectAll()
{
  if ( myLocked )
    return;

  if( GraphicsView_ViewPort* aViewPort = myViewer->getActiveViewPort() )
    aViewPort->clearHighlighted();
}

//================================================================
// Function : select
// Purpose  : 
//================================================================
void GraphicsView_Selector::select( const QRectF& selRect, bool append )
{
  if ( myLocked ) 
    return;

  int selBefore = numSelected();
  if( GraphicsView_ViewPort* aViewPort = myViewer->getActiveViewPort() )
  {
    int aStatus = aViewPort->select( selRect, append );
    checkSelection( selBefore, append, aStatus );
  }
}

//================================================================
// Function : unselectAll
// Purpose  : 
//================================================================
void GraphicsView_Selector::unselectAll()
{
  if ( myLocked ) 
    return;

  if ( numSelected() > 0 )
    emit selSelectionCancel();
}

//================================================================
// Function : checkSelection
// Purpose  : 
//================================================================
void GraphicsView_Selector::checkSelection( int selBefore, bool append, int theStatus )
{
  int selAfter = numSelected();
  if ( selBefore > 0 && selAfter < 1 )     
    emit selSelectionCancel();
  else if ( selAfter > 0 )
  {
    switch( theStatus )
    {
      case GVSS_LocalChanged:
        emit selSelectionDone( GVSCS_Local );
        break;
      case GVSS_GlobalChanged:
        emit selSelectionDone( GVSCS_Global );
        break;
    }
  }
}

//================================================================
// Function : numSelected
// Purpose  : 
//================================================================
int GraphicsView_Selector::numSelected() const
{
  if( GraphicsView_ViewPort* aViewPort = myViewer->getActiveViewPort() )
    return aViewPort->nbSelected();
  return 0;
}

