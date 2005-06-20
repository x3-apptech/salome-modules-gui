// File:      GLViewer_Viewer2d.cxx
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

/***************************************************************************
**  Class:   GLViewer_Viewer2d
**  Descr:   OpenGL Viewer 2D
**  Module:  GLViewer
**  Created: UI team, 04.09.02
****************************************************************************/

#include "GLViewer_Viewer2d.h"

#include "GLViewer_Object.h"
#include "GLViewer_Context.h"
#include "GLViewer_Drawer.h"
#include "GLViewer_Selector2d.h"
//#include "GLViewer_Sketcher.h"
#include "GLViewer_ViewPort2d.h"

#include "SUIT_Desktop.h"
#include "SUIT_ViewWindow.h"

#include <OSD_Timer.hxx>
#include <TColStd_MapOfInteger.hxx>

#include <qpointarray.h>
#include <qcolordialog.h>
#include <qpopupmenu.h>

GLViewer_Viewer2d::GLViewer_Viewer2d( const QString& title) :
GLViewer_Viewer( title )
{
    myGLContext = new GLViewer_Context( this );
    //myGLSketcher = new GLViewer_Sketcher( this );
    mySelMode = GLViewer_Viewer::Multiple;
    createSelector();
    myDrawers.clear();
}

GLViewer_Viewer2d::~GLViewer_Viewer2d()
{    
    //myGLSketcher = 0;
    //delete myGLSketcher;
}

SUIT_ViewWindow* GLViewer_Viewer2d::createView( SUIT_Desktop* theDesktop )
{
    return new GLViewer_ViewFrame( theDesktop, this );
}

void GLViewer_Viewer2d::contextMenuPopup( QPopupMenu* thePopup )
{
  // "Change background color" menu item is available if there are no selected objects
  if ( getSelector() == 0 || getSelector()->numSelected() == 0 )
  {
    if( thePopup->count() > 0 )
        thePopup->insertSeparator();
    thePopup->insertItem( tr( "MNU_DUMP_VIEW" ),  this, SLOT( onDumpView() ) );
    thePopup->insertItem( tr( "CHANGE_BGCOLOR" ), this, SLOT( onChangeBgColor() ) );
  }
}

void GLViewer_Viewer2d::onChangeBgColor()
{
  if( !getActiveView() )
    return;
  GLViewer_ViewPort2d* vp = ( ( GLViewer_ViewPort2d* )getActiveView()->getViewPort() );

  QColor selColor = QColorDialog::getColor( vp->backgroundColor(), vp );	
  if ( selColor.isValid() ) {
    vp->setBackgroundColor( selColor );
  }
}

void GLViewer_Viewer2d::updateColors( QColor colorH, QColor colorS )
{
//  cout << "GLViewer_Viewer2d::updateColors" << endl;

/*
    for ( DrawerMap::Iterator it = myDrawers.begin(); it != myDrawers.end(); ++it )
    {
        it.key()->setHColor( colorH );
        it.key()->setSColor( colorS );
    }
*/
    ObjectMap anObjects = myGLContext->getObjects();
    ObjectMap::Iterator beginIt = anObjects.begin();
    ObjectMap::Iterator endIt = anObjects.end();
    for ( ObjectMap::Iterator it = beginIt; it != endIt; ++it )
    {
        //GLViewer_Drawer* aDrawer = it.key()->getDrawer();
        //aDrawer->setHColor( colorH );
        //aDrawer->setSColor( colorS );
    }


  activateAllDrawers( TRUE );
}

void GLViewer_Viewer2d::updateBorders( const QRect& rect )
{
  float xa = rect.left();
  float xb = rect.right();
  float ya = rect.top();
  float yb = rect.bottom();

  QPtrVector<SUIT_ViewWindow> views = getViewManager()->getViews();
  for ( int i = 0, n = views.count(); i < n; i++ )
  {
    QRect* border = ( ( GLViewer_ViewPort2d* )((GLViewer_ViewFrame*)views[i])->getViewPort() )->getBorder();

    if ( xa < border->left() )   border->setLeft( (int)xa );
    if ( xb > border->right() )  border->setRight( (int)xb );
    if ( ya < border->top() )    border->setTop( (int)ya );
    if ( yb > border->bottom() ) border->setBottom( (int)yb );
  }
}

