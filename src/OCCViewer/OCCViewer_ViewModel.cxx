// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#include "OCCViewer_ViewModel.h"
#include "OCCViewer_ViewWindow.h"
#include "OCCViewer_ViewFrame.h"
#include "OCCViewer_VService.h"
#include "OCCViewer_ViewPort3d.h"

#include "SUIT_ViewWindow.h"
#include "SUIT_ViewManager.h"
#include "SUIT_Desktop.h"
#include "SUIT_Session.h"
#include "SUIT_ResourceMgr.h"

#include "QtxActionToolMgr.h"
#include "QtxBackgroundTool.h"

#include <QPainter>
#include <QApplication>
#include <QColorDialog>
#include <QFileDialog>
#include <QPalette>
#include <QKeyEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QToolBar>
#include <QDesktopWidget>

#include <AIS_Axis.hxx>
#include <AIS_Drawer.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>

#include <Geom_Axis2Placement.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_LengthAspect.hxx>
#include <Prs3d_AngleAspect.hxx>
#include <Prs3d_TextAspect.hxx>

#include <Visual3d_View.hxx>

#include <Basics_OCCTVersion.hxx>

// VSR: Uncomment below line to allow texture background support in OCC viewer
#define OCC_ENABLE_TEXTURED_BACKGROUND

/*!
  Get data for supported background modes: gradient types, identifiers and supported image formats
*/
QString OCCViewer_Viewer::backgroundData( QStringList& gradList, QIntList& idList, QIntList& txtList )
{
  gradList << tr("GT_HORIZONTALGRADIENT")    << tr("GT_VERTICALGRADIENT")       <<
              tr("GT_FIRSTDIAGONALGRADIENT") << tr("GT_SECONDDIAGONALGRADIENT") <<
              tr("GT_FIRSTCORNERGRADIENT")   << tr("GT_SECONDCORNERGRADIENT")   <<
              tr("GT_THIRDCORNERGRADIENT")   << tr("GT_FORTHCORNERGRADIENT");
  idList   << HorizontalGradient             << VerticalGradient  <<
              Diagonal1Gradient              << Diagonal2Gradient <<
              Corner1Gradient                << Corner2Gradient   <<
              Corner3Gradient                << Corner4Gradient;
#if OCC_VERSION_LARGE > 0x06050200 // enabled since OCCT 6.5.3, since in previous version this functionality is buggy
#ifdef OCC_ENABLE_TEXTURED_BACKGROUND
  txtList  << Qtx::CenterTexture << Qtx::TileTexture << Qtx::StretchTexture;
#endif
#endif
  return tr("BG_IMAGE_FILES");
}

/*!
  Constructor
  \param DisplayTrihedron - is trihedron displayed
*/
OCCViewer_Viewer::OCCViewer_Viewer( bool DisplayTrihedron)
: SUIT_ViewModel(),
  myBackgrounds(4, Qtx::BackgroundData( Qt::black )),
  myIsRelative(true),
  myTopLayerId( 0 ),
  myTrihedronSize(100)
{
  // init CasCade viewers
  myV3dViewer = OCCViewer_VService::Viewer3d( "", (short*) "Viewer3d", "", 1000.,
                                              V3d_XposYnegZpos, true, true );

  myV3dViewer->Init();

  myV3dCollector = OCCViewer_VService::Viewer3d( "", (short*) "Collector3d", "", 1000.,
                                                 V3d_XposYnegZpos, true, true );
  myV3dCollector->Init();

  // init selector
  myAISContext = new AIS_InteractiveContext( myV3dViewer, myV3dCollector);

  myAISContext->SelectionColor( Quantity_NOC_WHITE );
  
  // display isoline on planar faces (box for ex.)
  myAISContext->IsoOnPlane( true );

  double h = QApplication::desktop()->screenGeometry( QApplication::desktop()->primaryScreen() ).height() / 300. ;
  Handle(Prs3d_Drawer) drawer = myAISContext->DefaultDrawer();
  Handle(Prs3d_TextAspect) ta = drawer->TextAspect();
  ta->SetHeight(100); // VSR: workaround for CAS.CADE bug (is it really needed ???)
  ta->SetHeight(h);
  drawer->SetTextAspect(ta);
  drawer->AngleAspect()->SetTextAspect(ta);
  drawer->LengthAspect()->SetTextAspect(ta);
  
  /* create trihedron */
  if( DisplayTrihedron )
  {
    Handle(Geom_Axis2Placement) anAxis = new Geom_Axis2Placement(gp::XOY());
    myTrihedron = new AIS_Trihedron(anAxis);
    myTrihedron->SetInfiniteState( Standard_True );

    Quantity_Color Col(193/255., 205/255., 193/255., Quantity_TOC_RGB);
    //myTrihedron->SetColor( Col );
    myTrihedron->SetArrowColor( Col.Name() );
    myTrihedron->SetSize(100);
    Handle(AIS_Drawer) drawer = myTrihedron->Attributes();
    if (drawer->HasDatumAspect()) {
        Handle(Prs3d_DatumAspect) daspect = drawer->DatumAspect();
        daspect->FirstAxisAspect()->SetColor(Quantity_Color(1.0, 0.0, 0.0, Quantity_TOC_RGB));
        daspect->SecondAxisAspect()->SetColor(Quantity_Color(0.0, 1.0, 0.0, Quantity_TOC_RGB));
        daspect->ThirdAxisAspect()->SetColor(Quantity_Color(0.0, 0.0, 1.0, Quantity_TOC_RGB));
    }

    myAISContext->Display(myTrihedron);
    myAISContext->Deactivate(myTrihedron);
    }

  // set interaction style to standard
  myInteractionStyle = 0;

  // set zooming style to standard
  myZoomingStyle = 0;

  // selection
  mySelectionEnabled = true;
  myMultiSelectionEnabled = true;
  
  
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  if(resMgr)
    myShowStaticTrihedron = resMgr->booleanValue( "OCCViewer", "show_static_trihedron", true );
}

