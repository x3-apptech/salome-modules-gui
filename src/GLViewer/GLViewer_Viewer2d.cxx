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
// File:      GLViewer_Viewer2d.cxx
// Created:   November, 2004
//#include <GLViewerAfx.h>
//
#include "GLViewer_Viewer2d.h"
#include "GLViewer_Object.h"
#include "GLViewer_ViewFrame.h"
#include "GLViewer_BaseObjects.h"
#include "GLViewer_CoordSystem.h"
#include "GLViewer_Context.h"
#include "GLViewer_Drawer.h"
#include "GLViewer_Selector2d.h"
#include "GLViewer_ViewPort2d.h"

#include "SUIT_Desktop.h"
#include "SUIT_ViewWindow.h"
#include "SUIT_ViewManager.h"

#include <QMenu>
#include <QRect>
#include <QFile>
#include <QPolygon>
#include <QMouseEvent>
#include <QColorDialog>

/*!
  Constructor
  \param title - viewer title
*/
GLViewer_Viewer2d::GLViewer_Viewer2d( const QString& title) :
GLViewer_Viewer( title )
{
  myGLContext = new GLViewer_Context( this );

  createSelector();

  mySelMode = GLViewer_Viewer::Multiple;

  myDrawers.clear();
}

/*!
  Destructor
*/
GLViewer_Viewer2d::~GLViewer_Viewer2d()
{    
    //myGLSketcher = 0;
    //delete myGLSketcher;
  GLViewer_TexFont::clearTextBases();
}

/*!Create new instance of view window on desktop \a theDesktop.
 *\retval SUIT_ViewWindow* - created view window pointer.
 */
SUIT_ViewWindow* GLViewer_Viewer2d::createView( SUIT_Desktop* theDesktop )
{
    return new GLViewer_ViewFrame( theDesktop, this );
}

/*!
  Adds item for change background color
  \param thePopup - menu
*/
void GLViewer_Viewer2d::addPopupItems( QMenu* thePopup )
{
  // CTH8434. "Change background color" menu item is available if there are no selected objects
  if ( getSelector() == 0 || getSelector()->numSelected() == 0 )
  {
    if( thePopup->actions().count() > 0 )
        thePopup->addSeparator();
    thePopup->addAction( tr( "CHANGE_BGCOLOR" ), this, SLOT( onChangeBgColor() ) );
  }
}

/*!
  Changes background color
*/
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

/*!
  Updates colors for all drawers (does not work)
*/
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
    /*
    ObjList anObjects = myGLContext->getObjects();
    ObjList::Iterator beginIt = anObjects.begin();
    ObjList::Iterator endIt = anObjects.end();
    for ( ObjList::Iterator it = beginIt; it != endIt; ++it )
    {
        //GLViewer_Drawer* aDrawer = (*it)->getDrawer();
        //aDrawer->setHColor( colorH );
        //aDrawer->setSColor( colorS );
    }
    */


  activateAllDrawers( true );
}

/*!
  Updates rect of global scene by adding new rectangle
  \param theRect - rectangle
*/
void GLViewer_Viewer2d::updateBorders( GLViewer_Rect* theRect )
{
  QVector<SUIT_ViewWindow*> views = getViewManager()->getViews();
  for ( int i = 0, n = views.count(); i < n; i++ )
  {
    GLViewer_Rect* border = ( ( GLViewer_ViewPort2d* )((GLViewer_ViewFrame*)views[i])->getViewPort() )->getBorder();

    border->setLeft( qMin( border->left(), theRect->left() ) );
    border->setRight( qMax( border->right(), theRect->right() ) );
    border->setBottom( qMin( border->bottom(), theRect->bottom() ) );
    border->setTop( qMax( border->top(), theRect->top() ) );
  }
}