void GLViewer_Viewer2d::updateBorders()
{
    QPtrVector<SUIT_ViewWindow> views = getViewManager()->getViews();

    ObjectMap anObjects = myGLContext->getObjects();
    ObjectMap::Iterator beginIt = anObjects.begin();
    ObjectMap::Iterator endIt = anObjects.end();
    for ( int i = 0, n = views.count(); i < n; i++ )
    {
        QRect* border = ( ( GLViewer_ViewPort2d* )((GLViewer_ViewFrame*)views[i])->getViewPort() )->getBorder();
        border->setRect( 0, 0, 0, 0 );
        for ( ObjectMap::Iterator it = beginIt; it != endIt; ++it )
        {
            QRect* aRect = it.key()->getRect()->toQRect();
	    if( !it.key()->getVisible() || aRect->isNull() )
                continue;

            if( border->isNull() )
                border->setRect( aRect->left(), aRect->top(), aRect->width(), aRect->height() );
            else
	    {
	      border->setLeft( QMIN( border->left(), aRect->left() ) );
	      border->setRight( QMAX( border->right(), aRect->right() ) );
	      border->setTop( QMIN( border->top(), aRect->top() ) );
	      border->setBottom( QMAX( border->bottom(), aRect->bottom() ) );
	    }
        }
        /*
        float gap = QMAX( border->width(), border->height() ) / 20;
        border->setLeft( border->left() - gap );
        border->setRight( border->right() + gap );
        border->setTop( border->top() - gap );
        border->setBottom( border->bottom() + gap );
        */
    }
}

void GLViewer_Viewer2d::updateAll()
{
  if ( !getActiveView() )
    return;

  QPtrVector<SUIT_ViewWindow> views = getViewManager()->getViews();
  for ( int i = 0, n = views.count(); i < n; i++ )
    ( ( GLViewer_ViewPort2d* )( ( GLViewer_ViewFrame* )views[i] )->getViewPort() )->getGLWidget()->updateGL();
}

void GLViewer_Viewer2d::updateDrawers( GLboolean update, GLfloat scX, GLfloat scY )
{
//  cout << "GLViewer_Viewer2d::updateDrawers" << endl;

    //myGLContext->updateScales( scX, scY );
    //myGLSketcher->drawContour();
    activateAllDrawers( update );
}

void GLViewer_Viewer2d::activateDrawers( TColStd_SequenceOfInteger& sequence, bool onlyUpdate, GLboolean swap )
{
//  cout << "GLViewer_Viewer2d::activateDrawers" << endl;
//  if( onlyUpdate )
//    cout << "Only update" << endl;
//  else
//    cout << "Not only update" << endl;

  TColStd_MapOfInteger aMap;
  for ( int i = 1, n = sequence.Length(); i <= n; i++)
    if ( !aMap.Contains( sequence( i ) ) )
      aMap.Add( sequence( i ) );

  const ObjectMap& anObjects = myGLContext->getObjects();
  const ObjList& objList = myGLContext->getObjList();
  ObjList anActiveObjects;
  for( ObjList::const_iterator it = objList.begin(); it != objList.end(); ++it )
  {
    if ( (*it)->getVisible() && aMap.Contains( anObjects[*it] ) )
      anActiveObjects.append( *it );
  }

  activateDrawers( anActiveObjects, onlyUpdate, swap );
}

void GLViewer_Viewer2d::activateDrawers( QValueList<GLViewer_Object*>& theObjects, bool onlyUpdate, GLboolean swap )
{
    //cout << "GLViewer_Viewer2d::activateDrawers " << (int)onlyUpdate << " " << (int)swap << endl;
    float xScale;
    float yScale;

    QValueList<GLViewer_Drawer*>::Iterator anIt = myDrawers.begin();
    QValueList<GLViewer_Drawer*>::Iterator endDIt = myDrawers.end();
    for( ; anIt != endDIt; anIt++ )
            (*anIt)->clear();

    QValueList<GLViewer_Drawer*> anActiveDrawers;
    QValueList<GLViewer_Object*>::Iterator endOIt = theObjects.end();

    for( QValueList<GLViewer_Object*>::Iterator oit = theObjects.begin(); oit != endOIt; ++oit )
    {
        GLViewer_Drawer* aDrawer = (*oit)->getDrawer();
        if( !aDrawer )
        {
            anIt = myDrawers.begin();            

            for( ; anIt != endDIt; anIt++ )
                if( (*anIt)->getObjectType() == (*oit)->getObjectType() )
                {
                    (*oit)->setDrawer( *anIt );
                    aDrawer = *anIt;
                    break;
                }

            if( !aDrawer ) //are not exists
            {
                myDrawers.append( (*oit)->createDrawer() );
                aDrawer = (*oit)->getDrawer();
            }
        }
        aDrawer->addObject( (*oit) );
        if( anActiveDrawers.findIndex( aDrawer ) == -1 )
            anActiveDrawers.append( aDrawer );
    } 

    QValueList<GLViewer_Drawer*>::Iterator aDIt = anActiveDrawers.begin();
    QValueList<GLViewer_Drawer*>::Iterator aDEndIt = anActiveDrawers.end();

    QPtrVector<SUIT_ViewWindow> views = getViewManager()->getViews();
    for ( int i = 0, n = views.count(); i < n; i++ )
    {
        GLViewer_ViewPort2d* vp = ( GLViewer_ViewPort2d* )((GLViewer_ViewFrame*)views[i])->getViewPort();
        vp->getScale( xScale, yScale );
        vp->getGLWidget()->makeCurrent();


        for( ; aDIt != aDEndIt; aDIt++ )
            (*aDIt)->create( xScale, yScale, onlyUpdate );
  
        // tmp
        /*
        QRect* border = ( ( GLViewer_ViewPort2d* )((GLViewer_ViewFrame*)views[i])->getViewPort() )->getBorder();
        float x1 = border->left();
        float x2 = border->right();
        float y1 = border->bottom();
        float y2 = border->top();

        QColor color = Qt::blue;
        glColor3f( ( GLfloat )color.red() / 255,
        ( GLfloat )color.green() / 255,
        ( GLfloat )color.blue() / 255 );
        glLineWidth( 1.0 );

        glBegin( GL_LINE_LOOP );
        glVertex2f( x1, y1 );
        glVertex2f( x1, y2 );
        glVertex2f( x2, y2 );
        glVertex2f( x2, y1 );
        glEnd();

        QString coords = QString::number( border->left() ) + " " + QString::number( border->right() ) + " " +
                         QString::number( border->top() ) + " " + QString::number( border->bottom() );
        (*aDIt)->drawText( "Border : " + coords, x1, y1+10/yScale, Qt::blue, &QFont( "Courier", 8, QFont::Normal ), 2 );
        */
        if ( swap )
           vp->getGLWidget()->swapBuffers();
    }

    ( ( GLViewer_ViewPort2d* )getActiveView()->getViewPort() )->getGLWidget()->makeCurrent();
}

