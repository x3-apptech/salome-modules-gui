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
#include <qpopupmenu.h>
#include <qcolordialog.h>

#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkActorCollection.h>

#include "SUIT_Session.h"

#include "SVTK_Selection.h"
#include "SVTK_ViewModel.h"
#include "SVTK_ViewWindow.h"
#include "SVTK_View.h"
#include "SVTK_MainWindow.h"
#include "SVTK_Prs.h"

#include "VTKViewer_ViewModel.h"

#include <SALOME_Actor.h>
#include <SALOME_InteractiveObject.hxx>

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

/*!
  Constructor
*/
SVTK_Viewer::SVTK_Viewer()
{
  myTrihedronSize = 105;
  myTrihedronRelative = true;
}

/*!
  Destructor
*/
SVTK_Viewer::~SVTK_Viewer() 
{
}

/*!
  \return background color
*/
QColor
SVTK_Viewer
::backgroundColor() const
{
  return myBgColor;
}

/*!
  Changes background color
  \param theColor - new background color
*/
void
SVTK_Viewer
::setBackgroundColor( const QColor& theColor )
{
  if ( !theColor.isValid() )
    return;

  QPtrVector<SUIT_ViewWindow> aViews = myViewManager->getViews();
  for(int i = 0, iEnd = aViews.size(); i < iEnd; i++){
    if(SUIT_ViewWindow* aViewWindow = aViews.at(i)){
      if(TViewWindow* aView = dynamic_cast<TViewWindow*>(aViewWindow)){
	aView->setBackgroundColor(theColor);
      }
    }
  }

  myBgColor = theColor;
}

/*!Create new instance of view window on desktop \a theDesktop.
 *\retval SUIT_ViewWindow* - created view window pointer.
 */
SUIT_ViewWindow*
SVTK_Viewer::
createView( SUIT_Desktop* theDesktop )
{
  TViewWindow* aViewWindow = new TViewWindow(theDesktop);
  aViewWindow->Initialize(this);

  aViewWindow->setBackgroundColor( backgroundColor() );
  aViewWindow->SetTrihedronSize( trihedronSize(), trihedronRelative() );

  return aViewWindow;
}

/*!
  \return trihedron size
*/
int SVTK_Viewer::trihedronSize() const
{
  return myTrihedronSize;
}

/*!
  \return true if thihedron changes size in accordance with bounding box
*/
bool SVTK_Viewer::trihedronRelative() const
{
  return myTrihedronRelative;
}

/*!
  Sets trihedron size and relativeness( whether thihedron changes size in accordance with bounding box)
  \param theSize - new size
  \param theRelative - new relativeness
*/
void SVTK_Viewer::setTrihedronSize( const int theSize, const bool theRelative )
{
  myTrihedronSize = theSize;
  myTrihedronRelative = theRelative;

  if (SUIT_ViewManager* aViewManager = getViewManager()) {
    QPtrVector<SUIT_ViewWindow> aViews = aViewManager->getViews();
    for (int i = 0; i < aViews.count(); i++) {
      if (TViewWindow* aView = dynamic_cast<TViewWindow*>(aViews.at(i))) {
	aView->SetTrihedronSize(theSize, theRelative);
      }
    }
  }
}

/*!
  Sets new view manager
  \param theViewManager - new view manager
*/
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

/*!
  Builds popup for vtk viewer
*/
void
SVTK_Viewer
::contextMenuPopup( QPopupMenu* thePopup )
{
  thePopup->insertItem( VTKViewer_Viewer::tr( "MEN_DUMP_VIEW" ), this, SLOT( onDumpView() ) );
  thePopup->insertItem( VTKViewer_Viewer::tr( "MEN_CHANGE_BACKGROUD" ), this, SLOT( onChangeBgColor() ) );

  thePopup->insertSeparator();

  if(TViewWindow* aView = dynamic_cast<TViewWindow*>(myViewManager->getActiveView())){
    if ( !aView->getMainWindow()->getToolBar()->isVisible() ){
      thePopup->insertItem( VTKViewer_Viewer::tr( "MEN_SHOW_TOOLBAR" ), this, SLOT( onShowToolbar() ) );
    }
  }
}

/*!
  SLOT: called on mouse button press, empty implementation
*/
void 
SVTK_Viewer
::onMousePress(SUIT_ViewWindow* vw, QMouseEvent* event)
{}

/*!
  SLOT: called on mouse move, empty implementation
*/
void 
SVTK_Viewer
::onMouseMove(SUIT_ViewWindow* vw, QMouseEvent* event)
{}

/*!
  SLOT: called on mouse button release, empty implementation
*/
void 
SVTK_Viewer
::onMouseRelease(SUIT_ViewWindow* vw, QMouseEvent* event)
{}

/*!
  Enables/disables selection
  \param isEnabled - new state
*/
void 
SVTK_Viewer
::enableSelection(bool isEnabled)
{
  mySelectionEnabled = isEnabled;
  //!! To be done for view windows
}

