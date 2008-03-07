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

#include "OCCViewer_ViewModel.h"
#include "OCCViewer_ViewWindow.h"
#include "OCCViewer_VService.h"
#include "OCCViewer_ViewPort3d.h"

#include "SUIT_ViewWindow.h"
#include "SUIT_Desktop.h"
#include "SUIT_Session.h"

#include <qpainter.h>
#include <qapplication.h>
#include <qcolordialog.h>
#include <qpalette.h>
#include <qpopupmenu.h>

#include <AIS_Axis.hxx>
#include <AIS_Drawer.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>

#include <Geom_Axis2Placement.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_LineAspect.hxx>

/*!
  Constructor
  \param DisplayTrihedron - is trihedron displayed
*/
OCCViewer_Viewer::OCCViewer_Viewer( bool DisplayTrihedron )
: SUIT_ViewModel(),
myBgColor( Qt::black )
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

  clearViewAspects();

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

  // selection
  mySelectionEnabled = true;
  myMultiSelectionEnabled = true;
}

/*!
  Destructor
*/
OCCViewer_Viewer::~OCCViewer_Viewer() 
{
}

/*!
  \return background color of viewer
*/
QColor OCCViewer_Viewer::backgroundColor() const
{
  return myBgColor;
}

/*!
  Sets background color
  \param c - new background color
*/
void OCCViewer_Viewer::setBackgroundColor( const QColor& c )
{
  if ( c.isValid() )
    myBgColor = c;
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
    
    OCCViewer_ViewPort3d* vp3d = view->getViewPort();
    if ( vp3d )
      vp3d->setBackgroundColor( myBgColor );
  }
}

/*!
  Creates new view window
  \param theDesktop - main window of application
*/
SUIT_ViewWindow* OCCViewer_Viewer::createView( SUIT_Desktop* theDesktop )
{
  OCCViewer_ViewWindow* view = new OCCViewer_ViewWindow(theDesktop, this);
  initView( view );
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
  if ( isSelectionEnabled() )
    myAISContext->MoveTo(theEvent->x(), theEvent->y(), aView->getViewPort()->getView());
}


/*!
  SLOT: called on mouse button release, finishes transformation or selection
*/
void OCCViewer_Viewer::onMouseRelease(SUIT_ViewWindow* theWindow, QMouseEvent* theEvent)
{
  if (!mySelectionEnabled) return;
  if (theEvent->button() != Qt::LeftButton) return;
  if (!theWindow->inherits("OCCViewer_ViewWindow")) return;


  myEndPnt.setX(theEvent->x()); myEndPnt.setY(theEvent->y());
  OCCViewer_ViewWindow* aView = (OCCViewer_ViewWindow*) theWindow;
  bool aHasShift = (theEvent->state() & Qt::ShiftButton);
  
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
  Sets selection enabled status
  \param isEnabled - new status
*/
void OCCViewer_Viewer::enableSelection(bool isEnabled)
{
  mySelectionEnabled = isEnabled;
  //!! To be done for view windows
  if ( !myViewManager )
    return;

  QPtrVector<SUIT_ViewWindow> wins = myViewManager->getViews();
  for ( int i = 0; i < (int)wins.count(); i++ )
  {
    OCCViewer_ViewWindow* win = ::qt_cast<OCCViewer_ViewWindow*>( wins.at( i ) );
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

  QPtrVector<SUIT_ViewWindow> wins = myViewManager->getViews();
  for ( int i = 0; i < (int)wins.count(); i++ )
  {
    OCCViewer_ViewWindow* win = ::qt_cast<OCCViewer_ViewWindow*>( wins.at( i ) );
    if ( win )
      win->updateEnabledDrawMode();
  }
}

/*!
  Builds popup for occ viewer
*/
void OCCViewer_Viewer::contextMenuPopup(QPopupMenu* thePopup)
{
  thePopup->insertItem( tr( "MEN_DUMP_VIEW" ), this, SLOT( onDumpView() ) );
  thePopup->insertItem( tr( "MEN_CHANGE_BACKGROUD" ), this, SLOT( onChangeBgColor() ) );

  thePopup->insertSeparator();

  OCCViewer_ViewWindow* aView = (OCCViewer_ViewWindow*)(myViewManager->getActiveView());
  if ( aView && !aView->getToolBar()->isVisible() )
    thePopup->insertItem( tr( "MEN_SHOW_TOOLBAR" ), this, SLOT( onShowToolbar() ) );
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
void OCCViewer_Viewer::onChangeBgColor()
{
  OCCViewer_ViewWindow* aView = (OCCViewer_ViewWindow*)(myViewManager->getActiveView());
  if( !aView )
    return;
  OCCViewer_ViewPort3d* aViewPort3d = aView->getViewPort();
  if( !aViewPort3d )
    return;
  QColor aColorActive = aViewPort3d->backgroundColor();

  QColor selColor = QColorDialog::getColor( aColorActive, aView);
  if ( selColor.isValid() )
    aViewPort3d->setBackgroundColor(selColor);
}

/*!
  SLOT: called when popup item "Show toolbar" is activated, shows toolbar of active view window
*/
void OCCViewer_Viewer::onShowToolbar() {
  OCCViewer_ViewWindow* aView = (OCCViewer_ViewWindow*)(myViewManager->getActiveView());
  if ( aView )
    aView->getToolBar()->show();    
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
  SLOT, clears view aspects
*/
void OCCViewer_Viewer::onClearViewAspects()
{
    clearViewAspects();
}

/*!
  Clears view aspects
*/
void OCCViewer_Viewer::clearViewAspects()
{
	myViewAspects.clear();
}

/*!
  \return const reference to list of view aspects
*/
const viewAspectList& OCCViewer_Viewer::getViewAspects()
{
	return myViewAspects;
}

/*!
  Appends new view aspect
  \param aParams - new view aspects
*/
void OCCViewer_Viewer::appendViewAspect( const viewAspect& aParams )
{
	myViewAspects.append( aParams );
}

/*!
  Replaces old view aspects by new ones
  \param aViewList - list of new view aspects
*/
void OCCViewer_Viewer::updateViewAspects( const viewAspectList& aViewList )
{
	myViewAspects = aViewList;
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
void OCCViewer_Viewer::setTrihedronSize( const double sz )
{
  if ( !myTrihedron.IsNull() )
    myTrihedron->SetSize( sz );
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