void GLViewer_Viewer2d::activateDrawer( int index, bool onlyUpdate, GLboolean swap )
{
  GLViewer_Object* anObj = 0;
  const ObjectMap& anObjects = myGLContext->getObjects();
  for ( ObjectMap::const_iterator it = anObjects.begin(); it != anObjects.end(); ++it )
    if ( it.data() == index )
    {
      anObj = (GLViewer_Object*)it.key();
      break;
    }
    
  if ( anObj )
    activateDrawer( anObj, onlyUpdate, swap );
}

void GLViewer_Viewer2d::activateDrawer( GLViewer_Object* theObject, bool onlyUpdate, GLboolean swap )
{
  ObjList aList;
  aList.append( theObject );
  activateDrawers( aList, onlyUpdate, swap );
}

void GLViewer_Viewer2d::activateAllDrawers( bool onlyUpdate, GLboolean swap )
{
    if ( !getActiveView() )
      return;

    ObjList anActiveObjs;
    const ObjList& objs = myGLContext->getObjList();
    for( ObjList::const_iterator it = objs.begin(); it != objs.end(); ++it )
    {
      GLViewer_Object* obj = (GLViewer_Object*)(*it);
      if( obj->getVisible() )
          anActiveObjs.append( obj );
    }

    activateDrawers( anActiveObjs, onlyUpdate, swap );
}

void GLViewer_Viewer2d::onCreateGLMarkers( int theMarkersNum, int theMarkersRad )
{
    if ( !getActiveView() )
      return;

    GLViewer_MarkerSet* aMarkerSet = new GLViewer_MarkerSet( theMarkersNum, theMarkersRad );
    getGLContext()->insertObject( aMarkerSet );

    GLViewer_ViewPort2d* vp = ( GLViewer_ViewPort2d* )getActiveView()->getViewPort();
    int vpWidth = vp->getWidth();
    int vpHeight = vp->getHeight();

    float* aXCoord = new float[ theMarkersNum ];
    float* anYCoord = new float[ theMarkersNum ];

    srand( 1 );
    for ( long i = 0; i < theMarkersNum; i++ )  
    {
        aXCoord[i] = cos( PI * (rand() / (GLfloat)RAND_MAX) ) * ((GLfloat)vpWidth / 2.);
        anYCoord[i] = cos( PI * (rand() / (GLfloat)RAND_MAX) ) * ((GLfloat)vpHeight / 2.);
    }

    aMarkerSet->setXCoord( aXCoord, theMarkersNum );
    aMarkerSet->setYCoord( anYCoord, theMarkersNum );
    aMarkerSet->compute();

    QRect* rect = aMarkerSet->getRect()->toQRect();
    updateBorders( *rect );
    
    activateAllDrawers( false );
    activateTransform( GLViewer_Viewer::FitAll );

    delete[] aXCoord;
    delete[] anYCoord;
}

