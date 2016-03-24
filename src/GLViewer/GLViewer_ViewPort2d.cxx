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

//  Author : OPEN CASCADE
// File:      GLViewer_ViewPort2d.cxx
// Created:   November, 2004
//
/* GLViewer_ViewPort2d Source File */

//#include <GLViewerAfx.h>
#include "GLViewer_ViewPort2d.h"
#include "GLViewer_Viewer2d.h"
#include "GLViewer_ViewFrame.h"
#include "GLViewer_MimeData.h"
#include "GLViewer_Context.h"
#include "GLViewer_Compass.h"
#include "GLViewer_Grid.h"
#include "GLViewer_Drawer.h"

// TODO: Porting to Qt4
//#include <QtxToolTip.h>

#include <QHBoxLayout>
#include <QMouseEvent>
#include <QRect>
//#include <QMenu>
//#include <QToolTip>
#include <QApplication>
#include <QClipboard>
#include <QBitmap>
#include <QLabel>
#include <QWidget>
#include <QRubberBand>

#define WIDTH       640
#define HEIGHT      480
#define MARGIN      100

#define GRID_XSIZE  100
#define GRID_YSIZE  100

int static aLastViewPostId = 0;

void rotate_point( float& theX, float& theY, float theAngle )
{
    float aTempX = theX * cos(theAngle) - theY * sin(theAngle);
    float aTempY = theX * sin(theAngle) + theY * cos(theAngle);
    theX = aTempX;
    theY = aTempY;
}

/*!
  Constructor
*/
GLViewer_ViewPort2d::GLViewer_ViewPort2d( QWidget* parent, GLViewer_ViewFrame* theViewFrame )
: GLViewer_ViewPort( parent ),
  myMargin( MARGIN ), myWidth( WIDTH ), myHeight( HEIGHT ),
  myXScale( 1.0 ), myYScale( 1.0 ), myXOldScale( 1.0 ), myYOldScale( 1.0 ),
  myXPan( 0.0 ), myYPan( 0.0 ),
  myIsMouseReleaseBlock( false ),
  myRectBand( 0 )
{
  if ( !theViewFrame )
    myViewFrame = (GLViewer_ViewFrame*)parent;
  else
    myViewFrame = theViewFrame;

  myGrid = 0;
  myCompass = 0;
  myBorder = new GLViewer_Rect();

  QBoxLayout* qbl = new QHBoxLayout( this );
  qbl->setSpacing( 0 );
  qbl->setMargin( 0 );

  myGLWidget = new GLViewer_Widget( this, 0 ) ;
  qbl->addWidget( myGLWidget );
  myGLWidget->setFocusProxy( this );
  setMouseTracking( true );

  myIsDragProcess = noDrag;
  //myCurDragMousePos = QPoint();
  myCurDragPosX = NULL;
  myCurDragPosY = NULL;

  myIsPulling = false;

  myViewPortId = aLastViewPostId;
  aLastViewPostId++;

  mypFirstPoint = NULL;
  mypLastPoint = NULL;

    // TODO: Porting to Qt4
    /*myObjectTip = new QtxToolTip( myGLWidget );///GLViewer_ObjectTip( this );
    myObjectTip->setShowDelayTime( 60000 );
    connect( myObjectTip, SIGNAL( maybeTip( QPoint, QString&, QFont&, QRect&, QRect& ) ),
             this, SLOT( onMaybeTip( QPoint, QString&, QFont&, QRect&, QRect& ) ) );*/
//    myGLWidget->installEventFilter( myObjectTip );
}

/*!
  Destructor
*/
GLViewer_ViewPort2d::~GLViewer_ViewPort2d()
{
    if( myCompass )
        delete myCompass;

    if( myGrid )
        delete myGrid;

    delete myBorder;
    delete myGLWidget;

    if ( myRectBand ) myRectBand->hide();
    delete myRectBand;
}

/*!
  SLOT: initializes drag process
*/
void GLViewer_ViewPort2d::onStartDragObject( )
{
    if( myIsDragProcess == noDrag )
    {
        myIsDragProcess = initDrag;
        QCursor::setPos( (int)(*myCurDragPosX), (int)(*myCurDragPosY) );
        //myCurDragMousePos = QPoint( 0, 0 );
        delete myCurDragPosX;
        delete myCurDragPosY;
        myCurDragPosX = NULL;
        myCurDragPosY = NULL;
        return;
    }
}

/*!
  SLOT: cuts object to clipboard
*/
void GLViewer_ViewPort2d::onCutObject()
{
    /*GLViewer_Object* aMovingObject = ((GLViewer_Viewer2d*)getViewFrame()->getViewer())->getGLContext()->getCurrentObject();
    if( aMovingObject )
    {
        GLViewer_MimeSource* aMimeSource = new GLViewer_MimeSource();
        aMimeSource->setObject( aMovingObject );

        QClipboard *aClipboard = QApplication::clipboard();
        aClipboard->clear();
        aClipboard->setData( aMimeSource );

        ((GLViewer_Viewer2d*)getViewFrame()->getViewer())->getGLContext()->deleteObject( aMovingObject );
    }*/
    GLViewer_Context* aContext = ((GLViewer_Viewer2d*)getViewFrame()->getViewer())->getGLContext();
    int aObjNum = aContext->NbSelected();
    if( aObjNum > 0 )
    {
        QList<GLViewer_Object*> aObjects;
        GLViewer_MimeData* aMimeData = new GLViewer_MimeData();
        aContext->InitSelected();
        for( ; aContext->MoreSelected(); aContext->NextSelected() )
            aObjects.append( aContext->SelectedObject() );

        //aMimeData->setObjects( aObjects ); ouv 6.05.04

        QClipboard *aClipboard = QApplication::clipboard();
        aClipboard->clear();
        aClipboard->setMimeData( aMimeData );

        for( int i = 0; i < aObjNum; i++ )
            aContext->deleteObject( aObjects[i] );
    }
}

