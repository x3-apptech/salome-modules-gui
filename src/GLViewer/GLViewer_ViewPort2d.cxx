// File:      GLViewer_ViewPort2d.cxx
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

/* GLViewer_ViewPort2d Source File */

#include "GLViewer_ViewPort2d.h"
#include "GLViewer_Viewer2d.h"
#include "GLViewer_ViewFrame.h"
#include "GLViewer_Context.h"
#include "GLViewer_Object.h"
#include "QtxToolTip.h"

//#include "QAD_Desktop.h"

#include <Precision.hxx>

#include <cmath>
using namespace std;
//#include <math.h>
//#include <stdlib.h>
//#include <iostream.h>

#include <qevent.h>
#include <qrect.h>
#include <qlayout.h>
#include <qpopupmenu.h>
#include <qimage.h>
#include <qtooltip.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <qpainter.h>
#include <qbitmap.h>

/***************************************************************************
**  Class:   GLViewer_RectangularGrid
**  Descr:   OpenGL Grid for GLViewer_ViewPort2d
**  Module:  GLViewer
**  Created: UI team, 16.09.02
****************************************************************************/

#define SEGMENTS   20
#define STEP       2*PI/SEGMENTS
#define XSIZE      100
#define YSIZE      100
#define MIN_SIZE   1

GLViewer_RectangularGrid::GLViewer_RectangularGrid() :
       myGridList( -1 ), myGridHeight( (GLfloat)0.0 ), myGridWidth( (GLfloat)0.0 ),
       myWinW( (GLfloat)0.0 ), myWinH( (GLfloat)0.0 ), myXSize( (GLfloat)0.0 ), myYSize( (GLfloat)0.0 ),
       myXPan( (GLfloat)0.0 ), myYPan( (GLfloat)0.0 ), myXScale( (GLfloat)1.0 ), myYScale( (GLfloat)1.0 ),
       myLineWidth( (GLfloat)0.05 ), myCenterWidth( (GLfloat)1.5 ), myCenterRadius( (GLint)5.0 ), 
       myScaleFactor( 10 ), myIsUpdate( GL_FALSE )
{
  myGridColor[0] = 0.5;
  myGridColor[1] = 0.5;
  myGridColor[2] = 0.5;
  myAxisColor[0] = 0.75;
  myAxisColor[1] = 0.75;
  myAxisColor[2] = 0.75;
}

GLViewer_RectangularGrid::GLViewer_RectangularGrid( GLfloat width, GLfloat height,
                          GLfloat winW, GLfloat winH,
                          GLfloat xSize, GLfloat ySize,
                          GLfloat xPan, GLfloat yPan,
                          GLfloat xScale, GLfloat yScale ) :
       myGridList( -1 ), myGridHeight( (GLfloat)0.0 ), myGridWidth( (GLfloat)0.0 ),
       myWinW( (GLfloat)0.0 ), myWinH( (GLfloat)0.0 ), myXSize( (GLfloat)0.0 ), myYSize( (GLfloat)0.0 ),
       myXPan( (GLfloat)0.0 ), myYPan( (GLfloat)0.0 ), myXScale( (GLfloat)1.0 ), myYScale( (GLfloat)1.0 ),
       myLineWidth( (GLfloat)0.05 ), myCenterWidth( (GLfloat)1.5 ), myCenterRadius( (GLint)5.0 ), 
       myScaleFactor( 10 ), myIsUpdate( GL_FALSE )
{
  myGridColor[0] = 0.5;
  myGridColor[1] = 0.5;
  myGridColor[2] = 0.5;
  myAxisColor[0] = 0.75;
  myAxisColor[1] = 0.75;
  myAxisColor[2] = 0.75;
}

GLViewer_RectangularGrid::~GLViewer_RectangularGrid()
{
}

void GLViewer_RectangularGrid::draw()
{
  if ( myGridList == -1 || myIsUpdate )
    initList();

  glCallList( myGridList );
}

void GLViewer_RectangularGrid::setGridColor( GLfloat r, GLfloat g, GLfloat b )
{
  if( myGridColor[0] == r && myGridColor[1] == g && myGridColor[2] == b )
    return;

  myGridColor[0] = r;
  myGridColor[1] = g;
  myGridColor[2] = b;
  myIsUpdate = GL_TRUE;
}

void GLViewer_RectangularGrid::setAxisColor( GLfloat r, GLfloat g, GLfloat b )
{
  if( myAxisColor[0] == r && myAxisColor[1] == g && myAxisColor[2] == b )
    return;

  myAxisColor[0] = r;
  myAxisColor[1] = g;
  myAxisColor[2] = b;
  myIsUpdate = GL_TRUE;
}

void GLViewer_RectangularGrid::setGridWidth( float w )
{
  if( myGridWidth == w )
    return;

  myGridWidth = w;
  myIsUpdate = GL_TRUE;
}

void GLViewer_RectangularGrid::setCenterRadius( int r )
{
  if( myCenterRadius == r )
    return;

  myCenterRadius = r;
  myIsUpdate = GL_TRUE;
}

void GLViewer_RectangularGrid::setSize( float xSize, float ySize )
{
  if( myXSize == xSize && myYSize == ySize )
    return;
  
  myXSize = xSize;
  myYSize = ySize;
  myIsUpdate = GL_TRUE;
}