void GLViewer_Viewer2d::onCreateGLPolyline( int theAnglesNum, int theRadius, int thePolylineNumber )
{
    if ( !getActiveView() )
      return;

    GLViewer_ViewPort2d* vp = ( GLViewer_ViewPort2d* )getActiveView()->getViewPort();
    int vpWidth = vp->getWidth();
    int vpHeight = vp->getHeight();

    float* aXCoord = new float[ theAnglesNum ];
    float* anYCoord = new float[ theAnglesNum ];

    //srand( ( unsigned )time( NULL ) );
    srand( 1 );
    for( int j = 0; j < thePolylineNumber; j++)
    {
        GLViewer_Polyline* aPolyline = new GLViewer_Polyline( theAnglesNum, theRadius );
        getGLContext()->insertObject( aPolyline );

        float aXOffset = cos( PI * (rand() / (GLfloat)RAND_MAX) ) * ((GLfloat)vpWidth / 2.);
        float anYOffset = cos( PI * (rand() / (GLfloat)RAND_MAX) ) * ((GLfloat)vpHeight / 2.);
        for( int i = 0; i < theAnglesNum; i++ )  
        {
            aXCoord[i] = cos( 2. * PI * i / theAnglesNum ) * theRadius + aXOffset;
            anYCoord[i] = sin( 2. * PI * i / theAnglesNum ) * theRadius + anYOffset;
        }

        aPolyline->setHighSelAll( true );
        aPolyline->setClosed( true );
        aPolyline->setXCoord( aXCoord, theAnglesNum );
        aPolyline->setYCoord( anYCoord, theAnglesNum );
        aPolyline->compute();

        QRect* rect = aPolyline->getRect()->toQRect();
        updateBorders( *rect );
    }
    
    activateAllDrawers( false );
    activateTransform( GLViewer_Viewer::FitAll );

    delete[] aXCoord;
    delete[] anYCoord;
}

void GLViewer_Viewer2d::onCreateGLText( QString theStr, int theTextNumber )
{
    if ( !getActiveView() )
      return;

    if( theTextNumber <= 0 )
        return;
    
    GLViewer_ViewPort2d* vp = ( GLViewer_ViewPort2d* )getActiveView()->getViewPort();
    int vpWidth = vp->getWidth();
    int vpHeight = vp->getHeight();

    //srand( ( unsigned )time( NULL ) );
    srand( 1 );
    for( int j = 0; j < theTextNumber; j++)
    {
        float aXPos = cos( PI * (rand() / (GLfloat)RAND_MAX) ) * ((GLfloat)vpWidth / 2.);
        float anYPos = cos( PI * (rand() / (GLfloat)RAND_MAX) ) * ((GLfloat)vpHeight / 2.);
        QColor aColor( 255, 0, 255 );

        GLViewer_TextObject* aText = new GLViewer_TextObject( theStr, aXPos, anYPos, aColor  );
        aText->compute();
        getGLContext()->insertObject( aText );

        QRect* rect = aText->getRect()->toQRect();
        updateBorders( *rect );
    }

    activateAllDrawers( false );
}

void GLViewer_Viewer2d::transPoint( GLfloat& x, GLfloat& y )
{
    if ( !getActiveView() )
      return;

    GLfloat xScale, yScale;
    GLfloat xPan, yPan;

    GLViewer_ViewPort2d* curvp = ( GLViewer_ViewPort2d* )getActiveView()->getViewPort();

    curvp->getScale( xScale, yScale );
    curvp->getPan( xPan, yPan );

    GLfloat a = curvp->getGLWidget()->getRotationAngle() * PI / 180.;
    
    x = (  x - ( GLfloat )curvp->getWidth()  / 2 ) / xScale;
    y = ( -y + ( GLfloat )curvp->getHeight() / 2 ) / yScale;

    GLfloat x1 = x;
    GLfloat y1 = y;

    x = x1 * cos(a) + y1 * sin(a);
    y = -x1 * sin(a) + y1 * cos(a);

    x -= xPan;
    y -= yPan;
}

QRect* GLViewer_Viewer2d::getWinObjectRect( GLViewer_Object* theObject )
{
    if ( !getActiveView() )
      return 0;

    GLfloat xScale, yScale;
    GLfloat xPan, yPan;

    GLViewer_ViewPort2d* curvp = ( GLViewer_ViewPort2d* )getActiveView()->getViewPort();
    GLfloat aWidth = curvp->getWidth();
    GLfloat aHeight = curvp->getHeight();


    curvp->getScale( xScale, yScale );
    curvp->getPan( xPan, yPan );

    QRect* aObjRect = theObject->getRect()->toQRect();
    float aLeft = aObjRect->left() + xPan, aRight = aObjRect->right() + xPan;
    float aTop = aObjRect->top() + yPan, aBot = aObjRect->bottom() + yPan;

    GLfloat anAngle = curvp->getGLWidget()->getRotationAngle() * PI / 180.;

    QPointArray aPointArray(4);
    int ls = (int)(aLeft*sin(anAngle)),  lc = (int)(aLeft*cos(anAngle)), 
        rs = (int)(aRight*sin(anAngle)), rc = (int)(aRight*cos(anAngle)), 
        ts = (int)(aTop*sin(anAngle)),   tc = (int)(aTop*cos(anAngle)), 
        bs = (int)(aBot*sin(anAngle)),   bc = (int)(aBot*cos(anAngle)); 
    aPointArray[0] = QPoint( lc - ts, ls + tc );
    aPointArray[1] = QPoint( rc - ts, rs + tc );
    aPointArray[2] = QPoint( rc - bs, rs + bc );
    aPointArray[3] = QPoint( lc - bs, ls + bc );

    int aMinLeft = aPointArray[0].x(), aMaxRight = aPointArray[0].x(), 
        aMinTop = aPointArray[0].y(), aMaxBottom = aPointArray[0].y();
    for( int i = 1; i < 4; i++ )
    {
        int x = aPointArray[i].x();
        int y = aPointArray[i].y();
        aMinLeft = QMIN( aMinLeft,x );
        aMaxRight = QMAX( aMaxRight, x );
        aMinTop = QMIN( aMinTop, y );
        aMaxBottom = QMAX( aMaxBottom, y );
    }

    aLeft = (aMinLeft/* + xPan*/)*xScale + aWidth / 2;
    aRight = (aMaxRight/* + xPan*/)*xScale + aWidth / 2;

    aTop = -( (aMaxBottom/* + yPan*/)*yScale - aHeight / 2 );
    aBot = -( (aMinTop/* + yPan*/)*yScale - aHeight / 2 );    

    QRect* newRect = new QRect( (int)aLeft, (int)aTop, (int)(aRight-aLeft), (int)(aBot-aTop) );
    
    return newRect;
}