/*!
  Recomputes global scene rect
*/
void GLViewer_Viewer2d::updateBorders()
{
    QVector<SUIT_ViewWindow*> views = getViewManager()->getViews();

    ObjList anObjects = myGLContext->getObjects();
    ObjList::Iterator beginIt = anObjects.begin();
    ObjList::Iterator endIt = anObjects.end();
    for ( int i = 0, n = views.count(); i < n; i++ )
    {
        GLViewer_Rect* border = ( ( GLViewer_ViewPort2d* )((GLViewer_ViewFrame*)views[i])->getViewPort() )->getBorder();
        if ( !border )
          continue;
        border->setIsEmpty( true );
        // initialise border by default values to avoid old values
        border->setCoords( 0, 0, 0, 0 );
        for ( ObjList::Iterator it = beginIt; it != endIt; ++it )
        {
            GLViewer_Object* anObject = *it;
            GLViewer_Rect* aRect = anObject->getRect();
            if( !anObject->isSelectable() || !anObject->getVisible() )
                continue;

            if( border->isEmpty() )
            {
                border->setIsEmpty( false );
                border->setCoords( aRect->left(), aRect->right(), aRect->bottom(), aRect->top() );
            }
            else
            {
                border->setLeft( qMin( border->left(), aRect->left() ) );
                border->setRight( qMax( border->right(), aRect->right() ) );
                border->setBottom( qMin( border->bottom(), aRect->bottom() ) );
                border->setTop( qMax( border->top(), aRect->top() ) );
            }
        }
    }
}

/*!
  Redraws all active objects by updating all drawers in all views
*/
void GLViewer_Viewer2d::updateAll()
{
  if ( !getActiveView() )
    return;

  QVector<SUIT_ViewWindow*> views = getViewManager()->getViews();
  for ( int i = 0, n = views.count(); i < n; i++ )
    ( ( GLViewer_ViewPort2d* )( ( GLViewer_ViewFrame* )views[i] )->getViewPort() )->getGLWidget()->updateGL();
}

/*!
   \param onlyUpdate is passed to method activateAllDrawers drawers
*/
void GLViewer_Viewer2d::updateDrawers( GLboolean update, GLfloat scX, GLfloat scY )
{
//  cout << "GLViewer_Viewer2d::updateDrawers" << endl;

    //myGLContext->updateScales( scX, scY );
    //myGLSketcher->drawContour();
    activateAllDrawers( update );
}

/*!
  Activates drawers for objects from list \param theObjects only
*/
void GLViewer_Viewer2d::activateDrawers( QList<GLViewer_Object*>& theObjects, bool onlyUpdate, GLboolean swap )
{
    //cout << "GLViewer_Viewer2d::activateDrawers " << (int)onlyUpdate << " " << (int)swap << endl;
    QList<GLViewer_Drawer*>::Iterator anIt = myDrawers.begin();
    QList<GLViewer_Drawer*>::Iterator endDIt = myDrawers.end();
    for( ; anIt != endDIt; anIt++ )
        (*anIt)->clear();

    QList<GLViewer_Drawer*> anActiveDrawers;
    QList<GLViewer_Object*>::Iterator endOIt = theObjects.end();

    for( QList<GLViewer_Object*>::Iterator oit = theObjects.begin(); oit != endOIt; ++oit )
    {
        GLViewer_Drawer* aDrawer = (*oit)->getDrawer();
        if( !aDrawer )
        {
            anIt = myDrawers.begin();
            endDIt = myDrawers.end();

            for( ; anIt != endDIt; anIt++ )
                if( (*anIt)->getObjectType() == (*oit)->getObjectType() )
                {
                    (*oit)->setDrawer( *anIt );
                    aDrawer = *anIt;
                    break;
                }

            if( !aDrawer )
            {
                myDrawers.append( (*oit)->createDrawer() );
                aDrawer = (*oit)->getDrawer();
            }
        }
        if ( !aDrawer )
          continue;
        aDrawer->addObject( (*oit) );

        int aPriority = aDrawer->getPriority();

        if( anActiveDrawers.indexOf( aDrawer ) != -1 )
            continue;

        QList<GLViewer_Drawer*>::Iterator aDIt = anActiveDrawers.begin();
        QList<GLViewer_Drawer*>::Iterator aDEndIt = anActiveDrawers.end();
        for( ; aDIt != aDEndIt; ++aDIt )
            if( (*aDIt)->getPriority() > aPriority )
                break;

        anActiveDrawers.insert( aDIt, aDrawer );
    } 

    QList<GLViewer_Drawer*>::Iterator aDIt = anActiveDrawers.begin();
    QList<GLViewer_Drawer*>::Iterator aDEndIt = anActiveDrawers.end();

    QVector<SUIT_ViewWindow*> views = getViewManager()->getViews();
    for ( int i = 0, n = views.count(); i < n; i++ )
    {
        float xScale, yScale;
        GLViewer_ViewPort2d* vp = ( GLViewer_ViewPort2d* )((GLViewer_ViewFrame*)views[i])->getViewPort();
        vp->getScale( xScale, yScale );
        vp->getGLWidget()->makeCurrent();

        for( ; aDIt != aDEndIt; aDIt++ )
        {
            GLViewer_Drawer* aDrawer = *aDIt;
            if( aDrawer )
                aDrawer->create( xScale, yScale, onlyUpdate );
        }
/*
        // draw border
        GLViewer_Rect* border = ( ( GLViewer_ViewPort2d* )((GLViewer_ViewFrame*)views[i])->getViewPort() )->getBorder();
        (*aDIt)->drawRectangle( border, Qt::blue );

        QString coords = QString::number( border->left() ) + " " + QString::number( border->right() ) + " " +
                         QString::number( border->bottom() ) + " " + QString::number( border->top() );
        (*aDIt)->drawText( "Border : " + coords, border->left(), border->top() + 10 / yScale,
                           Qt::blue, &QFont( "Courier", 8, QFont::Normal ), 2 );
*/
        if ( swap )
           vp->getGLWidget()->swapBuffers();
    }

    ( ( GLViewer_ViewPort2d* )getActiveView()->getViewPort() )->getGLWidget()->makeCurrent();
}

