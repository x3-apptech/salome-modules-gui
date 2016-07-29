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

#include "OCCViewer_ViewFrame.h"
#include "OCCViewer_ViewWindow.h"
#include "OCCViewer_ViewModel.h"
#include "OCCViewer_ViewPort3d.h"

#include <SUIT_ViewManager.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>

#include <QtxResourceMgr.h>
#include <QtxWorkstackAction.h>

#include <QFrame>
#include <QLayout>
#include <QApplication>

OCCViewer_ViewFrame::OCCViewer_ViewFrame(SUIT_Desktop* theDesktop, OCCViewer_Viewer* theModel)
  : OCCViewer_ViewWindow( theDesktop, theModel ), myPopupRequestedView(0), mySplitMode(-1)
{
  QFrame* centralFrame = new QFrame( this );
  setCentralWidget( centralFrame );

  OCCViewer_ViewWindow* view0 = theModel->createSubWindow();
  updateWindowTitle( view0 );
  view0->setParent( centralFrame );
  myViews.append( view0 ); // MAIN_VIEW

  myLayout = new QGridLayout( centralFrame );
  myLayout->setMargin( 0 );
  myLayout->setSpacing( 1 );

  myLayout->addWidget( view0, 0, 0 );
  myMaximizedView = view0;
  myActiveView = view0;
  connectViewSignals(view0);
}

OCCViewer_ViewFrame::~OCCViewer_ViewFrame()
{
}

bool OCCViewer_ViewFrame::event( QEvent* e )
{
  if ( e->type() == QEvent::WindowTitleChange ) {
    updateWindowTitle( getView( MAIN_VIEW ) );
    updateWindowTitle( getView( BOTTOM_LEFT ) );
    updateWindowTitle( getView( TOP_LEFT ) );
    updateWindowTitle( getView( TOP_RIGHT ) );
  }
  return OCCViewer_ViewWindow::event( e );
}

//**************************************************************************************
OCCViewer_ViewWindow* OCCViewer_ViewFrame::getView( const int i ) const
{
  return ( i >= 0 && i < myViews.count() ) ? myViews.at( i ) : 0 ;
}

//**************************************************************************************
OCCViewer_ViewWindow* OCCViewer_ViewFrame::getActiveView( ) const
{
  return myActiveView;
}

//**************************************************************************************
void OCCViewer_ViewFrame::setViewManager( SUIT_ViewManager* theMgr )
{
  OCCViewer_ViewWindow::setViewManager(theMgr);
  foreach (OCCViewer_ViewWindow* aView, myViews) {
    aView->setViewManager(theMgr); 
  }
}

//**************************************************************************************
void OCCViewer_ViewFrame::returnTo3dView()
{
  OCCViewer_ViewWindow* view = 0;
  for (int i = BOTTOM_RIGHT; i <= TOP_RIGHT; i++ ) {
    view = myViews.at(i);
    view->setVisible( view->get2dMode() == No2dMode );
    view->setMaximized( true, false );
    if (view->get2dMode() == No2dMode) {
      myMaximizedView = view;
      myActiveView = view;
    }
  }
  myLayout->setColumnStretch(0, 0);
  myLayout->setColumnStretch(1, 0);
  myLayout->addWidget( myMaximizedView, 0, 0 );
  mySplitMode = -1;
  myViewsMode.clear();
  myLayout->invalidate();
}

