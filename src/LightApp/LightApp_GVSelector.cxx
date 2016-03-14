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

#include "LightApp_GVSelector.h"

#include "LightApp_DataOwner.h"

#include <GraphicsView_Object.h>
#include <GraphicsView_ViewPort.h>
#include <GraphicsView_Viewer.h>

LightApp_GVSelector::LightApp_GVSelector( GraphicsView_Viewer* theViewer,
                                          SUIT_SelectionMgr* theSelMgr )
: SUIT_Selector( theSelMgr, theViewer ),
  myViewer( theViewer )
{
  connect( theViewer, SIGNAL( selectionChanged( GV_SelectionChangeStatus ) ),
          this, SLOT( onSelectionChanged( GV_SelectionChangeStatus ) ) );
}

LightApp_GVSelector::~LightApp_GVSelector()
{
}

QString LightApp_GVSelector::type() const
{
  return GraphicsView_Viewer::Type();
}

void LightApp_GVSelector::getSelection( SUIT_DataOwnerPtrList& theList ) const
{
  if( GraphicsView_ViewPort* aViewport = myViewer->getActiveViewPort() )
    for( aViewport->initSelected(); aViewport->moreSelected(); aViewport->nextSelected() )
      theList.append( new LightApp_DataOwner( aViewport->selectedObject()->getName() ) );
}

void LightApp_GVSelector::setSelection( const SUIT_DataOwnerPtrList& theList )
{
}

void LightApp_GVSelector::onSelectionChanged( GV_SelectionChangeStatus )
{
  selectionChanged();
}