/*!
  Destructor
*/
OCCViewer_Viewer::~OCCViewer_Viewer() 
{
  myAISContext.Nullify();
  myV3dViewer.Nullify();
  myV3dCollector.Nullify();
}

/*!
  [obsolete]
  \return background color of viewer
*/
QColor OCCViewer_Viewer::backgroundColor() const
{
  return backgroundColor(0);
}

/*!
  \return background data of viewer
*/
Qtx::BackgroundData OCCViewer_Viewer::background() const
{
  return background(0);
}

/*!
  Sets background color [obsolete]
  \param c - new background color
*/
void OCCViewer_Viewer::setBackgroundColor( const QColor& c )
{
  setBackgroundColor( 0, c );
}

/*!
  Sets background data
  \param d - new background data
*/
void OCCViewer_Viewer::setBackground( const Qtx::BackgroundData& theBackground )
{
  setBackground( 0, theBackground );
}

/*!
  Start initialization of view window
  \param view - view window to be initialized
*/
void OCCViewer_Viewer::initView( OCCViewer_ViewWindow* view )
{
  if ( view ) {
    view->initLayout();
    view->initSketchers();
    view->setInteractionStyle( interactionStyle() );
    view->setZoomingStyle( zoomingStyle() );
    
    OCCViewer_ViewPort3d* vp3d = view->getViewPort();
    if ( vp3d )
    {
      vp3d->getView()->SetSurfaceDetail(V3d_TEX_ALL);
    }
  }
}

/*!
  Creates new view window
  \param theDesktop - main window of application
*/
SUIT_ViewWindow* OCCViewer_Viewer::createView( SUIT_Desktop* theDesktop )
{
  // create view frame
  OCCViewer_ViewFrame* view = new OCCViewer_ViewFrame(theDesktop, this);
  // get main view window (created by view frame)
  OCCViewer_ViewWindow* vw = view->getView(OCCViewer_ViewFrame::MAIN_VIEW);
  // initialize main view window
  initView( vw );
  // set default background for view window
  vw->setBackground( background(0) ); // 0 means MAIN_VIEW (other views are not yet created here)
  // connect signal from viewport
  connect(view->getViewPort(), SIGNAL(vpClosed()), this, SLOT(onViewClosed()));
  return view;
}