//**************************************************************************************
void OCCViewer_ViewFrame::onMaximizedView( OCCViewer_ViewWindow* theView, bool isMaximized)
{
  myMaximizedView = theView;
  myActiveView = theView;
  if (isMaximized) {
    if (myViews.count() <= 1)
      return;
    myLayout->setColumnStretch(0, 0);
    myLayout->setColumnStretch(1, 0);
    myLayout->addWidget( theView, 0, 0 );
    int i = 0;
    OCCViewer_ViewWindow* view = 0;
    for ( i = BOTTOM_RIGHT; i <= TOP_RIGHT; i++ ) {
      view = myViews.at(i);
      view->setVisible( view == theView );
      view->setMaximized( view == theView, false );
    }
    mySplitMode = -1;
    myViewsMode.clear();
  }
  else {
    createSubViews();

    QtxSplitDlg CreateSubViewsDlg( this, NULL, CreateSubViews );
    if ( CreateSubViewsDlg.exec() ) {
      mySplitMode = CreateSubViewsDlg.getSplitMode();
      myViewsMode = CreateSubViewsDlg.getViewsMode();
      splitSubViews();
      myMaximizedView->setMaximized(false, false);
    }
  }
  myLayout->invalidate();
}

//**************************************************************************************
void OCCViewer_ViewFrame::createSubViews()
{
  OCCViewer_Viewer* aModel = dynamic_cast<OCCViewer_Viewer*>(myManager->getViewModel());
  if (!aModel) return;
  int i = 0;
  if (myViews.count() == 1) {
    //QColor aColor = myViews.at( MAIN_VIEW )->backgroundColor();
    OCCViewer_ViewWindow* view = 0;
    for ( i = BOTTOM_LEFT; i <= TOP_RIGHT; i++) {
      view = aModel->createSubWindow();
      view->set2dMode( (Mode2dType) i );
      view->setParent( centralWidget() );
      view->setViewManager(myManager);
      updateWindowTitle( view );
      myViews.append( view );
      aModel->initView(view);
      view->setMaximized(false, false);
      view->setDropDownButtons( dropDownButtons() );
      connectViewSignals(view);
      view->setBackground(aModel->background(i));
      setSubViewParams( view );
    }
  }
}

//**************************************************************************************
void OCCViewer_ViewFrame::setSubViewParams( OCCViewer_ViewWindow* theView )
{
  Handle(V3d_View) aView = theView->getViewPort()->getView();
  Handle(V3d_View) aMainView = myViews.at( MAIN_VIEW )->getViewPort()->getView();

  // set ray tracing parameters
  aView->ChangeRenderingParams() = aMainView->RenderingParams();

  // set environment texture parameters
  aView->SetTextureEnv( aMainView->TextureEnv() );
#if OCC_VERSION_LARGE <= 0x07000000
  aView->SetSurfaceDetail( aMainView->SurfaceDetail() );
#endif
}