void GLViewer_RectangularGrid::setPan( float xPan, float yPan )
{
  if( myXPan == xPan && myYPan == yPan )
    return;
 
  myXPan = xPan;
  myYPan = yPan;
  myIsUpdate = GL_TRUE; 
}

bool GLViewer_RectangularGrid::setZoom( float zoom )
{
  if( zoom == 1.0 )
    return true;
  
  //backup values
  float bXScale = myXScale;
  float bYScale = myYScale;

  myXScale /= zoom; 
  myYScale /= zoom;

  if( fabs(myXScale) < Precision::Confusion() || fabs(myYScale) < Precision::Confusion() )
  { //undo
    myXScale = bXScale;
    myYScale = bYScale;
    return false;
  }
  
  myGridWidth /= zoom; 
  myGridHeight /= zoom;  
  myIsUpdate = GL_TRUE;
  return true;
}

void GLViewer_RectangularGrid::setResize( float WinW, float WinH, float zoom )
{
  if( myWinW == WinW && myWinH == WinH && zoom == 1.0 )
    return;

  myGridWidth = myGridWidth + ( WinW - myWinW ) * myXScale; 
  myGridHeight = myGridHeight + ( WinH - myWinH ) * myYScale;
  myWinW = WinW;
  myWinH = WinH;
  setZoom( zoom );
  myIsUpdate = GL_TRUE;
}

void GLViewer_RectangularGrid::getSize( float& xSize, float& ySize ) const
{
  xSize = myXSize;
  ySize = myYSize;
}

void GLViewer_RectangularGrid::getPan( float& xPan, float& yPan ) const
{
  xPan = myXPan;
  yPan = myYPan;
}

void GLViewer_RectangularGrid::getScale( float& xScale, float& yScale ) const
{
  xScale = myXScale;
  yScale = myYScale;
}

bool GLViewer_RectangularGrid::initList()
{
  myIsUpdate = GL_FALSE;
  int n, m; 
  float xLoc, yLoc; 
  int xLoc1, yLoc1; 
   
    if( myXSize == (GLfloat)0.0 )
        myXSize = (GLfloat)0.1;
    if( myYSize == (GLfloat)0.0 )
        myYSize = (GLfloat)0.1;

label:
  if( ( myXSize >= myGridWidth/5 ) && ( myYSize >= myGridHeight/5 ) )
  { //zoom in
    myXSize /= myScaleFactor;
    myYSize /= myScaleFactor;
    goto label;
  }
  else if( ( myXSize * myScaleFactor < myGridWidth/5 ) 
        || ( myYSize * myScaleFactor < myGridHeight/5 ) )
  { //zoom out
    myXSize *= myScaleFactor;
    myYSize *= myScaleFactor;
    goto label;
  }

  n = ( int )( myGridWidth / myXSize );
  m = ( int )( myGridHeight / myYSize );
        
  if( ( n != 0 ) || ( m != 0 ) ) 
  { 
    if ( myGridList != -1 )  
    { 
      glDeleteLists( myGridList, 1 ); 
      if ( glGetError() != GL_NO_ERROR ) 
    return FALSE;
    } 
         
    xLoc1 = ( int )( myXPan / myXSize ); 
    yLoc1 = ( int )( myYPan / myYSize ); 

    xLoc = xLoc1 * myXSize; 
    yLoc = yLoc1 * myYSize; 
 
    myGridList = glGenLists( 1 ); 
    glNewList( myGridList, GL_COMPILE ); 

    glColor3f( myGridColor[0], myGridColor[1], myGridColor[2] );  
    glLineWidth( myLineWidth ); 
    
    glBegin( GL_LINES ); 
    for( int j = 0; ( j-1 ) * myXSize <= myGridWidth / 2 ; j++ )
    { 
      glVertex2d( -myXSize * j - xLoc, -myGridHeight / 2 - myYSize - yLoc );
      glVertex2d( -myXSize * j - xLoc,  myGridHeight / 2 + myYSize - yLoc ); 
      glVertex2d(  myXSize * j - xLoc, -myGridHeight / 2 - myYSize - yLoc );
      glVertex2d(  myXSize * j - xLoc,  myGridHeight / 2 + myYSize - yLoc );
    }
    for( int i = 0; ( i-1 ) * myYSize <= myGridHeight / 2 ; i++)  
    {
      glVertex2d( -myGridWidth / 2 - myXSize - xLoc, -myYSize * i - yLoc ); 
      glVertex2d(  myGridWidth / 2 + myXSize - xLoc, -myYSize * i - yLoc ); 
      glVertex2d( -myGridWidth / 2 - myXSize - xLoc,  myYSize * i - yLoc ); 
      glVertex2d(  myGridWidth / 2 + myXSize - xLoc,  myYSize * i - yLoc ); 
    } 
    glEnd();

    glColor3f( myAxisColor[0], myAxisColor[1], myAxisColor[2] );
    glLineWidth( myCenterWidth );

    glBegin( GL_LINES );
    glVertex2d(  myGridWidth / 2 + myXSize - xLoc, 0); 
    glVertex2d( -myGridWidth / 2 - myXSize - xLoc, 0); 
    glVertex2d( 0,  myGridHeight / 2 + myYSize - yLoc );
    glVertex2d( 0, -myGridHeight / 2 - myYSize - yLoc );    
    glEnd();

    glBegin( GL_LINE_LOOP ); 
    double angle = 0.0;
    for ( int k = 0; k < SEGMENTS; k++ )     
    { 
      glVertex2f( cos(angle) * myCenterRadius * myXScale,
          sin(angle) * myCenterRadius * myYScale ); 
      angle += STEP; 
    } 
    glEnd();

    glEndList();
  }
  return TRUE;
}
/***************************************************************************
**  Class:   GLViewer_Compass
**  Descr:   OpenGL Compass for ViewPort 2D
**  Module:  GLViewer
**  Created: UI team, 29.03.04
****************************************************************************/
GLViewer_Compass::GLViewer_Compass ( const QColor& color, const int size, const Position pos,
                               const int WidthTop, const int WidthBottom, const int HeightTop,
                               const int HeightBottom ){
    myCol = color;
    mySize = size;
    myPos = pos;
    myArrowWidthTop = WidthTop;
    myArrowWidthBottom = WidthBottom;
    myArrowHeightTop = HeightTop;
    myArrowHeightBottom = HeightBottom;
    myIsVisible = true;
    QFont* aFont = new QFont("Times",16);
    myFont = new GLViewer_TexFont( aFont );
    isGenereted = false;
    //myFont->generateTexture();
}

