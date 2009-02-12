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
#include "LightApp_SelectionMgr.h"

#include "LightApp_Study.h"
#include "LightApp_DataOwner.h"
#include "LightApp_DataSubOwner.h"
#include "LightApp_Application.h"

#include <SUIT_Session.h>

#ifndef DISABLE_SALOMEOBJECT
  #include <SALOME_ListIO.hxx>
  #include <SALOME_ListIteratorOfListIO.hxx>

  // Open CASCADE Include
  #include <TColStd_MapOfInteger.hxx>
  #include <TColStd_MapIteratorOfMapOfInteger.hxx>
  #include <TColStd_IndexedMapOfInteger.hxx>
  #include <TCollection_AsciiString.hxx>
#endif

/*!
  Constructor.
*/
LightApp_SelectionMgr::LightApp_SelectionMgr( LightApp_Application* app, const bool fb )
: SUIT_SelectionMgr( fb ),
myApp( app )
{
}

/*!
  Destructor.
*/
LightApp_SelectionMgr::~LightApp_SelectionMgr()
{
}

/*!
  Gets application.
*/
LightApp_Application* LightApp_SelectionMgr::application() const
{
  return myApp;
}

#ifndef DISABLE_SALOMEOBJECT
/*!
  Get all selected objects from selection manager
*/
void LightApp_SelectionMgr::selectedObjects( SALOME_ListIO& theList, const QString& theType,
                                             const bool convertReferences ) const
{
  theList.Clear();

  SUIT_DataOwnerPtrList aList;
  selected( aList, theType );

  QMap<QString,int> entryMap;

  QString entry, checkEntry;
  for ( SUIT_DataOwnerPtrList::const_iterator itr = aList.begin(); itr != aList.end(); ++itr )
  {
    const LightApp_DataOwner* owner = dynamic_cast<const LightApp_DataOwner*>( (*itr).operator->() );
    if( !owner )
      continue;

    LightApp_Study* study = dynamic_cast<LightApp_Study*>( application()->activeStudy() );
    if ( !study )
      return;

    entry = owner->entry();
    // Entry to check object uniqueness.
    // It is selected owner entry in the case, when we do not convert references,
    // and entry of a real object, when we convert references.
    checkEntry = entry;
    if ( convertReferences ) {
      QString refEntry = study->referencedToEntry( entry );
      checkEntry = refEntry;
      if ( !entryMap.contains( checkEntry ) ) {
        if ( refEntry != entry ) {
          QString component = study->componentDataType( refEntry );
          theList.Append( new SALOME_InteractiveObject( refEntry.toLatin1().constData(), component.toLatin1().constData(), ""/*refobj->Name().c_str()*/ ) );
        }
        else if( !owner->IO().IsNull() )
          theList.Append( owner->IO() );
      }
    }
    else {
      if( !entryMap.contains( entry ) && !owner->IO().IsNull() )
	theList.Append( owner->IO() );
    }

    entryMap.insert(checkEntry, 1);
  }
}

/*!
  Append selected objects.
*/
void LightApp_SelectionMgr::setSelectedObjects( const SALOME_ListIO& lst, const bool append )
{
  SUIT_DataOwnerPtrList owners;
  for ( SALOME_ListIteratorOfListIO it( lst ); it.More(); it.Next() )
  {
    if ( it.Value()->hasEntry() )
      owners.append( new LightApp_DataOwner( it.Value() ) );
  }

  setSelected( owners, append );
}

#else
/*!
  Get all selected objects from selection manager
*/
void LightApp_SelectionMgr::selectedObjects( QStringList& theList, const QString& theType,
                                             const bool convertReferences ) const
{
  theList.clear();

  SUIT_DataOwnerPtrList aList;
  selected( aList, theType );

  QString entry;
  for ( SUIT_DataOwnerPtrList::const_iterator itr = aList.begin(); itr != aList.end(); ++itr )
  {
    const LightApp_DataOwner* owner = dynamic_cast<const LightApp_DataOwner*>( (*itr).operator->() );
    if( !owner )
      continue;

    LightApp_Study* study = dynamic_cast<LightApp_Study*>( application()->activeStudy() );
    if ( !study )
      return;

    entry = owner->entry();
    if( !theList.contains( entry ) )
      theList.append( entry );
  }
}

#endif

/*!
  Emit current selection changed.
*/
void LightApp_SelectionMgr::selectionChanged( SUIT_Selector* theSel )
{
  SUIT_SelectionMgr::selectionChanged( theSel );

  emit currentSelectionChanged();
}

#ifndef DISABLE_SALOMEOBJECT

/*!
  get map of indexes for the given SALOME_InteractiveObject
*/
void LightApp_SelectionMgr::GetIndexes( const Handle(SALOME_InteractiveObject)& IObject, 
                                        TColStd_IndexedMapOfInteger& theIndex)
{
  theIndex.Clear();

  SUIT_DataOwnerPtrList aList;
  selected( aList );

  for ( SUIT_DataOwnerPtrList::const_iterator itr = aList.begin(); itr != aList.end(); ++itr )
  {
    const LightApp_DataSubOwner* subOwner = dynamic_cast<const LightApp_DataSubOwner*>( (*itr).operator->() );
    if ( subOwner )
      if ( subOwner->entry() == QString(IObject->getEntry()) )
	theIndex.Add( subOwner->index() );
  }
  
}