void OCCViewer_ViewFrame::splitSubViews()
{
	if( mySplitMode == -1 )
		return;

  int aNbViews;
  if ( mySplitMode >= 0 && mySplitMode < 2)
    aNbViews = 2;
  else if( mySplitMode >= 2 && mySplitMode < 8 )
    aNbViews = 3;
  else if( mySplitMode >=8 && mySplitMode < 29 )
    aNbViews = 4;

  if( aNbViews != myViewsMode.count() )
  	return;

  int SubViews3Map[6][3][4] = {
    { {0,0,1,1}, {0,1,1,1}, {0,2,1,1} },
    { {0,0,1,1}, {1,0,1,1}, {2,0,1,1} },
    { {0,0,1,1}, {1,0,1,1}, {0,1,2,1} },
    { {0,0,2,1}, {0,1,1,1}, {1,1,1,1} },
    { {0,0,1,2}, {1,0,1,1}, {1,1,1,1} },
    { {0,0,1,1}, {0,1,1,1}, {1,0,1,2} }
  };

  int SubViews4Map[21][4][4] = {
    { {0,0,1,1}, {0,1,1,1}, {0,2,1,1}, {0,3,1,1} },
    { {0,0,1,1}, {1,0,1,1}, {2,0,1,1}, {3,0,1,1} },
    { {0,0,1,1}, {0,1,1,1}, {1,0,1,1}, {1,1,1,1} },
    { {0,0,1,1}, {1,0,1,1}, {0,1,2,1}, {0,2,2,1} },
    { {0,0,2,1}, {0,1,1,1}, {1,1,1,1}, {0,2,2,1} },
    { {0,0,2,1}, {0,1,2,1}, {0,2,1,1}, {1,2,1,1} },
    { {0,0,1,1}, {0,1,1,1}, {1,0,1,2}, {2,0,1,2} },
    { {0,0,1,2}, {1,0,1,1}, {1,1,1,1}, {2,0,1,2} },
    { {0,0,1,2}, {1,0,1,2}, {2,0,1,1}, {2,1,1,1} },
    { {0,0,1,1}, {1,0,1,1}, {0,1,2,1}, {2,0,1,2} },
    { {0,0,2,1}, {0,1,1,1}, {1,1,1,1}, {2,0,1,2} },
    { {0,0,1,2}, {1,0,1,1}, {2,0,1,1}, {1,1,2,1} },
    { {0,0,1,2}, {1,0,2,1}, {1,1,1,1}, {2,1,1,1} },
    { {0,0,2,1}, {0,1,1,1}, {0,2,1,1}, {1,1,1,2} },
    { {0,0,2,1}, {0,1,1,2}, {1,1,1,1}, {1,2,1,1} },
    { {0,0,1,1}, {0,1,1,1}, {1,0,1,2}, {0,2,2,1} },
    { {0,0,1,2}, {1,0,1,1}, {1,1,1,1}, {0,2,2,1} },
    { {0,0,1,3}, {1,0,1,1}, {1,1,1,1}, {1,2,1,1} },
    { {0,0,1,1}, {0,1,1,1}, {0,2,1,1}, {1,0,1,3} },
    { {0,0,1,1}, {1,0,1,1}, {2,0,1,1}, {0,1,3,1} },
    { {0,0,3,1}, {0,1,1,1}, {1,1,1,1}, {2,1,1,1} },
  };

  if( aNbViews == 2 ) {
    if( mySplitMode == 0 ) {
      myLayout->addWidget( myViews.at(myViewsMode[0]), 0,0,2,1 );
      myLayout->addWidget( myViews.at(myViewsMode[1]), 0,1,2,1 );
    }
    else if( mySplitMode == 1 ) {
      myLayout->addWidget( myViews.at(myViewsMode[0]), 0,0,1,2 );
      myLayout->addWidget( myViews.at(myViewsMode[1]), 1,0,1,2 );
    }
  }
  else if( aNbViews == 3 ) {
  	int aSplitMode = mySplitMode - 2;
    for( int i = 0; i < 3; i++ ) {
    myLayout->addWidget( myViews.at(myViewsMode[i]),
                         SubViews3Map[aSplitMode][i][0],
                         SubViews3Map[aSplitMode][i][1],
                         SubViews3Map[aSplitMode][i][2],
                         SubViews3Map[aSplitMode][i][3]);
    }
  }
  else if( aNbViews == 4 ) {
  	int aSplitMode = mySplitMode - 8;
    for( int i = 0; i < 4; i++ ) {
    myLayout->addWidget( myViews.at(myViewsMode[i]),
                         SubViews4Map[aSplitMode][i][0],
                         SubViews4Map[aSplitMode][i][1],
                         SubViews4Map[aSplitMode][i][2],
                         SubViews4Map[aSplitMode][i][3]);
    }
  }

  OCCViewer_ViewWindow* view = 0;
  bool isVisible3dView = false;
  for ( int i = 0; i< myViews.count(); i++ ) {
    view = myViews.at(i);
    bool isShowed = false;
    for( int j = 0; j < myViewsMode.count(); j++ ) {
      OCCViewer_ViewWindow* view2 = 0;
      view2 = myViews.at( myViewsMode[j] );
      if( view == view2 )
        isShowed = true;
    }
    if( isShowed ) {
      view->show();
      view->setMaximized( false, false );
      if ( view->get2dMode() == No2dMode ) isVisible3dView = true;
      ///////////////QApplication::processEvents(); // VSR: hangs up ?
      if ( view != myMaximizedView )
        view->onViewFitAll();
    }
    else
      view->setVisible( false );
  }
  if ( !isVisible3dView ) {
    for ( int i = 0; i< myViews.count(); i++ ){
      view = myViews.at(i);
      if( view->isVisible() )
        view->setReturnedTo3dView( false );
    }
  }
}