/*!
  SLOT: copies object to clipboard
*/
void GLViewer_ViewPort2d::onCopyObject()
{
    /*GLViewer_Object* aMovingObject = ((GLViewer_Viewer2d*)getViewFrame()->getViewer())->getGLContext()->getCurrentObject();
    if( aMovingObject )
    {
        GLViewer_MimeSource* aMimeSource = new GLViewer_MimeSource();
        aMimeSource->setObject( aMovingObject );

        QClipboard *aClipboard = QApplication::clipboard();
        aClipboard->clear();
        aClipboard->setData( aMimeSource );
    }
    */
    GLViewer_Context* aContext = ((GLViewer_Viewer2d*)getViewFrame()->getViewer())->getGLContext();
    int aObjNum = aContext->NbSelected();
    if( aObjNum > 0 )
    {
        QList<GLViewer_Object*> aObjects;
        GLViewer_MimeData* aMimeData = new GLViewer_MimeData();
        aContext->InitSelected();
        for( ; aContext->MoreSelected(); aContext->NextSelected() )
            aObjects.append( aContext->SelectedObject() );

        //aMimeData->setObjects( aObjects ); ouv 6.05.04

        QClipboard *aClipboard = QApplication::clipboard();
        aClipboard->clear();
        aClipboard->setMimeData( aMimeData );
    }
}

/*!
  SLOT: pastes object from clipboard
*/
void GLViewer_ViewPort2d::onPasteObject()
{
    /*QClipboard *aClipboard = QApplication::clipboard();
    QMimeSource* aMimeSource = aClipboard->data();
    if( aMimeSource->provides( "GLViewer_Object" ) )
    {
        const char* aType;
        int i = 1;
        QByteArray anArray;
        do
        {
            aType = aMimeSource->format( i );
            anArray = aMimeSource->encodedData( aType );
            if( anArray.size() != 0 )
                break;
            i++;
        }
        while( aType != 0 );
        if( anArray.size() == 0 )
            return;

        GLViewer_Object* aObject = GLViewer_MimeSource::getObject( anArray, aType );
        if( !aObject )
            return;

        ((GLViewer_Viewer2d*)getViewFrame()->getViewer())->getGLContext()->insertObject( aObject, true );
    }
    */
    /* ouv 6.05.04
    QClipboard *aClipboard = QApplication::clipboard();

    QMimeSource* aMimeSource = aClipboard->data();
    if( aMimeSource->provides( "GLViewer_Objects" ) )
    {
        QByteArray anArray = aMimeSource->encodedData( "GLViewer_Objects" );
        QList<GLViewer_Object*> aObjects = GLViewer_MimeSource::getObjects( anArray, "GLViewer_Objects" );
        if( aObjects.empty() )
            return;
        GLViewer_Context* aContext = ((GLViewer_Viewer2d*)getViewFrame()->getViewer())->getGLContext();
        for( int i = 0; i < aObjects.count(); i++ )
            aContext->insertObject( aObjects[i], true );
    }
    */
}

/*!
  SLOT: called when object is being dragged
*/
void GLViewer_ViewPort2d::onDragObject( QMouseEvent* e )
{
  //cout << "---GLViewer_ViewPort2d::onDragObject()---" << endl;
  GLViewer_Viewer2d* aViewer = (GLViewer_Viewer2d*)getViewFrame()->getViewer();
  GLViewer_Context* aContext = aViewer->getGLContext();
  GLViewer_Object* anObject = aContext->getCurrentObject();

  if( !aContext )
    return;

  float aX = e->pos().x();
  float anY = e->pos().y();
  aViewer->transPoint( aX, anY );

  if( myCurDragPosX == NULL && myCurDragPosY == NULL )
  {
    myCurDragPosX = new float(aX);
    myCurDragPosY = new float(anY);
    return;
  }

  //QPoint aNewPos = e->pos();
  //GLViewer_Viewer2d* aViewer = (GLViewer_Viewer2d*)getViewFrame()->getViewer();

  if( anObject && (e->buttons() & Qt::LeftButton ) )
  {
    if( aContext->isSelected( anObject ) )
    {
      for( aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected() )
      {
        GLViewer_Object* aMovingObject = aContext->SelectedObject();
        if( aMovingObject )
          aMovingObject->moveObject( aX - *myCurDragPosX, anY - *myCurDragPosY);
      }
    }
    else
      anObject->moveObject( aX - *myCurDragPosX, anY - *myCurDragPosY);
  }
  else if( aContext->NbSelected() && (e->buttons() & Qt::MidButton ) )
    for( aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected() )
        (aContext->SelectedObject())->moveObject( aX - *myCurDragPosX, anY - *myCurDragPosY);

  delete myCurDragPosX;
  delete myCurDragPosY;
  myCurDragPosX = new float(aX);
  myCurDragPosY = new float(anY);

  myGLWidget->updateGL();
}

/*!
    Emits 'mouseEvent' signal. [ virtual protected ]
*/
void GLViewer_ViewPort2d::mousePressEvent( QMouseEvent* e )
{
    emit vpMouseEvent( e );

    GLViewer_Viewer2d* aViewer = (GLViewer_Viewer2d*)getViewFrame()->getViewer();
    GLViewer_Context* aContext = aViewer->getGLContext();

    GLViewer_Object* anObject = NULL;
    if( aContext )
        anObject = aContext->getCurrentObject();

    bool accel = e->modifiers() & GLViewer_ViewTransformer::accelKey();
    if( ( anObject && !( accel || e->button() == Qt::RightButton ) ) ||
        ( aContext->NbSelected() && !accel && e->button() == Qt::MidButton )  )
    {
        myIsDragProcess = inDrag;
    }
}

