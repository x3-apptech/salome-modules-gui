#include "SalomeApp_SelectionMgr.h"

#include "SalomeApp_Study.h"
#include "SalomeApp_DataOwner.h"
#include "SalomeApp_DataSubOwner.h"
#include "SalomeApp_Application.h"

#include <SUIT_Session.h>

#include <SALOME_ListIO.hxx>
#include <SALOME_ListIteratorOfListIO.hxx>

// Open CASCADE Include
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_MapIteratorOfMapOfInteger.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>

#include "SALOMEDSClient.hxx"

SalomeApp_SelectionMgr::SalomeApp_SelectionMgr( SalomeApp_Application* app, const bool fb )
: SUIT_SelectionMgr( fb ),
myApp( app )
{
}

SalomeApp_SelectionMgr::~SalomeApp_SelectionMgr()
{
}

SalomeApp_Application* SalomeApp_SelectionMgr::application() const
{
  return myApp;
}

/*
  get all selected objects from selection manager

*/
void SalomeApp_SelectionMgr::selectedObjects( SALOME_ListIO& lst ) const
{
  lst.Clear();

  if ( !application() )
    return;

  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( application()->activeStudy() );
  if ( !appStudy )
    return;

  _PTR(Study) aStudy ( appStudy->studyDS() );
  if ( !aStudy )
    return;

  SUIT_DataOwnerPtrList aList;
  selected( aList );

  QValueList< QString > anEntryList;
  Handle(SALOME_InteractiveObject) anIO;

  for ( SUIT_DataOwnerPtrList::const_iterator itr = aList.begin(); itr != aList.end(); ++itr )
  {
    const SalomeApp_DataOwner* owner = dynamic_cast<const SalomeApp_DataOwner*>( (*itr).operator->() );

    if ( owner && dynamic_cast<const SalomeApp_DataSubOwner*>( owner ) ) 
    { //get only subowners, insert into lst unique subowners (subowners with different entries)
      if ( !anEntryList.contains( owner->entry() ) )
      {	
	anEntryList.append( owner->entry() );

	//construct SALOME_InteractiveObject with predefined entry
	_PTR(SObject) anObj ( aStudy->FindObjectID( owner->entry().latin1() ) );
	if ( anObj )
	{
	  _PTR(SComponent) aFC (anObj->GetFatherComponent());
	  if ( aFC )
	  {
	    anIO = new SALOME_InteractiveObject( anObj->GetID().c_str(), aFC->ComponentDataType().c_str(), anObj->GetName().c_str() );
	    if ( anIO ) lst.Append( anIO );
	  }
	}
      }
    }
    else if ( owner )
    { //get not subowners data owners
      _PTR(SObject) anObj ( aStudy->FindObjectID( owner->entry().latin1() ) );
      if ( anObj )
      {
        _PTR(SComponent) aFC (anObj->GetFatherComponent());
        if ( aFC )
        {
          anIO = new SALOME_InteractiveObject( anObj->GetID().c_str(), aFC->ComponentDataType().c_str(), anObj->GetName().c_str() );
	  if ( anIO ) lst.Append( anIO );
        }
      }
    }
  }
}

void SalomeApp_SelectionMgr::setSelectedObjects( const SALOME_ListIO& lst, const bool append )
{
  if ( !application() )
    return;

  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( application()->activeStudy() );
  if ( !appStudy )
    return;

  _PTR(Study) aStudy ( appStudy->studyDS() );
  if ( !aStudy )
    return;

  SUIT_DataOwnerPtrList owners;
  for ( SALOME_ListIteratorOfListIO it( lst ); it.More(); it.Next() )
  {
    if ( it.Value()->hasEntry() )
      owners.append( new SalomeApp_DataOwner( it.Value()->getEntry() ) );
  }
  setSelected( owners, append );
}

void SalomeApp_SelectionMgr::selectionChanged( SUIT_Selector* theSel )
{
  SUIT_SelectionMgr::selectionChanged( theSel );

  emit currentSelectionChanged();
}

/*
  get map of indexes for the given SALOME_InteractiveObject

*/
void SalomeApp_SelectionMgr::GetIndexes( const Handle(SALOME_InteractiveObject)& IObject, 
					 TColStd_IndexedMapOfInteger& theIndex)
{
  theIndex.Clear();

  SUIT_DataOwnerPtrList aList;
  selected( aList );

  for ( SUIT_DataOwnerPtrList::const_iterator itr = aList.begin(); itr != aList.end(); ++itr )
  {
    const SalomeApp_DataSubOwner* subOwner = dynamic_cast<const SalomeApp_DataSubOwner*>( (*itr).operator->() );
    if ( subOwner )
      if ( subOwner->entry() == QString(IObject->getEntry()) )
	theIndex.Add( subOwner->index() );
  }
  
}