/*!
  Sets new view manager
  \param theViewManager - new view manager
*/
void OCCViewer_Viewer::setViewManager(SUIT_ViewManager* theViewManager)
{
  SUIT_ViewModel::setViewManager(theViewManager);
  if (theViewManager) {
    connect(theViewManager, SIGNAL(mousePress(SUIT_ViewWindow*, QMouseEvent*)), 
            this, SLOT(onMousePress(SUIT_ViewWindow*, QMouseEvent*)));

    connect(theViewManager, SIGNAL(mouseMove(SUIT_ViewWindow*, QMouseEvent*)), 
            this, SLOT(onMouseMove(SUIT_ViewWindow*, QMouseEvent*)));

    connect(theViewManager, SIGNAL(mouseRelease(SUIT_ViewWindow*, QMouseEvent*)), 
            this, SLOT(onMouseRelease(SUIT_ViewWindow*, QMouseEvent*)));

    connect(theViewManager, SIGNAL(keyPress(SUIT_ViewWindow*, QKeyEvent*)), 
            this, SLOT(onKeyPress(SUIT_ViewWindow*, QKeyEvent*)));
  }
}

/*!
  SLOT: called on mouse button press, stores current mouse position as start point for transformations
*/
void OCCViewer_Viewer::onMousePress(SUIT_ViewWindow* theWindow, QMouseEvent* theEvent)
{
  myStartPnt.setX(theEvent->x()); myStartPnt.setY(theEvent->y());
}

/*!
  SLOT: called on mouse move, processes transformation or hilighting
*/
void OCCViewer_Viewer::onMouseMove(SUIT_ViewWindow* theWindow, QMouseEvent* theEvent)
{
  if (!mySelectionEnabled) return;
  if (!theWindow->inherits("OCCViewer_ViewWindow")) return;

  OCCViewer_ViewWindow* aView = (OCCViewer_ViewWindow*) theWindow;

  if ( isSelectionEnabled() ) {
    if (aView->getViewPort()->isBusy()) {
      QCoreApplication::processEvents();
      return; // Check that the ViewPort initialization completed
                                                // To Prevent call move event if the View port is not initialized
                                                // IPAL 20883
    }
    Handle(V3d_View) aView3d = aView->getViewPort()->getView();
    if ( !aView3d.IsNull() ) {
      myAISContext->MoveTo(theEvent->x(), theEvent->y(), aView3d);
    }
  }
}


/*!
  SLOT: called on mouse button release, finishes transformation or selection
*/
void OCCViewer_Viewer::onMouseRelease(SUIT_ViewWindow* theWindow, QMouseEvent* theEvent)
{
  if (!mySelectionEnabled) return;
  if (theEvent->button() != Qt::LeftButton) return;
  if (!theWindow->inherits("OCCViewer_ViewWindow")) return;

  OCCViewer_ViewWindow* aView = (OCCViewer_ViewWindow*) theWindow;
  if (!aView || aView->interactionStyle() != SUIT_ViewModel::STANDARD)
    return;

  myEndPnt.setX(theEvent->x()); myEndPnt.setY(theEvent->y());
  bool aHasShift = (theEvent->modifiers() & Qt::ShiftModifier);
  
  if (!aHasShift) emit deselection();

  if (myStartPnt == myEndPnt)
  {
    if (aHasShift && myMultiSelectionEnabled)
      myAISContext->ShiftSelect();
    else
      myAISContext->Select();
  }
  else
  {
    if (aHasShift && myMultiSelectionEnabled)
      myAISContext->ShiftSelect(myStartPnt.x(), myStartPnt.y(),
                                myEndPnt.x(), myEndPnt.y(),
                                aView->getViewPort()->getView(), Standard_False );
    else
      myAISContext->Select(myStartPnt.x(), myStartPnt.y(),
                           myEndPnt.x(), myEndPnt.y(),
                           aView->getViewPort()->getView(), Standard_False );

    int Nb = myAISContext->NbSelected();
    if( Nb>1 && !myMultiSelectionEnabled )
    {
        myAISContext->InitSelected();
        Handle( SelectMgr_EntityOwner ) anOwner = myAISContext->SelectedOwner();
        if( !anOwner.IsNull() )
        {
            myAISContext->ClearSelected( Standard_False );
            myAISContext->AddOrRemoveSelected( anOwner, Standard_False );
        }
    }

    myAISContext->UpdateCurrentViewer();
  }
  emit selectionChanged();
}