/*!
    Emits 'mouseEvent' signal. [ virtual protected ]
*/
void GLViewer_ViewPort2d::mouseMoveEvent( QMouseEvent* e )
{
    emit vpMouseEvent( e );

    if( myIsDragProcess == inDrag )
        onDragObject( e );

    /*GLViewer_Viewer2d* aViewer = (GLViewer_Viewer2d*)getViewFrame()->getViewer();
    GLViewer_Context* aContext = aViewer->getGLContext();

    GLViewer_Object* anObj = aContext->getCurrentObject();
    if( anObj && aContext->currentObjectIsChanged() )
    {
        //cout << "GLViewer_ViewPort2d::mouseMoveEvent{QToolTip::add}" << endl;
        //QToolTip::remove( myGLWidget );
        QRect* aRect = (aViewer->getWinObjectRect(anObj));
        //QToolTip::add( myGLWidget, *aRect, anObj->getToolTipText() );
        myGLWidget->addToolTip( anObj->getToolTipText(), *aRect );
    }
    if(!anObj)
    {
        //cout << "GLViewer_ViewPort2d::mouseMoveEvent{QToolTip::remove}" << endl;
        //QRect* aRect = (aViewer->getWinObjectRect(anObj));
        //QToolTip::remove( myGLWidget, *aRect );
        myGLWidget->removeToolTip();
    }*/
}

/*!
    Emits 'mouseEvent' signal. [ virtual protected ]
*/
void GLViewer_ViewPort2d::mouseReleaseEvent( QMouseEvent* e )
{
    if ( myIsMouseReleaseBlock )
    {
      // skip mouse release after double click
      myIsMouseReleaseBlock = false;
      return;
    }

    /* show popup menu */
    if ( e->button() == Qt::RightButton )
    {
        //QPopupMenu* popup = createPopup();
        //if ( popup && popup->count() )
        //    popup->exec( QCursor::pos() );
        //destroyPopup( /*popup*/ );
    }
    emit vpMouseEvent( e );

    if( myIsDragProcess == inDrag )
    {
      bool isAnyMoved = false;
      GLViewer_Viewer2d* aViewer = (GLViewer_Viewer2d*)getViewFrame()->getViewer();
      GLViewer_Context* aContext = aViewer->getGLContext();
      GLViewer_Object* aMovingObject;
      for( aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected() )
      {
        aMovingObject = aContext->SelectedObject();
        if( aMovingObject )
          isAnyMoved = aMovingObject->finishMove() || isAnyMoved;
      }

      aMovingObject = aContext->getCurrentObject();
      if( aMovingObject )
        isAnyMoved = aMovingObject->finishMove() || isAnyMoved;

      myIsDragProcess = noDrag;
      //myCurDragMousePos.setX( 0 );
      //myCurDragMousePos.setY( 0 );
      delete myCurDragPosX;
      delete myCurDragPosY;
      myCurDragPosX = NULL;
      myCurDragPosY = NULL;

      if( isAnyMoved )
      {
        emit objectMoved();
        aViewer->updateBorders();
      }
    }
}

/*!
  Custom mouse double click event handler
*/
void GLViewer_ViewPort2d::mouseDoubleClickEvent( QMouseEvent * e )
{
  //redefined to block mouse release after mouse double click
  myIsMouseReleaseBlock = true;
  // invoke base implementation
  GLViewer_ViewPort::mouseDoubleClickEvent( e );
}

/*!
  Creates or deletes compass
  \param on - if it is true, then to create
*/
void GLViewer_ViewPort2d::turnCompass( GLboolean on )
{
    if( on )
        myCompass = new GLViewer_Compass( Qt::green, 30, GLViewer_Compass::TopRight, 10, 5, 12, 3 );
    else if( myCompass )
        delete myCompass;
}

/*!
  Creates or deletes grid
  \param on - if it is true, then to create
*/
void GLViewer_ViewPort2d::turnGrid( GLboolean on )
{
    if( on )
    {
        myGrid = new GLViewer_Grid( 2*WIDTH, 2*HEIGHT,
                                    2*WIDTH, 2*HEIGHT,
                                    GRID_XSIZE, GRID_YSIZE,
                                    myXPan, myYPan,
                                    myXScale, myYScale );
    }
    else if( myGrid )
        delete myGrid;
}

/*!
  Changes grid color
  \param gridColor - new grid color
  \param axisColor - new axis color
*/
void GLViewer_ViewPort2d::setGridColor( const QColor gridColor, const QColor axisColor )
{
    if( myGrid )
    {
        myGrid->setGridColor( ( GLfloat )gridColor.red() / 255,
                  ( GLfloat )gridColor.green() / 255,
                  ( GLfloat )gridColor.blue() / 255 );
        myGrid->setAxisColor( ( GLfloat )axisColor.red() / 255,
                  ( GLfloat )axisColor.green() / 255,
                  ( GLfloat )axisColor.blue() / 255 );
    }
}

/*!
  Changes background color
  \param color - new background color
*/
void GLViewer_ViewPort2d::setBackgroundColor( const QColor& color )
{
    GLViewer_ViewPort::setBackgroundColor( color );
    myGLWidget->makeCurrent();
    glClearColor( ( GLfloat )color.red() / 255,
            ( GLfloat )color.green() / 255,
            ( GLfloat )color.blue() / 255, 1.0 );
    myGLWidget->repaint();
}

/*!
  \return background color
*/
QColor GLViewer_ViewPort2d::backgroundColor() const
{
    return GLViewer_ViewPort::backgroundColor();
}

