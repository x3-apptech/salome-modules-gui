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

#include "GLViewer_ViewFrame.h"
#include "GLViewer_Viewer.h"
#include "GLViewer_ViewPort2d.h"

#include <QtxToolBar.h>
#include <QtxMultiAction.h>
#include <QtxActionToolMgr.h>

#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_MessageBox.h>

#include <QColor>
#include <QFileDialog>
#include <QImage>
#include <QHBoxLayout>
#include <QString>
#include <QFrame>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>

#ifdef WIN32
#include <Standard_Integer.hxx>
#include <iostream>
#endif

/*!
    Constructor
*/
GLViewer_ViewFrame::GLViewer_ViewFrame( SUIT_Desktop* d, GLViewer_Viewer* vw )
: SUIT_ViewWindow( d ),
myViewer( vw ),
myVP( 0 )
{
    QFrame* client = new QFrame( this );
    setCentralWidget( client );

    QBoxLayout* layout = new QHBoxLayout( client );
    layout->setMargin( 0 );
    layout->setSpacing( 0 );

    GLViewer_ViewPort2d* vp = new GLViewer_ViewPort2d( client, this );
    setViewPort( vp );
    setBackgroundColor( Qt::white );
    layout->addWidget( vp );

    createActions();
    createToolBar();
}

/*!
    Destructor
*/
GLViewer_ViewFrame::~GLViewer_ViewFrame()
{
}