GLViewer_Rect GLViewer_Viewer2d::getGLVRect( const QRect& theRect ) const
{
  if ( !getActiveView() )
      return GLViewer_Rect();

  GLViewer_ViewPort2d* vp = ( GLViewer_ViewPort2d* )getActiveView()->getViewPort();

  if( !vp )
    return GLViewer_Rect();

  return vp->win2GLV( theRect );
}

QRect GLViewer_Viewer2d::getQRect( const GLViewer_Rect& theRect ) const
{
  if ( !getActiveView() )
      return QRect();

  GLViewer_ViewPort2d* vp = ( GLViewer_ViewPort2d* )getActiveView()->getViewPort();

  if( !vp )
    return QRect();

  return vp->GLV2win( theRect );
}

GLViewer_Selector* GLViewer_Viewer2d::createSelector()
{
  return new GLViewer_Selector2d( this, getGLContext() );
}

GLViewer_ViewTransformer* GLViewer_Viewer2d::createTransformer( int type )
{
    return new GLViewer_View2dTransformer( this, type );
}
/*
GLViewer_Sketcher* GLViewer_Viewer2d::createGLSketcher( int type )
{
    return new GLViewer_Sketcher( this, type );
}

void GLViewer_Viewer2d::activateGLSketching( int type )
{
    GLViewer_ViewPort2d* vp = 0;
    if ( !getActiveView() || !( vp = ( GLViewer_ViewPort2d* )getActiveView()->getViewPort() ) )
        return;

    // Finish current sketching
    if ( type == None )
    {
        if ( myGLSketcher->getType() != None )
        {
            myGLSketcher->setType( None );
            finishSketching();
        }
    }
    // Activate new sketching
    else
    {
        activateGLSketching( None );  // concurrency not supported
        myGLSketcher->setType( type );
        startSketching();
    }
}

void GLViewer_Viewer2d::startSketching()
{
    GLViewer_ViewPort2d* avp = (GLViewer_ViewPort2d*)getActiveView()->getViewPort();
    avp->setCursor( *avp->getSketchCursor() );
    //avp->enablePopup( false );
    myGLSketcher->startSketching();
}

void GLViewer_Viewer2d::finishSketching()
{
    GLViewer_ViewPort2d* avp = (GLViewer_ViewPort2d*)getActiveView()->getViewPort();
    avp->setCursor( *avp->getDefaultCursor() );
    //avp->enablePopup( true );
    myGLSketcher->finishSketching();
}

bool GLViewer_Viewer2d::isSketchingActive()
{
    return myGLSketcher->getType() != None; 
}

int GLViewer_Viewer2d::getSketchingType()
{
    return myGLSketcher->getType();
}

void GLViewer_Viewer2d::onSketchDelObject()
{
    GLViewer_ViewPort2d* avp = (GLViewer_ViewPort2d*)getActiveView()->getViewPort();
    avp->setCursor( *avp->getDefaultCursor() );    
    myGLSketcher->finishSketching( true );
}

void GLViewer_Viewer2d::onSketchUndoLast()
{

}

void GLViewer_Viewer2d::onSketchFinish()
{
    finishSketching();
}
*/
void GLViewer_Viewer2d::onMouseEvent( SUIT_ViewWindow*, QMouseEvent* e )
{
    if ( !getActiveView() )
        return;

    //if ( testRotation( e ) )
    //    return;

    switch( e->type() )
    {
        case QEvent::MouseButtonPress :
        case QEvent::MouseMove :
        case QEvent::MouseButtonRelease :
            //if( myGLSketcher->getType() != None )
            //    myGLSketcher->sketch( e );
        default: break;
    }

    GLViewer_Viewer::onMouseEvent( 0, e );
}

bool GLViewer_Viewer2d::testRotation( QMouseEvent* e )
{
    if ( ( e->button() == GLViewer_View2dTransformer::rotateButton() ) &&
         ( e->type() == QEvent::MouseButtonPress ) &&
         ( e->state() & GLViewer_ViewTransformer::accelKey() ) )
    {
        activateTransform( GLViewer_Viewer::Rotate );
        return true;
    }
    return false;
}