/*!
  Resize view
*/
void GLViewer_ViewPort2d::initResize( int x, int y )
{
    float xa, xb, ya, yb;
    xa = myBorder->left() - myMargin;
    xb = myBorder->right() + myMargin;
    ya = myBorder->top() - myMargin;
    yb = myBorder->bottom() + myMargin;

    GLfloat zoom, xzoom, yzoom;
    GLfloat w = x;
    GLfloat h = y;
    bool max = false;

    xzoom = (GLfloat)x / myWidth;
    yzoom = (GLfloat)y / myHeight;

    if ( ( xzoom < yzoom ) && ( xzoom < 1 ) )
        zoom = xzoom;
    else if ( ( yzoom < xzoom ) && ( yzoom < 1 ) )
        zoom = yzoom;
    else
    {
        max = true;
        zoom = xzoom > yzoom ? xzoom : yzoom;
    }

    if ( !max && ( ! ( ( ( myXPan + w/2 ) < xb * myXScale * zoom ) ||
             ( ( myXPan - w/2 ) > xa * myXScale * zoom ) ||
             ( ( myYPan + h/2 ) < yb * myYScale * zoom ) ||
             ( ( myYPan - h/2 ) > ya * myYScale * zoom ) ) ) )
        zoom = 1;

    if ( max && ( ( ( myXPan + w/2 ) < xb * myXScale * zoom ) ||
            ( ( myXPan - w/2 ) > xa * myXScale * zoom ) ||
            ( ( myYPan + h/2 ) < yb * myYScale * zoom ) ||
            ( ( myYPan - h/2 ) > ya * myYScale * zoom ) ) )
        zoom = 1;

    myWidth = x;
    myHeight = y;

    myXScale *= zoom;
    myYScale = myXScale;

    if ( myGrid )
        myGrid->setResize( 2*x, 2*y, zoom );

    myGLWidget->setScale( myXScale, myYScale, 1.0 );
}

/*!
  Custom paint event handler
*/
void GLViewer_ViewPort2d::paintEvent( QPaintEvent* e )
{
    //cout << "GLViewer_ViewPort2d::paintEvent" << endl;
    myGLWidget->updateGL();
    GLViewer_ViewPort::paintEvent( e );
}

/*!
  Custom resize event handler
*/
void GLViewer_ViewPort2d::resizeEvent( QResizeEvent* e )
{
    //cout << "GLViewer_ViewPort2d::resizeEvent" << endl;
    GLViewer_ViewPort::resizeEvent( e );
}

/*!
  Resets view to start state
*/
void GLViewer_ViewPort2d::reset()
{
    //cout << "GLViewer_ViewPort2d::reset" << endl;

    GLint val[4];
    GLint vpWidth, vpHeight;

    myGLWidget->makeCurrent();
    glGetIntegerv( GL_VIEWPORT, val );
    vpWidth = val[2];
    vpHeight = val[3];

    GLint w = myGLWidget->getWidth();
    GLint h = myGLWidget->getHeight();
    GLfloat zoom = vpWidth / ( GLfloat )w < vpHeight / ( GLfloat )h ?
                 vpWidth / ( GLfloat )w : vpHeight / ( GLfloat )h;

    if( myGrid )
    {
        myGrid->setPan( 0.0, 0.0 );
        myGrid->setZoom( zoom / myXScale );
    }

    myXPan = 0.0;
    myYPan = 0.0;
    myXScale = zoom;
    myYScale = zoom;

    myGLWidget->setPan( myXPan, myYPan, 0.0 );
    myGLWidget->setScale( myXScale, myYScale, 1.0 );
    myGLWidget->setRotationAngle( 0.0 );
    myGLWidget->setRotation( 0.0, 0.0, 0.0, 1.0 );
    myGLWidget->updateGL();
}

/*!
  Sets offset to view
  \param dx - X offset
  \param dy - Y offset
*/
void GLViewer_ViewPort2d::pan( int dx, int dy )
{
    //cout << "GLViewer_ViewPort2d::pan " << dx << " " << dy << endl;

    /*myXPan += dx / myXScale;
    myYPan += dy / myYScale;

    float ra, rx, ry, rz;
    myGLWidget->getRotation( ra, rx, ry, rz );
    GLfloat angle = ra * PI / 180.;

    if( myGrid )
        myGrid->setPan( myXPan*cos(angle) + myYPan*sin(angle),
                        -myXPan*sin(angle) + myYPan*cos(angle) );

    */
    float ra, rx, ry, rz;
    myGLWidget->getRotation( ra, rx, ry, rz );
    GLfloat angle = ra * PI / 180.;

    myXPan += (dx*cos(angle) + dy*sin(angle)) / myXScale;
    myYPan += (-dx*sin(angle) + dy*cos(angle)) / myXScale;

    if( myGrid )
        myGrid->setPan( myXPan, myYPan );

    myGLWidget->setPan( myXPan, myYPan, 0.0 );
    myGLWidget->setScale( myXScale, myYScale, 1.0 );
    myGLWidget->updateGL();
}

/*!
  Sets view center in global coords
  \param x, y - global co-ordinates of center
*/
void GLViewer_ViewPort2d::setCenter( int x, int y )
{
    //cout << "GLViewer_ViewPort2d::setCenter" << endl;

    GLint val[4];
    GLint vpWidth, vpHeight;

    myGLWidget->makeCurrent();
    glGetIntegerv( GL_VIEWPORT, val );
    vpWidth = val[2];
    vpHeight = val[3];

    myXPan -= ( x - vpWidth/2 ) / myXScale;
    myYPan += ( y - vpHeight/2 ) / myYScale;

    if( myGrid )
    {
        myGrid->setPan( myXPan, myYPan );
        myGrid->setZoom( myXOldScale / myXScale );
    }

    myXScale = myXOldScale;
    myYScale = myYOldScale;

    myGLWidget->setPan( myXPan, myYPan, 0.0 );
    myGLWidget->setScale( myXScale, myYScale, 1.0 );
    myGLWidget->updateGL();
}