//**************************************************************************************
OCCViewer_ViewPort3d* OCCViewer_ViewFrame::getViewPort(int theView) 
{ 
  return getView(theView)? getView(theView)->getViewPort() : 0;
}
  
//**************************************************************************************
void OCCViewer_ViewFrame::updateEnabledDrawMode() 
{ 
  foreach (OCCViewer_ViewWindow* aView, myViews) {
    aView->updateEnabledDrawMode(); 
  }
}

//**************************************************************************************
void OCCViewer_ViewFrame::setCuttingPlane( bool on, const double x , const double y , const double z,
                                           const double dx, const double dy, const double dz)  
{ 
  foreach (OCCViewer_ViewWindow* aView, myViews) {
    aView->setCuttingPlane(on, x, y, z, dx, dy, dz); 
    aView->update();
  }
}

//**************************************************************************************
void OCCViewer_ViewFrame::setCuttingPlane( bool on, const gp_Pln thePln ) 
{ 
  foreach (OCCViewer_ViewWindow* aView, myViews) {
    aView->setCuttingPlane(on, thePln); 
    aView->update();
  }
}
  
//**************************************************************************************
void OCCViewer_ViewFrame::setInteractionStyle( const int i ) 
{ 
  foreach (OCCViewer_ViewWindow* aView, myViews) {
    aView->setInteractionStyle(i); 
  }
}

//**************************************************************************************
int OCCViewer_ViewFrame::projectionType() const
{
  return getView(MAIN_VIEW)->projectionType();
}

//**************************************************************************************
void OCCViewer_ViewFrame::setProjectionType( int t)
{
  foreach (OCCViewer_ViewWindow* aView, myViews) {
    aView->setProjectionType(t);
  }
}

//**************************************************************************************
int OCCViewer_ViewFrame::stereoType() const
{
  return getView(MAIN_VIEW)->stereoType();
}

//**************************************************************************************
void OCCViewer_ViewFrame::setStereoType( int t)
{
  foreach (OCCViewer_ViewWindow* aView, myViews) {
    aView->setStereoType(t);
  }
}

//**************************************************************************************
int OCCViewer_ViewFrame::anaglyphFilter() const
{
  return getView(MAIN_VIEW)->anaglyphFilter();
}

//**************************************************************************************
void OCCViewer_ViewFrame::setAnaglyphFilter( int t)
{
  foreach (OCCViewer_ViewWindow* aView, myViews) {
    aView->setAnaglyphFilter(t);
  }
}

//**************************************************************************************
int OCCViewer_ViewFrame::stereographicFocusType() const
{
  return getView(MAIN_VIEW)->stereographicFocusType();
}

//**************************************************************************************
double OCCViewer_ViewFrame::stereographicFocusValue() const
{
  return getView(MAIN_VIEW)->stereographicFocusValue();
}

//**************************************************************************************
void OCCViewer_ViewFrame::setStereographicFocus( int t, double v)
{
  foreach (OCCViewer_ViewWindow* aView, myViews) {
    aView->setStereographicFocus(t, v);
  }
}

//**************************************************************************************
int OCCViewer_ViewFrame::interocularDistanceType() const
{
  return getView(MAIN_VIEW)->interocularDistanceType();
}

//**************************************************************************************
double OCCViewer_ViewFrame::interocularDistanceValue() const
{
  return getView(MAIN_VIEW)->interocularDistanceValue();
}