GLViewer_TexFont* GLViewer_Compass::getFont()
{ 
    if(!isGenereted) 
    {
        myFont->generateTexture();
        isGenereted = true;
    }    
    return myFont;
}


/***************************************************************************
**  Class:   GLViewer_ViewPort2d
**  Descr:   OpenGL ViewPort 2D
**  Module:  GLViewer
**  Created: UI team, 02.09.02
****************************************************************************/

#define WIDTH    640
#define HEIGHT   480
#define MARGIN   10

int static aLastViewPostId = 0;

void rotate_point( float& theX, float& theY, float theAngle )
{
    float aTempX = theX * cos(theAngle) - theY * sin(theAngle);
    float aTempY = theX * sin(theAngle) + theY * cos(theAngle);
    theX = aTempX;
    theY = aTempY;
}

GLViewer_ViewPort2d::GLViewer_ViewPort2d( QWidget* parent, GLViewer_ViewFrame* theViewFrame ) :
       GLViewer_ViewPort( parent ),
       myMargin( MARGIN ), myWidth( WIDTH ), myHeight( HEIGHT ),
       myXScale( 1.0 ), myYScale( 1.0 ), myXOldScale( 1.0 ), myYOldScale( 1.0 ),
       myXPan( 0.0 ), myYPan( 0.0 )
{
    if( theViewFrame == NULL )
        myViewFrame = ( GLViewer_ViewFrame* )parent;
    else
        myViewFrame = theViewFrame;

    myBorder = new QRect(0,0,0,0);
    myGrid = NULL;
    QBoxLayout* qbl = new QHBoxLayout( this );
    myGLWidget = new GLViewer_Widget( this, 0 ) ;
    qbl->addWidget( myGLWidget );
    myGLWidget->setFocusProxy( this );
    setMouseTracking( TRUE );

    myIsDragProcess = noDrag;
    //myCurDragMousePos = QPoint();
    myCurDragPosX = NULL;
    myCurDragPosY = NULL;

    myCompass = 0;
    //myCompass = new GLViewer_Compass();
    //myCompass = new GLViewer_Compass( Qt::green, 30, GLViewer_Compass::TopRight, 10, 5, 12, 3 );

    myIsPulling = false;

    myViewPortId = aLastViewPostId;
    aLastViewPostId++;

    mypFirstPoint = NULL;
    mypLastPoint = NULL;

    myObjectTip = new QtxToolTip( myGLWidget );///GLViewer_ObjectTip( this );
    connect( myObjectTip, SIGNAL( maybeTip( QPoint, QString&, QFont&, QRect&, QRect& ) ),
             this, SLOT( onMaybeTip( QPoint, QString&, QFont&, QRect&, QRect& ) ) );
//    myGLWidget->installEventFilter( myObjectTip );
}

GLViewer_ViewPort2d::~GLViewer_ViewPort2d()
{
    if( myCompass )
        delete myCompass;

    if( myGrid )
        delete myGrid;

    delete myBorder;
    delete myGLWidget;
}