/*!
  SLOT: called on key press, processes selection in "key free" interaction style
*/
void OCCViewer_Viewer::onKeyPress(SUIT_ViewWindow* theWindow, QKeyEvent* theEvent)
{
  if (!mySelectionEnabled) return;
  if (theEvent->key() != Qt::Key_S) return;
  if (!theWindow->inherits("OCCViewer_ViewWindow")) return;

  OCCViewer_ViewWindow* aView = (OCCViewer_ViewWindow*) theWindow;
  if (!aView || aView->interactionStyle() != SUIT_ViewModel::KEY_FREE)
    return;

  emit deselection();
  myAISContext->Select();

  emit selectionChanged();
}

void OCCViewer_Viewer::onViewClosed()
{
  Standard_Integer aViewsNb = 0;
  for ( myV3dViewer->InitActiveViews(); myV3dViewer->MoreActiveViews(); myV3dViewer->NextActiveViews())
    ++aViewsNb;
  if ( aViewsNb < 2 ) {
    //clean up presentations before last view is closed
    myAISContext->RemoveAll(Standard_False);
  }
}

int OCCViewer_Viewer::getTopLayerId()
{
#if OCC_VERSION_LARGE > 0x06050200
  if ( myTopLayerId == 0 && !myAISContext->CurrentViewer().IsNull() )    
    myAISContext->CurrentViewer()->AddZLayer( myTopLayerId );
#endif

  return myTopLayerId;
}

/*!
  \return interaction style
*/
int OCCViewer_Viewer::interactionStyle() const
{
  return myInteractionStyle;
}

/*!
  Sets interaction style: 0 - standard, 1 - keyboard free interaction
  \param theStyle - new interaction style
*/
void OCCViewer_Viewer::setInteractionStyle( const int theStyle )
{
  myInteractionStyle = theStyle;
  //!! To be done for view windows
  if ( !myViewManager )
    return;

  QVector<SUIT_ViewWindow*> wins = myViewManager->getViews();
  for ( int i = 0; i < (int)wins.count(); i++ )
  {
    OCCViewer_ViewWindow* win = ::qobject_cast<OCCViewer_ViewWindow*>( wins.at( i ) );
    if ( win )
      win->setInteractionStyle( theStyle );
  }
}

/*!
  \return zooming style
*/
int OCCViewer_Viewer::zoomingStyle() const
{
  return myZoomingStyle;
}

/*!
  Sets zooming style: 0 - standard, 1 - advanced (at cursor)
  \param theStyle - new zooming style
*/
void OCCViewer_Viewer::setZoomingStyle( const int theStyle )
{
  myZoomingStyle = theStyle;
  //!! To be done for view windows
  if ( !myViewManager )
    return;

  QVector<SUIT_ViewWindow*> wins = myViewManager->getViews();
  for ( int i = 0; i < (int)wins.count(); i++ )
  {
    OCCViewer_ViewWindow* win = ::qobject_cast<OCCViewer_ViewWindow*>( wins.at( i ) );
    if ( win )
      win->setZoomingStyle( theStyle );
  }
}

/*!
  Sets selection enabled status
  \param isEnabled - new status
*/
void OCCViewer_Viewer::enableSelection(bool isEnabled)
{
  mySelectionEnabled = isEnabled;
  //!! To be done for view windows
  if ( !myViewManager )
    return;

  QVector<SUIT_ViewWindow*> wins = myViewManager->getViews();
  for ( int i = 0; i < (int)wins.count(); i++ )
  {
    OCCViewer_ViewWindow* win = ::qobject_cast<OCCViewer_ViewWindow*>( wins.at( i ) );
    if ( win )
      win->updateEnabledDrawMode();
  }
}

/*!
  Sets multiselection enabled status
  \param isEnabled - new status
*/
void OCCViewer_Viewer::enableMultiselection(bool isEnable)
{
  myMultiSelectionEnabled = isEnable;
  //!! To be done for view windows
  if ( !myViewManager )
    return;

  QVector<SUIT_ViewWindow*> wins = myViewManager->getViews();
  for ( int i = 0; i < (int)wins.count(); i++ )
  {
    OCCViewer_ViewWindow* win = ::qobject_cast<OCCViewer_ViewWindow*>( wins.at( i ) );
    if ( win )
      win->updateEnabledDrawMode();
  }
}