/*!
  Enables/disables selection of many object
  \param isEnabled - new state
*/
void
SVTK_Viewer
::enableMultiselection(bool isEnable)
{
  myMultiSelectionEnabled = isEnable;
  //!! To be done for view windows
}

/*!
  SLOT: called on dump view operation is activated, stores scene to raster file
*/
void
SVTK_Viewer
::onDumpView()
{
  if(SUIT_ViewWindow* aView = myViewManager->getActiveView())
    aView->onDumpView();
}

/*!
  SLOT: called if background color is to be changed changed, passes new color to view port
*/
void
SVTK_Viewer
::onChangeBgColor()
{
  if(SUIT_ViewWindow* aView = myViewManager->getActiveView()){
    QColor aColor = QColorDialog::getColor( backgroundColor(), aView);
    setBackgroundColor(aColor);
  }
}

/*!
  SLOT: called when popup item "Show toolbar" is activated, shows toolbar of active view window
*/
void
SVTK_Viewer
::onShowToolbar() 
{
  QPtrVector<SUIT_ViewWindow> aViews = myViewManager->getViews();
  for(int i = 0, iEnd = aViews.size(); i < iEnd; i++){
    if(TViewWindow* aView = dynamic_cast<TViewWindow*>(aViews.at(i))){
      aView->getMainWindow()->getToolBar()->show();
    }
  }
}

/*!
  Display presentation
  \param prs - presentation
*/
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
	    if(SVTK_ViewWindow* aViewWindow = dynamic_cast<SVTK_ViewWindow*>(aViews.at(i))){
	      if(SVTK_View* aView = aViewWindow->getView()){
		aView->Display(anAct,false);
		if(anAct->IsSetCamera()){
		  vtkRenderer* aRenderer = aView->getRenderer();
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

/*!
  Erase presentation
  \param prs - presentation
  \param forced - removes object from view
*/
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
	    if(SVTK_ViewWindow* aViewWindow = dynamic_cast<SVTK_ViewWindow*>(aViews.at(i)))
	      if(SVTK_View* aView = aViewWindow->getView())
		if ( forced )
		  aView->Remove(anAct,false);
		else
		  aView->Erase(anAct,forced);
	  }
	}
    }
  }
}

/*!
  Erase all presentations
  \param forced - removes all objects from view
*/
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
    if(SVTK_ViewWindow* aViewWindow = dynamic_cast<SVTK_ViewWindow*>(aViews.at(i)))
      if(SVTK_View* aView = aViewWindow->getView()){
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
  Repaint();
}

/*!
  Create presentation corresponding to the entry
  \param entry - entry
*/
SALOME_Prs* 
SVTK_Viewer
::CreatePrs( const char* entry )
{
  SVTK_Prs* prs = new SVTK_Prs();
  if ( entry ) {
    if(SVTK_ViewWindow* aViewWindow = dynamic_cast<SVTK_ViewWindow*>(getViewManager()->getActiveView()))
      if(SVTK_View* aView = aViewWindow->getView()){
	vtkRenderer* aRenderer =  aView->getRenderer();
	vtkActorCollection* theActors = aRenderer->GetActors();
	theActors->InitTraversal();
	vtkActor* ac;
	while( ( ac = theActors->GetNextActor() ) ) {
	  SALOME_Actor* anActor = SALOME_Actor::SafeDownCast( ac );
	  if ( anActor && anActor->hasIO() && !strcmp( anActor->getIO()->getEntry(), entry ) ) {
	    prs->AddObject( ac );
	  }
	}
      }
  }
  return prs;
}

/*!
  Auxiliary method called before displaying of objects
*/
void
SVTK_Viewer
::BeforeDisplay( SALOME_Displayer* d )
{
  d->BeforeDisplay( this, SALOME_VTKViewType() );
}

/*!
  Auxiliary method called after displaying of objects
*/
void
SVTK_Viewer::AfterDisplay( SALOME_Displayer* d )
{
  d->AfterDisplay( this, SALOME_VTKViewType() );
}

/*!
  \return true if object is displayed in viewer
  \param obj - object to be checked
*/
bool
SVTK_Viewer
::isVisible( const Handle(SALOME_InteractiveObject)& io )
{
  QPtrVector<SUIT_ViewWindow> aViews = myViewManager->getViews();
  for(int i = 0, iEnd = aViews.size(); i < iEnd; i++)
    if(SUIT_ViewWindow* aViewWindow = aViews.at(i))
      if(TViewWindow* aViewWnd = dynamic_cast<TViewWindow*>(aViewWindow))
        if(SVTK_View* aView = aViewWnd->getView())
          if(!aView->isVisible( io ))
            return false;

  return true;
}

/*!
  Updates current viewer
*/
void 
SVTK_Viewer
::Repaint()
{
//  if (theUpdateTrihedron) onAdjustTrihedron();
  QPtrVector<SUIT_ViewWindow> aViews = myViewManager->getViews();
  for(int i = 0, iEnd = aViews.size(); i < iEnd; i++)
    if(TViewWindow* aViewWindow = dynamic_cast<TViewWindow*>(aViews.at(i)))
      if(SVTK_View* aView = aViewWindow->getView())
	aView->Repaint();
}
