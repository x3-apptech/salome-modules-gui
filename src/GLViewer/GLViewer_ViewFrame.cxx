//  Copyright (C) 2005 OPEN CASCADE
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  Author : OPEN CASCADE
//

// File:      GLViewer_ViewFrame.cxx
// Created:   November, 2004

//#include <GLViewerAfx.h>
#include "GLViewer_ViewFrame.h"
#include "GLViewer_Viewer.h"
#include "GLViewer_Viewer2d.h"
#include "GLViewer_ViewPort2d.h"

#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_ToolButton.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_MessageBox.h>

#include <qcolor.h>
#include <qfiledialog.h>
#include <qimage.h>
#include <qlayout.h>
#include <qstring.h>

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

    QBoxLayout* layout = new QHBoxLayout( client, 1, 1 );
    layout->setAutoAdd( true );

    GLViewer_ViewPort2d* vp = new GLViewer_ViewPort2d( client, this );
    //vp->turnGrid( true );
    //vp->turnCompass( true );
    //vp->enablePopup( false );
    setViewPort( vp );
    setBackgroundColor( Qt::white );

    myToolBar = new QToolBar(this);
    myToolBar->setCloseMode(QDockWindow::Undocked);
    myToolBar->setLabel(tr("LBL_TOOLBAR_LABEL"));
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
  if (!myActionsMap.isEmpty()) return;
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QAction* aAction;

  // Dump view
  aAction = new QAction(tr("MNU_DUMP_VIEW"), aResMgr->loadPixmap( "GLViewer", tr( "ICON_GL_DUMP" ) ),
                           tr( "MNU_DUMP_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_DUMP_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onViewDump()));
  myActionsMap[ DumpId ] = aAction;

  // FitAll
  aAction = new QAction(tr("MNU_FITALL"), aResMgr->loadPixmap( "GLViewer", tr( "ICON_GL_FITALL" ) ),
                           tr( "MNU_FITALL" ), 0, this);
  aAction->setStatusTip(tr("DSC_FITALL"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onViewFitAll()));
  myActionsMap[ FitAllId ] = aAction;

  // FitRect
  aAction = new QAction(tr("MNU_FITRECT"), aResMgr->loadPixmap( "GLViewer", tr( "ICON_GL_FITAREA" ) ),
                           tr( "MNU_FITRECT" ), 0, this);
  aAction->setStatusTip(tr("DSC_FITRECT"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onViewFitArea()));
  myActionsMap[ FitRectId ] = aAction;

  // FitSelect
  aAction = new QAction(tr("MNU_FITSELECT"), aResMgr->loadPixmap( "GLViewer", tr( "ICON_GL_FITSELECT" ) ),
                           tr( "MNU_FITSELECT" ), 0, this);
  aAction->setStatusTip(tr("DSC_FITSELECT"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onViewFitSelect()));
  myActionsMap[ FitSelectId ] = aAction;

  // Zoom
  aAction = new QAction(tr("MNU_ZOOM_VIEW"), aResMgr->loadPixmap( "GLViewer", tr( "ICON_GL_ZOOM" ) ),
                           tr( "MNU_ZOOM_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_ZOOM_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onViewZoom()));
  myActionsMap[ ZoomId ] = aAction;

  // Panning
  aAction = new QAction(tr("MNU_PAN_VIEW"), aResMgr->loadPixmap( "GLViewer", tr( "ICON_GL_PAN" ) ),
                           tr( "MNU_PAN_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_PAN_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onViewPan()));
  myActionsMap[ PanId ] = aAction;

  // Global Panning
  aAction = new QAction(tr("MNU_GLOBALPAN_VIEW"), aResMgr->loadPixmap( "GLViewer", tr( "ICON_GL_GLOBALPAN" ) ),
                           tr( "MNU_GLOBALPAN_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_GLOBALPAN_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onViewGlobalPan()));
  myActionsMap[ GlobalPanId ] = aAction;

  aAction = new QAction(tr("MNU_RESET_VIEW"), aResMgr->loadPixmap( "GLViewer", tr( "ICON_GL_RESET" ) ),
                           tr( "MNU_RESET_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_RESET_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onViewReset()));
  myActionsMap[ ResetId ] = aAction;
}

/*!
  Creates toolbar of GL view frame
*/
void GLViewer_ViewFrame::createToolBar()
{
  myActionsMap[DumpId]->addTo(myToolBar);

  SUIT_ToolButton* aScaleBtn = new SUIT_ToolButton(myToolBar);
  aScaleBtn->AddAction(myActionsMap[FitAllId]);
  aScaleBtn->AddAction(myActionsMap[FitRectId]);
  aScaleBtn->AddAction(myActionsMap[FitSelectId]);
  aScaleBtn->AddAction(myActionsMap[ZoomId]);

  SUIT_ToolButton* aPanBtn = new SUIT_ToolButton(myToolBar);
  aPanBtn->AddAction(myActionsMap[PanId]);
  aPanBtn->AddAction(myActionsMap[GlobalPanId]);

  myActionsMap[ResetId]->addTo(myToolBar);
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
    return QMainWindow::backgroundColor();
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
void GLViewer_ViewFrame::onViewDump()
{
    GLViewer_Widget* aWidget = ((GLViewer_ViewPort2d*)myVP)->getGLWidget();
    int width, height;
    width = aWidget->width();
    height = aWidget->height();
    
    int imageSize = width*height*3;
    unsigned char* imageBits = NULL;

    int reserve_bytes = width % 4; //32 bits platform
    imageSize = (width+reserve_bytes)*height*3;
    imageBits = new unsigned char[imageSize];

    
#ifdef WNT

    int num;
    HBITMAP hBmp;
    HDC hdc_old, hdc;
    HGLRC hglrc_old, hglrc;

    BITMAPINFO bi;

    hglrc_old = wglGetCurrentContext();
    hdc_old = wglGetCurrentDC();

    hdc = CreateCompatibleDC( hdc_old );
    if( !hdc )
    {
        cout << "Can't create compatible DC. Last Error Code: " << GetLastError() << endl;
        return;
    }

    int sizeBmi = Standard_Integer( sizeof(BITMAPINFO) + sizeof(RGBQUAD)*3 );
    PBITMAPINFO pBmi = (PBITMAPINFO)( new char[sizeBmi] );
    ZeroMemory( pBmi, sizeBmi );

    pBmi->bmiHeader.biSize        = sizeof( BITMAPINFOHEADER ); //sizeBmi
    pBmi->bmiHeader.biWidth       = width;
    pBmi->bmiHeader.biHeight      = height;
    pBmi->bmiHeader.biPlanes      = 1;
    pBmi->bmiHeader.biBitCount    = 24;
    pBmi->bmiHeader.biCompression = BI_RGB;

    LPVOID ppvBits;
    hBmp = CreateDIBSection ( hdc, pBmi, DIB_RGB_COLORS, &ppvBits, NULL, 0 );
    SelectObject ( hdc, hBmp );
    delete[] pBmi;

    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory( &pfd, sizeof( PIXELFORMATDESCRIPTOR ) );
    pfd.nSize      = sizeof( PIXELFORMATDESCRIPTOR );
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_BITMAP;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 32;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int iPf = ChoosePixelFormat( hdc, &pfd);    
    if( iPf == 0 )
    {
        if ( !DescribePixelFormat ( hdc, iPf, sizeof(PIXELFORMATDESCRIPTOR), &pfd ) )
        {
            cout << "Can't describe Pixel Format. Last Error Code: " << GetLastError() << endl;
        }
    }
    if ( !SetPixelFormat(hdc, iPf, &pfd) )
    {
        cout << "Can't set Pixel Format. Last Error Code: " << GetLastError() << endl;
    }

    hglrc = wglCreateContext( hdc );
    if( !hglrc )
    {
        cout << "Can't create new GL Context. Last Error Code: " << GetLastError() << endl;
        return;
    }
    if( !wglMakeCurrent( hdc, hglrc) )
    {
        cout << "Can't make current new context!" << endl;
        return;
    }
    
    glViewport( 0, 0, width, height );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    GLfloat w_c = width / 2., h_c = height / 2.; 

    gluOrtho2D( -w_c, w_c, -h_c, h_c ); 

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    //set background
    QColor aColor = ((GLViewer_ViewPort2d*)myVP)->backgroundColor();
    glClearColor( ( GLfloat )aColor.red() / 255,
                  ( GLfloat )aColor.green() / 255,
                  ( GLfloat )aColor.blue() / 255,
                  1.0 );

    aWidget->exportRepaint();

      memset(&bi, 0, sizeof(BITMAPINFOHEADER));
    bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biPlanes      = 1;
    bi.bmiHeader.biBitCount    = 24;
    bi.bmiHeader.biHeight      = -height;
    bi.bmiHeader.biWidth       = width;
    bi.bmiHeader.biCompression = BI_RGB;

    num = GetDIBits(hdc, hBmp, 0, height, imageBits, &bi, DIB_RGB_COLORS);

    wglMakeCurrent( hdc_old, hglrc_old );
    wglDeleteContext( hglrc );
    

#else //XWindows
#endif

    unsigned int* aPix = NULL;
    QImage  anImage( width, height, 32 );
    for( int i = 0; i < height; i++ )
    {
        memset( anImage.scanLine( i ), 0, sizeof(unsigned int)*width );
        unsigned char* pos;
        for( int j = 0; j < width; j++ )
        {
            pos = imageBits + i*width*3 + j*3 + reserve_bytes*i;
            aPix = (unsigned int*)anImage.scanLine(i)+j;
            *aPix = qRgb( *pos, *(pos+1), *(pos+2) );
        }
    }

    delete [] imageBits;

    QString aFilter( "*.bmp\n*.png" );

    QFileDialog aFileDlg( QDir::current().absPath(), aFilter, this );
    aFileDlg.setCaption( tr( "DUMP_VIEW_SAVE_FILE_DLG_CAPTION" ) );
    aFileDlg.setMode( QFileDialog::AnyFile );

    if( !aFileDlg.exec() )
        return;

    QString aFileName = aFileDlg.selectedFile();
    QString aFileExt = aFileDlg.selectedFilter();

    if( aFileName.isEmpty() )
    {
        SUIT_MessageBox::error1( this,
                                tr( "DUMP_VIEW_ERROR_DLG_CAPTION" ),
                                tr( "DUMP_VIEW_ERROR_DLG_TEXT" ),
                                tr( "BUT_OK" ) );
    }

    QString aSaveOp = "BMP";
    QString aTypedFileExt = QFileInfo( aFileName ).extension( false ).lower();

    if( aFileExt == "*.bmp" )
    {
        if( aTypedFileExt.isEmpty() )
            aFileName += ".bmp";
        aSaveOp = "BMP";
    }
    else if( aFileExt == "*.png" )
        if( aTypedFileExt.isEmpty() )
            aFileName += ".png";
        aSaveOp = "PNG";

//#ifdef WNT
//    if( !anImage.save( aFileName, aSaveOp ) )
//#else
    if( !aWidget->grabFrameBuffer().save( aFileName, aSaveOp ) )
//#endif
    {
        SUIT_MessageBox::error1( this,
                                tr( "DUMP_VIEW_ERROR_DLG_CAPTION" ),
                                tr( "DUMP_VIEW_ERROR_DLG_TEXT" ),
                                tr( "BUT_OK" ) );
    }
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
  QStringList paramsLst = QStringList::split( '*', parameters, true );
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
