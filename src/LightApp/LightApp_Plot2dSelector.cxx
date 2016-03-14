// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "LightApp_Plot2dSelector.h"

#include "LightApp_DataOwner.h"
#include "LightApp_DataObject.h"
#include "LightApp_Application.h"
#include "SUIT_SelectionMgr.h"

#ifndef DISABLE_SALOMEOBJECT
  #include <SALOME_ListIO.hxx>
  #ifndef DISABLE_PLOT2DVIEWER
    #include <SPlot2d_ViewModel.h>
  #endif
#endif

/*!
  Constructor
*/
LightApp_Plot2dSelector::LightApp_Plot2dSelector( Plot2d_Viewer* v, SUIT_SelectionMgr* mgr )
  : SUIT_Selector( mgr, v ),
    myViewer(v)
{
  if ( v )
    connect( v, SIGNAL( legendSelected( const QString& ) ), this, SLOT( onSelectionChanged( const QString& ) ) );
    connect( v, SIGNAL( clearSelected() ), this, SLOT( onClearSelected( ) ) );
}

/*!
  Destructor
*/
LightApp_Plot2dSelector::~LightApp_Plot2dSelector()
{
}

/*!
  Gets selection.
*/
void LightApp_Plot2dSelector::getSelection( SUIT_DataOwnerPtrList& theList ) const
{
  if( !myCurEntry.isNull() ) {
#ifndef DISABLE_SALOMEOBJECT
    theList.append( new LightApp_DataOwner( new SALOME_InteractiveObject(qPrintable(myCurEntry),"","") ) );
#else
    theList.append( new LightApp_DataOwner( myCurEntry ) );
#endif
  }
}

/*!Sets selection.*/
void LightApp_Plot2dSelector::setSelection( const SUIT_DataOwnerPtrList& theList )
{
#ifndef DISABLE_SALOMEOBJECT
  SALOME_ListIO anIOList;
  for ( SUIT_DataOwnerPtrList::const_iterator it = theList.begin(); it != theList.end(); ++it ) {
    const LightApp_DataOwner* owner = dynamic_cast<const LightApp_DataOwner*>( (*it).operator->() );
    if ( owner  ) {
      if( !owner->IO().IsNull() ) {
	    anIOList.Append(owner->IO());
      }
      else if ( !owner->entry().isEmpty() ) {
	    anIOList.Append( new SALOME_InteractiveObject(qPrintable(owner->entry()),"","") );
      }      
    }
  }
  SPlot2d_Viewer* v = dynamic_cast<SPlot2d_Viewer*>(myViewer);

  if(v)
    v->setObjectsSelected(anIOList);
#endif
}

/*!On selection changed.*/
void LightApp_Plot2dSelector::onSelectionChanged( const QString& entry )
{
  myCurEntry = entry;
  selectionChanged();
  myCurEntry = QString();
}

/*!On clear selected.*/
void LightApp_Plot2dSelector::onClearSelected( ) 
{
	selectionMgr()->clearSelected();
}