/*!
  Activates drawer for \param theObject
*/
void GLViewer_Viewer2d::activateDrawer( GLViewer_Object* theObject, bool onlyUpdate, GLboolean swap )
{
  ObjList aList;
  aList.append( theObject );
  activateDrawers( aList, onlyUpdate, swap );
}

/*!
   \param onlyUpdate is passed to drawers
*/
void GLViewer_Viewer2d::activateAllDrawers( bool onlyUpdate, GLboolean swap )
{
    if ( !getActiveView() )
      return;

    ObjList anActiveObjs;
    const ObjList& objs = myGLContext->getObjects();
    for( ObjList::const_iterator it = objs.begin(); it != objs.end(); ++it )
    {
      GLViewer_Object* obj = (GLViewer_Object*)(*it);
      if( obj->getVisible() )
          anActiveObjs.append( obj );
    }

    activateDrawers( anActiveObjs, onlyUpdate, swap );
}

/*!
  Creates set of marker
  \param theMarkersNum - number of markers 
  \param theMarkersRad - radius of markers
*/
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

    updateBorders( aMarkerSet->getRect() );
    
    activateAllDrawers( false );
    activateTransform( GLViewer_Viewer::FitAll );

    delete[] aXCoord;
    delete[] anYCoord;
}

/*!
  Creates GL polyline
  \param theAnglesNum - number of angles
  \param theRadius - radius
  \param thePolylineNumber - number
*/
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

        updateBorders( aPolyline->getRect() );
    }
    
    activateAllDrawers( false );
    activateTransform( GLViewer_Viewer::FitAll );

    delete[] aXCoord;
    delete[] anYCoord;
}

/*!
  Creates text
  \param theStr - text string
  \param theTextNumber - number
*/
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

        updateBorders( aText->getRect() );
    }

    activateAllDrawers( false );
}

/*!
  Translates point from global CS to curreent viewer CS
  \param x, y - co-ordinates to be translated
*/
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

