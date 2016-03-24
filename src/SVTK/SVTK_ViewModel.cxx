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

#include <QMenu>
#include <QColorDialog>
#include <QToolBar>

#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkActorCollection.h>

//#include "SUIT_Session.h"
#include "SVTK_ViewModel.h"
#include "SVTK_ViewWindow.h"
#include "SVTK_View.h"
#include "SVTK_Renderer.h"
//#include "SVTK_MainWindow.h"
#include "SVTK_Prs.h"

#include "VTKViewer_Algorithm.h"
#include "VTKViewer_ViewModel.h"

#include "SUIT_ViewModel.h"
#include "SUIT_ViewManager.h"

#include "SALOME_Actor.h"

#include "QtxActionToolMgr.h"
#include "QtxBackgroundTool.h"

// VSR: Uncomment below line to allow texture background support in VTK viewer
#define VTK_ENABLE_TEXTURED_BACKGROUND


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
  myIsStaticTrihedronVisible = true;
  myIncrementSpeed = 10;
  myIncrementMode = 0;
  myProjMode = 0;
  myStereoType = 0;
  myAnaglyphFilter = 0;
  myStyle = 0;
  myZoomingStyle = 0;
  mySelectionEnabled = true;
  myPreSelectionMode = Standard_Preselection;
  mySpaceBtn[0] = 1;
  mySpaceBtn[1] = 2;
  mySpaceBtn[2] = 9;
  myDefaultBackground = Qtx::BackgroundData( Qt::black );
  myQuadBufferSupport = false;
}

/*!
  Destructor
*/
SVTK_Viewer::~SVTK_Viewer() 
{
}

/*! Get data for supported background modes: gradient types, identifiers and supported image formats */
QString SVTK_Viewer::backgroundData( QStringList& gradList, QIntList& idList, QIntList& txtList )
{
  gradList << tr( "GT_HORIZONTALGRADIENT" )
           << tr( "GT_VERTICALGRADIENT" )
           << tr( "GT_FIRSTDIAGONALGRADIENT" )
           << tr( "GT_SECONDDIAGONALGRADIENT" )
           << tr( "GT_FIRSTCORNERGRADIENT" )
           << tr( "GT_SECONDCORNERGRADIENT" )
           << tr( "GT_THIRDCORNERGRADIENT" )
           << tr( "GT_FOURTHCORNERGRADIENT" );
  idList   << HorizontalGradient
           << VerticalGradient
           << FirstDiagonalGradient
           << SecondDiagonalGradient
           << FirstCornerGradient
           << SecondCornerGradient
           << ThirdCornerGradient
           << FourthCornerGradient;
#ifdef VTK_ENABLE_TEXTURED_BACKGROUND
  txtList  << Qtx::CenterTexture << Qtx::TileTexture << Qtx::StretchTexture;
#endif
  return tr("BG_IMAGE_FILES");
}

/*! Get data for supported background modes: gradient types, identifiers and supported image formats */
void SVTK_Viewer::stereoData( QStringList& typeList, QIntList& idList )
{
  typeList << tr("ST_CRYSTALEYES") << tr("ST_REDBLUE")      <<
              tr("ST_INTERLACED")  << tr("ST_LEFT")         <<
              tr("ST_RIGHT")       << tr("ST_DRESDEN")      <<
              tr("ST_ANAGLYPH")    << tr("ST_CHECKERBOARD") <<
              tr("ST_SPLITVIEWPORTHORIZONTAL");
  idList   << CrystalEyesType << RedBlueType      <<
              InterlacedType  << LeftType         <<
              RightType       << DresdenType      <<
              AnaglyphType    << CheckerboardType <<
              SplitViewPortHorizontalType;
}

//! Get background color of the viewer [obsolete]
QColor SVTK_Viewer::backgroundColor() const
{
  return background().color();
}

//! Set background color to the viewer [obsolete]
void SVTK_Viewer::setBackgroundColor( const QColor& c )
{
  Qtx::BackgroundData bg = background();
  bg.setColor( c );
  setBackground( bg );
}

/*!
  \return background data
*/
Qtx::BackgroundData SVTK_Viewer::background() const
{
  return myDefaultBackground;
}

