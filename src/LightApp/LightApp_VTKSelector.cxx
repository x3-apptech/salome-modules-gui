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

#include "LightApp_VTKSelector.h"
#include "LightApp_DataOwner.h"

#include "SUIT_Desktop.h"

#ifndef DISABLE_VTKVIEWER
  #include "SVTK_ViewModelBase.h"
  #include "SVTK_ViewManager.h"
  #include "SVTK_Selector.h"
  #include "SVTK_ViewWindow.h"
  #include "SVTK_Functor.h"
  #include "VTKViewer_Algorithm.h"
  #include <vtkRenderer.h>
  #include "SVTK_ViewModel.h"
#endif

#ifndef DISABLE_SALOMEOBJECT
  #include "SALOME_Actor.h"
  #include "SALOME_ListIO.hxx"
#endif



#ifndef DISABLE_VTKVIEWER
#ifndef DISABLE_SALOMEOBJECT
/*!
  Constructor.
*/
LightApp_SVTKDataOwner::LightApp_SVTKDataOwner( const Handle(SALOME_InteractiveObject)& theIO,
                                                                  SUIT_Desktop* theDesktop )
: LightApp_DataOwner( theIO ),
myDesktop( theDesktop )
{
}
#else
LightApp_SVTKDataOwner::LightApp_SVTKDataOwner( const QString& theEntry )
: LightApp_DataOwner( theEntry )
{
}
#endif

/*!
  \return active SVTK view window
*/
SVTK_ViewWindow* 
LightApp_SVTKDataOwner
::GetActiveViewWindow() const
{
  if(SUIT_ViewWindow* aViewWindow = myDesktop->activeWindow())
    return dynamic_cast<SVTK_ViewWindow*>(aViewWindow);

  return NULL;
}

/*!
  Gets dataowners ids list.
*/
const TColStd_IndexedMapOfInteger& 
LightApp_SVTKDataOwner
::GetIds() const
{
  if(SVTK_ViewWindow* aViewWindow = GetActiveViewWindow()){
    if(SVTK_Selector* aSelector = aViewWindow->GetSelector()){
      aSelector->GetIndex(IO(),myIds);
    }
  }

  return myIds;
}

/*!
  Gets selection mode.
*/
Selection_Mode
LightApp_SVTKDataOwner
::GetMode() const
{
  if(SVTK_ViewWindow* aViewWindow = GetActiveViewWindow()){
    if(SVTK_Selector* aSelector = aViewWindow->GetSelector()){
      return aSelector->SelectionMode();
    }
  }
  
  return -1;
}

/*!
  Gets actor pointer.
*/
SALOME_Actor* 
LightApp_SVTKDataOwner
::GetActor() const
{
  if(SVTK_ViewWindow* aViewWindow = GetActiveViewWindow()){
    using namespace SVTK;
    VTK::ActorCollectionCopy aCopy(aViewWindow->getRenderer()->GetActors());
    return Find<SALOME_Actor>(aCopy.GetActors(),TIsSameIObject<SALOME_Actor>(IO()));
  }

  return NULL;
}

#endif

/*!
  Destuctor.
*/
/*
LightApp_SVTKDataOwner::~LightApp_SVTKDataOwner()
{
}
*/

#ifndef DISABLE_VTKVIEWER
/*!
  Constructor.
*/
LightApp_VTKSelector
::LightApp_VTKSelector( SVTK_ViewModelBase* viewer, 
                        SUIT_SelectionMgr* mgr ): 
  SUIT_Selector( mgr, viewer ),
  myViewer( viewer )
{
  if ( myViewer )
    connect( myViewer, SIGNAL( selectionChanged() ), this, SLOT( onSelectionChanged() ) );
}

/*!
  Gets viewer.
*/
SVTK_ViewModelBase* 
LightApp_VTKSelector
::viewer() const
{
  return myViewer;
}

/*!
  Gets type of salome vtk viewer.
*/
QString
LightApp_VTKSelector
::type() const
{ 
  return myViewer->getType(); 
}

#endif
/*!
  On selection changed.
*/
void
LightApp_VTKSelector
::onSelectionChanged()
{
  selectionChanged();
}

#ifndef DISABLE_VTKVIEWER

/*!
  Gets list of selected data owners.(output \a aList).
*/
void
LightApp_VTKSelector
::getSelection( SUIT_DataOwnerPtrList& aList ) const
{
  if( myViewer ) {
    if( SVTK_Viewer* aSViewer = dynamic_cast<SVTK_Viewer*>(myViewer) ) {
      if( !aSViewer->isSelectionEnabled() ) 
	return;
    }
    if(SUIT_ViewManager* aViewManager = myViewer->getViewManager()){
      if(SVTK_ViewManager* aViewMgr = dynamic_cast<SVTK_ViewManager*>(aViewManager)){
        if(SVTK_ViewWindow* aView = dynamic_cast<SVTK_ViewWindow*>(aViewMgr->getActiveView())){
          if(SVTK_Selector* aSelector = aView->GetSelector()){
            const SALOME_ListIO& aListIO = aSelector->StoredIObjects();
            SALOME_ListIteratorOfListIO anIter(aListIO);
            for(; anIter.More(); anIter.Next()){
              Handle(SALOME_InteractiveObject) anIO = anIter.Value();
              if(anIO->hasEntry())
                aList.append(new LightApp_SVTKDataOwner(anIO,aViewMgr->getDesktop()));
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
  SVTK_Viewer* aViewer = dynamic_cast<SVTK_Viewer*>(myViewer);
  if(myViewer && aViewer && aViewer->isSelectionEnabled()){
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

              aSelector->AddIObject(anIO);

              anAppendList.Append(anIO);
              aSelector->AddOrRemoveIndex(anIO,anOwner->GetIds(),false);
            }else if(const LightApp_DataOwner* anOwner = dynamic_cast<const LightApp_DataOwner*>(aDataOwner)){
              Handle(SALOME_InteractiveObject) anIO = 
                new SALOME_InteractiveObject(anOwner->entry().toLatin1(),"");
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
            aSelector->RemoveIObject( RIt.value() );
          
          aView->onSelectionChanged();
        }
      }
    }
  }
}

#endif
