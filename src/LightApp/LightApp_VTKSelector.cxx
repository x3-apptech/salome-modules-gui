#include "LightApp_VTKSelector.h"
#include "LightApp_DataOwner.h"

#include "SVTK_ViewModel.h"
#include "SVTK_Selector.h"
#include "SVTK_ViewWindow.h"
#include "SVTK_Functor.h"

#include "SALOME_Actor.h"
#include "SALOME_ListIteratorOfListIO.hxx"

#include "VTKViewer_Algorithm.h"

#include <vtkRenderer.h>

/*!
  Constructor.
*/
LightApp_SVTKDataOwner
::LightApp_SVTKDataOwner( const Handle(SALOME_InteractiveObject)& theIO,
                          const TColStd_IndexedMapOfInteger& theIds,
                          Selection_Mode theMode,
                          SALOME_Actor* theActor):
  LightApp_DataOwner( theIO ),
  mySelectionMode(theMode),
  myActor(theActor)
{
  myIds = theIds; // workaround - there is no constructor copy for the container
}

/*!
  Destuctor.
*/
LightApp_SVTKDataOwner
::~LightApp_SVTKDataOwner()
{
}

/*!
  Gets actor pointer.
*/
SALOME_Actor* 
LightApp_SVTKDataOwner
::GetActor() const
{
  return myActor.GetPointer();
}

/*!
  Constructor.
*/
LightApp_VTKSelector
::LightApp_VTKSelector( SVTK_Viewer* viewer, 
                        SUIT_SelectionMgr* mgr ): 
  SUIT_Selector( mgr, viewer ),
  myViewer( viewer )
{
  if ( myViewer )
    connect( myViewer, SIGNAL( selectionChanged() ), this, SLOT( onSelectionChanged() ) );
}

/*!
  Destructor.
*/
LightApp_VTKSelector
::~LightApp_VTKSelector()
{
}

/*!
  Gets viewer.
*/
SVTK_Viewer* 
LightApp_VTKSelector
::viewer() const
{
  return myViewer;
}

/*!
  Gets type of vtk viewer.
*/
QString
LightApp_VTKSelector
::type() const
{ 
  return SVTK_Viewer::Type(); 
}

/*!
  On selection changed.
*/
void
LightApp_VTKSelector
::onSelectionChanged()
{
  selectionChanged();
}

/*!
  Gets list of selected data owners.(output \a aList).
*/
void
LightApp_VTKSelector
::getSelection( SUIT_DataOwnerPtrList& aList ) const
{
  if(myViewer){
    if(SUIT_ViewManager* aViewMgr = myViewer->getViewManager()){
      if(SVTK_ViewWindow* aView = dynamic_cast<SVTK_ViewWindow*>(aViewMgr->getActiveView())){
	if(SVTK_Selector* aSelector = aView->GetSelector()){
	  Selection_Mode aMode = aSelector->SelectionMode();
	  const SALOME_ListIO& aListIO = aSelector->StoredIObjects();
	  SALOME_ListIteratorOfListIO anIter(aListIO);
	  for(; anIter.More(); anIter.Next()){
	    Handle(SALOME_InteractiveObject) anIO = anIter.Value();
	    if(anIO->hasEntry()){
	      TColStd_IndexedMapOfInteger anIds;
	      aSelector->GetIndex(anIO,anIds);
	      SALOME_Actor* anActor = aSelector->GetActor(anIO);
	      if( !anActor )
	        anActor = VTK::Find<SALOME_Actor>(aView->getRenderer()->GetActors(),VTK::TIsSameIObject<SALOME_Actor>(anIO));

	      aList.append(new LightApp_SVTKDataOwner(anIO,anIds,aMode,anActor));
	    }
	  }
	}
      }
    }
  }
}

/*!
  Sets selection to selector from data owner list \a theList.
*/
void 
LightApp_VTKSelector
::setSelection( const SUIT_DataOwnerPtrList& theList )
{
  if(myViewer){
    if(SUIT_ViewManager* aViewMgr = myViewer->getViewManager()){
      if(SVTK_ViewWindow* aView = dynamic_cast<SVTK_ViewWindow*>(aViewMgr->getActiveView())){
	if(SVTK_Selector* aSelector = aView->GetSelector()){
	  SALOME_ListIO anAppendList;
	  const SALOME_ListIO& aStoredList = aSelector->StoredIObjects();
	  SUIT_DataOwnerPtrList::const_iterator anIter = theList.begin();
	  for(; anIter != theList.end(); ++anIter){
	    const SUIT_DataOwner* aDataOwner = (*anIter).get();
	    if(const LightApp_SVTKDataOwner* anOwner = dynamic_cast<const LightApp_SVTKDataOwner*>(aDataOwner)){
	      aSelector->SetSelectionMode(anOwner->GetMode());
	      Handle(SALOME_InteractiveObject) anIO = anOwner->IO();

	      if( anOwner->GetActor() )
		aSelector->AddIObject( anOwner->GetActor() );
	      else
		aSelector->AddIObject(anIO);

	      anAppendList.Append(anIO);
	      aSelector->AddOrRemoveIndex(anIO,anOwner->GetIds(),false);
	    }else if(const LightApp_DataOwner* anOwner = dynamic_cast<const LightApp_DataOwner*>(aDataOwner)){
	      Handle(SALOME_InteractiveObject) anIO = 
		new SALOME_InteractiveObject(anOwner->entry().latin1(),"");
	      aSelector->AddIObject(anIO);
	      anAppendList.Append(anIO);
	    }
	  }
	  // To remove IOs, which is not selected.
	  QMap< QString, Handle( SALOME_InteractiveObject )> toRemove;
	  SALOME_ListIteratorOfListIO anIt( aStoredList );
	  for( ; anIt.More(); anIt.Next() )
	    if( !anIt.Value().IsNull() )
	      toRemove[ anIt.Value()->getEntry() ] = anIt.Value();

	  anIt = SALOME_ListIteratorOfListIO(anAppendList);
	  for( ; anIt.More(); anIt.Next() )
	    toRemove.remove( anIt.Value()->getEntry() );

	  QMap< QString, Handle( SALOME_InteractiveObject )>::const_iterator RIt = toRemove.begin(),
	                                                                     REnd = toRemove.end();
	  for( ; RIt!=REnd; RIt++ )
	    aSelector->RemoveIObject( RIt.data() );
	  
	  aView->onSelectionChanged();
	}
      }
    }
  }
}