/*
  get map of indexes for the given entry of SALOME_InteractiveObject

*/
void SalomeApp_SelectionMgr::GetIndexes( const QString& theEntry, TColStd_IndexedMapOfInteger& theIndex )
{
  theIndex.Clear();

  SUIT_DataOwnerPtrList aList;
  selected( aList );

  for ( SUIT_DataOwnerPtrList::const_iterator itr = aList.begin(); itr != aList.end(); ++itr )
  {
    const SalomeApp_DataSubOwner* subOwner = dynamic_cast<const SalomeApp_DataSubOwner*>( (*itr).operator->() );
    if ( subOwner )
      if ( subOwner->entry() == theEntry )
	theIndex.Add( subOwner->index() );
  }

}

bool SalomeApp_SelectionMgr::AddOrRemoveIndex( const Handle(SALOME_InteractiveObject)& IObject, 
					       const TColStd_MapOfInteger& theIndexes, 
					       bool modeShift)
{
  SUIT_DataOwnerPtrList remainsOwners;
  
  SUIT_DataOwnerPtrList aList;
  selected( aList );

  if ( !modeShift ) {
    for ( SUIT_DataOwnerPtrList::const_iterator itr = aList.begin(); itr != aList.end(); ++itr )
    {
      const SalomeApp_DataOwner* owner = dynamic_cast<const SalomeApp_DataOwner*>( (*itr).operator->() );
      if ( owner ) 
      {
	if ( owner->entry() != QString(IObject->getEntry()) ) 
	{	  
	  const SalomeApp_DataSubOwner* subOwner = dynamic_cast<const SalomeApp_DataSubOwner*>( owner );
	  if ( subOwner )
	    remainsOwners.append( new SalomeApp_DataSubOwner( subOwner->entry(), subOwner->index() ) );
	  else
	    remainsOwners.append( new SalomeApp_DataOwner( owner->entry() ) );
	}
      }
    }
  }
  else
    remainsOwners = aList;

  TColStd_MapIteratorOfMapOfInteger It;
  It.Initialize(theIndexes);
  for(;It.More();It.Next())
    remainsOwners.append( new SalomeApp_DataSubOwner( QString(IObject->getEntry()), It.Key() ) );
  
  bool append = false;
  setSelected( remainsOwners, append );

  emit currentSelectionChanged();

  TColStd_IndexedMapOfInteger anIndexes;
  GetIndexes( IObject, anIndexes );
  return !anIndexes.IsEmpty();

}

/*
  select 'subobjects' with given indexes

*/
void SalomeApp_SelectionMgr::selectObjects( const Handle(SALOME_InteractiveObject)& IObject, 
					    TColStd_IndexedMapOfInteger theIndex, bool append )
{
  SUIT_DataOwnerPtrList aList;

  if ( theIndex.IsEmpty() )
    aList.append( new SalomeApp_DataOwner( QString(IObject->getEntry()) ) );
  else
    {
      int i;
      for ( i = 1; i <= theIndex.Extent(); i++ )
	aList.append( new SalomeApp_DataSubOwner( QString(IObject->getEntry()), theIndex( i ) ) );
    }

  setSelected( aList, append );

}

/*
  select 'subobjects' with given indexes

*/
void SalomeApp_SelectionMgr::selectObjects( MapIOOfMapOfInteger theMapIO, bool append )
{
  SUIT_DataOwnerPtrList aList;

  MapIOOfMapOfInteger::Iterator it;
  for ( it = theMapIO.begin(); it != theMapIO.end(); ++it ) 
    {
      if ( it.data().IsEmpty() )
	aList.append( new SalomeApp_DataOwner( QString(it.key()->getEntry()) ) );
      else
	{
	  int i;
	  for ( i = 1; i <= it.data().Extent(); i++ )
	    aList.append( new SalomeApp_DataSubOwner( QString(it.key()->getEntry()), it.data()( i ) ) );
	}
    }
  
  setSelected( aList, append );

}

/*
  get map of selected subowners : object's entry <-> map of indexes

*/
void SalomeApp_SelectionMgr::selectedSubOwners( MapEntryOfMapOfInteger& theMap )
{
  theMap.clear();

  TColStd_IndexedMapOfInteger anIndexes;

  SUIT_DataOwnerPtrList aList;
  selected( aList );

  for ( SUIT_DataOwnerPtrList::const_iterator itr = aList.begin(); itr != aList.end(); ++itr )
  {
    const SalomeApp_DataSubOwner* subOwner = dynamic_cast<const SalomeApp_DataSubOwner*>( (*itr).operator->() );
    if ( subOwner ) 
    {
      if ( !theMap.contains( subOwner->entry() ) )
      {
	anIndexes.Clear();
	GetIndexes( subOwner->entry(), anIndexes );
	theMap.insert( subOwner->entry(), anIndexes );
      }
    }
  }
}