/*!
  Changes background
  \param theBackground - new background data
*/
void SVTK_Viewer::setBackground( const Qtx::BackgroundData& theBackground )
{
  myDefaultBackground = theBackground.isValid() ? theBackground : Qtx::BackgroundData( Qt::black );
  QVector<SUIT_ViewWindow*> aViews = myViewManager->getViews();
  for(int i = 0, iEnd = aViews.size(); i < iEnd; i++){
    if(SUIT_ViewWindow* aViewWindow = aViews.at(i)){
      if(TViewWindow* aView = dynamic_cast<TViewWindow*>(aViewWindow)){
        aView->setBackground(myDefaultBackground);
      }
    }
  }  
}

/*!Create new instance of view window on desktop \a theDesktop.
 *\retval SUIT_ViewWindow* - created view window pointer.
 */
SUIT_ViewWindow* SVTK_Viewer::createView( SUIT_Desktop* theDesktop )
{
  TViewWindow* aViewWindow = new TViewWindow(theDesktop);
  aViewWindow->Initialize(this);

  aViewWindow->setBackground( background() );
  aViewWindow->SetTrihedronSize( trihedronSize(), trihedronRelative() );
  aViewWindow->SetStaticTrihedronVisible( isStaticTrihedronVisible() );
  aViewWindow->SetProjectionMode( projectionMode() );
  aViewWindow->SetStereoType( stereoType() );
  aViewWindow->SetAnaglyphFilter( anaglyphFilter() );
  aViewWindow->SetQuadBufferSupport( isQuadBufferSupport() );
  aViewWindow->SetInteractionStyle( interactionStyle() );
  aViewWindow->SetZoomingStyle( zoomingStyle() );
  aViewWindow->SetPreSelectionMode( preSelectionMode() );
  aViewWindow->SetSelectionEnabled( isSelectionEnabled() );
  aViewWindow->SetIncrementalSpeed( incrementalSpeed(), incrementalSpeedMode() );
  aViewWindow->SetSpacemouseButtons( spacemouseBtn(1), spacemouseBtn(2), spacemouseBtn(3) );

  connect(aViewWindow, SIGNAL( actorAdded(VTKViewer_Actor*) ), 
          this,  SLOT(onActorAdded(VTKViewer_Actor*)));
  connect(aViewWindow, SIGNAL( actorRemoved(VTKViewer_Actor*) ), 
          this,  SLOT(onActorRemoved(VTKViewer_Actor*)));

  return aViewWindow;
}

/*!
  \return trihedron size
*/
double SVTK_Viewer::trihedronSize() const
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
void SVTK_Viewer::setTrihedronSize( const double theSize, const bool theRelative )
{
  myTrihedronSize = theSize;
  myTrihedronRelative = theRelative;

  if (SUIT_ViewManager* aViewManager = getViewManager()) {
    QVector<SUIT_ViewWindow*> aViews = aViewManager->getViews();
    for ( uint i = 0; i < aViews.count(); i++ )
    {
      if ( TViewWindow* aView = dynamic_cast<TViewWindow*>(aViews.at( i )) )
              aView->SetTrihedronSize( theSize, theRelative );
    }
  }
}

/*!
  \return visibility status of the static trihedron
*/
bool SVTK_Viewer::isStaticTrihedronVisible() const
{
  return myIsStaticTrihedronVisible;
}

/*!
  Sets visibility status of the static trihedron
  \param theIsVisible - new visibility status
*/
void SVTK_Viewer::setStaticTrihedronVisible( const bool theIsVisible )
{
  myIsStaticTrihedronVisible = theIsVisible;

  if (SUIT_ViewManager* aViewManager = getViewManager()) {
    QVector<SUIT_ViewWindow*> aViews = aViewManager->getViews();
    for ( uint i = 0; i < aViews.count(); i++ )
    {
      if ( TViewWindow* aView = dynamic_cast<TViewWindow*>(aViews.at( i )) )
        aView->SetStaticTrihedronVisible( theIsVisible );
    }
  }
}

/*!
  \return projection mode
*/
int SVTK_Viewer::projectionMode() const
{
  return myProjMode;
}


/*!
  Sets projection mode: 0 - orthogonal, 1 - perspective projection
  \param theMode - new projection mode
*/
void SVTK_Viewer::setProjectionMode( const int theMode )
{
  if ( myProjMode != theMode ) {
    if ( theMode != SVTK_ViewWindow::Stereo )
      myProjMode = theMode;
    if (SUIT_ViewManager* aViewManager = getViewManager()) {
      QVector<SUIT_ViewWindow*> aViews = aViewManager->getViews();
      for ( uint i = 0; i < aViews.count(); i++ )
      {
        if ( TViewWindow* aView = dynamic_cast<TViewWindow*>(aViews.at( i )) )
          aView->SetProjectionMode( theMode );
      }
    }
  }
}