//**************************************************************************************
void OCCViewer_ViewFrame::setInterocularDistance( int t, double v)
{
  foreach (OCCViewer_ViewWindow* aView, myViews) {
    aView->setInterocularDistance(t, v);
  }
}

//**************************************************************************************
bool OCCViewer_ViewFrame::isReverseStereo() const
{
  return getView(MAIN_VIEW)->isReverseStereo();
}

//**************************************************************************************
void OCCViewer_ViewFrame::setReverseStereo( bool t)
{
  foreach (OCCViewer_ViewWindow* aView, myViews) {
    aView->setReverseStereo(t);
  }
}

//**************************************************************************************
bool OCCViewer_ViewFrame::isVSync() const
{
  return getView(MAIN_VIEW)->isVSync();
}

//**************************************************************************************
void OCCViewer_ViewFrame::setVSync( bool t)
{
  foreach (OCCViewer_ViewWindow* aView, myViews) {
    aView->setVSync(t);
  }
}

//**************************************************************************************
bool OCCViewer_ViewFrame::isQuadBufferSupport() const
{
  return getView(MAIN_VIEW)->isQuadBufferSupport();
}

//**************************************************************************************
void OCCViewer_ViewFrame::setQuadBufferSupport( bool t)
{
  foreach (OCCViewer_ViewWindow* aView, myViews) {
    aView->setQuadBufferSupport(t);
  }
}

//**************************************************************************************
void OCCViewer_ViewFrame::setZoomingStyle( const int i ) 
{ 
  foreach (OCCViewer_ViewWindow* aView, myViews) {
    aView->setZoomingStyle(i); 
  }
}
  
//**************************************************************************************
void OCCViewer_ViewFrame::connectViewSignals(OCCViewer_ViewWindow* theView)
{
  connect( theView, SIGNAL( returnedTo3d( ) ), this, SLOT( returnTo3dView( ) ) );

  connect( theView, SIGNAL( maximized( OCCViewer_ViewWindow*, bool ) ), 
           this, SLOT( onMaximizedView( OCCViewer_ViewWindow*, bool ) ) );

  connect( theView, SIGNAL( wheeling(SUIT_ViewWindow*, QWheelEvent*) ), 
           this, SIGNAL( wheeling(SUIT_ViewWindow*, QWheelEvent*) ) );

  connect( theView, SIGNAL( keyReleased(SUIT_ViewWindow*, QKeyEvent*) ), 
           this, SIGNAL( keyReleased(SUIT_ViewWindow*, QKeyEvent*) ) );
  connect( theView, SIGNAL( keyPressed(SUIT_ViewWindow*, QKeyEvent*) ), 
           this, SIGNAL( keyPressed(SUIT_ViewWindow*, QKeyEvent*) ) );

  connect( theView, SIGNAL( mouseDoubleClicked(SUIT_ViewWindow*, QMouseEvent*) ), 
           this, SIGNAL( mouseDoubleClicked(SUIT_ViewWindow*, QMouseEvent*) ) );
  connect( theView, SIGNAL( mousePressed(SUIT_ViewWindow*, QMouseEvent*) ), 
           this, SIGNAL( mousePressed(SUIT_ViewWindow*, QMouseEvent*) ) );
  // The signal is used to mouse pressed for choose the current window
  connect( theView, SIGNAL( mousePressed(SUIT_ViewWindow*, QMouseEvent*) ),
           this, SLOT( onMousePressed(SUIT_ViewWindow*, QMouseEvent*) ) );
  connect( theView, SIGNAL( mouseReleased(SUIT_ViewWindow*, QMouseEvent*) ), 
           this, SIGNAL( mouseReleased(SUIT_ViewWindow*, QMouseEvent*) ) );
  connect( theView, SIGNAL( mouseMoving(SUIT_ViewWindow*, QMouseEvent*) ), 
           this, SIGNAL( mouseMoving(SUIT_ViewWindow*, QMouseEvent*) ) );

  // The signal is used to process get/set background color from popup
  connect( theView, SIGNAL( contextMenuRequested(QContextMenuEvent*) ), 
           this, SLOT( onContextMenuRequested(QContextMenuEvent*) ) );

  connect( theView, SIGNAL( contextMenuRequested(QContextMenuEvent*) ), 
           this, SIGNAL( contextMenuRequested(QContextMenuEvent*) ) );

  connect( theView, SIGNAL( viewCloned( SUIT_ViewWindow* ) ),
	   this,    SIGNAL( viewCloned( SUIT_ViewWindow* ) ) );
}