/*!
  Process zoming transformation with mouse tracking from ( x0, y0 ) to ( x1, y1 )
*/
void GLViewer_ViewPort2d::zoom( int x0, int y0, int x, int y )
{
    //cout << "GLViewer_ViewPort2d::zoom" << endl;

    float dx, dy, zm;
    dx = x - x0;
    dy = y - y0;

    if ( dx == 0. && dy == 0. )
        return;

    zm = sqrt(dx * dx + dy * dy) / 100. + 1;
    zm = (dx > 0.) ?  zm : 1. / zm;

    //backup values
    float bX = myXScale;
    float bY = myYScale;
    myXScale *= zm;
    myYScale *= zm;

    if( myGrid )
    {
        if( myGrid->setZoom( zm ) )
        {
            myGLWidget->setPan( myXPan, myYPan, 0.0 );
            myGLWidget->setScale( myXScale, myYScale, 1.0 );
            myGLWidget->updateGL();
        }
        else
        {// undo
            myXScale = bX;
            myYScale = bY;
        }
    }
    else
    {
        myGLWidget->setPan( myXPan, myYPan, 0.0 );
        myGLWidget->setScale( myXScale, myYScale, 1.0 );
        myGLWidget->updateGL();
    }
}

/*!
  Transforms view by rectangle
  \param rect - rectangle
*/
void GLViewer_ViewPort2d::fitRect( const QRect& rect )
{
    float x0, x1, y0, y1;
    float dx, dy, zm, centerX, centerY;

    GLint val[4];
    GLint vpWidth, vpHeight;

    myGLWidget->makeCurrent();
    glGetIntegerv( GL_VIEWPORT, val );
    vpWidth = val[2];
    vpHeight = val[3];

    x0 = rect.left();
    x1 = rect.right();
    y0 = rect.top();
    y1 = rect.bottom();

    dx = fabs( x1 - x0 );
    dy = fabs( y1 - y0 );
    centerX = ( x0 + x1 ) / 2.;
    centerY = ( y0 + y1 ) / 2.;

    if ( dx == 0. || dy == 0. )
        return;

    zm = vpWidth / dx < vpHeight / dy ? vpWidth / dx : vpHeight / dy;

    float aDX = ( vpWidth / 2. - centerX ) / myXScale;
    float aDY = ( vpHeight / 2. - centerY ) / myYScale;

    float ra, rx, ry, rz;
    myGLWidget->getRotation( ra, rx, ry, rz );
    GLfloat angle = ra * PI / 180.;

    myXPan += (aDX*cos(angle) - aDY*sin(angle));
    myYPan -= (aDX*sin(angle) + aDY*cos(angle));

    if( myGrid )
        myGrid->setPan( myXPan, myYPan );

    myXScale *= zm;
    myYScale = myXScale;

    if( myGrid )
        myGrid->setZoom( zm );

    myGLWidget->setPan( myXPan, myYPan, 0.0 );
    myGLWidget->setScale( myXScale, myYScale, 1.0 );
    myGLWidget->updateGL();
}

/*!
  Transforms view by selection
*/
void GLViewer_ViewPort2d::fitSelect()
{
  GLViewer_Viewer2d* aViewer = (GLViewer_Viewer2d*)getViewFrame()->getViewer();
  GLViewer_Context* aContext = aViewer->getGLContext();
  if( !aContext )
    return;

  QRect aSelRect;
  for( aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected() )
    aSelRect |= *(aViewer->getWinObjectRect( aContext->SelectedObject() ));

  if( aSelRect.isValid() )
  {
    aSelRect.setTop( aSelRect.top() - SELECTION_RECT_GAP );
    aSelRect.setBottom( aSelRect.bottom() + SELECTION_RECT_GAP );
    aSelRect.setLeft( aSelRect.left() - SELECTION_RECT_GAP );
    aSelRect.setRight( aSelRect.right() + SELECTION_RECT_GAP );
    fitRect( aSelRect );
  }
}

/*!
  Transform view by view borders
  \param keepScale - if it is true, zoom does not change
*/
void GLViewer_ViewPort2d::fitAll( bool keepScale, bool withZ )
{
    //cout << "GLViewer_ViewPort2d::fitAll" << endl;

    float xa, xb, ya, yb;
    float dx, dy, zm;
    float xScale, yScale;

    myMargin = qMax( myBorder->width(), myBorder->height() ) / 5;

    xa = myBorder->left() - myMargin;
    xb = myBorder->right() + myMargin;
    ya = myBorder->bottom() - myMargin;
    yb = myBorder->top() + myMargin;

    float aPoints[8] = { xa, ya, xb, ya, xa, yb, xb, yb };

    float ra, rx, ry, rz;
    myGLWidget->getRotation( ra, rx, ry, rz );
    float angle = ra * PI / 180.;

    int i;
    for( i = 0; i < 7; i = i + 2 )
        rotate_point( aPoints[i], aPoints[i+1], angle );

    float aBorders[4] = { aPoints[0], aPoints[0], aPoints[1], aPoints[1] };

    for( i = 2; i < 7; i = i + 2 )
    {
        if( aBorders[0] < aPoints[i] )
            aBorders[0] = aPoints[i];
        if( aBorders[1] > aPoints[i] )
            aBorders[1] = aPoints[i];

        if( aBorders[2] < aPoints[i+1] )
            aBorders[2] = aPoints[i+1];
        if( aBorders[3] > aPoints[i+1] )
            aBorders[3] = aPoints[i+1];
    }

    GLint val[4];
    GLint vpWidth, vpHeight;

    myGLWidget->makeCurrent();
    glGetIntegerv( GL_VIEWPORT, val );
    vpWidth = val[2];
    vpHeight = val[3];

    dx = fabs( aBorders[1] - aBorders[0] );
    dy = fabs( aBorders[3] - aBorders[2] );

    myXPan = -( aBorders[0] + aBorders[1] ) / 2;
    myYPan = -( aBorders[2] + aBorders[3] ) / 2;


    if( keepScale )
    {
        myXOldScale = myXScale;
        myYOldScale = myYScale;
    }

    xScale = myXScale;
    yScale = myYScale;
    if( dx && dy )
        zm = vpWidth / dx < vpHeight / dy ? vpWidth / dx : vpHeight / dy;
    else
        zm = 1.0;
    myXScale = zm;
    myYScale = zm;


    if( myGrid )
    {
        myGrid->setPan( myXPan, myYPan );
        if( dx > dy )
            myGrid->setZoom(  zm / xScale );
        else
            myGrid->setZoom( zm / yScale );
    }

    myGLWidget->setPan( myXPan, myYPan, 0.0 );
    myGLWidget->setScale( myXScale, myYScale, 1.0 );
    myGLWidget->updateGL();

    if( keepScale )
        emit vpUpdateValues();
}