/*!
  \return stereo type
*/
int SVTK_Viewer::stereoType() const
{
  return myStereoType;
}

/*!
  Sets stereo type
  \param theType - new stereo type
*/
void SVTK_Viewer::setStereoType( const int theType )
{
  if ( myStereoType != theType ) {
    myStereoType = theType;

    if (SUIT_ViewManager* aViewManager = getViewManager()) {
      QVector<SUIT_ViewWindow*> aViews = aViewManager->getViews();
      for ( uint i = 0; i < aViews.count(); i++ )
      {
        if ( TViewWindow* aView = dynamic_cast<TViewWindow*>(aViews.at( i )) )
          aView->SetStereoType( theType );
      }
    }
  }
}

/*!
  \return anaglyph filter
*/
int SVTK_Viewer::anaglyphFilter() const
{
  return myAnaglyphFilter;
}

/*!
  Sets anaglyph filter
  \param theFilter - new anaglyph filter
*/
void SVTK_Viewer::setAnaglyphFilter( const int theFilter )
{
  if ( myAnaglyphFilter != theFilter ) {
	  myAnaglyphFilter = theFilter;

    if (SUIT_ViewManager* aViewManager = getViewManager()) {
      QVector<SUIT_ViewWindow*> aViews = aViewManager->getViews();
      for ( uint i = 0; i < aViews.count(); i++ )
      {
        if ( TViewWindow* aView = dynamic_cast<TViewWindow*>(aViews.at( i )) )
          aView->SetAnaglyphFilter( theFilter );
      }
    }
  }
}

/*!
  \return support quad-buffered stereo
*/
bool SVTK_Viewer::isQuadBufferSupport() const
{
  return myQuadBufferSupport;
}

/*!
  Set support quad-buffered stereo
  \param theEnable - enable/disable support quad-buffered stereo
*/
void SVTK_Viewer::setQuadBufferSupport( const bool theEnable )
{
  if ( myQuadBufferSupport != theEnable ) {
    myQuadBufferSupport = theEnable;

    if (SUIT_ViewManager* aViewManager = getViewManager()) {
      QVector<SUIT_ViewWindow*> aViews = aViewManager->getViews();
      for ( uint i = 0; i < aViews.count(); i++ )
      {
        if ( TViewWindow* aView = dynamic_cast<TViewWindow*>(aViews.at( i )) )
          aView->SetQuadBufferSupport( theEnable );
      }
    }
  }
}
/*!
  \return interaction style
*/
int SVTK_Viewer::interactionStyle() const
{
  return myStyle;
}

/*!
  Sets interaction style: 0 - standard, 1 - keyboard free interaction
  \param theStyle - new interaction style
*/
void SVTK_Viewer::setInteractionStyle( const int theStyle )
{
  myStyle = theStyle;
  
  if (SUIT_ViewManager* aViewManager = getViewManager()) {
    QVector<SUIT_ViewWindow*> aViews = aViewManager->getViews();
    for ( uint i = 0; i < aViews.count(); i++ )
    {
      if ( TViewWindow* aView = dynamic_cast<TViewWindow*>(aViews.at( i )) )
        aView->SetInteractionStyle( theStyle );
    }
  }
}

/*!
  \return zooming style
*/
int SVTK_Viewer::zoomingStyle() const
{
  return myZoomingStyle;
}

/*!
  Sets zooming style: 0 - standard, 1 - advanced (at cursor)
  \param theStyle - new zooming style
*/
void SVTK_Viewer::setZoomingStyle( const int theStyle )
{
  myZoomingStyle = theStyle;
  
  if (SUIT_ViewManager* aViewManager = getViewManager()) {
    QVector<SUIT_ViewWindow*> aViews = aViewManager->getViews();
    for ( uint i = 0; i < aViews.count(); i++ )
    {
      if ( TViewWindow* aView = dynamic_cast<TViewWindow*>(aViews.at( i )) )
        aView->SetZoomingStyle( theStyle );
    }
  }
}

/*!
  \return current preselection mode
*/
Preselection_Mode SVTK_Viewer::preSelectionMode() const
{
  return myPreSelectionMode;
}