// obsolete
void OCCViewer_ViewFrame::setBackgroundColor( const QColor& theColor )
{
  if (myPopupRequestedView)
    myPopupRequestedView->setBackgroundColor(theColor); 
  else {
    foreach (OCCViewer_ViewWindow* aView, myViews) {
      if (aView->isVisible())
        aView->setBackgroundColor(theColor); 
    }
  }
}
void OCCViewer_ViewFrame::setBackground( const Qtx::BackgroundData& theBackground )
{
  if (myPopupRequestedView)
    myPopupRequestedView->setBackground(theBackground); 
  else {
    foreach (OCCViewer_ViewWindow* aView, myViews) {
      if (aView->isVisible())
        aView->setBackground(theBackground); 
    }
  }
}

void OCCViewer_ViewFrame::onViewFitAll()
{
  foreach (OCCViewer_ViewWindow* aView, myViews) {
    aView->onViewFitAll(); 
  }
}

void OCCViewer_ViewFrame::onFitAll()
{
  foreach (OCCViewer_ViewWindow* aView, myViews) {
    aView->onFitAll(); 
  }
}

// obsolete  
QColor OCCViewer_ViewFrame::backgroundColor() const 
{ 
  if (myPopupRequestedView)
    return myPopupRequestedView->backgroundColor(); 

  foreach (OCCViewer_ViewWindow* aView, myViews) {
    if (aView->isVisible())
      return aView->backgroundColor(); 
  }
  return getView(MAIN_VIEW)->backgroundColor(); 
}

Qtx::BackgroundData OCCViewer_ViewFrame::background() const 
{ 
  if (myPopupRequestedView)
    return myPopupRequestedView->background(); 

  foreach (OCCViewer_ViewWindow* aView, myViews) {
    if (aView->isVisible())
      return aView->background(); 
  }
  return getView(MAIN_VIEW)->background(); 
}

void OCCViewer_ViewFrame::showStaticTrihedron( bool on )
{
  if ( myPopupRequestedView )
    myPopupRequestedView->showStaticTrihedron( on ); 
  else {
    foreach ( OCCViewer_ViewWindow* aView, myViews ) {
      aView->showStaticTrihedron( on ); 
    }
  }
}

QImage OCCViewer_ViewFrame::dumpView()
{
  foreach (OCCViewer_ViewWindow* aView, myViews) {
    if (aView->isVisible())
      return aView->dumpView();
  }
  return QImage();
}

bool OCCViewer_ViewFrame::dumpViewToFormat( const QImage& image, const QString& fileName, const QString& format )
{
  foreach (OCCViewer_ViewWindow* aView, myViews) {
    if (aView->isVisible())
      return aView->dumpViewToFormat( image, fileName, format );
  }
  return false;
}

void OCCViewer_ViewFrame::onContextMenuRequested(QContextMenuEvent*)
{
  myPopupRequestedView = dynamic_cast<OCCViewer_ViewWindow*>(sender());
}

void OCCViewer_ViewFrame::onMousePressed(SUIT_ViewWindow* view, QMouseEvent*)
{
  myActiveView = dynamic_cast<OCCViewer_ViewWindow*>(view);
}