/*!
  Begins rotation
  \param x, y - start point
*/
void GLViewer_ViewPort2d::startRotation( int x, int y )
{
    myGLWidget->setRotationStart( x, y, 1.0 );
}

/*!
  Performs rotation
  \param intX, intY - current point
*/
void GLViewer_ViewPort2d::rotate( int intX, int intY )
{
    GLint val[4];
    GLint vpWidth, vpHeight;

    myGLWidget->makeCurrent();
    glGetIntegerv( GL_VIEWPORT, val );
    vpWidth = val[2];
    vpHeight = val[3];

    float x = intX, y = intY;
    float x0 = vpWidth/2;
    float y0 = vpHeight/2;

    float xs, ys, zs, dx, dy;
    myGLWidget->getRotationStart( xs, ys, zs );

    xs = xs - x0;
    x = x - x0;
    dx = x - xs;
    ys = y0 - ys;
    y = y0 - y;
    dy = y - ys;

    float l1 = pow( double( xs*xs + ys*ys ), 0.5 );
    float l2 = pow( double( x*x + y*y ), 0.5 );
    float l = pow( double( dx*dx + dy*dy ), 0.5 );

    double mult = xs * y - x * ys;
    short sign;
    if( mult > 0 ) sign = 1;
    else if( mult < 0 ) sign = -1;
    else sign = 0;

    float anglePrev = myGLWidget->getRotationAngle();
    float angleNew = sign * acos( ( l1*l1 + l2*l2 - l*l ) / ( 2 * l1 * l2 )) * 180. / PI;
    float angle = anglePrev + angleNew;

    // GLfloat anAngle = angle * PI / 180.; unused

    float ra, rx, ry, rz;
    myGLWidget->getRotation( ra, rx, ry, rz );
    myGLWidget->setRotation( angle, rx, ry, rz );
    myGLWidget->updateGL();
}

/*!
  Finishes rotation
*/
void GLViewer_ViewPort2d::endRotation()
{
    float ra, rx, ry, rz;
    myGLWidget->getRotation( ra, rx, ry, rz );
    myGLWidget->setRotationAngle( ra );
}