/*!
  \return object rect in window CS
  \param theObject - object
*/
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

    QRect aObjRect = theObject->getRect()->toQRect();
    float aLeft = aObjRect.left() + xPan, aRight = aObjRect.right() + xPan;
    float aTop = aObjRect.top() + yPan, aBot = aObjRect.bottom() + yPan;

    GLfloat anAngle = curvp->getGLWidget()->getRotationAngle() * PI / 180.;

    QPolygon aPointArray(4);
    aPointArray[0] = QPoint( (int)(aLeft*cos(anAngle) - aTop*sin(anAngle)),
                             (int)(aLeft*sin(anAngle) + aTop*cos(anAngle)) );
    aPointArray[1] = QPoint( (int)(aRight*cos(anAngle) - aTop*sin(anAngle)),
                             (int)(aRight*sin(anAngle) + aTop*cos(anAngle)) );
    aPointArray[2] = QPoint( (int)(aRight*cos(anAngle) - aBot*sin(anAngle)),
                             (int)(aRight*sin(anAngle) + aBot*cos(anAngle)) );
    aPointArray[3] = QPoint( (int)(aLeft*cos(anAngle) - aBot*sin(anAngle)),
                             (int)(aLeft*sin(anAngle) + aBot*cos(anAngle)) );

    int aMinLeft = aPointArray[0].x(), aMaxRight = aPointArray[0].x(), 
        aMinTop = aPointArray[0].y(), aMaxBottom = aPointArray[0].y();
    for( int i = 1; i < 4; i++ )
    {
        int x = aPointArray[i].x();
        int y = aPointArray[i].y();
        aMinLeft = qMin( aMinLeft,x );
        aMaxRight = qMax( aMaxRight, x );
        aMinTop = qMin( aMinTop, y );
        aMaxBottom = qMax( aMaxBottom, y );
    }

    aLeft = (aMinLeft/* + xPan*/)*xScale + aWidth / 2;
    aRight = (aMaxRight/* + xPan*/)*xScale + aWidth / 2;

    aTop = -( (aMaxBottom/* + yPan*/)*yScale - aHeight / 2 );
    aBot = -( (aMinTop/* + yPan*/)*yScale - aHeight / 2 );    

    QRect* newRect = new QRect( (int)aLeft, (int)aTop, (int)(aRight-aLeft), (int)(aBot-aTop) );
    
    return newRect;
}

/*!
  Translates rect in window CS to rect in global CS
  \param theRect - rectangle to be translated
  \return transformed rect
*/
GLViewer_Rect GLViewer_Viewer2d::getGLVRect( const QRect& theRect ) const
{
  if ( !getActiveView() )
      return GLViewer_Rect();

  GLViewer_ViewPort2d* vp = ( GLViewer_ViewPort2d* )getActiveView()->getViewPort();

  if( !vp )
    return GLViewer_Rect();

  return vp->win2GLV( theRect );
}

/*!
  Translates rect in global CS to rect in window CS
  \param theRect - rectangle to be translated
  \return transformed rect
*/
QRect GLViewer_Viewer2d::getQRect( const GLViewer_Rect& theRect ) const
{
  if ( !getActiveView() )
      return QRect();

  GLViewer_ViewPort2d* vp = ( GLViewer_ViewPort2d* )getActiveView()->getViewPort();

  if( !vp )
    return QRect();

  return vp->GLV2win( theRect );
}

/*!
  \return new selector
*/
GLViewer_Selector* GLViewer_Viewer2d::createSelector()
{
  return new GLViewer_Selector2d( this, getGLContext() );
}

/*!
  \return new Transformer 
  \param type - type of new transformer
*/
GLViewer_ViewTransformer* GLViewer_Viewer2d::createTransformer( int type )
{
    return new GLViewer_View2dTransformer( this, type );
}

/*!
  Custom mouse event handler
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

/*!
  Rotation transformation
*/
bool GLViewer_Viewer2d::testRotation( QMouseEvent* e )
{
    if ( ( e->button() == GLViewer_View2dTransformer::rotateButton() ) &&
         ( e->type() == QEvent::MouseButtonPress ) &&
         ( e->modifiers() & GLViewer_ViewTransformer::accelKey() ) )
    {
        activateTransform( GLViewer_Viewer::Rotate );
        return true;
    }
    return false;
}