/*!
  Sets preselection mode
  \param theMode - new preselection mode
*/
void SVTK_Viewer::setPreSelectionMode( Preselection_Mode theMode )
{
  myPreSelectionMode = theMode;
  
  if (SUIT_ViewManager* aViewManager = getViewManager()) {
    QVector<SUIT_ViewWindow*> aViews = aViewManager->getViews();
    for ( uint i = 0; i < aViews.count(); i++ )
    {
      if ( TViewWindow* aView = dynamic_cast<TViewWindow*>(aViews.at( i )) )
        aView->SetPreSelectionMode( theMode );
    }
  }
}

/*!
  \return incremental speed value
*/
int SVTK_Viewer::incrementalSpeed() const
{
  return myIncrementSpeed;
}

/*!
  \return modification mode of the incremental speed 
*/
int SVTK_Viewer::incrementalSpeedMode() const
{
  return myIncrementMode;
}

/*!
  Set the incremental speed value and modification mode
  \param theValue - new value
  \param theMode - new mode: 0 - arithmetic, 1 - geometrical progression
*/
void SVTK_Viewer::setIncrementalSpeed( const int theValue, const int theMode )
{
  myIncrementSpeed = theValue;
  myIncrementMode = theMode;

  if (SUIT_ViewManager* aViewManager = getViewManager()) {
    QVector<SUIT_ViewWindow*> aViews = aViewManager->getViews();
    for ( uint i = 0; i < aViews.count(); i++ )
    {
      if ( TViewWindow* aView = dynamic_cast<TViewWindow*>(aViews.at( i )) )
        aView->SetIncrementalSpeed( theValue, theMode );
    }
  }
}

/*!
  \return spacemouse button assigned to the specified function
  \param theIndex - function by number (from 1 to 3)
*/
int SVTK_Viewer::spacemouseBtn( const int theIndex ) const
{
  if ( theIndex < 1 || theIndex > 3 ) 
    return -1;
  return mySpaceBtn[theIndex-1];
}

