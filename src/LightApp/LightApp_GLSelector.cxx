// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
      LightApp_GLOwner* owner = dynamic_cast< LightApp_GLOwner* >( obj->owner() );
      if( owner )
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

  QMap<QString, GLViewer_Object*> aDisplayed;
  const ObjList& displayed = cont->getObjects();
  for ( ObjList::const_iterator it = displayed.begin(); it != displayed.end(); ++it )
  {
    GLViewer_Object* obj = *it;
    if ( obj && obj->getVisible() )
    {
      LightApp_GLOwner* owner = dynamic_cast< LightApp_GLOwner* >( obj->owner() );
      if ( owner )
	aDisplayed.insert( owner->entry(), obj );
    }
  }

  int Nb = 0;
  cont->clearSelected( false );
  for ( SUIT_DataOwnerPtrList::const_iterator itr = aList.begin(); itr != aList.end(); ++itr )
  {
    const LightApp_DataOwner* owner = dynamic_cast<const LightApp_DataOwner*>( (*itr).operator->() );

    if ( !owner )
      continue;

    if ( aDisplayed.contains( owner->entry() ) )
    {
      cont->setSelected( aDisplayed[owner->entry()], false );
      Nb++;
    }
  }

  if ( Nb > 0 )
    myViewer->updateAll();
}


/*!
  Constructor
  \param entry - entry of object
*/
LightApp_GLOwner::LightApp_GLOwner( const char* entry )
: GLViewer_Owner()
{
  setEntry( entry );
}

/*!
  Destructor
*/
LightApp_GLOwner::~LightApp_GLOwner()
{
}

/*!
  \return entry
*/
const char* LightApp_GLOwner::entry() const
{
  return myEntry.c_str();
}

/*!
  Sets new entry
  \param entry - entry of object
*/
void LightApp_GLOwner::setEntry( const char* entry )
{
  myEntry = entry;
}
