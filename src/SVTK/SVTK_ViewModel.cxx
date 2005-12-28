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
// See http://www.salome-platform.org/
//
#include <qpopupmenu.h>
#include <qcolordialog.h>

#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkActorCollection.h>

#include "SUIT_Session.h"

#include "SVTK_Selection.h"
#include "SVTK_ViewModel.h"
#include "SVTK_ViewWindow.h"
#include "SVTK_Prs.h"

#include "VTKViewer_ViewModel.h"

#include "SVTK_RenderWindowInteractor.h"
#include "SVTK_RenderWindow.h"

//#include <ToolsGUI.h>

#include <SALOME_Actor.h>
#include <SALOME_InteractiveObject.hxx>

// Temporarily commented to avoid awful dependecy on SALOMEDS
// TODO: better mechanism of storing display/erse status in a study
// should be provided...
//#include "Utils_ORB_INIT.hxx"
//#include "Utils_SINGLETON.hxx"
//#include "SALOME_ModuleCatalog_impl.hxx"
//#include "SALOME_NamingService.hxx"

//#include "SALOMEDSClient.hxx"
//#include "SALOMEDS_StudyManager.hxx"

// in order NOT TO link with SalomeApp, here the code returns SALOMEDS_Study.
// SalomeApp_Study::studyDS() does it as well, but -- here it is retrieved from 
// SALOMEDS::StudyManager - no linkage with SalomeApp. 

// Temporarily commented to avoid awful dependecy on SALOMEDS
// TODO: better mechanism of storing display/erse status in a study
// should be provided...
//static _PTR(Study) getStudyDS() 
//{
//  SALOMEDSClient_Study* aStudy = NULL;
//  _PTR(StudyManager) aMgr( new SALOMEDS_StudyManager() );
  // get id of SUIT_Study, if it's a SalomeApp_Study, it will return
  //    id of its underlying SALOMEDS::Study
//  SUIT_Application* app = SUIT_Session::session()->activeApplication();
//  if ( !app )  return _PTR(Study)(aStudy); 
//  SUIT_Study* stud = app->activeStudy();
//  if ( !stud ) return _PTR(Study)(aStudy);  
//  const int id = stud->id(); // virtual method, must return SALOMEDS_Study id
  // get SALOMEDS_Study with this id from StudyMgr
//  return aMgr->GetStudyByID( id );
//}

//==========================================================
SVTK_Viewer::SVTK_Viewer()
{
  myTrihedronSize = 100;
}

//==========================================================
SVTK_Viewer::~SVTK_Viewer() 
{
}

QColor SVTK_Viewer::backgroundColor() const
{
  return myBgColor;
}

void SVTK_Viewer::setBackgroundColor( const QColor& c )
{
  if ( c.isValid() )
    myBgColor = c;
}

//==========================================================
SUIT_ViewWindow* SVTK_Viewer::createView( SUIT_Desktop* theDesktop )
{
  SVTK_ViewWindow* vw = new SVTK_ViewWindow( theDesktop, this );
  vw->setBackgroundColor( backgroundColor() );
  vw->SetTrihedronSize( trihedronSize() );
  return vw;
}

int SVTK_Viewer::trihedronSize() const
{
  return myTrihedronSize;
}

void SVTK_Viewer::setTrihedronSize( const int sz )
{
  myTrihedronSize = sz;

  SUIT_ViewManager* vm = getViewManager();
  if ( !vm )
    return;

  QPtrVector<SUIT_ViewWindow> vec = vm->getViews();
  for ( int i = 0; i < vec.count(); i++ )
  {
    SUIT_ViewWindow* win = vec.at( i );
    if ( !win || !win->inherits( "SVTK_ViewWindow" ) )
      continue;

    SVTK_ViewWindow* vw = (SVTK_ViewWindow*)win;
    vw->SetTrihedronSize( sz );
  }
}

//==========================================================
void SVTK_Viewer::setViewManager(SUIT_ViewManager* theViewManager)
{
  SUIT_ViewModel::setViewManager(theViewManager);

  if ( !theViewManager )
    return;

  connect(theViewManager, SIGNAL(mousePress(SUIT_ViewWindow*, QMouseEvent*)), 
          this, SLOT(onMousePress(SUIT_ViewWindow*, QMouseEvent*)));
  
  connect(theViewManager, SIGNAL(mouseMove(SUIT_ViewWindow*, QMouseEvent*)), 
          this, SLOT(onMouseMove(SUIT_ViewWindow*, QMouseEvent*)));
  
  connect(theViewManager, SIGNAL(mouseRelease(SUIT_ViewWindow*, QMouseEvent*)), 
          this, SLOT(onMouseRelease(SUIT_ViewWindow*, QMouseEvent*)));
}