/*!
  Draws compass
*/
void GLViewer_ViewPort2d::drawCompass()
{
    if( !myCompass->getVisible() )
        return;

    GLfloat xScale, yScale, xPan, yPan;

    int xPos = getWidth();
    int yPos = getHeight();

    int cPos = myCompass->getPos();
    int cSize = myCompass->getSize();
    QColor cCol = myCompass->getColor();
    int cWidthTop = myCompass->getArrowWidthTop();
    int cWidthBot = myCompass->getArrowWidthBottom();
    int cHeightTop = myCompass->getArrowHeightTop();
    int cHeightBot = myCompass->getArrowHeightBottom();

    GLfloat colorR = (cCol.red())/255;
    GLfloat colorG = (cCol.green())/255;
    GLfloat colorB = (cCol.blue())/255;

    float delX = cSize * 0.5;
    float delY = cSize * 0.5;

    getScale( xScale, yScale );
    getPan( xPan, yPan);

    float centerX = (xPos/2 - delX - cSize)/xScale;
    float centerY = (yPos/2 - delY - cSize)/yScale;

    switch ( cPos )
    {
    case GLViewer_Compass::TopLeft:
            centerX = -centerX;
            break;
        case GLViewer_Compass::BottomLeft:
            centerX = -centerX;
            centerY = -centerY;
            break;
        case GLViewer_Compass::BottomRight:
            centerY = -centerY;
            break;
        default: break;
    }

    float ra, rx, ry, rz;
    myGLWidget->getRotation( ra, rx, ry, rz );
    GLfloat angle = ra * PI / 180.;
    GLfloat /*r = 0.0,*/ x = 0.0 , y = 0.0;

    rotate_point( centerX, centerY, -angle );

    centerX -= xPan;
    centerY -= yPan;

    glColor3f( colorR, colorG, colorB );
    glBegin( GL_POLYGON );
    //arrow
        x = centerX;                      y = centerY + cSize / yScale;
        glVertex2f( x, y );
        //point #2
        x = centerX + cWidthTop / xScale; y = centerY + ( cSize - cHeightTop ) / yScale ;
        glVertex2f( x, y );
        //point #3
        x = centerX + cWidthBot / xScale; y = centerY + ( cSize - cHeightTop ) / yScale ;
        glVertex2f( x, y );
        //point #4
        x = centerX + cWidthBot / xScale; y = centerY - cSize/yScale;
        glVertex2f( x, y );
        //point #5
        x = centerX;                      y = centerY - (cSize - cHeightBot) / yScale ;
        glVertex2f( x, y );
        //point #6
        x = centerX - cWidthBot / xScale; y = centerY - cSize/yScale;
        glVertex2f( x, y );
        //point #7
        x = centerX - cWidthBot / xScale; y = centerY + ( cSize - cHeightTop ) / yScale ;
        glVertex2f( x, y );
        //point #8
        x = centerX - cWidthTop / xScale; y = centerY + ( cSize - cHeightTop ) / yScale ;
        glVertex2f( x, y );
    glEnd();
    glLineWidth( 2.0 );
    glEnable( GL_LINE_SMOOTH );
    glBegin(GL_LINE_LOOP);
    //circle
        float aCircAngle = 0;
        for ( int i = 0; i < 20 * SEGMENTS + 1; i++ )
        {
            x = centerX + cos(aCircAngle) * cSize / xScale;
            y = centerY + sin(aCircAngle) * cSize / yScale;
            glVertex2f( x, y );
            aCircAngle += float( STEP ) / 2;
        }
    glEnd();

    GLdouble        modelMatrix[16], projMatrix[16];
    GLint           viewport[4];
    GLdouble        winxN, winyN, winz;
    GLdouble        winxE, winyE;
    GLdouble        winxS, winyS;
    GLdouble        winxW, winyW;
    GLuint          aTextList;

    GLViewer_TexFont* aFont = myCompass->getFont();
    float widN = (float)aFont->getStringWidth( "N" );
    float widW = (float)aFont->getStringWidth( "W" );
    float widS = (float)aFont->getStringWidth( "S" );
    float widE = (float)aFont->getStringWidth( "E" );
    float heightL = (float)aFont->getStringHeight();

    float xGapN = - widN/2 *( 1.0 + sin(angle) );
    float xGapS = - widS/2 *( 1.0 - sin(angle) );
    float xGapW = - widW/2 *( 1.0 + cos(angle) );
    float xGapE = - widE/2 *( 1.0 - cos(angle) );

    float yGapN = - heightL/2 *( 1.0 - cos(angle) ) * 0.75;
    float yGapS = - heightL/2 *( 1.0 + cos(angle) ) * 0.75;
    float yGapW = - heightL/2 *( 1.0 + sin(angle) ) * 0.75;
    float yGapE = - heightL/2 *( 1.0 - sin(angle) ) * 0.75;

    glGetIntegerv (GL_VIEWPORT, viewport);
    glGetDoublev (GL_MODELVIEW_MATRIX, modelMatrix);
    glGetDoublev (GL_PROJECTION_MATRIX, projMatrix);

    gluProject (centerX, centerY + cSize / yScale, 0, modelMatrix, projMatrix, viewport, &winxN, &winyN, &winz);
    gluProject (centerX + cSize / xScale, centerY, 0, modelMatrix, projMatrix, viewport, &winxE, &winyE, &winz);
    gluProject (centerX, centerY - cSize / yScale, 0, modelMatrix, projMatrix, viewport, &winxS, &winyS, &winz);
    gluProject (centerX - cSize / xScale, centerY, 0, modelMatrix, projMatrix, viewport, &winxW, &winyW, &winz);

    glColor3f( 1.0, 1.0, 1.0 );

    aTextList = glGenLists( 1 );
    glNewList( aTextList, GL_COMPILE );

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0,viewport[2],0,viewport[3],-100,100);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    aFont->drawString( "N", winxN + xGapN, winyN + yGapN );
    aFont->drawString( "E", winxE + xGapE, winyE + yGapE );
    aFont->drawString( "S", winxS + xGapS, winyS + yGapS );
    aFont->drawString( "W", winxW + xGapW, winyW + yGapW );

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glEndList();

    if ( aTextList != -1 )
        glCallList( aTextList );
}

/*!
  \return blocking status for current started operations
*/
BlockStatus GLViewer_ViewPort2d::currentBlock()
{
    if( myIsDragProcess == inDrag && myCurDragPosX != NULL && myCurDragPosY != NULL)
        return BlockStatus(BS_Highlighting | BS_Selection);

    if( mypFirstPoint && mypLastPoint )
        return BlockStatus(BS_Highlighting | BS_Selection);

    return BS_NoBlock;
}

/*!
  Initializes rectangle selection
  \param x, y - start point
*/
void GLViewer_ViewPort2d::startSelectByRect( int x, int y )
{
    if( !mypFirstPoint && !mypLastPoint )
    {
        mypFirstPoint = new QPoint( x, y );
        mypLastPoint = new QPoint( x, y );
    }

    if ( !myRectBand ) {
      myRectBand = new QRubberBand( QRubberBand::Rectangle, this );
      QPalette palette;
      palette.setColor(myRectBand->foregroundRole(), Qt::white);
      myRectBand->setPalette(palette);
    }
    myRectBand->hide();
}

/*!
  Draws rectangle selection
  \param x, y - current point
*/
void GLViewer_ViewPort2d::drawSelectByRect( int x, int y )
{
    if( mypFirstPoint && mypLastPoint )
    {
        myRectBand->hide();    /* erase */

        mypLastPoint->setX( x );
        mypLastPoint->setY( y );

        QRect aRect = selectionRect();
        myRectBand->setGeometry( aRect );    /* draw */
        myRectBand->setVisible( aRect.isValid() );
    }

}

/*!
  Finishes rectangle selection
*/
void GLViewer_ViewPort2d::finishSelectByRect()
{
    if( mypFirstPoint && mypLastPoint )
    {


        if ( myRectBand ) myRectBand->hide();    /* erase */

        delete mypFirstPoint;
        delete mypLastPoint;

        mypFirstPoint = NULL;
        mypLastPoint = NULL;

        delete myRectBand;
        myRectBand = 0;
    }
}

/*!
  \return rectangle selection
*/
QRect GLViewer_ViewPort2d::selectionRect()
{
    QRect aRect;
    if( mypFirstPoint && mypLastPoint )
    {
        aRect.setLeft( qMin( mypFirstPoint->x(), mypLastPoint->x() ) );
        aRect.setTop( qMin( mypFirstPoint->y(), mypLastPoint->y() ) );
        aRect.setRight( qMax( mypFirstPoint->x(), mypLastPoint->x() ) );
        aRect.setBottom( qMax( mypFirstPoint->y(), mypLastPoint->y() ) );
    }

    return aRect;
}