/*!
  Builds popup for occ viewer
*/
void OCCViewer_Viewer::contextMenuPopup(QMenu* thePopup)
{
  thePopup->addAction( tr( "MEN_DUMP_VIEW" ), this, SLOT( onDumpView() ) );
  thePopup->addAction( tr( "MEN_CHANGE_BACKGROUND" ), this, SLOT( onChangeBackground() ) );

  thePopup->addSeparator();

  OCCViewer_ViewWindow* aView = (OCCViewer_ViewWindow*)(myViewManager->getActiveView());

  //Support of several toolbars in the popup menu
  QList<QToolBar*> lst = qFindChildren<QToolBar*>( aView );
  QList<QToolBar*>::const_iterator it = lst.begin(), last = lst.end();
  for ( ; it!=last; it++ ) {
    if ( (*it)->parentWidget()->isVisible() )
      thePopup->addAction( (*it)->toggleViewAction() );
  }
}

/*!
  SLOT: called on dump view operation is activated, stores scene to raster file
*/
void OCCViewer_Viewer::onDumpView()
{
  OCCViewer_ViewWindow* aView = (OCCViewer_ViewWindow*)(myViewManager->getActiveView());
  if ( aView )
    aView->onDumpView();
}

/*!
  SLOT: called if background color is to be changed changed, passes new color to view port
*/
void OCCViewer_Viewer::onChangeBackground()
{
  OCCViewer_ViewWindow* aView = dynamic_cast<OCCViewer_ViewWindow*>(myViewManager->getActiveView());
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
								   !txtList.isEmpty(),   // enable/disable texture mode
								   gradList,             // gradient names
								   idList,               // gradient identifiers
								   formats );            // image formats

  // set chosen background data to the viewer
  if ( bgData.isValid() )
    aView->setBackground( bgData );
}

/*!
  Updates OCC 3D viewer
*/
void OCCViewer_Viewer::update()
{
  if (!myV3dViewer.IsNull())
    myV3dViewer->Update();

  OCCViewer_ViewWindow* aView = (OCCViewer_ViewWindow*)(myViewManager->getActiveView());
  if ( aView )
    aView->updateGravityCoords();
}

/*!
  \return objects selected in 3D viewer
  \param theList - list to be filled with selected objects
*/
void OCCViewer_Viewer::getSelectedObjects(AIS_ListOfInteractive& theList)
{
  theList.Clear();
  for (myAISContext->InitSelected(); myAISContext->MoreSelected(); myAISContext->NextSelected())
    theList.Append(myAISContext->SelectedInteractive());
}

/*!
  Selects objects in 3D viewer. Other selected objects are left as selected
  \param theList - list objects to be selected
*/
void OCCViewer_Viewer::setObjectsSelected(const AIS_ListOfInteractive& theList)
{
  AIS_ListIteratorOfListOfInteractive aIt;
  for (aIt.Initialize(theList); aIt.More(); aIt.Next())
    myAISContext->AddOrRemoveSelected(aIt.Value(), false);
  myAISContext->UpdateCurrentViewer();
}

/*!
  Auxiliary method to emit signal selectionChanged()
*/
void OCCViewer_Viewer::performSelectionChanged()
{
    emit selectionChanged();
}

/*!
  Hilights/unhilights object in viewer
  \param obj - object to be updated
  \param hilight - if it is true, object will be hilighted, otherwise it will be unhilighted
  \param update - update current viewer
*/
bool OCCViewer_Viewer::highlight( const Handle(AIS_InteractiveObject)& obj,
                                  bool hilight, bool update )
{
  bool isInLocal = myAISContext->HasOpenedContext();
  if( !obj.IsNull() )
    if( !isInLocal )
    {
      if ( hilight && !myAISContext->IsSelected( obj ) )
        myAISContext->AddOrRemoveCurrentObject( obj, false );
      else if ( !hilight && myAISContext->IsSelected( obj ) )
        myAISContext->AddOrRemoveCurrentObject( obj, false );
    }

  if ( update )
    myV3dViewer->Redraw();
    
  return false;
}

/*!
  Unhilights all objects in viewer
  \param updateviewer - update current viewer
*/
bool OCCViewer_Viewer::unHighlightAll( bool updateviewer )
{
  if ( myAISContext->HasOpenedContext() )
    myAISContext->ClearSelected( updateviewer );
  else
    myAISContext->ClearCurrents( updateviewer );
  return false;
}