void GLViewer_Viewer2d::insertHeader( VectorFileType aType, QFile& hFile )
{
    if( aType == POST_SCRIPT )
    {
        QString header = "%!PS-Adobe-3.0\n";
        header += "%%Creator: OpenCascade 2004\n";
        header += "%%Title: Our document\n";        
        header += "%%PageOrder: Ascend\n";      
        header += "%%Orientation: Portrait\n";
        header += "%%LanguageLevel: 2\n";

        header += "%%Pages: 1\n";
        header += "%%Page: 1\n\n";
        
        hFile.writeBlock( header.ascii(), header.length() );
    }
    else if( aType == HPGL )
    {
        QString header = "[Esc].(;\n";
        header += "[Esc].I81;;17:\n";
        header += "[Esc].N;19:\n";
        header += "IN;\n";
        header += "SC;\n";
        header += "PU;\n";
        header += "SP1;\n";
        header += "LT;\n";
        header += "VS36;\n";
        
        hFile.writeBlock( header.ascii(), header.length() );
    }
}

void GLViewer_Viewer2d::insertEnding( VectorFileType aType, QFile& hFile )
{
    if( aType == POST_SCRIPT )
    {
        QString ending = "showpage\n\n%%EOF";
        hFile.writeBlock( ending.ascii(), ending.length() );
    }
    else if( aType == HPGL )
    {
        QString ending = "PU;PA0,0;SP;EC;PG1;EC1;OE\n"; 
        hFile.writeBlock( ending.ascii(), ending.length() );
    }
}

inline void mm2custom( GLViewer_Viewer2d::VectorFileType aType, double& value )
{
    if( aType==GLViewer_Viewer2d::POST_SCRIPT )
        value*=2.8346; //mm to pt

    else if( aType==GLViewer_Viewer2d::HPGL )
        value*=40;     //mm to plu (there are 40 plues in mm)
#ifdef WIN32
    else if( aType==GLViewer_Viewer2d::ENH_METAFILE )
        value*=100;    //this unit is 1/100 mm
#endif 
}

bool GLViewer_Viewer2d::translateTo( VectorFileType aType, QString FileName, PaperType aPType, 
                                  double mmLeft, double mmRight, double mmTop, double mmBottom )
{
    if ( !getActiveView() )
      return false;

        QFile hFile( FileName.ascii() );

#ifdef WIN32
    HDC hMetaFileDC;
#endif

    GLViewer_ViewPort2d* aCurVP = (GLViewer_ViewPort2d*) getActiveView()->getViewPort();

    GLfloat xPan, yPan;
    aCurVP->getPan( xPan, yPan );
    GLfloat aRotation = aCurVP->getGLWidget()->getRotationAngle() * 3.14159265 / 180.0;

    GLViewer_CoordSystem aViewerCS( GLViewer_CoordSystem::Cartesian, xPan, yPan, 1.0, 1.0, aRotation );

    double AW = Sizes[2*int(aPType)], 
           AH = Sizes[2*int(aPType)+1]; //size of Axx paper in mm

    mm2custom( aType, mmLeft ); //we translate mm to custom units
    mm2custom( aType, mmRight );
    mm2custom( aType, mmTop );
    mm2custom( aType, mmBottom );
    mm2custom( aType, AW );
    mm2custom( aType, AH );

    float xScale, yScale;
    aCurVP->getScale( xScale, yScale );

    double VPWidth = aCurVP->getWidth()/xScale,   //the width in reference units
           VPHeight = aCurVP->getHeight()/yScale;

    double k1 = ( AW-mmLeft-mmRight ) / VPWidth,
           k2 = ( AH-mmTop-mmBottom ) / VPHeight;

    if( k1>k2 )
        k1 = k2; //We select the minimum

    double hdelta = ( AW-mmLeft-mmRight - VPWidth * k1 )/2.0, //addition in horizontal
           vdelta = ( AH-mmTop-mmBottom - VPHeight * k1 )/2.0; //addition in vertical

    mmLeft   += hdelta; //The real free space on the left and right borders
    mmRight  += hdelta;
    mmTop    += vdelta;
    mmBottom += vdelta;

    GLViewer_CoordSystem aPaperCS( GLViewer_CoordSystem::Cartesian, 
        -(mmLeft/k1+VPWidth/2.0), -(mmBottom/k1+VPHeight/2.0), 1/k1, 1/k1 );

    if( aType==POST_SCRIPT || aType==HPGL )
    {
        hFile.open( IO_ReadWrite | IO_Truncate );
        hFile.at( 0 );
        insertHeader( aType, hFile );
    }
#ifdef WIN32
    else if( aType==ENH_METAFILE )
    {
        RECT r; 
        r.left = 0; r.right = AW; 
        r.top = 0; r.bottom = AH; 
        HDC screen_dc = GetDC( 0 ); //The screen device context
        HDC bitDC = CreateCompatibleDC ( screen_dc ); //The context compatible with screen

        hMetaFileDC = CreateEnhMetaFile( bitDC, FileName.ascii(), &r, "" );
        SetMapMode( hMetaFileDC, MM_HIMETRIC );
        SetWindowOrgEx( hMetaFileDC, 0, r.bottom, NULL );
        HRGN ClipRgn = CreateRectRgn( 0, 0, AW, AH );
        SelectClipRgn( hMetaFileDC, ClipRgn );

        LOGBRUSH aBrushData;
        aBrushData.lbColor = RGB( 255, 255, 255 );      
        aBrushData.lbStyle = BS_SOLID;

        FillRect( hMetaFileDC, &r, CreateBrushIndirect( &aBrushData ) );

        ReleaseDC( 0, screen_dc );
        DeleteDC( bitDC );

        aCurVP->getGLWidget()->translateBackgroundToEMF( hMetaFileDC, &aViewerCS, &aPaperCS );
    }
#endif

    if( aType==POST_SCRIPT )
    {
        QString temp = "%1 %2 %3 %4 rectclip\n\n",
                aBuffer = temp.arg( mmLeft ).arg( mmBottom ).
                               arg( AW-mmLeft-mmRight ).arg( AH-mmBottom-mmTop );
        //It is set clipping path

        hFile.writeBlock( aBuffer.ascii(), aBuffer.length() );

        aCurVP->getGLWidget()->translateBackgroundToPS( hFile, &aViewerCS, &aPaperCS );
    }

    bool result = true;
    for( int i=0, n=myDrawers.count(); i<n; i++ )
        if( aType==POST_SCRIPT )
            result &= myDrawers[ i ]->translateToPS( hFile, &aViewerCS, &aPaperCS );
        else if( aType==HPGL )
            result &= myDrawers[ i ]->translateToHPGL( hFile, &aViewerCS, &aPaperCS );
#ifdef WIN32
        else if( aType==ENH_METAFILE )
            result &= myDrawers[ i ]->translateToEMF( hMetaFileDC, &aViewerCS, &aPaperCS );
#endif

    if( aType==POST_SCRIPT || aType==HPGL )
    {
        insertEnding( aType, hFile);
        hFile.close();
    }
#ifdef WIN32
    else if( aType==ENH_METAFILE )  
        DeleteEnhMetaFile( CloseEnhMetaFile( hMetaFileDC ) );
#endif

    return true;
}


