#include "SalomeApp_VTKSelector.h"
#include "SalomeApp_DataOwner.h"

#include "SVTK_ViewModel.h"
#include "SVTK_Selector.h"
#include "SVTK_ViewWindow.h"
#include "SALOME_Actor.h"

#include "SALOME_ListIteratorOfListIO.hxx"

#include "utilities.h"

#ifdef _DEBUG_
static int MYDEBUG = 0;
#else
static int MYDEBUG = 0;
#endif

SalomeApp_SVTKDataOwner
::SalomeApp_SVTKDataOwner( const Handle(SALOME_InteractiveObject)& theIO,
			   const TColStd_IndexedMapOfInteger& theIds,
			   Selection_Mode theMode,
			   SALOME_Actor* theActor):
  SalomeApp_DataOwner( theIO ),
  mySelectionMode(theMode),
  myActor(theActor)
{
  myIds = theIds; // workaround - there is no constructor copy for the container
}

SalomeApp_SVTKDataOwner
::~SalomeApp_SVTKDataOwner()
{
}

SALOME_Actor* 
SalomeApp_SVTKDataOwner
::GetActor() const
{
  return myActor.GetPointer();
}


SalomeApp_VTKSelector
::SalomeApp_VTKSelector( SVTK_Viewer* viewer, 
			 SUIT_SelectionMgr* mgr ): 
  QObject( 0 ),
  SUIT_Selector( mgr ),
  myViewer( viewer )
{
  if ( myViewer )
    connect( myViewer, SIGNAL( selectionChanged() ), this, SLOT( onSelectionChanged() ) );
}

SalomeApp_VTKSelector
::~SalomeApp_VTKSelector()
{
}

SVTK_Viewer* 
SalomeApp_VTKSelector
::viewer() const
{
  return myViewer;
}

QString
SalomeApp_VTKSelector
::type() const
{ 
  return SVTK_Viewer::Type(); 
}

void
SalomeApp_VTKSelector
::onSelectionChanged()
{
  selectionChanged();
}

void
SalomeApp_VTKSelector
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
	      aList.append(new SalomeApp_SVTKDataOwner(anIO,anIds,aMode,anActor));
	      if(MYDEBUG) MESSAGE("VTKSelector::getSelection - "<<anIO->getEntry());
	    }
	  }
	}
      }
    }
  }
}

void 
SalomeApp_VTKSelector
::setSelection( const SUIT_DataOwnerPtrList& theList )
{
  if(myViewer){
    if(SUIT_ViewManager* aViewMgr = myViewer->getViewManager()){
      if(SVTK_ViewWindow* aView = dynamic_cast<SVTK_ViewWindow*>(aViewMgr->getActiveView())){
	if(SVTK_Selector* aSelector = aView->GetSelector()){
	  aSelector->ClearIObjects();
	  SUIT_DataOwnerPtrList::const_iterator anIter = theList.begin();
	  for(; anIter != theList.end(); ++anIter){
	    const SUIT_DataOwner* aDataOwner = (*anIter).get();
	    if(const SalomeApp_SVTKDataOwner* anOwner = dynamic_cast<const SalomeApp_SVTKDataOwner*>(aDataOwner)){
	      aSelector->SetSelectionMode(anOwner->GetMode());
	      Handle(SALOME_InteractiveObject) anIO = anOwner->IO();
	      aSelector->AddIObject(anIO);
	      aSelector->AddOrRemoveIndex(anIO,anOwner->GetIds(),false);
	      if(MYDEBUG) MESSAGE("VTKSelector::setSelection - SVTKDataOwner - "<<anIO->getEntry());
	    }else if(const SalomeApp_DataOwner* anOwner = dynamic_cast<const SalomeApp_DataOwner*>(aDataOwner)){
	      Handle(SALOME_InteractiveObject) anIO = 
		new SALOME_InteractiveObject(anOwner->entry().latin1(),"");
	      aSelector->AddIObject(anIO);
	      if(MYDEBUG) MESSAGE("VTKSelector::setSelection - DataOwner - "<<anIO->getEntry());
	    }
	  }
	  aView->onSelectionChanged();
	}
      }
    }
  }
}