/*!
  \return true if object is in viewer or in collector
  \param obj - object to be checked
  \param onlyInViewer - search object only in viewer (so object must be displayed)
*/
bool OCCViewer_Viewer::isInViewer( const Handle(AIS_InteractiveObject)& obj,
                                   bool onlyInViewer )
{
  AIS_ListOfInteractive List;
  myAISContext->DisplayedObjects(List);

  if( !onlyInViewer )
  {
    AIS_ListOfInteractive List1;
    myAISContext->ObjectsInCollector(List1);
    List.Append(List1);
  }

  AIS_ListIteratorOfListOfInteractive ite(List);
  for ( ; ite.More(); ite.Next() )
    if( ite.Value()==obj )
      return true;

  return false;
}

/*!
  \return true if object is displayed in viewer
  \param obj - object to be checked
*/
bool OCCViewer_Viewer::isVisible( const Handle(AIS_InteractiveObject)& obj )
{
  return myAISContext->IsDisplayed( obj );
}

/*!
  Sets color of object
  \param obj - object to be updated
  \param color - new color
  \param update - update current viewer
*/
void OCCViewer_Viewer::setColor( const Handle(AIS_InteractiveObject)& obj,
                                 const QColor& color,
                                 bool update )
{
  if( !obj.IsNull() )
  {
    Quantity_Color CSFColor = Quantity_Color ( color.red() / 255.,
                                               color.green() / 255.,
                                               color.blue() / 255.,
                                               Quantity_TOC_RGB );
    obj->SetColor( CSFColor );
  }

  if( update )
    myV3dViewer->Update();
}

/*!
  Changes display mode of object
  \param obj - object to be processed
  \param mode - new display mode
  \param update - update current viewer
*/
void OCCViewer_Viewer::switchRepresentation( const Handle(AIS_InteractiveObject)& obj,
                                             int mode, bool update )
{
  myAISContext->SetDisplayMode( obj, (Standard_Integer)mode, update );
  if( update )
    myV3dViewer->Update();
}

/*!
  Changes transparency of object
  \param obj - object to be processed
  \param trans - new transparency
  \param update - update current viewer
*/
void OCCViewer_Viewer::setTransparency( const Handle(AIS_InteractiveObject)& obj,
                                        float trans, bool update )
{
  myAISContext->SetTransparency( obj, trans, false );
  myAISContext->Redisplay( obj, Standard_False, Standard_True );
  if( update )
    myV3dViewer->Update();
}

/*!
  Changes visibility of trihedron to opposite
*/
void OCCViewer_Viewer::toggleTrihedron()
{
  setTrihedronShown( !isTrihedronVisible() );
}

/*!
  \return true if trihedron is visible
*/
bool OCCViewer_Viewer::isTrihedronVisible() const
{
  return !myTrihedron.IsNull() && !myAISContext.IsNull() && myAISContext->IsDisplayed( myTrihedron );
}

/*!
  Sets visibility state of trihedron
  \param on - new state
*/

void OCCViewer_Viewer::setTrihedronShown( const bool on )
{
  if ( myTrihedron.IsNull() )
    return;

  if ( on )
    myAISContext->Display( myTrihedron );
  else
    myAISContext->Erase( myTrihedron );
}

/*!
  \return trihedron size
*/
double OCCViewer_Viewer::trihedronSize() const
{
  double sz = 0;
  if ( !myTrihedron.IsNull() )
    sz = myTrihedron->Size();
  return sz;
}

/*!
  Changes trihedron size
  \param sz - new size
*/
void OCCViewer_Viewer::setTrihedronSize( const double sz, bool isRelative )
{
  if ( myTrihedronSize != sz || isRelative != myIsRelative) {
    myTrihedronSize = sz; 
    myIsRelative = isRelative;
    updateTrihedron();
  }
}

/*!
  Set number of isolines
  \param u - u-isolines (first parametric co-ordinate)
  \param v - v-isolines (second parametric co-ordinate)
*/
void OCCViewer_Viewer::setIsos( const int u, const int v )
{
  Handle(AIS_InteractiveContext) ic = getAISContext();
  if ( ic.IsNull() )
  return;

  ic->SetIsoNumber( u, AIS_TOI_IsoU );
  ic->SetIsoNumber( v, AIS_TOI_IsoV );
}

/*!
  \return number of isolines
  \param u - to return u-isolines (first parametric co-ordinate)
  \param v - to return v-isolines (second parametric co-ordinate)
*/
void OCCViewer_Viewer::isos( int& u, int& v ) const
{
  Handle(AIS_InteractiveContext) ic = getAISContext();
  if ( !ic.IsNull() )
  {
    u = ic->IsoNumber( AIS_TOI_IsoU );
    v = ic->IsoNumber( AIS_TOI_IsoV );
  }
}