void GLViewer_ViewPort2d::onCreatePopup()
{
    //cout << "GLViewer_ViewPort2d::onCreatePopup" << endl;

    //QAD_Desktop* desktop = (QAD_Desktop*) QAD_Application::getDesktop();
    
    QString theContext;
    QString theParent("Viewer");
    QString theObject;
    
    //desktop->definePopup( theContext, theParent, theObject );
    //desktop->createPopup( myPopup, theContext, theParent, theObject);
    //desktop->customPopup( myPopup, theContext, theParent, theObject );

    /*
    if ( myPopup->count() > 0 )
      myIDs.append ( myPopup->insertSeparator() );  
    int id;
    myIDs.append ( id = myPopup->insertItem (tr ("MEN_VP3D_CHANGEBGR")) );  
    QAD_ASSERT ( myPopup->connectItem ( id, this, SLOT(onChangeBackgroundColor())) );
    */
    
    /*
    if ( myPopup )
    {
        GLViewer_Viewer2d* aViewer = (GLViewer_Viewer2d*)getViewFrame()->getViewer();

        QAD_ASSERT( myPopupActions.isEmpty() );

        QAction* a = new QAction( "", tr( "MEN_VP_CHANGEBGR" ), 0, this );
        a->setStatusTip( tr( "PRP_VP_CHANGEBGR" ) );
        connect( a, SIGNAL( activated() ), SLOT( onChangeBgColor() ) );
        myPopupActions.append( a );
        a->addTo( myPopup );        

        myPopup->insertSeparator();

        a = new QAction( "", tr( "MEN_VP_CREATE_GLMARKERS" ), 0, this );
        a->setStatusTip( tr( "PRP_VP_CREATE_GLMARKERS" ) );
        connect( a, SIGNAL( activated() ), aViewer, SLOT( onCreateGLMarkers() ) );
        myPopupActions.append( a );
        a->addTo( myPopup );        

        a = new QAction( "", tr( "MEN_VP_CREATE_GLPOLYLINE" ), 0, this );
        a->setStatusTip( tr( "PRP_VP_CREATE_GLPOLYLINE" ) );
        connect( a, SIGNAL( activated() ), aViewer, SLOT( onCreateGLPolyline() ) );
        myPopupActions.append( a );
        a->addTo( myPopup );        

        a = new QAction( "", tr( "MEN_VP_CREATE_GLTEXT" ), 0, this );
        a->setStatusTip( tr( "PRP_VP_CREATE_GLTEXT" ) );
        connect( a, SIGNAL( activated() ), aViewer, SLOT( onCreateGLText() ) );
        myPopupActions.append( a );
        a->addTo( myPopup );        

        myPopup->insertSeparator();

        //GLViewer_Object* aMovingObject = ((GLViewer_Viewer2d*)getViewFrame()->getViewer())->getGLContext()->getCurrentObject();
        int aSelObjects = aViewer->getGLContext()->NbSelected();
        if( aSelObjects > 0 )
        {
            a = new QAction( "", tr( "MEN_VP_CUTOBJ" ), 0, this );
            a->setStatusTip( tr( "PRP_VP_CUTOBJ" ) );
            connect( a, SIGNAL( activated() ), SLOT( onCutObject() ) );
            myPopupActions.append( a );
            a->addTo( myPopup );

            a = new QAction( "", tr( "MEN_VP_COPYOBJ" ), 0, this );
            a->setStatusTip( tr( "PRP_VP_COPYOBJ" ) );
            connect( a, SIGNAL( activated() ), SLOT( onCopyObject() ) );
            myPopupActions.append( a );
            a->addTo( myPopup );
        }

        a = new QAction( "", tr( "MEN_VP_PASTEOBJ" ), 0, this );
        a->setStatusTip( tr( "PRP_VP_PASTEOBJ" ) );
        connect( a, SIGNAL( activated() ), SLOT( onPasteObject() ) );
        myPopupActions.append( a );
        a->addTo( myPopup );

        QClipboard *aClipboard = QApplication::clipboard();
        QMimeSource* aMimeSource = aClipboard->data();
        if( !aMimeSource->provides( "GLViewer_Objects" ) )
            a->setEnabled( false );
        
    
        if( aSelObjects > 0 )
        {
            myPopup->insertSeparator();
            a = new QAction( "", tr( "MEN_VP_DRAGOBJ" ), 0, this );
            a->setStatusTip( tr( "PRP_VP_DRAGOBJ" ) );
            connect( a, SIGNAL( activated() ), SLOT( onStartDragObject() ) );
            myPopupActions.append( a );
            a->addTo( myPopup );
            myCurDragPosX = new float((float)QCursor::pos().x());
            myCurDragPosY = new float((float)QCursor::pos().y());
            //myCurDragMousePos = QCursor::pos();            
        }
    }
    */
}