void GLViewer_Viewer2d::repaintView( GLViewer_ViewFrame* theView, bool makeCurrent )
{
    GLViewer_ViewFrame* aCurView;
    if( !theView )
        aCurView = (GLViewer_ViewFrame*)getActiveView();
    else
        aCurView = theView;
    
    if ( !aCurView )
      return;

    ObjList anActiveObjs;
    const ObjList& objs = myGLContext->getObjList();
    for( ObjList::const_iterator it = objs.begin(); it != objs.end(); ++it )
    {
      GLViewer_Object* obj = (GLViewer_Object*)(*it);
      if( obj->getVisible() )
          anActiveObjs.append( obj );
    }

    float xScale;
    float yScale;

    QValueList<GLViewer_Drawer*>::Iterator anIt = myDrawers.begin();
    QValueList<GLViewer_Drawer*>::Iterator endDIt = myDrawers.end();
    for( ; anIt != endDIt; anIt++ )
            (*anIt)->clear();

    QValueList<GLViewer_Drawer*> anActiveDrawers;
    QValueList<GLViewer_Object*>::Iterator endOIt = anActiveObjs.end();

    for( QValueList<GLViewer_Object*>::Iterator oit = anActiveObjs.begin(); oit != endOIt; ++oit )
    {
        GLViewer_Drawer* aDrawer = (*oit)->getDrawer();
        if( !aDrawer )
        {
            anIt = myDrawers.begin();            

            for( ; anIt != endDIt; anIt++ )
                if( (*anIt)->getObjectType() == (*oit)->getObjectType() )
                {
                    (*oit)->setDrawer( *anIt );
                    aDrawer = *anIt;
                    break;
                }

            if( !aDrawer ) //are not exists
            {
                myDrawers.append( (*oit)->createDrawer() );
                aDrawer = (*oit)->getDrawer();
            }
        }
        aDrawer->addObject( (*oit) );
        if( anActiveDrawers.findIndex( aDrawer ) == -1 )
            anActiveDrawers.append( aDrawer );
    } 

    QValueList<GLViewer_Drawer*>::Iterator aDIt = anActiveDrawers.begin();
    QValueList<GLViewer_Drawer*>::Iterator aDEndIt = anActiveDrawers.end();

    GLViewer_ViewPort2d* vp = ( GLViewer_ViewPort2d* )aCurView->getViewPort();
    vp->getScale( xScale, yScale );

    if( makeCurrent )
        vp->getGLWidget()->makeCurrent();

    for( ; aDIt != aDEndIt; aDIt++ )
        (*aDIt)->create( xScale, yScale, false );
    
//    if ( swap )
    vp->getGLWidget()->swapBuffers();

//    ( ( GLViewer_ViewPort2d* )getActiveView()->getViewPort() )->getGLWidget()->makeCurrent();
}