//==========================================================
void SVTK_Viewer::contextMenuPopup( QPopupMenu* thePopup )
{
  thePopup->insertItem( VTKViewer_Viewer::tr( "MEN_DUMP_VIEW" ), this, SLOT( onDumpView() ) );
  thePopup->insertItem( VTKViewer_Viewer::tr( "MEN_CHANGE_BACKGROUD" ), this, SLOT( onChangeBgColor() ) );

  thePopup->insertSeparator();

  SVTK_ViewWindow* aView = (SVTK_ViewWindow*)(myViewManager->getActiveView());
  if ( aView && !aView->getToolBar()->isVisible() )
    thePopup->insertItem( VTKViewer_Viewer::tr( "MEN_SHOW_TOOLBAR" ), this, SLOT( onShowToolbar() ) );
}

//==========================================================
void SVTK_Viewer::onMousePress(SUIT_ViewWindow* vw, QMouseEvent* event)
{
  if(SVTK_ViewWindow* aVW = dynamic_cast<SVTK_ViewWindow*>(vw)){
    if(SVTK_RenderWindowInteractor* aRWI = aVW->getRWInteractor()){
      switch(event->button()) {
      case LeftButton:
	aRWI->LeftButtonPressed(event) ;
	break ;
      case MidButton:
	aRWI->MiddleButtonPressed(event) ;
	break ;
      case RightButton:
	aRWI->RightButtonPressed(event) ;
	break;
      default:
	break ;
      }
    }
  }
}

//==========================================================
void 
SVTK_Viewer
::onMouseMove(SUIT_ViewWindow* vw, QMouseEvent* event)
{
  if(SVTK_ViewWindow* aVW = dynamic_cast<SVTK_ViewWindow*>(vw)){
    if(SVTK_RenderWindowInteractor* aRWI = aVW->getRWInteractor()){
      aRWI->MouseMove( event );
    }
  }
}

//==========================================================
void 
SVTK_Viewer
::onMouseRelease(SUIT_ViewWindow* vw, QMouseEvent* event)
{
  if(SVTK_ViewWindow* aVW = dynamic_cast<SVTK_ViewWindow*>(vw)){
    if(SVTK_RenderWindowInteractor* aRWI = aVW->getRWInteractor()){
      switch(event->button()) {
      case LeftButton:
	aRWI->LeftButtonReleased(event) ;
	break ;
      case MidButton:
	aRWI->MiddleButtonReleased(event) ;
	break ;
      case RightButton:
	aRWI->RightButtonReleased(event) ;
	break;
      default:
	break ;
      }
    }
  }
}

//==========================================================
void 
SVTK_Viewer
::enableSelection(bool isEnabled)
{
  mySelectionEnabled = isEnabled;
  //!! To be done for view windows
}

//==========================================================
void
SVTK_Viewer
::enableMultiselection(bool isEnable)
{
  myMultiSelectionEnabled = isEnable;
  //!! To be done for view windows
}

void SVTK_Viewer::onDumpView()
{
  SVTK_ViewWindow* aView = dynamic_cast<SVTK_ViewWindow*>(myViewManager->getActiveView());
  if ( aView )
    aView->onDumpView();
}

//==========================================================
void SVTK_Viewer::onChangeBgColor()
{
  SVTK_ViewWindow* aView = dynamic_cast<SVTK_ViewWindow*>(myViewManager->getActiveView());
  if ( aView ) {
    QColor aColor = QColorDialog::getColor( aView->backgroundColor(), aView );
    if ( aColor.isValid() )
      aView->setBackgroundColor( aColor );
  }
}

//==========================================================
void
SVTK_Viewer
::onShowToolbar() 
{
  QPtrVector<SUIT_ViewWindow> aViews = myViewManager->getViews();
  for(int i = 0, iEnd = aViews.size(); i < iEnd; i++)
    if(SUIT_ViewWindow* aViewWindow = aViews.at(i))
      if(SVTK_ViewWindow* aView = dynamic_cast<SVTK_ViewWindow*>(aViewWindow))
	aView->getToolBar()->show();    
}