void GLViewer_ViewPort2d::onCreatePopup( QPopupMenu* popup )
{
/*
    if ( popup )
    {
        GLViewer_Viewer2d* aViewer = (GLViewer_Viewer2d*)getViewFrame()->getViewer();
        if( !aViewer->isSketchingActive() )
        {
            if( !myPopupActions.isEmpty() )
                return;
            QAction* a = new QAction( "", tr( "MEN_VP_CHANGEBGR" ), 0, this );
            a->setStatusTip( tr( "PRP_VP_CHANGEBGR" ) );
            connect( a, SIGNAL( activated() ), SLOT( onChangeBgColor() ) );
            myPopupActions.append( a );
            a->addTo( popup );        

            popup->insertSeparator();

            //GLViewer_Object* aMovingObject = ((GLViewer_Viewer2d*)getViewFrame()->getViewer())->getGLContext()->getCurrentObject();
            int aSelObjects = aViewer->getGLContext()->NbSelected();
            if( aSelObjects > 0 )
            {
                a = new QAction( "", tr( "MEN_VP_CUTOBJ" ), 0, this );
                a->setStatusTip( tr( "PRP_VP_CUTOBJ" ) );
                connect( a, SIGNAL( activated() ), SLOT( onCutObject() ) );
                myPopupActions.append( a );
                a->addTo( popup );

                a = new QAction( "", tr( "MEN_VP_COPYOBJ" ), 0, this );
                a->setStatusTip( tr( "PRP_VP_COPYOBJ" ) );
                connect( a, SIGNAL( activated() ), SLOT( onCopyObject() ) );
                myPopupActions.append( a );
                a->addTo( popup );
            }

            a = new QAction( "", tr( "MEN_VP_PASTEOBJ" ), 0, this );
            a->setStatusTip( tr( "PRP_VP_PASTEOBJ" ) );
            connect( a, SIGNAL( activated() ), SLOT( onPasteObject() ) );
            myPopupActions.append( a );
            a->addTo( popup );

            QClipboard *aClipboard = QApplication::clipboard();
            QMimeSource* aMimeSource = aClipboard->data();
            if( !aMimeSource->provides( "GLViewer_Objects" ) )
                a->setEnabled( false );
            
        
            if( aSelObjects > 0 )
            {
                popup->insertSeparator();
                a = new QAction( "", tr( "MEN_VP_DRAGOBJ" ), 0, this );
                a->setStatusTip( tr( "PRP_VP_DRAGOBJ" ) );
                connect( a, SIGNAL( activated() ), SLOT( onStartDragObject() ) );
                myPopupActions.append( a );
                a->addTo( popup );
                myCurDragPosX = new float((float)QCursor::pos().x());
                myCurDragPosY = new float((float)QCursor::pos().y());
                //myCurDragMousePos = QCursor::pos();            
            }
        }
        else
        {//sketching mode
            QAD_ASSERT( myPopupActions.isEmpty() );
            QAction* a = new QAction( "", tr( "MEN_VP_SKETCH_DEL_OBJECT" ), 0, this );
            a->setStatusTip( tr( "PRP_VP_SKETCH_DEL_OBJECT" ) );
            connect( a, SIGNAL( activated() ), aViewer, SLOT( onSketchDelObject() ) );
            myPopupActions.append( a );
            a->addTo( popup );        

            popup->insertSeparator();

            a = new QAction( "", tr( "MEN_VP_SKETCH_UNDO_LAST" ), 0, this );
            a->setStatusTip( tr( "PRP_VP_SKETCH_UNDO_LAST" ) );
            connect( a, SIGNAL( activated() ), aViewer, SLOT( onSketchUndoLast() ) );
            myPopupActions.append( a );
            a->addTo( popup );

            int aSkType = aViewer->getSketchingType();
            if( ( aSkType == GLViewer_Viewer2d::Polyline ) || 
                ( aSkType == GLViewer_Viewer2d::Curve) ||
                ( aSkType == GLViewer_Viewer2d::Scribble ) )
            {
                a = new QAction( "", tr( "MEN_VP_SKETCH_FINISH" ), 0, this );
                a->setStatusTip( tr( "PRP_VP_SKETCH_FINISH" ) );
                connect( a, SIGNAL( activated() ), aViewer, SLOT( onSketchFinish() ) );
                myPopupActions.append( a );
                a->addTo( popup );
            }
        }
    }
*/
}
    
void GLViewer_ViewPort2d::onDestroyPopup( QPopupMenu* popup )
{
/*
    if ( popup )
    {
        for ( QAction* a = myPopupActions.first(); a; a = myPopupActions.next() )
            a->removeFrom( popup );
        myPopupActions.clear();
        popup->clear();
    }
*/
}