void GLViewer_Viewer2d::startOperations( QMouseEvent* e )
{
    GLViewer_ViewPort2d* vp = ( GLViewer_ViewPort2d* )((GLViewer_ViewFrame*)getActiveView())->getViewPort();

    float x = e->pos().x();
    float y = e->pos().y();
    transPoint( x, y );
    GLViewer_Pnt point( x, y );

    if( e->button() == Qt::LeftButton && !myGLContext->getCurrentObject() && vp->startPulling( point ) )
        return;

    if( e->button() == Qt::LeftButton && !(vp->currentBlock() & BS_Selection) && !myGLContext->getCurrentObject() )
        vp->startSelectByRect( e->x(), e->y() );
}

bool GLViewer_Viewer2d::updateOperations( QMouseEvent* e )
{
    GLViewer_ViewPort2d* vp = ( GLViewer_ViewPort2d* )((GLViewer_ViewFrame*)getActiveView())->getViewPort();

    if( vp->isPulling() )
    {
        float x = e->pos().x();
        float y = e->pos().y();
        transPoint( x, y );

        vp->drawPulling( GLViewer_Pnt( x, y ) );
        updateAll();
        return true;
    }

    if( !myGLContext->getCurrentObject() )
    {
        vp->drawSelectByRect( e->x(), e->y() );
        return true;
    }
    return false;
}

void GLViewer_Viewer2d::finishOperations( QMouseEvent* e )
{
    GLViewer_ViewPort2d* vp = ( GLViewer_ViewPort2d* )((GLViewer_ViewFrame*)getActiveView())->getViewPort();

    if( vp->isPulling() )
    {
        vp->finishPulling();
        updateAll();
        return;
    }

    if( !myGLContext->getCurrentObject() )
    {
        QRect aSelRect = vp->selectionRect();
        vp->finishSelectByRect();
        if ( getSelector() && !aSelRect.isNull() )
        {            
            bool append = bool ( e->state() & GLViewer_Selector::appendKey() );
            getSelector()->select( aSelRect, append );
        }
    }
}

void GLViewer_Viewer2d::startOperations( QWheelEvent* e )
{
    bool zoomIn = e->delta() > 0;
    bool update = false;
    for( myGLContext->InitSelected(); myGLContext->MoreSelected(); myGLContext->NextSelected() )
    {
        GLViewer_Object* anObject = myGLContext->SelectedObject();
        update = anObject->updateZoom( zoomIn ) || update;
    }

    if( update )
        updateAll();
}

/*!
    Processes "Dump view..." context popup menu command
*/
void GLViewer_Viewer2d::onDumpView()
{
  if ( getActiveView() )
    getActiveView()->onDumpView();
}

/****************************************************************
**  Class: GLViewer_View2dTransformer
**
*****************************************************************/

int GLViewer_View2dTransformer::rotateBtn = RightButton;

GLViewer_View2dTransformer::GLViewer_View2dTransformer( GLViewer_Viewer* viewer, int typ )
: GLViewer_ViewTransformer( viewer, typ )
{
    if ( type() == GLViewer_Viewer::Rotate )
        initTransform( true );
}

GLViewer_View2dTransformer::~GLViewer_View2dTransformer()
{
    if ( type() == GLViewer_Viewer::Rotate )
        initTransform( false );
}

/*!
    Redefined to provide specific 3D transfomations. [ virtual public ]
*/
void GLViewer_View2dTransformer::exec()
{
    if ( !myViewer->getActiveView() )
      return;

    /* additional transforms */
    GLViewer_ViewPort* vp = myViewer->getActiveView()->getViewPort();
    //QAD_ASSERT( vp->inherits( "QAD_ViewPort3d" ) );
    GLViewer_ViewPort2d* avp = (GLViewer_ViewPort2d*)vp;
    switch ( myType )
    {
        case GLViewer_Viewer::Rotate:
            myMajorBtn = rotateButton();
            avp->setCursor( *avp->getRotCursor() );
            break;
        default:
            GLViewer_ViewTransformer::exec();
    }
}

/*!
    Handles rotation. [ protected virtual ]
*/
void GLViewer_View2dTransformer::onTransform( TransformState state )
{
    if ( !myViewer->getActiveView() )
      return;

    GLViewer_ViewPort* vp = myViewer->getActiveView()->getViewPort();
    //QAD_ASSERT( vp->inherits( "QAD_ViewPort3d" ) );
    GLViewer_ViewPort2d* avp = (GLViewer_ViewPort2d*)vp;
    if ( type() == GLViewer_Viewer::Rotate )
    {
        switch ( state )
        {
            case Debut:
                if ( myButtonState & myMajorBtn )
                    avp->startRotation( myStart.x(), myStart.y() );
                break;
            case EnTrain:
                if ( myButtonState & myMajorBtn )
                    avp->rotate( myCurr.x(), myCurr.y() );
                break;
            case Fin:
                avp->endRotation();
                break;
            default: break;
        }
    }
    GLViewer_ViewTransformer::onTransform( state );
}