/*!
*/
bool GLViewer_ViewPort2d::startPulling( GLViewer_Pnt point )
{
    GLViewer_Viewer2d* aViewer = (GLViewer_Viewer2d*)getViewFrame()->getViewer();
    GLViewer_Context* aContext = aViewer->getGLContext();
    ObjList anObjects = aContext->getObjects();

    for( ObjList::Iterator it = anObjects.begin(); it != anObjects.end(); ++it )
    {
        GLViewer_Object* anObject = *it;
        GLViewer_Rect aRect = anObject->getPullingRect();

        if( aRect.contains( point ) && anObject->startPulling( point ) )
        {
            myIsPulling = true;
            myPullingObject = anObject;
            setCursor( *getHandCursor() );
            return true;
        }
    }

    return false;
}

/*!
*/
void GLViewer_ViewPort2d::drawPulling( GLViewer_Pnt point )
{
    GLViewer_Viewer2d* aViewer = (GLViewer_Viewer2d*)getViewFrame()->getViewer();
    GLViewer_Context* aContext = aViewer->getGLContext();
    ObjList anObjects = aContext->getObjects();

    GLViewer_Object* aLockedObject = 0;
    for( ObjList::Iterator it = anObjects.begin(); it != anObjects.end(); ++it )
    {
        GLViewer_Object* anObject = *it;
        if( !anObject->getVisible() )
            continue;

        GLViewer_Rect aRect = anObject->getPullingRect();

        if( aRect.contains( point ) && anObject->portContains( point ) )
        {
            aLockedObject = anObject;
            break;
        }
    }

    myPullingObject->pull( point, aLockedObject );
}

/*!
*/
void GLViewer_ViewPort2d::finishPulling()
{
    myIsPulling = false;
    myPullingObject->finishPulling();
    setCursor( *getDefaultCursor() );
}

/*!
  Convert rectangle in window co-ordinates to GL co-ordinates
  \return converted rectangle
*/
GLViewer_Rect GLViewer_ViewPort2d::win2GLV( const QRect& theRect ) const
{
  GLViewer_Rect aRect;

  GLdouble        modelMatrix[16], projMatrix[16];
  GLint           viewport[4];

  GLdouble        objx1, objy1;
  GLdouble        objx2, objy2;
  GLdouble        objz;

  glGetIntegerv (GL_VIEWPORT, viewport);
  glGetDoublev (GL_MODELVIEW_MATRIX, modelMatrix);
  glGetDoublev (GL_PROJECTION_MATRIX, projMatrix);

  gluUnProject( theRect.left(), viewport[3] - theRect.top(), 0, modelMatrix, projMatrix, viewport, &objx1, &objy1, &objz );
  gluUnProject( theRect.right(), viewport[3] - theRect.bottom(), 0, modelMatrix, projMatrix, viewport, &objx2, &objy2, &objz );

  aRect.setLeft( objx1 );
  aRect.setTop( objy1 );
  aRect.setRight( objx2 );
  aRect.setBottom( objy2 );

  return aRect;
}

/*!
  Convert rectangle in GL co-ordinates to window co-ordinates
  \return converted rectangle
*/
QRect GLViewer_ViewPort2d::GLV2win( const GLViewer_Rect& theRect ) const
{
  QRect aRect;

  GLdouble        modelMatrix[16], projMatrix[16];
  GLint           viewport[4];

  GLdouble        winx1, winy1;
  GLdouble        winx2, winy2;
  GLdouble        winz;

  glGetIntegerv (GL_VIEWPORT, viewport);
  glGetDoublev (GL_MODELVIEW_MATRIX, modelMatrix);
  glGetDoublev (GL_PROJECTION_MATRIX, projMatrix);

  gluProject( theRect.left(), theRect.top(), 0, modelMatrix, projMatrix, viewport, &winx1, &winy1, &winz );
  gluProject( theRect.right(), theRect.bottom(), 0, modelMatrix, projMatrix, viewport, &winx2, &winy2, &winz );

  aRect.setLeft( (int)winx1 );
  aRect.setTop( viewport[3] - (int)winy1 );
  aRect.setRight( (int)winx2 );
  aRect.setBottom( viewport[3] - (int)winy2 );

  return aRect;
}

/*!
  SLOT: called when tooltip should be shown
*/
void GLViewer_ViewPort2d::onMaybeTip( QPoint thePoint, QString& theText, QFont& theFont, QRect& theTextReg, QRect& theRegion )
{
  GLViewer_Context* aContext = ((GLViewer_Viewer2d*)getViewFrame()->getViewer())->getGLContext();

  GLViewer_Object* anObj = aContext->getCurrentObject();
  if( anObj )
  {
    theText = anObj->getToolTipText();
    if( theText.isEmpty() )
      theText = anObj->getName();

    QStringList aList;
    if( anObj->isTooTipHTML() )
      aList = theText.split( "<br>", QString::SkipEmptyParts );
    else
      aList = theText.split( "\n", QString::SkipEmptyParts );

    if( !aList.isEmpty() )
    {
      int str_size = aList.first().length();
      for( int i = 1, size = aList.count(); i < size; i++ )
      {
        if ( str_size < (int)aList[i].length() )
        {
          str_size = aList[i].length();
        }
      }
      theFont = font();
      int cur_height = 24;
      QCursor* aCursor = QApplication::overrideCursor();
      if( aCursor )
      {
        const QBitmap* aBitmap = aCursor->bitmap();
        if( aBitmap )
          cur_height = aBitmap->height();
      }

      //temp
      QSize aSize = QLabel( theText, 0 ).sizeHint();
      theTextReg = QRect( thePoint.x(), thePoint.y() + cur_height,
                          aSize.width(), aSize.height() );
      theRegion = QRect( thePoint.x(), thePoint.y(), 1, 1 );
    }
  }
}