/*!
  Creates actions of GL view frame
*/
void GLViewer_ViewFrame::createActions()
{
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QtxAction* aAction;

  // Dump view
  aAction = new QtxAction(tr("MNU_DUMP_VIEW"), aResMgr->loadPixmap( "GLViewer", tr( "ICON_GL_DUMP" ) ),
                          tr( "MNU_DUMP_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_DUMP_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onDumpView()));
  toolMgr()->registerAction( aAction, DumpId );

  // FitAll
  aAction = new QtxAction(tr("MNU_FITALL"), aResMgr->loadPixmap( "GLViewer", tr( "ICON_GL_FITALL" ) ),
                          tr( "MNU_FITALL" ), 0, this);
  aAction->setStatusTip(tr("DSC_FITALL"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onViewFitAll()));
  toolMgr()->registerAction( aAction, FitAllId );

  // FitRect
  aAction = new QtxAction(tr("MNU_FITRECT"), aResMgr->loadPixmap( "GLViewer", tr( "ICON_GL_FITAREA" ) ),
                          tr( "MNU_FITRECT" ), 0, this);
  aAction->setStatusTip(tr("DSC_FITRECT"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onViewFitArea()));
  toolMgr()->registerAction( aAction, FitRectId );

  // FitSelect
  aAction = new QtxAction(tr("MNU_FITSELECT"), aResMgr->loadPixmap( "GLViewer", tr( "ICON_GL_FITSELECT" ) ),
                          tr( "MNU_FITSELECT" ), 0, this);
  aAction->setStatusTip(tr("DSC_FITSELECT"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onViewFitSelect()));
  toolMgr()->registerAction( aAction, FitSelectId );

  // Zoom
  aAction = new QtxAction(tr("MNU_ZOOM_VIEW"), aResMgr->loadPixmap( "GLViewer", tr( "ICON_GL_ZOOM" ) ),
                          tr( "MNU_ZOOM_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_ZOOM_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onViewZoom()));
  toolMgr()->registerAction( aAction, ZoomId );

  // Panning
  aAction = new QtxAction(tr("MNU_PAN_VIEW"), aResMgr->loadPixmap( "GLViewer", tr( "ICON_GL_PAN" ) ),
                          tr( "MNU_PAN_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_PAN_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onViewPan()));
  toolMgr()->registerAction( aAction, PanId );

  // Global Panning
  aAction = new QtxAction(tr("MNU_GLOBALPAN_VIEW"), aResMgr->loadPixmap( "GLViewer", tr( "ICON_GL_GLOBALPAN" ) ),
                          tr( "MNU_GLOBALPAN_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_GLOBALPAN_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onViewGlobalPan()));
  toolMgr()->registerAction( aAction, GlobalPanId );

  aAction = new QtxAction(tr("MNU_RESET_VIEW"), aResMgr->loadPixmap( "GLViewer", tr( "ICON_GL_RESET" ) ),
                          tr( "MNU_RESET_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_RESET_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onViewReset()));
  toolMgr()->registerAction( aAction, ResetId );
}

/*!
  Creates toolbar of GL view frame
*/
void GLViewer_ViewFrame::createToolBar()
{
  int tid = toolMgr()->createToolBar( tr( "LBL_TOOLBAR_LABEL" ),             // title (language-dependant)
				      QString( "GLViewerViewOperations" ),   // name (language-independant)
				      false );                               // disable floatable toolbar
  toolMgr()->append( DumpId, tid );

  QtxMultiAction* aScaleAction = new QtxMultiAction( this );
  aScaleAction->insertAction( toolMgr()->action( FitAllId ) );
  aScaleAction->insertAction( toolMgr()->action( FitRectId ) );
  aScaleAction->insertAction( toolMgr()->action( FitSelectId ) );
  aScaleAction->insertAction( toolMgr()->action( ZoomId ) );
  toolMgr()->append( aScaleAction, tid );

  QtxMultiAction* aPanAction = new QtxMultiAction( this );
  aPanAction->insertAction( toolMgr()->action( PanId ) );
  aPanAction->insertAction( toolMgr()->action( GlobalPanId ) );
  toolMgr()->append( aPanAction, tid );

  toolMgr()->append( toolMgr()->action( ResetId ), tid );
}

/*!
    Sets the viewport for this frame
*/
void GLViewer_ViewFrame::setViewPort( GLViewer_ViewPort* vp )
{
    if ( myVP == vp )
        return;

    if ( myVP )
    {
        disconnect( myVP, SIGNAL( vpDrawExternal( QPainter* ) ), this, SIGNAL( vfDrawExternal( QPainter* ) ) );
        disconnect( myVP, SIGNAL( vpMouseEvent( QMouseEvent* ) ), this, SLOT( mouseEvent( QMouseEvent* ) ) );
        disconnect( myVP, SIGNAL( vpKeyEvent( QKeyEvent* ) ), this, SLOT( keyEvent( QKeyEvent* ) ) );
        disconnect( myVP, SIGNAL( vpWheelEvent( QWheelEvent* ) ), this, SLOT( wheelEvent( QWheelEvent* ) ) );
        disconnect( myVP, SIGNAL( contextMenuRequested( QContextMenuEvent* ) ),
                    this, SIGNAL( contextMenuRequested( QContextMenuEvent* ) ) );
    }
    myVP = vp;
    if ( myVP )
    {
        connect( myVP, SIGNAL( vpDrawExternal( QPainter* ) ), this, SIGNAL( vfDrawExternal( QPainter* ) ) );
        connect( myVP, SIGNAL( vpMouseEvent( QMouseEvent* ) ), this, SLOT( mouseEvent( QMouseEvent* ) ) );
        connect( myVP, SIGNAL( vpKeyEvent( QKeyEvent* ) ), this, SLOT( keyEvent( QKeyEvent* ) ) );
        connect( myVP, SIGNAL( vpWheelEvent( QWheelEvent* ) ), this, SLOT( wheelEvent( QWheelEvent* ) ) );
        connect( myVP, SIGNAL( contextMenuRequested( QContextMenuEvent* ) ),
                 this, SIGNAL( contextMenuRequested( QContextMenuEvent* ) ) );
    }
}

/*!
    Returns the viewport of this frame. [ public ]
*/
GLViewer_ViewPort* GLViewer_ViewFrame::getViewPort() const
{
    return myVP;
}

/*!
    Set background of the viewport. [ public ]
*/
void GLViewer_ViewFrame::setBackgroundColor( const QColor& color )
{
    if ( myVP )
        myVP->setBackgroundColor( color );
}

/*!
    Returns background of the viewport. [ public ]
*/
QColor GLViewer_ViewFrame::backgroundColor() const
{
    if ( myVP )
        return myVP->backgroundColor();
    return palette().color( backgroundRole() );
}

/*!
    Sets the viewer for this view. [ public ]
*/
void GLViewer_ViewFrame::setViewer( GLViewer_Viewer* v )
{
    myViewer = v;
}

/*!
    Returns the viewer of this view. [ public ]
*/
GLViewer_Viewer* GLViewer_ViewFrame::getViewer() const
{
    return myViewer;
}

/*!
    Returns the preferred view size. [ virtual public ]
*/
QSize GLViewer_ViewFrame::sizeHint() const
{
    QWidget* p = parentWidget();
    if ( p && p->inherits( "QWorkspaceChild" ) )
        p = p->parentWidget();
    if ( !p )
        return QMainWindow::sizeHint();
    return QSize( 9 * p->width() / 10 , 9 * p->height() / 10  );
}

/*!
   Called by viewer's 'update()' method. Does nothing by default [ virtual public ]
*/
void GLViewer_ViewFrame::onUpdate( int )
{
}

//#include <windows.h>

/*!
  SLOT: called on dump view operation is activated, stores scene to raster file
*/

QImage GLViewer_ViewFrame::dumpView()
{
  QImage img;

  GLViewer_Widget* aWidget = ((GLViewer_ViewPort2d*)myVP)->getGLWidget();
  if ( aWidget )
    img = aWidget->grabFrameBuffer();

  return img;
}

/*!
  Start panning
*/
void GLViewer_ViewFrame::onViewPan()
{
    myViewer->activateTransform( GLViewer_Viewer::Pan );
}

/*!
  Start zooming
*/
void GLViewer_ViewFrame::onViewZoom()
{
    myViewer->activateTransform( GLViewer_Viewer::Zoom );
}

/*!
  Start fit all
*/
void GLViewer_ViewFrame::onViewFitAll()
{
    myViewer->activateTransform( GLViewer_Viewer::FitAll );
}

/*!
  Start fit area
*/
void GLViewer_ViewFrame::onViewFitArea()
{
    myViewer->activateTransform( GLViewer_Viewer::FitRect );
}

/*!
  Start fit selected
*/
void GLViewer_ViewFrame::onViewFitSelect()
{
    myViewer->activateTransform( GLViewer_Viewer::FitSelect );
}

/*!
  Start global panning
*/
void GLViewer_ViewFrame::onViewGlobalPan()
{
    myViewer->activateTransform( GLViewer_Viewer::PanGlobal );
}

/*!
  Start rotating
*/
void GLViewer_ViewFrame::onViewRotate()
{
    //myViewer->activateTransform( GLViewer_Viewer::Rotate );
}

/*!
  Start reset default view aspects
*/
void GLViewer_ViewFrame::onViewReset()
{
    myViewer->activateTransform( GLViewer_Viewer::Reset );
}

/*!
  Dispatches mouse events
*/
void GLViewer_ViewFrame::mouseEvent( QMouseEvent* e )
{
  switch ( e->type() )
  {
  case QEvent::MouseButtonPress:
    emit mousePressed( this, e );
    break;
  case QEvent::MouseButtonRelease:
    emit mouseReleased( this, e );
    break;
  case QEvent::MouseButtonDblClick:
    emit mouseDoubleClicked( this, e );
    break;
  case QEvent::MouseMove:
    emit mouseMoving( this, e );
    break;
  default:
    break;
  }
}

/*!
  Dispatches key events
*/
void GLViewer_ViewFrame::keyEvent( QKeyEvent* e )
{
  switch ( e->type() )
  {
  case QEvent::KeyPress:
    emit keyPressed( this, e );
    break;
  case QEvent::KeyRelease:
    emit keyReleased( this, e );
    break;
  default:
    break;
  }
}

/*!
  Dispatches wheel events
*/
void GLViewer_ViewFrame::wheelEvent( QWheelEvent* e )
{
  switch ( e->type() )
  {
  case QEvent::Wheel:
    emit wheeling( this, e );
    break;
  default:
    break;
  }
}

/*!
  \return the visual parameters of this view as a formated string
*/
QString GLViewer_ViewFrame::getVisualParameters()
{
  QString retStr;
  if ( myVP && myVP->inherits( "GLViewer_ViewPort2d" ) ) {
    GLViewer_ViewPort2d* vp2d = (GLViewer_ViewPort2d*)myVP;
    GLfloat xSc, ySc, xPan, yPan;
    vp2d->getScale( xSc, ySc );
    vp2d->getPan( xPan, yPan );
    retStr.sprintf( "%.12e*%.12e*%.12e*%.12e", xSc, ySc, xPan, yPan );
  }
  return retStr;
}

/*!
  The method restores visual parameters of this view from a formated string
*/
void GLViewer_ViewFrame::setVisualParameters( const QString& parameters )
{
  QStringList paramsLst = parameters.split( '*' );
  if ( myVP && myVP->inherits( "GLViewer_ViewPort2d" ) && paramsLst.size() == 4) {
    GLViewer_ViewPort2d* vp2d = (GLViewer_ViewPort2d*)myVP;

    GLfloat xSc, ySc, xPan, yPan;
    xSc = paramsLst[0].toDouble();
    ySc = paramsLst[1].toDouble();
    xPan = paramsLst[2].toDouble();
    yPan = paramsLst[3].toDouble();

    vp2d->getGLWidget()->setScale( xSc, ySc, 1. );
    vp2d->getGLWidget()->setPan( xPan, yPan, 0. );
  }
}