/* 
 * Returns a new OCCViewer_ViewWindow instance which will be placed as a sub window in ViewFrame
 */
OCCViewer_ViewWindow* OCCViewer_Viewer::createSubWindow()
{
  return new OCCViewer_ViewWindow( 0,  this);
}

// obsolete  
QColor OCCViewer_Viewer::backgroundColor( int theViewId ) const
{
  return background( theViewId ).color();
}

Qtx::BackgroundData OCCViewer_Viewer::background( int theViewId ) const
{
  return ( theViewId >= 0 && theViewId < myBackgrounds.count() ) ? myBackgrounds[theViewId] : Qtx::BackgroundData();
}

// obsolete
void OCCViewer_Viewer::setBackgroundColor( int theViewId, const QColor& theColor )
{
  if ( theColor.isValid() ) {
    Qtx::BackgroundData bg = background( theViewId );
    bg.setColor( theColor );
    setBackground( theViewId, bg );
  }
}

void OCCViewer_Viewer::setBackground( int theViewId, const Qtx::BackgroundData& theBackground )
{
  if ( theBackground.isValid() && theViewId >= 0 && theViewId < myBackgrounds.count() )
    myBackgrounds[theViewId] = theBackground;    
}


/*!
  Set the show static trihedron flag
*/
void OCCViewer_Viewer::setStaticTrihedronDisplayed(const bool on) {
  if(myShowStaticTrihedron != on) {
    OCCViewer_ViewWindow* aView = (OCCViewer_ViewWindow*)(myViewManager->getActiveView());
    if(!aView)
      return;

    OCCViewer_ViewPort3d* vp3d = aView->getViewPort();
    if(vp3d) {
      myShowStaticTrihedron = on;
      vp3d->updateStaticTriedronVisibility();
    }
  }
}

/*!
  Get new and current trihedron size corresponding to the current model size
*/
bool OCCViewer_Viewer::computeTrihedronSize( double& theNewSize, double& theSize )
{
  theNewSize = 100;
  theSize = 100;

  //SRN: BUG IPAL8996, a usage of method ActiveView without an initialization
  Handle(V3d_Viewer) viewer = getViewer3d();
  viewer->InitActiveViews();
  if(!viewer->MoreActiveViews()) return false;

  Handle(V3d_View) view3d = viewer->ActiveView();
  //SRN: END of fix

  if ( view3d.IsNull() )
    return false;

  double Xmin = 0, Ymin = 0, Zmin = 0, Xmax = 0, Ymax = 0, Zmax = 0;
  double aMaxSide;

  view3d->View()->MinMaxValues( Xmin, Ymin, Zmin, Xmax, Ymax, Zmax );

  if ( Xmin == RealFirst() || Ymin == RealFirst() || Zmin == RealFirst() ||
       Xmax == RealLast()  || Ymax == RealLast()  || Zmax == RealLast() )
    return false;

  aMaxSide = Xmax - Xmin;
  if ( aMaxSide < Ymax -Ymin ) aMaxSide = Ymax -Ymin;
  if ( aMaxSide < Zmax -Zmin ) aMaxSide = Zmax -Zmin;

  // IPAL21687
  // The boundary box of the view may be initialized but nullified
  // (case of infinite objects)
  if ( aMaxSide < Precision::Confusion() )
    return false;

  float aSizeInPercents = SUIT_Session::session()->resourceMgr()->doubleValue("OCCViewer","trihedron_size", 100.);

  static float EPS = 5.0E-3;
  theSize = getTrihedron()->Size();
  theNewSize = aMaxSide*aSizeInPercents / 100.0;

  return fabs( theNewSize - theSize ) > theSize * EPS ||
         fabs( theNewSize - theSize) > theNewSize * EPS;
}

/*! 
 * Update the size of the trihedron
 */
void OCCViewer_Viewer::updateTrihedron() {
  if(myIsRelative){
    double newSz, oldSz;
    
    if(computeTrihedronSize(newSz, oldSz))
      myTrihedron->SetSize(newSz);
    
  } else if(myTrihedron->Size() != myTrihedronSize) {
    myTrihedron->SetSize(myTrihedronSize);
  }
}