/*!
  Set the spacemouse buttons
  \param theBtn1, theBtn2, theBtn3 - new buttons
*/
void SVTK_Viewer::setSpacemouseButtons( const int theBtn1, const int theBtn2, const int theBtn3 )
{
  mySpaceBtn[0] = theBtn1;
  mySpaceBtn[1] = theBtn2;
  mySpaceBtn[2] = theBtn3;

  if (SUIT_ViewManager* aViewManager = getViewManager()) {
    QVector<SUIT_ViewWindow*> aViews = aViewManager->getViews();
    for ( uint i = 0; i < aViews.count(); i++ )
    {
      if ( TViewWindow* aView = dynamic_cast<TViewWindow*>(aViews.at( i )) )
        aView->SetSpacemouseButtons( theBtn1, theBtn2, theBtn3 );
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
void SVTK_Viewer::contextMenuPopup( QMenu* thePopup )
{
  thePopup->addAction( VTKViewer_Viewer::tr( "MEN_DUMP_VIEW" ), this, SLOT( onDumpView() ) );
  thePopup->addAction( VTKViewer_Viewer::tr( "MEN_CHANGE_BACKGROUND" ), this, SLOT( onChangeBackground() ) );

  thePopup->addSeparator();

  if(TViewWindow* aView = dynamic_cast<TViewWindow*>(myViewManager->getActiveView())){
    //Support of several toolbars in the popup menu
    QList<QToolBar*> lst = aView->findChildren<QToolBar*>();
    QList<QToolBar*>::const_iterator it = lst.begin(), last = lst.end();
    for( ; it!=last; it++ )
      thePopup->addAction( (*it)->toggleViewAction() );
    aView->RefreshDumpImage();
  }
}

/*!
  SLOT: called on mouse button press, empty implementation
*/
void SVTK_Viewer::onMousePress(SUIT_ViewWindow* vw, QMouseEvent* event)
{}

/*!
  SLOT: called on mouse move, empty implementation
*/
void SVTK_Viewer::onMouseMove(SUIT_ViewWindow* vw, QMouseEvent* event)
{}

/*!
  SLOT: called on mouse button release, empty implementation
*/
void SVTK_Viewer::onMouseRelease(SUIT_ViewWindow* vw, QMouseEvent* event)
{}

/*!
  Enables/disables selection
  \param isEnabled - new state
*/
void SVTK_Viewer::enableSelection(bool isEnabled)
{
  mySelectionEnabled = isEnabled;
  //!! To be done for view windows
   
  if (SUIT_ViewManager* aViewManager = getViewManager()) {
    QVector<SUIT_ViewWindow*> aViews = aViewManager->getViews();
    for ( uint i = 0; i < aViews.count(); i++ )
    {
      if ( TViewWindow* aView = dynamic_cast<TViewWindow*>(aViews.at( i )) )
        aView->SetSelectionEnabled( isEnabled );
    }
  }

  if(!isEnabled) {
    //clear current selection in the viewer
    bool blocked = blockSignals( true );
    if ( SUIT_ViewManager* aViewMgr = getViewManager() ) {
      if( SVTK_ViewWindow* aViewWindow = dynamic_cast<SVTK_ViewWindow*>( aViewMgr->getActiveView() ) ){
	if( SVTK_Selector* aSelector = aViewWindow->GetSelector() ) {
	  if(SVTK_View* aView = aViewWindow->getView()){
	    aSelector->ClearIObjects();
	    aView->onSelectionChanged();
	  }
	}
      }
    }
    blockSignals( blocked );  
  }

}

/*!
  Enables/disables selection of many object
  \param isEnabled - new state
*/
void SVTK_Viewer::enableMultiselection(bool isEnable)
{
  myMultiSelectionEnabled = isEnable;
  //!! To be done for view windows
}

/*!
  SLOT: called on dump view operation is activated, stores scene to raster file
*/
void SVTK_Viewer::onDumpView()
{
  if(SUIT_ViewWindow* aView = myViewManager->getActiveView())
    aView->onDumpView();
}

/*!
  SLOT: called if background color is to be changed changed, passes new color to view port
*/
void SVTK_Viewer::onChangeBackground()
{
  SVTK_ViewWindow* aView = dynamic_cast<SVTK_ViewWindow*>(myViewManager->getActiveView());
  if ( !aView )
    return;

  // get supported gradient types
  QStringList gradList;
  QIntList    idList, txtList;
  QString     formats = backgroundData( gradList, idList, txtList );

  // invoke dialog box
  Qtx::BackgroundData bgData = QtxBackgroundDialog::getBackground( aView->background(),  // initial background
								   aView,                // parent for dialog box
								   txtList,              // allowed texture modes
								   true,                 // enable solid color mode
								   true,                 // enable gradient mode
								   false,                // disable custom gradient mode
								   !txtList.isEmpty(),   // enable texture mode
								   gradList,             // gradient names
								   idList,               // gradient identifiers
								   formats );            // image formats

  // set chosen background data to the viewer
  if ( bgData.isValid() )
    aView->setBackground( bgData );
}

/*!
  Display presentation
  \param prs - presentation
*/
void SVTK_Viewer::Display( const SALOME_VTKPrs* prs )
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
          if(!anAct->ShouldBeDisplayed())
            continue;
          // Set visibility flag
          // Temporarily commented to avoid awful dependecy on SALOMEDS
          // TODO: better mechanism of storing display/erse status in a study
          // should be provided...
          //Handle(SALOME_InteractiveObject) anObj = anAct->getIO();
          //if(!anObj.IsNull() && anObj->hasEntry() && aStudy){
          //  ToolsGUI::SetVisibility(aStudy,anObj->getEntry(),true,this);
          //}
          // just display the object
          QVector<SUIT_ViewWindow*> aViews = myViewManager->getViews();
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
void SVTK_Viewer::Erase( const SALOME_VTKPrs* prs, const bool forced )
{
  // try do downcast object
  if(const SVTK_Prs* aPrs = dynamic_cast<const SVTK_Prs*>( prs )){
    if(aPrs->IsNull())
      return;
    if(vtkActorCollection* anActorCollection = aPrs->GetObjects()){
      // get SALOMEDS Study
      // Temporarily commented to avoid awful dependecy on SALOMEDS
      // TODO: better mechanism of storing display/erase status in a study
      // should be provided...
      //_PTR(Study) aStudy(getStudyDS());
      anActorCollection->InitTraversal();
      while(vtkActor* anActor = anActorCollection->GetNextActor())
        if(SALOME_Actor* anAct = SALOME_Actor::SafeDownCast(anActor)){
          // Set visibility flag
          // Temporarily commented to avoid awful dependecy on SALOMEDS
          // TODO: better mechanism of storing display/erase status in a study
          // should be provided...
          //Handle(SALOME_InteractiveObject) anObj = anAct->getIO();
          //if(!anObj.IsNull() && anObj->hasEntry() && aStudy){
          //  ToolsGUI::SetVisibility(aStudy,anObj->getEntry(),false,this);
          //}
          // just display the object
          QVector<SUIT_ViewWindow*> aViews = myViewManager->getViews();
          for(int i = 0, iEnd = aViews.size(); i < iEnd; i++){
            if(SVTK_ViewWindow* aViewWindow = dynamic_cast<SVTK_ViewWindow*>(aViews.at(i)))
              if(SVTK_View* aView = aViewWindow->getView())
              {
                if ( forced )
                  aView->Remove(anAct,false);
                else
                  aView->Erase(anAct,forced);
              }
          }
        }
    }
  }
}

/*!
  Erase all presentations
  \param forced - removes all objects from view
*/
void SVTK_Viewer::EraseAll( SALOME_Displayer* d, const bool forced )
{
  // Temporarily commented to avoid awful dependecy on SALOMEDS
  // TODO: better mechanism of storing display/erse status in a study
  // should be provided...
  //_PTR(Study) aStudy(getStudyDS());
  QVector<SUIT_ViewWindow*> aViews = myViewManager->getViews();
  for(int i = 0, iEnd = aViews.size(); i < iEnd; i++){
    if(SVTK_ViewWindow* aViewWindow = dynamic_cast<SVTK_ViewWindow*>(aViews.at(i)))
      if(SVTK_View* aView = aViewWindow->getView()){
        vtkRenderer* aRenderer =  aView->getRenderer();
        VTK::ActorCollectionCopy aCopy(aRenderer->GetActors());
        vtkActorCollection* anActorCollection = aCopy.GetActors();
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
            if(forced){
              if(SVTK_Renderer* aRnd = aView->GetRenderer())
                aRnd->RemoveActor(anAct);
            }else{
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

  SALOME_View::EraseAll( d, forced );

  Repaint();
}

/*!
  Create presentation corresponding to the entry
  \param entry - entry
*/
SALOME_Prs* SVTK_Viewer::CreatePrs( const char* entry )
{
  SVTK_Prs* prs = new SVTK_Prs( entry );
  if ( entry ) {
    if(SVTK_ViewWindow* aViewWindow = dynamic_cast<SVTK_ViewWindow*>(getViewManager()->getActiveView()))
      if(SVTK_View* aView = aViewWindow->getView()){
        vtkRenderer* aRenderer =  aView->getRenderer();
        VTK::ActorCollectionCopy aCopy(aRenderer->GetActors());
        vtkActorCollection* theActors = aCopy.GetActors();
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
  \return true if object is displayed in viewer
  \param obj - object to be checked
*/
bool SVTK_Viewer::isVisible( const Handle(SALOME_InteractiveObject)& io )
{
  QVector<SUIT_ViewWindow*> aViews = myViewManager->getViews();
  for(int i = 0, iEnd = aViews.size(); i < iEnd; i++)
    if(SUIT_ViewWindow* aViewWindow = aViews.at(i))
      if(TViewWindow* aViewWnd = dynamic_cast<TViewWindow*>(aViewWindow))
        if(SVTK_View* aView = aViewWnd->getView())
          if(!aView->isVisible( io ))
            return false;

  return true;
}

/*!
  \Collect objects visible in viewer
  \param theList - visible objects collection
*/
void SVTK_Viewer::GetVisible( SALOME_ListIO& theList )
{
  // here we collect object if it is disaplaed even one view
  QVector<SUIT_ViewWindow*> aViews = myViewManager->getViews();
  for(int i = 0, iEnd = aViews.size(); i < iEnd; i++)
    if(SUIT_ViewWindow* aViewWindow = aViews.at(i))
      if(TViewWindow* aViewWnd = dynamic_cast<TViewWindow*>(aViewWindow))
        if(SVTK_View* aView = aViewWnd->getView())
          aView->GetVisible( theList );
}

/*!
  Updates current viewer
*/
void SVTK_Viewer::Repaint()
{
//  if (theUpdateTrihedron) onAdjustTrihedron();
  QVector<SUIT_ViewWindow*> aViews = myViewManager->getViews();
  for(int i = 0, iEnd = aViews.size(); i < iEnd; i++)
    if(TViewWindow* aViewWindow = dynamic_cast<TViewWindow*>(aViews.at(i)))
      if(SVTK_View* aView = aViewWindow->getView())
        aView->Repaint();
}
 

void SVTK_Viewer::onActorAdded(VTKViewer_Actor* theActor)
{
  emit actorAdded((SVTK_ViewWindow*)sender(), theActor);
}

void SVTK_Viewer::onActorRemoved(VTKViewer_Actor* theActor)
{
  emit actorRemoved((SVTK_ViewWindow*)sender(), theActor);
}
