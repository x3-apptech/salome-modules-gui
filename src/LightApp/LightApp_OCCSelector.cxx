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

#include "LightApp_DataOwner.h"
#include "LightApp_OCCSelector.h"

#ifndef DISABLE_SALOMEOBJECT
  #include <SALOME_InteractiveObject.hxx>
#endif
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>

/*!
  Constructor
*/
LightApp_OCCSelector::LightApp_OCCSelector( OCCViewer_Viewer* viewer, SUIT_SelectionMgr* mgr )
: SUIT_Selector( mgr, viewer ),
  myViewer( viewer )
{
  if ( myViewer ) {
    connect( myViewer, SIGNAL( selectionChanged() ), this, SLOT( onSelectionChanged() ) );
    connect( myViewer, SIGNAL( deselection() ), this, SLOT( onDeselection() ) );
  }
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

/*!On selection cleared.*/
void LightApp_OCCSelector::onDeselection()
{
  mySelectedExternals.clear();
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
#ifndef DISABLE_SALOMEOBJECT
      Handle(SALOME_InteractiveObject) anObj = Handle(SALOME_InteractiveObject)::DownCast(anIt.Value()->GetOwner());
      if( !anObj.IsNull() )
        aList.append( SUIT_DataOwnerPtr( new LightApp_DataOwner( anObj ) ) );
#else
      aList.append( SUIT_DataOwnerPtr( new LightApp_DataOwner( entry( anIt.Value() ) ) ) );
#endif
    }
  // add externally selected objects
  SUIT_DataOwnerPtrList::const_iterator anExtIter;
  for(anExtIter = mySelectedExternals.begin(); anExtIter != mySelectedExternals.end(); anExtIter++) {
    aList.append(*anExtIter);
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
  
  mySelectedExternals.clear();

  for ( SUIT_DataOwnerPtrList::const_iterator itr = aList.begin(); itr != aList.end(); ++itr )
  {
    const LightApp_DataOwner* owner = dynamic_cast<const LightApp_DataOwner*>( (*itr).operator->() );
    if ( owner && aDisplayed.contains( owner->entry() ) )
      aSelList.Append( aDisplayed[owner->entry()] );
    else
      mySelectedExternals.append(*itr);
  }

  myViewer->unHighlightAll( false );
  myViewer->setObjectsSelected( aSelList );
}

/*!Gets entry ob object.*/
QString LightApp_OCCSelector::entry( const Handle(AIS_InteractiveObject)& anAIS ) const
{
  if ( anAIS.IsNull() || !anAIS->HasOwner() )
    return QString::null;

  QString res;

#ifndef DISABLE_SALOMEOBJECT
  Handle(SALOME_InteractiveObject) anObj = Handle(SALOME_InteractiveObject)::DownCast(anAIS->GetOwner());
  if ( !anObj.IsNull() )
    res = QString( anObj->getEntry() );
#endif

  return res;
}