//==========================================================
void
SVTK_Viewer
::Display( const SALOME_VTKPrs* prs )
{
  // try do downcast object
  if(const SVTK_Prs* aPrs = dynamic_cast<const SVTK_Prs*>( prs )){
    if(aPrs->IsNull())
      return;
    if(vtkActorCollection* anActorCollection = aPrs->GetObjects()){
      // get SALOMEDS Study
      // Temporarily commented to avoid awful dependecy on SALOMEDS
      // TODO: better mechanism of storing display/erse status in a study
      // should be provided...
      // _PTR(Study) aStudy(getStudyDS());
      anActorCollection->InitTraversal();
      while(vtkActor* anActor = anActorCollection->GetNextActor()){
	if(SALOME_Actor* anAct = SALOME_Actor::SafeDownCast(anActor)){
	  // Set visibility flag
          // Temporarily commented to avoid awful dependecy on SALOMEDS
          // TODO: better mechanism of storing display/erse status in a study
          // should be provided...
	  //Handle(SALOME_InteractiveObject) anObj = anAct->getIO();
	  //if(!anObj.IsNull() && anObj->hasEntry() && aStudy){
	  //  ToolsGUI::SetVisibility(aStudy,anObj->getEntry(),true,this);
	  //}
	  // just display the object
	  QPtrVector<SUIT_ViewWindow> aViews = myViewManager->getViews();
	  for(int i = 0, iEnd = aViews.size(); i < iEnd; i++){
	    if(SUIT_ViewWindow* aViewWindow = aViews.at(i)){
	      if(SVTK_ViewWindow* aView = dynamic_cast<SVTK_ViewWindow*>(aViewWindow)){
		if(SVTK_RenderWindowInteractor* aRWI = aView->getRWInteractor()){
		  aRWI->Display(anAct,false);
		  if(anAct->IsSetCamera()){
		    vtkRenderer* aRenderer =  aView->getRenderer();
		    anAct->SetCamera( aRenderer->GetActiveCamera() );
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

//==========================================================
void
SVTK_Viewer
::Erase( const SALOME_VTKPrs* prs, const bool forced )
{
  // try do downcast object
  if(const SVTK_Prs* aPrs = dynamic_cast<const SVTK_Prs*>( prs )){
    if(aPrs->IsNull())
      return;
    if(vtkActorCollection* anActorCollection = aPrs->GetObjects()){
      // get SALOMEDS Study
      // Temporarily commented to avoid awful dependecy on SALOMEDS
      // TODO: better mechanism of storing display/erse status in a study
      // should be provided...
      //_PTR(Study) aStudy(getStudyDS());
      anActorCollection->InitTraversal();
      while(vtkActor* anActor = anActorCollection->GetNextActor())
	if(SALOME_Actor* anAct = SALOME_Actor::SafeDownCast(anActor)){
	  // Set visibility flag
          // Temporarily commented to avoid awful dependecy on SALOMEDS
          // TODO: better mechanism of storing display/erse status in a study
          // should be provided...
	  //Handle(SALOME_InteractiveObject) anObj = anAct->getIO();
	  //if(!anObj.IsNull() && anObj->hasEntry() && aStudy){
	  //  ToolsGUI::SetVisibility(aStudy,anObj->getEntry(),false,this);
	  //}
	  // just display the object
	  QPtrVector<SUIT_ViewWindow> aViews = myViewManager->getViews();
	  for(int i = 0, iEnd = aViews.size(); i < iEnd; i++){
	    if(SUIT_ViewWindow* aViewWindow = aViews.at(i))
	      if(SVTK_ViewWindow* aView = dynamic_cast<SVTK_ViewWindow*>(aViewWindow))
		if(SVTK_RenderWindowInteractor* aRWI = aView->getRWInteractor())
		  if ( forced )
		    aRWI->Remove(anAct,false);
		  else
		    aRWI->Erase(anAct,forced);
	  }
	}
    }
  }
}
  
//==========================================================
void
SVTK_Viewer
::EraseAll( const bool forced )
{
  // Temporarily commented to avoid awful dependecy on SALOMEDS
  // TODO: better mechanism of storing display/erse status in a study
  // should be provided...
  //_PTR(Study) aStudy(getStudyDS());
  QPtrVector<SUIT_ViewWindow> aViews = myViewManager->getViews();
  for(int i = 0, iEnd = aViews.size(); i < iEnd; i++){
    if(SUIT_ViewWindow* aViewWindow = aViews.at(i)){
      if(SVTK_ViewWindow* aView = dynamic_cast<SVTK_ViewWindow*>(aViewWindow)){

	aView->unHighlightAll();

	vtkRenderer* aRenderer =  aView->getRenderer();
	vtkActorCollection* anActorCollection = aRenderer->GetActors();
	anActorCollection->InitTraversal();
	while(vtkActor* anActor = anActorCollection->GetNextActor()){
	  if(SALOME_Actor* anAct = SALOME_Actor::SafeDownCast(anActor)){
	    // Set visibility flag
            // Temporarily commented to avoid awful dependecy on SALOMEDS
            // TODO: better mechanism of storing display/erse status in a study
            // should be provided...
	    //Handle(SALOME_InteractiveObject) anObj = anAct->getIO();
	    //if(!anObj.IsNull() && anObj->hasEntry() && aStudy)
	    //  ToolsGUI::SetVisibility(aStudy,anObj->getEntry(),false,this);
	    if(forced)
	      aRenderer->RemoveActor(anAct);
	    else{
	      // just erase actor
	      anAct->SetVisibility( false );
	      // erase dependent actors
	      vtkActorCollection* aCollection = vtkActorCollection::New();
	      anAct->GetChildActors( aCollection );
	      aCollection->InitTraversal();
	      while(vtkActor* aSubAct = aCollection->GetNextActor())
		aSubAct->SetVisibility( false );
	      aCollection->Delete();
	    }
	  }
	}
      }
    }
  }
  Repaint();
}

//==========================================================
SALOME_Prs* 
SVTK_Viewer
::CreatePrs( const char* entry )
{
  SVTK_Prs* prs = new SVTK_Prs();
  if ( entry ) {
    vtkRenderer* rnr =  ( (SVTK_ViewWindow*) getViewManager()->getActiveView() )->getRenderer();
    vtkActorCollection* theActors = rnr->GetActors();
    theActors->InitTraversal();
    vtkActor* ac;
    while( ( ac = theActors->GetNextActor() ) ) {
      SALOME_Actor* anActor = SALOME_Actor::SafeDownCast( ac );
      if ( anActor && anActor->hasIO() && !strcmp( anActor->getIO()->getEntry(), entry ) ) {
	prs->AddObject( ac );
      }
    }
  }
  return prs;
}

//==========================================================
void
SVTK_Viewer
::BeforeDisplay( SALOME_Displayer* d )
{
  d->BeforeDisplay( this, SALOME_VTKViewType() );
}

//==========================================================
void
SVTK_Viewer::AfterDisplay( SALOME_Displayer* d )
{
  d->AfterDisplay( this, SALOME_VTKViewType() );
}

//==========================================================
bool
SVTK_Viewer
::isVisible( const Handle(SALOME_InteractiveObject)& io )
{
  QPtrVector<SUIT_ViewWindow> aViews = myViewManager->getViews();
  for(int i = 0, iEnd = aViews.size(); i < iEnd; i++)
    if(SUIT_ViewWindow* aViewWindow = aViews.at(i))
      if(SVTK_ViewWindow* aView = dynamic_cast<SVTK_ViewWindow*>(aViewWindow))
	if(SVTK_RenderWindowInteractor* aRWI = aView->getRWInteractor())
	  if(!aRWI->isVisible( io ))
	    return false;
  return true;
}

//==========================================================
void 
SVTK_Viewer
::Repaint()
{
//  if (theUpdateTrihedron) onAdjustTrihedron();
  QPtrVector<SUIT_ViewWindow> aViews = myViewManager->getViews();
  for(int i = 0, iEnd = aViews.size(); i < iEnd; i++)
    if(SUIT_ViewWindow* aViewWindow = aViews.at(i))
      if(SVTK_ViewWindow* aView = dynamic_cast<SVTK_ViewWindow*>(aViewWindow))
	if(SVTK_RenderWindow* aRW = aView->getRenderWindow())
	  aRW->update();
}

void 
SVTK_Viewer
::onSelectionChanged()
{
  emit selectionChanged();
}