void GLViewer_ViewPort2d::onStartDragObject( )
{
    if( myIsDragProcess == noDrag )
    {
        myIsDragProcess = initDrag;
        QCursor::setPos( ( int )( *myCurDragPosX ), ( int )( *myCurDragPosY ) );
        //myCurDragMousePos = QPoint( 0, 0 );
        delete myCurDragPosX;
        delete myCurDragPosY;
        myCurDragPosX = NULL;
        myCurDragPosY = NULL;
        return;
    } 
}

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
        QValueList<GLViewer_Object*> aObjects;
        GLViewer_MimeSource* aMimeSource = new GLViewer_MimeSource();
        aContext->InitSelected();
        for( ; aContext->MoreSelected(); aContext->NextSelected() )
            aObjects.append( aContext->SelectedObject() );

        //aMimeSource->setObjects( aObjects ); ouv 6.05.04

        QClipboard *aClipboard = QApplication::clipboard();
        aClipboard->clear();
        aClipboard->setData( aMimeSource );

        for( int i = 0; i < aObjNum; i++ )
            aContext->deleteObject( aObjects[i] );
    }
}

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
        QValueList<GLViewer_Object*> aObjects;
        GLViewer_MimeSource* aMimeSource = new GLViewer_MimeSource();
        aContext->InitSelected();
        for( ; aContext->MoreSelected(); aContext->NextSelected() )
            aObjects.append( aContext->SelectedObject() );

        //aMimeSource->setObjects( aObjects ); ouv 6.05.04

        QClipboard *aClipboard = QApplication::clipboard();
        aClipboard->clear();
        aClipboard->setData( aMimeSource );
    }
}

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
    //QClipboard *aClipboard = QApplication::clipboard();
    //QMimeSource* aMimeSource = aClipboard->data();

    /* ouv 6.05.04
    if( aMimeSource->provides( "GLViewer_Objects" ) )
    {
        QByteArray anArray = aMimeSource->encodedData( "GLViewer_Objects" );
        QValueList<GLViewer_Object*> aObjects = GLViewer_MimeSource::getObjects( anArray, "GLViewer_Objects" );
        if( aObjects.empty() )
            return;
        GLViewer_Context* aContext = ((GLViewer_Viewer2d*)getViewFrame()->getViewer())->getGLContext();
        for( int i = 0; i < aObjects.count(); i++ )
            aContext->insertObject( aObjects[i], true );
    }
    */
}

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
  
  if( anObject && (e->state() & LeftButton ) )
  {
    if( aContext->isSelected( anObject ) )
    {
      for( aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected() )
      {
	GLViewer_Object* aMovingObject = aContext->SelectedObject();
	//= aViewer->getGLContext()->getCurrentObject();
	if( aMovingObject )
	{
	  aMovingObject->moveObject( aX - *myCurDragPosX ,
				     anY - *myCurDragPosY);
	  //QRect* rect = aMovingObject->getRect()->toQRect();
	  //aViewer->updateBorders( *rect );
	  aViewer->updateBorders();
	}
      }
    }
    else
      anObject->moveObject( aX - *myCurDragPosX, anY - *myCurDragPosY);
  }
  else if( aContext->NbSelected() && (e->state() & MidButton ) )
    for( aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected() )
      (aContext->SelectedObject())->moveObject( aX - *myCurDragPosX, anY - *myCurDragPosY);
  
  aViewer->updateBorders();

  delete myCurDragPosX;
  delete myCurDragPosY;
  myCurDragPosX = new float(aX);
  myCurDragPosY = new float(anY);    

  myGLWidget->updateGL();
}

/*!
    Emits 'mouseEvent' signal. [ virtual protected ]
*/
void GLViewer_ViewPort2d::mousePressEvent( QMouseEvent *e )
{
    
    //if(  )
    /*GLViewer_Object* aMovingObject = ((GLViewer_Viewer2d*)getViewFrame()->getViewer())->getGLContext()->getCurrentObject();
    if( aMovingObject )
    {
        onStartDragObject();
    }*/
    emit vpMouseEvent( e );
    
    GLViewer_Viewer2d* aViewer = (GLViewer_Viewer2d*)getViewFrame()->getViewer();   
    GLViewer_Context* aContext = aViewer->getGLContext();

    GLViewer_Object* anObject = NULL;
    if( aContext )
      anObject = aContext->getCurrentObject();
    
    bool accel = e->state() & GLViewer_ViewTransformer::accelKey();
    if( ( anObject && !( accel || e->button() == Qt::RightButton ) )
        ||
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
    //GLViewer_Context* context = ((GLViewer_Viewer2d*)getViewFrame()->getViewer())->getGLContext();
    //if( context->getCurrentObject() ) cout << "LASTPICKED" << endl;
    emit vpMouseEvent( e );
    
    GLViewer_Viewer2d* aViewer = (GLViewer_Viewer2d*)getViewFrame()->getViewer();   
    GLViewer_Context* aContext = aViewer->getGLContext();

    if( myIsDragProcess == inDrag )
        onDragObject( e );
    
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
    }
}

/*!
    Emits 'mouseEvent' signal. [ virtual protected ]
*/
void GLViewer_ViewPort2d::mouseReleaseEvent( QMouseEvent *e )
{
  /*if( myIsDragProcess == inDrag )
    {
        GLViewer_Viewer2d* aViewer = (GLViewer_Viewer2d*)getViewFrame()->getViewer();
        GLViewer_Context* aContext = aViewer->getGLContext();
        for( aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected() )
        {
            GLViewer_Object* aMovingObject = aContext->SelectedObject();
            if( aMovingObject )
                aMovingObject->finishMove();
        }

        myIsDragProcess = noDrag;
        //yCurDragMousePos.setX( 0 );
        //myCurDragMousePos.setY( 0 );
        delete myCurDragPosX;
        delete myCurDragPosY;
        myCurDragPosX = NULL;
        myCurDragPosY = NULL;
	}*/
    

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
      for( aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected() )
      {
        GLViewer_Object* aMovingObject = aContext->SelectedObject();
        if( aMovingObject )
        {
          aMovingObject->finishMove();
          isAnyMoved = true;
        }
      }
      
      GLViewer_Object* anObject = aContext->getCurrentObject();
      if( anObject )
        anObject->finishMove();
      
      myIsDragProcess = noDrag;
      //yCurDragMousePos.setX( 0 );
      //myCurDragMousePos.setY( 0 );
      delete myCurDragPosX;
      delete myCurDragPosY;
      myCurDragPosX = NULL;
      myCurDragPosY = NULL;
      emit objectMoved();
    }
}

void GLViewer_ViewPort2d::turnCompass( GLboolean on )
{
    if( on )
        myCompass = new GLViewer_Compass( Qt::green, 30, GLViewer_Compass::TopRight, 10, 5, 12, 3 );
    else if( myCompass )
        delete myCompass;
}

