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

// File   : LightApp_OBSelector.cxx
// Author :
//
#include "LightApp_OBSelector.h"

#include "LightApp_DataOwner.h"
#include "LightApp_DataObject.h"
#include "LightApp_Application.h"
#include <SUIT_DataBrowser.h>
#include <SUIT_Session.h>
#include <SUIT_DataObjectIterator.h>
#include <QTime>
#include <time.h>

/*!
  \class LightApp_OBSelector
  \brief Object browser selection handler class.
*/

/*!
  \brief Constructor.
  \param ob object browser
  \param mgr selection manager
*/
LightApp_OBSelector::LightApp_OBSelector( SUIT_DataBrowser* ob, SUIT_SelectionMgr* mgr )
: SUIT_Selector( mgr, ob ),
  myBrowser( ob )
{
  if ( myBrowser ) {
    connect( myBrowser, SIGNAL( selectionChanged() ), this, SLOT( onSelectionChanged() ) );
  }
  setModified();
}

/*!
  \brief Destructor.
*/
LightApp_OBSelector::~LightApp_OBSelector()
{
}

/*!
  \brief Get object browser.
  \return a pointer to the object browser
*/
SUIT_DataBrowser* LightApp_OBSelector::browser() const
{
  return myBrowser;
}

/*!
  \brief Get selector type.
  \return selector type
*/
QString LightApp_OBSelector::type() const
{ 
  return "ObjectBrowser"; 
}

/*!
  \brief Get the time of the last selection changing.
  \return latest selection changing time
*/
unsigned long LightApp_OBSelector::getModifiedTime() const
{
  return myModifiedTime;
}

/*!
  \brief Update the time of the latest selection changing.
*/
void LightApp_OBSelector::setModified()
{
  myModifiedTime = clock();
}

/*!
  \brief Called when the Object browser selection is changed.
*/
void LightApp_OBSelector::onSelectionChanged()
{
  QTime t1 = QTime::currentTime();
  mySelectedList.clear();
  selectionChanged();
  QTime t2 = QTime::currentTime();
  qDebug( QString( "selection time = %1 msecs" ).arg( t1.msecsTo( t2 ) ).toLatin1().constData() );
}

/*!
  \brief Get list of currently selected objects.
  \param theList list to be filled with the selected objects owners
*/
void LightApp_OBSelector::getSelection( SUIT_DataOwnerPtrList& theList ) const
{
  if ( mySelectedList.count() == 0 ) {
    SUIT_Session* session = SUIT_Session::session();
    SUIT_Application* sapp = session ? session->activeApplication() : 0;
    LightApp_Application* app = dynamic_cast<LightApp_Application*>( sapp );
    if( !app || !myBrowser )
      return;

    DataObjectList objlist;
    myBrowser->getSelected( objlist );
    LightApp_OBSelector* that = (LightApp_OBSelector*)this;
    QListIterator<SUIT_DataObject*> it( objlist );
    while ( it.hasNext() ) {
      LightApp_DataObject* obj = dynamic_cast<LightApp_DataObject*>( it.next() );
      if ( obj && app->checkDataObject( obj) ) {
#ifndef DISABLE_SALOMEOBJECT
        Handle(SALOME_InteractiveObject) aSObj = new SALOME_InteractiveObject
          ( obj->entry().toLatin1().constData(),
            obj->componentDataType().toLatin1().constData(),
            obj->name().toLatin1().constData() );
        LightApp_DataOwner* owner = new LightApp_DataOwner( aSObj  );
#else
        LightApp_DataOwner* owner = new LightApp_DataOwner( obj->entry() );
#endif
        that->mySelectedList.append( SUIT_DataOwnerPtr( owner ) );
      }
    }
  }
  theList = mySelectedList;
}

/*!
  \brief Set selection.
  \param theList list of the object owners to be set selected
*/
void LightApp_OBSelector::setSelection( const SUIT_DataOwnerPtrList& theList )
{
  if ( !myBrowser )
    return;

  if( myEntries.count() == 0 || myModifiedTime < myBrowser->getModifiedTime() )
    fillEntries( myEntries );

  DataObjectList objList;
  for ( SUIT_DataOwnerPtrList::const_iterator it = theList.begin(); 
        it != theList.end(); ++it ) {
    const LightApp_DataOwner* owner = dynamic_cast<const LightApp_DataOwner*>( (*it).operator->() );
    if ( owner && myEntries.contains( owner->entry() ) )
      objList.append( myEntries[owner->entry()] );
  }

  myBrowser->setSelected( objList );
  mySelectedList.clear();
}

/*!
  \brief Fill map of the data objects currently shown in the Object Browser.
  \param entries map to be filled
*/
void LightApp_OBSelector::fillEntries( QMap<QString, LightApp_DataObject*>& entries )
{
  entries.clear();

  if ( !myBrowser )
    return;

  for ( SUIT_DataObjectIterator it( myBrowser->root(),
                                    SUIT_DataObjectIterator::DepthLeft ); it.current(); ++it ) {
    LightApp_DataObject* obj = dynamic_cast<LightApp_DataObject*>( it.current() );
    if ( obj )
      entries.insert( obj->entry(), obj );
  }

  setModified();
}