void OCCViewer_ViewFrame::onDumpView()
{
  if (myPopupRequestedView) {
    myPopupRequestedView->onDumpView(); 
  }
  else {
    getView(MAIN_VIEW)->onDumpView(); 
  }
}

void OCCViewer_ViewFrame::setDropDownButtons( bool on )
{
  foreach( OCCViewer_ViewWindow* aView, myViews ) {
    aView->setDropDownButtons( on );
  }
  OCCViewer_ViewWindow::setDropDownButtons( on );
}

QString OCCViewer_ViewFrame::getVisualParameters()
{
  QStringList params;
  QStringList splitParams;
  if( mySplitMode != -1 && myViewsMode.count() != 0 ) {
    splitParams << QString::number( mySplitMode );
    foreach ( int aViewMode, myViewsMode )
      splitParams << QString::number( aViewMode );
    params.append( splitParams.join("*") );
  }
  else
    params.append( QString::number( mySplitMode ) );
  int maximizedView = 999;
  for ( int i = BOTTOM_RIGHT; i <= TOP_RIGHT && i < myViews.count(); i++) {
    if ( getView(i)->isVisible() )
      maximizedView = ( maximizedView != -1 ) ? ( maximizedView == 999 ? i : -1 ) : ( maximizedView );
    params << getView(i)->getVisualParameters();
  }
  params.prepend( QString::number( maximizedView ) );
  return params.join( "|" );
}

void OCCViewer_ViewFrame::setVisualParameters( const QString& parameters ) 
{
  QStringList params = parameters.split( "|" );
  if ( params.count() > 1 ) {
    int maximizedView = params[0].toInt();
    if ( myViews.count() < params.count()-2 )
      createSubViews(); // secondary views are not created yet, but should be

    for ( int i = 2; i < params.count(); i++ ) {
      int idx = i-2;
      getView( idx )->setVisualParameters( params[i] );
    }

    QStringList aSplitParams = params[1].split("*");
    if( aSplitParams.count() > 1 ) {
      mySplitMode = aSplitParams[0].toInt();
      for( int i = 1; i < aSplitParams.count(); i++ )
        myViewsMode << aSplitParams[i].toInt();
    }
    if( mySplitMode != -1 )
      splitSubViews();
    else
      onMaximizedView( getView( maximizedView ), true ); // set proper sub-window maximized
  }
  else {
    // handle obsolete versions - no parameters for xy, yz, xz views
    getView(MAIN_VIEW)->setVisualParameters( parameters );
  }
}

SUIT_CameraProperties OCCViewer_ViewFrame::cameraProperties()
{
  // view frame does not have camera properties
  return SUIT_CameraProperties();
}

void OCCViewer_ViewFrame::updateWindowTitle(OCCViewer_ViewWindow* theView)
{
  if ( !theView )
    return;
  QString title;
  switch ( theView->get2dMode() ) {
  case No2dMode:
    title = "3D"; break;
  case XYPlane:
    title = "YX"; break;
  case XZPlane:
    title = "XZ"; break;
  case YZPlane:
    title = "YZ"; break;
  default:
    break;
  }
  if ( !title.isEmpty() )
    theView->setWindowTitle( windowTitle() + " - " + title );
}

void OCCViewer_ViewFrame::enableSelection( bool isEnabled )
{
  foreach (OCCViewer_ViewWindow* aView, myViews) {
    aView->enableSelection(isEnabled); 
  }
}

void OCCViewer_ViewFrame::enablePreselection( bool isEnabled )
{
  foreach (OCCViewer_ViewWindow* aView, myViews) {
    aView->enablePreselection(isEnabled); 
  }
}

bool OCCViewer_ViewFrame::enableDrawMode( bool on )
{
  bool prev = false;
  foreach (OCCViewer_ViewWindow* aView, myViews) {
    prev = prev || aView->enableDrawMode( on ); 
  }
  return prev;
}
