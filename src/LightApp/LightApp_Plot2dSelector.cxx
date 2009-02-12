//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "LightApp_Plot2dSelector.h"

#include "LightApp_DataOwner.h"
#include "LightApp_DataObject.h"
#include "LightApp_Application.h"

/*!
  Constructor
*/
LightApp_Plot2dSelector::LightApp_Plot2dSelector( SPlot2d_Viewer* v, SUIT_SelectionMgr* mgr )
: SUIT_Selector( mgr, v )
{
  if ( v )
    connect( v, SIGNAL( legendSelected( const QString& ) ), this, SLOT( onSelectionChanged( const QString& ) ) );
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
  if( !myCurEntry.isNull() )
    theList.append( new LightApp_DataOwner( myCurEntry ) );
}

/*!Sets selection.*/
void LightApp_Plot2dSelector::setSelection( const SUIT_DataOwnerPtrList& theList )
{
  /*  if( theList.count()>0 )
    myCurEntry = theList.first()->getEntry();
  else*/
  myCurEntry = QString::null;
}

/*!On selection changed.*/
void LightApp_Plot2dSelector::onSelectionChanged( const QString& entry )
{
  myCurEntry = entry;
  selectionChanged();
}
