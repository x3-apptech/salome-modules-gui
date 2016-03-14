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

#include "LightApp_GLSelector.h"

#include "LightApp_DataOwner.h"

#include <GLViewer_Context.h>
#include <GLViewer_Object.h>

/*!Constructor. Initialize by GLViewer_Viewer2d and SUIT_SelectionMgr.*/
LightApp_GLSelector::LightApp_GLSelector( GLViewer_Viewer2d* viewer, SUIT_SelectionMgr* mgr )
: SUIT_Selector( mgr, viewer ),
  myViewer( viewer )
{
  if ( myViewer )
    connect( myViewer, SIGNAL( selectionChanged( SelectionChangeStatus ) ), 
             this, SLOT( onSelectionChanged() ) );
}

/*!Destructor. Do nothing.*/
LightApp_GLSelector::~LightApp_GLSelector()
{
}

/*!Gets viewer*/
GLViewer_Viewer2d* LightApp_GLSelector::viewer() const
{
  return myViewer;
}

/*!On selection changed event.*/
void LightApp_GLSelector::onSelectionChanged()
{
  selectionChanged();
}

/*!Gets list of selected Data Owner objects.*/
void LightApp_GLSelector::getSelection( SUIT_DataOwnerPtrList& aList ) const
{
  if ( !myViewer )
    return;

  GLViewer_Context* cont = myViewer->getGLContext();
  if ( !cont )
    return;

  for ( cont->InitSelected(); cont->MoreSelected(); cont->NextSelected() )
  {
    GLViewer_Object* obj = cont->SelectedObject();
    if ( obj )
    {
      LightApp_DataOwner* owner = dynamic_cast<LightApp_DataOwner*>( obj->owner() );
      if ( owner )
        aList.append( SUIT_DataOwnerPtr( new LightApp_DataOwner( owner->entry() ) ) );
    }
  }
}

/*!Sets to selected list of Data Owner objects.*/
void LightApp_GLSelector::setSelection( const SUIT_DataOwnerPtrList& aList )
{
  if ( !myViewer )
    return;

  GLViewer_Context* cont = myViewer->getGLContext();
  if ( !cont )
    return;

  QMap<QString, int> aSelected;
  for ( SUIT_DataOwnerPtrList::const_iterator itr = aList.begin(); itr != aList.end(); ++itr )
  {
    const LightApp_DataOwner* owner = dynamic_cast<const LightApp_DataOwner*>( (*itr).operator->() );

    if ( owner )
      aSelected.insert( owner->entry(), 0 );
  }

  bool changed = false;
  const ObjList& displayed = cont->getObjects();
  for ( ObjList::const_iterator it = displayed.begin(); it != displayed.end(); ++it )
  {
    GLViewer_Object* obj = *it;
    if ( obj && obj->getVisible() )
    {
      LightApp_DataOwner* owner = dynamic_cast<LightApp_DataOwner*>( obj->owner() );
      bool sel = owner && aSelected.contains( owner->entry() );
      changed = changed || sel != (bool)obj->isSelected();
      if ( sel && !obj->isSelected() )
        cont->setSelected( obj, false );
      else if ( !sel && obj->isSelected() )
        cont->remSelected( obj, false );
    }
  }

  if ( changed )
    myViewer->updateAll();
}