/*!
  get map of indexes for the given entry of SALOME_InteractiveObject
*/
void LightApp_SelectionMgr::GetIndexes( const QString& theEntry, TColStd_IndexedMapOfInteger& theIndex )
{
  theIndex.Clear();

  SUIT_DataOwnerPtrList aList;
  selected( aList );

  for ( SUIT_DataOwnerPtrList::const_iterator itr = aList.begin(); itr != aList.end(); ++itr )
  {
    const LightApp_DataSubOwner* subOwner = dynamic_cast<const LightApp_DataSubOwner*>( (*itr).operator->() );
    if ( subOwner )
      if ( subOwner->entry() == theEntry )
	theIndex.Add( subOwner->index() );
  }

}

/*!
  Add or remove interactive objects from selection manager.
*/
//bool LightApp_SelectionMgr::AddOrRemoveIndex( const Handle(SALOME_InteractiveObject)& IObject,
void LightApp_SelectionMgr::AddOrRemoveIndex( const Handle(SALOME_InteractiveObject)& IObject,
                                              const TColStd_MapOfInteger& theIndexes,
                                              bool modeShift)
{
  SUIT_DataOwnerPtrList remainsOwners;
  
  SUIT_DataOwnerPtrList aList;
  selected( aList );
  
  QString ioEntry (IObject->getEntry());
  
  if ( !modeShift ) {
    for ( SUIT_DataOwnerPtrList::const_iterator itr = aList.begin(); itr != aList.end(); ++itr )
    {
      const LightApp_DataOwner* owner = dynamic_cast<const LightApp_DataOwner*>( (*itr).operator->() );
      if ( owner )
      {
	if ( owner->entry() != ioEntry )
	{
	  const LightApp_DataSubOwner* subOwner = dynamic_cast<const LightApp_DataSubOwner*>( owner );
	  if ( subOwner )
	    remainsOwners.append( new LightApp_DataSubOwner( subOwner->entry(), subOwner->index() ) );
	  else
	    remainsOwners.append( new LightApp_DataOwner( owner->entry() ) );
	}
      }
    }
  }
  else
    remainsOwners = aList;

  TColStd_MapIteratorOfMapOfInteger It;
  It.Initialize(theIndexes);
  for(;It.More();It.Next())
    remainsOwners.append( new LightApp_DataSubOwner( ioEntry, It.Key() ) );
  
  bool append = false;
  setSelected( remainsOwners, append );

  emit currentSelectionChanged();

  // Bug 17269: To avoid calling of selected(aList)
  //TColStd_IndexedMapOfInteger anIndexes;
  //GetIndexes( IObject, anIndexes );
  //return !anIndexes.IsEmpty();
}

/*!
  select 'subobjects' with given indexes
*/
void LightApp_SelectionMgr::selectObjects( const Handle(SALOME_InteractiveObject)& IObject, 
					    TColStd_IndexedMapOfInteger theIndex, bool append )
{
  SUIT_DataOwnerPtrList aList;

  if ( theIndex.IsEmpty() )
    aList.append( new LightApp_DataOwner( QString(IObject->getEntry()) ) );
  else
    {
      int i;
      for ( i = 1; i <= theIndex.Extent(); i++ )
	aList.append( new LightApp_DataSubOwner( QString(IObject->getEntry()), theIndex( i ) ) );
    }

  setSelected( aList, append );

}

/*!
  select 'subobjects' with given indexes
*/
void LightApp_SelectionMgr::selectObjects( MapIOOfMapOfInteger theMapIO, bool append )
{
  SUIT_DataOwnerPtrList aList;

  MapIOOfMapOfInteger::Iterator it(theMapIO);
  for ( ; it.More(); it.Next() ) 
    {
      if ( it.Value().IsEmpty() )
	aList.append( new LightApp_DataOwner( QString(it.Key()->getEntry()) ) );
      else
	{
	  int i;
	  for ( i = 1; i <= it.Value().Extent(); i++ )
	    aList.append( new LightApp_DataSubOwner( QString(it.Key()->getEntry()), it.Value()( i ) ) );
	}
    }
  
  setSelected( aList, append );

}

/*!
  get map of selected subowners : object's entry <-> map of indexes
*/
void LightApp_SelectionMgr::selectedSubOwners( MapEntryOfMapOfInteger& theMap )
{
  theMap.Clear();

  TColStd_IndexedMapOfInteger anIndexes;

  SUIT_DataOwnerPtrList aList;
  selected( aList );

  for ( SUIT_DataOwnerPtrList::const_iterator itr = aList.begin(); itr != aList.end(); ++itr )
  {
    const LightApp_DataSubOwner* subOwner =
      dynamic_cast<const LightApp_DataSubOwner*>( (*itr).operator->() );
    if ( subOwner ) 
    {
#ifndef WNT
      if ( !theMap.IsBound( TCollection_AsciiString(subOwner->entry().toLatin1().data()) ) )
#else
      if ( !theMap.IsBound( subOwner->entry().toLatin1().data() ) )
#endif
      {
	anIndexes.Clear();
	//Bug 17269: GetIndexes( subOwner->entry(), anIndexes );
        //Bug 17269: To avoid multiple calling of selected(aList)
        for ( SUIT_DataOwnerPtrList::const_iterator itr2 = itr; itr2 != aList.end(); ++itr2 )
        {
          const LightApp_DataSubOwner* subOwner2 =
            dynamic_cast<const LightApp_DataSubOwner*>( (*itr2).operator->() );
          if ( subOwner2 )
            if ( subOwner2->entry() == subOwner->entry() )
              anIndexes.Add( subOwner2->index() );
        }
        //
	theMap.Bind( subOwner->entry().toLatin1().data(), anIndexes );
      }
    }
  }
}

#endif
