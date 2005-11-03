
#include "LightApp_DataOwner.h"
#include "LightApp_OCCSelector.h"

#include <SALOME_InteractiveObject.hxx>

#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>

/*!
  Constructor
*/
LightApp_OCCSelector::LightApp_OCCSelector( OCCViewer_Viewer* viewer, SUIT_SelectionMgr* mgr )
: SUIT_Selector( mgr, viewer ),
  myViewer( viewer )
{
  if ( myViewer )
    connect( myViewer, SIGNAL( selectionChanged() ), this, SLOT( onSelectionChanged() ) );
}

/*!
  Destructor.
*/
LightApp_OCCSelector::~LightApp_OCCSelector()
{
}

/*!
  Gets viewer.
*/
OCCViewer_Viewer* LightApp_OCCSelector::viewer() const
{
  return myViewer;
}

/*!On selection changed.*/
void LightApp_OCCSelector::onSelectionChanged()
{
  selectionChanged();
}

/*!Gets selection list.*/
void LightApp_OCCSelector::getSelection( SUIT_DataOwnerPtrList& aList ) const
{
  if ( !myViewer )
    return;

  AIS_ListOfInteractive aSelList;
  myViewer->getSelectedObjects( aSelList );
  for ( AIS_ListIteratorOfListOfInteractive anIt( aSelList ); anIt.More(); anIt.Next() )
    if ( !anIt.Value().IsNull() )
    {
      Handle(SALOME_InteractiveObject) anObj = Handle(SALOME_InteractiveObject)::DownCast(anIt.Value()->GetOwner());
      if( !anObj.IsNull() )
        aList.append( SUIT_DataOwnerPtr( new LightApp_DataOwner( anObj ) ) );
    }
}

/*!Sets selection list.*/
void LightApp_OCCSelector::setSelection( const SUIT_DataOwnerPtrList& aList )
{
  if ( !myViewer )
    return;

  QMap<QString, Handle(AIS_InteractiveObject)> aDisplayed;
  Handle(AIS_InteractiveContext) aContext = myViewer->getAISContext();
  if ( aContext.IsNull() )
    return;
    
  AIS_ListOfInteractive aDispList, aSelList;
  aContext->DisplayedObjects( aDispList );

  for ( AIS_ListIteratorOfListOfInteractive it( aDispList ); it.More(); it.Next() )
  {
    QString entryStr = entry( it.Value() );
    if ( !entryStr.isEmpty() )
      aDisplayed.insert( entryStr, it.Value() );
  }

  for ( SUIT_DataOwnerPtrList::const_iterator itr = aList.begin(); itr != aList.end(); ++itr )
  {
    const LightApp_DataOwner* owner = dynamic_cast<const LightApp_DataOwner*>( (*itr).operator->() );
    if ( owner && aDisplayed.contains( owner->entry() ) )
      aSelList.Append( aDisplayed[owner->entry()] );
  }

  myViewer->unHighlightAll( false );
  myViewer->setObjectsSelected( aSelList );
}

/*!Gets entry ob object.*/
QString LightApp_OCCSelector::entry( const Handle(AIS_InteractiveObject)& anAIS ) const
{
  if ( anAIS.IsNull() || !anAIS->HasOwner() )
    return QString::null;

  Handle(SALOME_InteractiveObject) anObj = Handle(SALOME_InteractiveObject)::DownCast(anAIS->GetOwner());

  QString res;
  if ( !anObj.IsNull() )
    res = QString( anObj->getEntry() );
  return res;
}