/*!
  Inserts text lines as header for file
  \param aType - file type
  \param hFile - file instance
*/
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
        
        hFile.write( header.toLatin1() );
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
        
        hFile.write( header.toLatin1() );
    }
}

/*!
  Inserts text lines as ending for file
  \param aType - file type
  \param hFile - file instance
*/
void GLViewer_Viewer2d::insertEnding( VectorFileType aType, QFile& hFile )
{
    if( aType == POST_SCRIPT )
    {
        QString ending = "showpage\n\n%%EOF";
        hFile.write( ending.toLatin1() );
    }
    else if( aType == HPGL )
    {
        QString ending = "PU;PA0,0;SP;EC;PG1;EC1;OE\n"; 
        hFile.write( ending.toLatin1() );
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

/*!
  Translates current view content to vector file
  \param aType - type of file
  \param FileName - name of file,
  \param aPType - paper size type
  \param mmLeft, mmRight, mmTop, mmBottom - margins
*/
bool GLViewer_Viewer2d::translateTo( VectorFileType aType, QString FileName, PaperType aPType, 
                                  double mmLeft, double mmRight, double mmTop, double mmBottom )
{
    if ( !getActiveView() )
      return false;

        QFile hFile( FileName.toLatin1() );

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
        hFile.open( QIODevice::ReadWrite | QIODevice::Truncate );
        hFile.seek( 0 );
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

        hMetaFileDC = CreateEnhMetaFile( bitDC, FileName.toLatin1().data(), &r, "" );
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

        hFile.write( aBuffer.toLatin1() );

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

/*!
  Repaints view
  \param theView - view to be repainted. If it is NULL then all views will be repainted
*/
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
    const ObjList& objs = myGLContext->getObjects();
    for( ObjList::const_iterator it = objs.begin(); it != objs.end(); ++it )
    {
      GLViewer_Object* obj = (GLViewer_Object*)(*it);
      if( obj->getVisible() )
          anActiveObjs.append( obj );
    }

    float xScale;
    float yScale;

    QList<GLViewer_Drawer*>::Iterator anIt = myDrawers.begin();
    QList<GLViewer_Drawer*>::Iterator endDIt = myDrawers.end();
    for( ; anIt != endDIt; anIt++ )
            (*anIt)->clear();

    QList<GLViewer_Drawer*> anActiveDrawers;
    QList<GLViewer_Object*>::Iterator endOIt = anActiveObjs.end();

    for( QList<GLViewer_Object*>::Iterator oit = anActiveObjs.begin(); oit != endOIt; ++oit )
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
        if( anActiveDrawers.indexOf( aDrawer ) == -1 )
            anActiveDrawers.append( aDrawer );
    } 

    QList<GLViewer_Drawer*>::Iterator aDIt = anActiveDrawers.begin();
    QList<GLViewer_Drawer*>::Iterator aDEndIt = anActiveDrawers.end();

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

/*!
  Starts some operation on mouse event
*/
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

/*!
  Updates started operation on mouse event
*/
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

/*!
  Completes started operation on mouse event
*/
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
            bool append = bool ( e->modifiers() & GLViewer_Selector::appendKey() );
            getSelector()->select( aSelRect, append );
        }
    }
}

/*!
  Starts some operation on mouse wheel event
*/
void GLViewer_Viewer2d::startOperations( QWheelEvent* e )
{
    bool zoomIn = e->delta() > 0;
    bool update = false;
    for( myGLContext->InitSelected(); myGLContext->MoreSelected(); myGLContext->NextSelected() )
    {
        GLViewer_Object* anObject = myGLContext->SelectedObject();
        update = anObject->updateZoom( zoomIn ) || update;
    }

    emit wheelZoomChange( zoomIn );

    if( update )
        updateAll();
}


int GLViewer_View2dTransformer::rotateBtn = Qt::RightButton;

/*!
  Constructor
*/
GLViewer_View2dTransformer::GLViewer_View2dTransformer( GLViewer_Viewer* viewer, int typ )
: GLViewer_ViewTransformer( viewer, typ )
{
    if ( type() == GLViewer_Viewer::Rotate )
        initTransform( true );
}

/*!
  Destructor
*/
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