void GLViewer_ViewPort2d::turnGrid( GLboolean on )
{
    if( on )
      myGrid = new GLViewer_RectangularGrid( 2*WIDTH, 2*HEIGHT,
					     2*WIDTH, 2*HEIGHT, 
					     XSIZE, YSIZE,
					     myXPan, myYPan,
					     myXScale, myYScale );
    else if( myGrid )
      delete myGrid;
}

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

void GLViewer_ViewPort2d::setBackgroundColor( const QColor& color )
{
    GLViewer_ViewPort::setBackgroundColor( color );
    myGLWidget->makeCurrent();
    glClearColor( ( GLfloat )color.red() / 255,
            ( GLfloat )color.green() / 255,
            ( GLfloat )color.blue() / 255, 1.0 );
    myGLWidget->repaint();
}

QColor GLViewer_ViewPort2d::backgroundColor() const
{
    return GLViewer_ViewPort::backgroundColor();
}

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
    bool max = FALSE;

    xzoom = (GLfloat)x / myWidth; 
    yzoom = (GLfloat)y / myHeight; 

    if ( ( xzoom < yzoom ) && ( xzoom < 1 ) ) 
        zoom = xzoom; 
    else if ( ( yzoom < xzoom ) && ( yzoom < 1 ) ) 
        zoom = yzoom; 
    else 
    { 
        max = TRUE; 
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

void GLViewer_ViewPort2d::paintEvent( QPaintEvent* e )
{
    //cout << "GLViewer_ViewPort2d::paintEvent" << endl;
    myGLWidget->updateGL();
    GLViewer_ViewPort::paintEvent( e );
}

void GLViewer_ViewPort2d::resizeEvent( QResizeEvent* e )
{
    //cout << "GLViewer_ViewPort2d::resizeEvent" << endl;
    GLViewer_ViewPort::resizeEvent( e );
}

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

void GLViewer_ViewPort2d::fitRect( const QRect& rect )
{
    //cout << "GLViewer_ViewPort2d::fitRect" << endl;

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

    //myXPan += ( vpWidth / 2. - centerX ) / myXScale;
    //myYPan -= ( vpHeight / 2. - centerY ) / myYScale;

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

void GLViewer_ViewPort2d::fitAll( bool keepScale, bool withZ )
{
    //cout << "GLViewer_ViewPort2d::fitAll" << endl;

    float xa, xb, ya, yb;
    float dx, dy, zm;
    float xScale, yScale;

    myMargin = QMAX( myBorder->width(), myBorder->height() ) / 10;

    xa = myBorder->left() - myMargin;
    xb = myBorder->right() + myMargin;
    ya = myBorder->top() - myMargin;
    yb = myBorder->bottom() + myMargin;

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
    zm = vpWidth / dx < vpHeight / dy ? vpWidth / dx : vpHeight / dy; 
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

void GLViewer_ViewPort2d::startRotation( int x, int y )
{
    //cout << "GLViewer_ViewPort2d::startRotation" << endl;

    myGLWidget->setRotationStart( x, y, 1.0 );
}

void GLViewer_ViewPort2d::rotate( int x, int y )
{
    //cout << "GLViewer_ViewPort2d::rotate " << x << " " << y << endl;

    GLint val[4];
    GLint vpWidth, vpHeight;

    myGLWidget->makeCurrent();
    glGetIntegerv( GL_VIEWPORT, val );
    vpWidth = val[2];
    vpHeight = val[3];

    float x0 = vpWidth/2;
    float y0 = vpHeight/2;

    float xs, ys, zs, dx, dy;
    myGLWidget->getRotationStart( xs, ys, zs );

    xs = xs - x0;
    x = ( int )( x - x0 );
    dx = x - xs;
    ys = y0 - ys;
    y = ( int )( y0 - y );
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

    //GLfloat anAngle = angle * PI / 180.;

    //if( myGrid )
    //    myGrid->setPan( myXPan*cos(anAngle) + myYPan*sin(anAngle),
    //                    -myXPan*sin(anAngle) + myYPan*cos(anAngle) );

    //cout << l1 << endl;
    //cout << l2 << endl;
    //cout << l << endl;

    //cout << xs << " " << ys << " " << x << " " << y << endl;
    //cout << "MULTIPLICATION : " << mult << endl;

    //cout << "ANGLE_PREV = " << anglePrev << endl;
    //cout << "ANGLE_NEW = " << angleNew << endl;
    //cout << "ANGLE = " << angle << endl;

    float ra, rx, ry, rz;
    myGLWidget->getRotation( ra, rx, ry, rz );
    myGLWidget->setRotation( angle, rx, ry, rz );
    myGLWidget->updateGL();
}

void GLViewer_ViewPort2d::endRotation()
{
    //cout << "GLViewer_ViewPort2d::endRotation" << endl;

    float ra, rx, ry, rz;
    myGLWidget->getRotation( ra, rx, ry, rz );
    myGLWidget->setRotationAngle( ra );
}

void GLViewer_ViewPort2d::drawCompass(){
    if( !myCompass->getVisible() ) return;

    GLfloat xScale, yScale, xPan, yPan;

    int xPos = getWidth();
    int yPos = getHeight();

    int cPos = myCompass->getPos();
    int cSize = myCompass->getSize();
    QColor* cCol = &(myCompass->getColor());
    int cWidthTop = myCompass->getArrowWidthTop();
    int cWidthBot = myCompass->getArrowWidthBottom();
    int cHeightTop = myCompass->getArrowHeightTop();
    int cHeightBot = myCompass->getArrowHeightBottom();

    GLfloat colorR = (cCol->red())/255;
    GLfloat colorG = (cCol->green())/255;
    GLfloat colorB = (cCol->blue())/255;

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

BlockStatus GLViewer_ViewPort2d::currentBlock()
{
    if( myIsDragProcess == inDrag && myCurDragPosX != NULL && myCurDragPosY != NULL)
        return BlockStatus(BS_Highlighting | BS_Selection);
    
    if( mypFirstPoint && mypLastPoint )
        return BlockStatus(BS_Highlighting | BS_Selection);
    
    return BS_NoBlock;
}

void GLViewer_ViewPort2d::startSelectByRect( int x, int y )
{
    if( !mypFirstPoint && !mypLastPoint )
    {
        mypFirstPoint = new QPoint( x, y );
        mypLastPoint = new QPoint( x, y );
    }
}
void GLViewer_ViewPort2d::drawSelectByRect( int x, int y )
{
    if( mypFirstPoint && mypLastPoint )
    {

        QPainter p( getPaintDevice() );
        p.setPen( Qt::white );
        p.setRasterOp( Qt::XorROP );

        p.drawRect( selectionRect() );    /* erase */

        mypLastPoint->setX( x );
        mypLastPoint->setY( y );
        
        p.drawRect( selectionRect() );    /* draw */
    }

}
void GLViewer_ViewPort2d::finishSelectByRect()
{
    if( mypFirstPoint && mypLastPoint )
    {

        QPainter p( getPaintDevice() );
        p.setPen( Qt::white );
        p.setRasterOp( Qt::XorROP );

        p.drawRect( selectionRect() );    /* erase */

        delete mypFirstPoint;
        delete mypLastPoint;

        mypFirstPoint = NULL;
        mypLastPoint = NULL;
    }
}

QRect GLViewer_ViewPort2d::selectionRect()
{
    QRect aRect;
    if( mypFirstPoint && mypLastPoint )
    {
        aRect.setLeft( QMIN( mypFirstPoint->x(), mypLastPoint->x() ) );
        aRect.setTop( QMIN( mypFirstPoint->y(), mypLastPoint->y() ) );
        aRect.setRight( QMAX( mypFirstPoint->x(), mypLastPoint->x() ) );
        aRect.setBottom( QMAX( mypFirstPoint->y(), mypLastPoint->y() ) );
    }

    return aRect;
}

bool GLViewer_ViewPort2d::startPulling( GLViewer_Pnt point )
{
    GLViewer_Viewer2d* aViewer = (GLViewer_Viewer2d*)getViewFrame()->getViewer();
    GLViewer_Context* aContext = aViewer->getGLContext();
    ObjectMap anObjects = aContext->getObjects();

    for( ObjectMap::Iterator it = anObjects.begin(); it != anObjects.end(); ++it )
    {
        GLViewer_Rect* aRect = it.key()->getRect();

        if( aRect->contains( point ) && it.key()->startPulling( point ) )
        {
            myIsPulling = true;
            myPullingObject = it.key();
            setCursor( *getHandCursor() );
            return true;
        }
    }

    return false;
}

void GLViewer_ViewPort2d::drawPulling( GLViewer_Pnt point )
{
    GLViewer_Viewer2d* aViewer = (GLViewer_Viewer2d*)getViewFrame()->getViewer();
    GLViewer_Context* aContext = aViewer->getGLContext();
    ObjectMap anObjects = aContext->getObjects();

    GLViewer_Object* aLockedObject = 0;
    for( ObjectMap::Iterator it = anObjects.begin(); it != anObjects.end(); ++it )
    {
        GLViewer_Rect* aRect = it.key()->getRect();

        if( aRect->contains( point ) && it.key()->portContains( point ) )
        {
            aLockedObject = it.key();
            break;
        }
    }

    myPullingObject->pull( point, aLockedObject );
}

void GLViewer_ViewPort2d::finishPulling()
{
    myIsPulling = false;
    myPullingObject->finishPulling();
    setCursor( *getDefaultCursor() );
}

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
  
  aRect.setLeft( (int)(winx1) );
  aRect.setTop( (int)(viewport[3] - winy1) );
  aRect.setRight( (int)(winx2) );
  aRect.setBottom( (int)(viewport[3] - winy2) );

  return aRect;
}

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
      aList = QStringList::split( "<br>", theText );
    else
      aList = QStringList::split( "\n", theText );

    if( !aList.isEmpty() )
    {
      int index = 0;
      int str_size = aList.first().length();
      for( int i = 1, size = aList.count(); i < size; i++ )
      {
        if( str_size < aList[i].length() )
        {
          index = i;
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
      QFontMetrics fm( theFont );
      //temp
      theTextReg = QRect( thePoint.x(), thePoint.y() + cur_height, fm.width( /*theText*/aList[index] ), fm.height()*aList.count() + 2 );
      theRegion = QRect( thePoint.x(), thePoint.y(), 1, 1 );
    }
  }
}
