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
// File:      GLViewer_Viewer.cxx
// Created:   November, 2004
//#include <GLViewerAfx.h>
//
#include "GLViewer_Viewer.h"
#include "GLViewer_Selector.h"
#include "GLViewer_ViewPort.h"
#include "GLViewer_ViewFrame.h"

//#include "SUIT_Desktop.h"
#include "SUIT_ViewWindow.h"
#include "SUIT_ViewManager.h"

#include <QApplication>
#include <QMenu>
#include <QColorDialog>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QRect>
#include <QRubberBand>

/* used for sketching */
static QEvent* l_mbPressEvent = 0;

/*!
    Constructor
*/
GLViewer_Viewer::GLViewer_Viewer( const QString& title )
: SUIT_ViewModel(),
mySelector( 0 ),
mySketcher( 0 ),
myTransformer( 0 ),
mySelMode( NoSelection )
{
}

/*!
    Destructor
*/
GLViewer_Viewer::~GLViewer_Viewer()
{
    delete mySelector;
}

/*!
  Sets new view manager
  \param theViewManager - new view manager
*/
void GLViewer_Viewer::setViewManager(SUIT_ViewManager* theViewManager)
{
  SUIT_ViewModel::setViewManager(theViewManager);
  if (theViewManager) 
  {
    connect(theViewManager, SIGNAL(mousePress(SUIT_ViewWindow*, QMouseEvent*)), 
            this, SLOT(onMouseEvent(SUIT_ViewWindow*, QMouseEvent*)));

    connect(theViewManager, SIGNAL(mouseMove(SUIT_ViewWindow*, QMouseEvent*)), 
            this, SLOT(onMouseEvent(SUIT_ViewWindow*, QMouseEvent*)));

    connect(theViewManager, SIGNAL(mouseRelease(SUIT_ViewWindow*, QMouseEvent*)), 
            this, SLOT(onMouseEvent(SUIT_ViewWindow*, QMouseEvent*)));

    connect(theViewManager, SIGNAL(wheel(SUIT_ViewWindow*, QWheelEvent*)), 
            this, SLOT(onWheelEvent(SUIT_ViewWindow*, QWheelEvent*)));
  }
}

/*!
  Builds popup for GL viewer
*/
void GLViewer_Viewer::contextMenuPopup( QMenu* thePopup )
{
  if( thePopup->actions().count() > 0 )
      thePopup->addSeparator();

  thePopup->addAction( tr( "CHANGE_BGCOLOR" ), this, SLOT( onChangeBgColor() ) );
}

/*!
    Sets the background color with color selection dialog. [ virtual protected slot ]
*/
void GLViewer_Viewer::onChangeBgColor()
{
  GLViewer_ViewPort* vp = getActiveView()->getViewPort();
  QColor selColor = QColorDialog::getColor( vp->backgroundColor() );

  if( selColor.isValid() )
    vp->setBackgroundColor( selColor );
}

/*!
    Returns the active view. [ public ]
*/
GLViewer_ViewFrame* GLViewer_Viewer::getActiveView() const
{
  SUIT_ViewManager* aMgr = getViewManager();
  return (GLViewer_ViewFrame*)( aMgr != 0 ? aMgr->getActiveView() : 0 );
}


/*!
    Sets the selection mode for this viewer. [ public ]
*/
void GLViewer_Viewer::setSelectionMode( GLViewer_Viewer::SelectionMode mode )
{
    if ( mySelMode != mode )
    {
        mySelMode = mode;
        onSelectionModeChanged();
    }
}

/*!
    Returns selector of this viewer. [ public ]
*/
GLViewer_Selector* GLViewer_Viewer::getSelector() const
{
    if ( !mySelector )
    {
        GLViewer_Viewer* mthis = (GLViewer_Viewer*)this;
        mthis->mySelector = mthis->createSelector();
        if ( mySelector )
        {
            connect( mySelector, SIGNAL( selSelectionDone( bool, SelectionChangeStatus ) ), SLOT( onSelectionDone( bool, SelectionChangeStatus ) ) );
            connect( mySelector, SIGNAL( selSelectionCancel() ), SLOT( onSelectionCancel() ) );
        }
    }
    return mySelector;
}

/*!
    Returns the selection mode of this viewer. [ public ]
*/
GLViewer_Viewer::SelectionMode GLViewer_Viewer::getSelectionMode() const
{
    return mySelMode;
}

/*!
    Handles requests for sketching in the active view. [ virtual public ]
*/
void GLViewer_Viewer::activateSketching( int type )
{
    GLViewer_ViewPort* vp = 0;
    if ( !getActiveView() || !( vp = getActiveView()->getViewPort() ) )
        return;

    if ( !vp->isSketchingEnabled() )
        return;

    /* Finish current sketching */
    if ( type == NoSketching )
    {
        if ( mySketcher )
        {
            onSketchingFinished();
            delete mySketcher;
            mySketcher = 0;
        }
    }
    /* Activate new sketching */
    else
    {
        activateSketching( NoSketching );  /* concurrency not suported */
        mySketcher = createSketcher( type );
        onSketchingStarted();
    }
}

/*!
    Handles requests for transformations in the active view. [ virtual public ]
*/
void GLViewer_Viewer::activateTransform( int type )
{
    GLViewer_ViewPort* vp = 0;
    if ( !getActiveView() || !( vp = getActiveView()->getViewPort() ) )
        return;

    if ( !vp->isTransformEnabled() )
        return;

    /* Finish current transform */
    if ( type == NoTransform )
    {
        if ( myTransformer )
        {
            onTransformationFinished();
            delete myTransformer;
            myTransformer = 0;
        }
    }
    /* Activate new transform */
    else
    {
        activateTransform( NoTransform );  /* concurrency not suported */
        myTransformer = createTransformer( type );
        onTransformationStarted();
        myTransformer->exec();
    }
}

/*!
    Creates default transformer. [ virtual protected ]
*/
GLViewer_ViewTransformer* GLViewer_Viewer::createTransformer( int type )
{
    return new GLViewer_ViewTransformer( this, type );
}

/*!
    Creates default sketcher. [ virtual protected ]
*/
GLViewer_ViewSketcher* GLViewer_Viewer::createSketcher( int type )
{
    return new GLViewer_ViewSketcher( this, type );
}

/*!
    Returns null by default. [ virtual protected ]
*/
GLViewer_Selector* GLViewer_Viewer::createSelector()
{
    return 0;
}

/*!
    Unhilights detected entities, lock selection, sets event filter
    on the whole application. [ virtual protected ]
*/
void GLViewer_Viewer::onTransformationStarted()
{
    unhilightDetected();
    if ( getSelector() )
        getSelector()->lock( true );        /* disable selection */

    /*  Watch events: any mouse/key event outside the
        viewport will be considered as the end of
        transform */
    if( !myTransformer )
        return;
    qApp->installEventFilter( this );
}

/*!
    Unlock selection, removes event filter. [ virtual protected ]
*/
void GLViewer_Viewer::onTransformationFinished()
{
    if ( getSelector() )
        getSelector()->lock( false );           /* enable selection */

    /* Stop watch events */
    if( !myTransformer )
        return;
    qApp->removeEventFilter( this );
}

/*!
    Unhilights detected entities. [ virtual protected ]
*/
void GLViewer_Viewer::onSketchingStarted()
{
    unhilightDetected();
}

/*!
    Selection by rectangle. [ virtual protected ]
*/
void GLViewer_Viewer::onSketchingFinished()
{
    if ( !getSelector() )
        return;

    if( !mySketcher )
        return;
    if ( mySketcher->type() == Rect )
    {
        QRect* selRect = (QRect*)mySketcher->data();
        if ( selRect )
        {
            bool append = bool( mySketcher->buttonState() & GLViewer_Selector::appendKey() );
            getSelector()->select( *selRect, append );
        }
    }
}

/*!
    Installed while 'fit area' and 'global panning' operations are active. [ virtual protected ]
*/
bool GLViewer_Viewer::eventFilter( QObject* o, QEvent* e )
{
    if( !getActiveView() )
        return false;

    if( getActiveView()->getViewPort() == o->parent() )
      o = o->parent();

    bool mouseClickedOutside = ( e->type() == QEvent::MouseButtonPress &&
                                 o != getActiveView()->getViewPort() );
    bool anyKeyPressed = ( e->type() == QEvent::KeyPress );
    if ( mouseClickedOutside || anyKeyPressed )
    {   /* terminate all */
        activateTransform( NoTransform );
        activateSketching( NoSketching );
        //cout << "mouseClickedOutside || anyKeyPressed" << endl;
    }
    return QObject::eventFilter( o, e );
}

/*!
    Called when smth is selected in this viewer. [ virtual protected slot ]
*/
void GLViewer_Viewer::onSelectionDone( bool bAdded, SelectionChangeStatus status  )
{
    emit selectionChanged( status );
}

/*!
    Called when selection is cleared in this viewer. [ virtual protected slot ]
*/
void GLViewer_Viewer::onSelectionCancel()
{
    emit selectionChanged( SCS_Invalid );
}

/*!
    Listens to key events of the active view. [ virtual protected slot ]
*/
void GLViewer_Viewer::onKeyEvent( SUIT_ViewWindow*, QKeyEvent* )
{
}

/*!
    Listens to mouse events of the active view. [ virtual protected slot ]
*/
void GLViewer_Viewer::onMouseEvent( SUIT_ViewWindow*, QMouseEvent* e )
{
    //cout << "GLViewer_Viewer::onMouseEvent" << endl;
    switch( e->type() )
    {
        case QEvent::MouseButtonPress:
            handleMousePress( e );
            break;
        case QEvent::MouseMove:
            handleMouseMove( e );
            break;
        case QEvent::MouseButtonRelease:
            handleMouseRelease( e );
            break;
        default: break;
    }
}

/*!
    Listens to mouse events of the active view. [ virtual protected slot ]
*/
void GLViewer_Viewer::onWheelEvent( SUIT_ViewWindow*, QWheelEvent* e )
{
    //cout << "GLViewer_Viewer::onMouseEvent" << endl;
    switch( e->type() )
    {
        case QEvent::Wheel:
            handleWheel( e );
            break;
        default: break;
    }
}

/*!
    Enables / disables rectangle sketching. [ virtual protected ]
*/
void GLViewer_Viewer::onSelectionModeChanged()
{
    bool enable = ( mySelMode == Multiple );    
    QVector<SUIT_ViewWindow*> views = getViewManager()->getViews();
    for ( int i = 0, n = views.count(); i < n; i++ )
    {
        GLViewer_ViewPort* vp = ((GLViewer_ViewFrame*)views[i])->getViewPort();
        if ( vp )
            vp->setSketchingEnabled( enable );
    }
}

/*!
    Updates all views of this viewer. Use 'flags' to customize update process. [ virtual public ]
*/
void GLViewer_Viewer::update( int flags )
{
    QVector<SUIT_ViewWindow*> views = getViewManager()->getViews();
    for ( int i = 0, n = views.count(); i < n; i++ )
        ((GLViewer_ViewFrame*)views[i])->onUpdate( flags );
}

/*!
    Unhilights the currect detected objects. [ virtual private ]
*/
void GLViewer_Viewer::unhilightDetected()
{
    if ( getSelector() )
        getSelector()->undetectAll();
}

/*!
    Mouse press handler. If 'accelKey()' is pressed, activates default
    transformations( Zoom or Pan ) in the active viewport. [ private ]
*/
void GLViewer_Viewer::handleMousePress( QMouseEvent* e )
{
    /* test accel for transforms */
    if ( e->modifiers() & GLViewer_ViewTransformer::accelKey() )
    {
        Qt::MouseButton bs = e->button();
        if ( bs == GLViewer_ViewTransformer::zoomButton() )
            activateTransform( Zoom );
        else if ( bs == GLViewer_ViewTransformer::panButton() )
            activateTransform( Pan );
    }
    else
    {
        //checking for other operations before selection in release event
        startOperations( e );
    }

    /* we may need it for sketching... */
    l_mbPressEvent = new QMouseEvent( *e );

    //checking for other operations before selection in release event
//    startOperations( e );

    /*e->button() == LeftButton && getSelector() )
    {
        bool append = bool ( e->state() & GLViewer_Selector::appendKey() );
        getSelector()->select( append );
    }*/
}

/*!
    Mouse move handler. If dragged with MB1, activates rectangle sketching in
    the active viewport, otherwise highlights the selectable entities. [ private ]
*/
void GLViewer_Viewer::handleMouseMove( QMouseEvent* e )
{
    /* Highlight for selection */
    bool dragged = ( e->buttons() & ( Qt::LeftButton | Qt::MidButton | Qt::RightButton ) );
    if ( !dragged )
    {
        if ( getSelector() )
            getSelector()->detect( e->x(), e->y() );
    }
    /* Try to activate default sketching
    */
    else if ( e->button() == GLViewer_ViewSketcher::sketchButton() )
    {
        activateSketching( Rect );
        if ( mySketcher )
        {
            /*  Activated OK. We should not forget initial mousepress
                event and this mousemove event to start sketching from
                the point of initial click */
            if ( l_mbPressEvent )
            {
                QApplication::sendEvent( getActiveView()->getViewPort(), l_mbPressEvent );
                delete l_mbPressEvent;
                l_mbPressEvent = 0;
            }
            QApplication::sendEvent( getActiveView()->getViewPort(), e );
        }
    }
    
    //Try to activate other operations
    updateOperations( e );
}

/*!
    Mouse move handler. Activates popup of the active view. [ private ]
*/
void GLViewer_Viewer::handleMouseRelease( QMouseEvent* e )
{
    /* selection */
    /* tmp - in handleMousePress*/  
    if( e->button() == Qt::LeftButton && !(getActiveView()->getViewPort()->currentBlock() & BS_Selection) )
    {
        if ( getSelector() )
        {
            bool append = bool ( e->modifiers() & GLViewer_Selector::appendKey() );
            getSelector()->select( append );
        }
    }

    //Try to done active operations
    finishOperations( e );        

    if ( l_mbPressEvent )
    {
        delete l_mbPressEvent;
        l_mbPressEvent = 0;
    }
}

/*!
    Wheel rolling handler
*/
void GLViewer_Viewer::handleWheel( QWheelEvent* e )
{
    startOperations( e );
}

int GLViewer_ViewTransformer::panBtn = Qt::MidButton;
int GLViewer_ViewTransformer::zoomBtn = Qt::LeftButton;
int GLViewer_ViewTransformer::fitRectBtn = Qt::LeftButton;
int GLViewer_ViewTransformer::panGlobalBtn = Qt::LeftButton;
int GLViewer_ViewTransformer::acccelKey = Qt::ControlModifier;

/*!
    Constructor
*/
GLViewer_ViewTransformer::GLViewer_ViewTransformer( GLViewer_Viewer* v, int type )
: QObject( 0 ),
myViewer( v ),
myType( type ),
myMajorBtn( Qt::NoButton ),
myButtonState( 0 ),
myRectBand( 0 )
{
    if ( myType == GLViewer_Viewer::Pan ||
         myType == GLViewer_Viewer::Zoom ||
         myType == GLViewer_Viewer::PanGlobal ||
         myType == GLViewer_Viewer::FitRect )
    {
        /* 'long' transforms need this */
        initTransform( true );
    }
}

/*!
    Destructor
*/
GLViewer_ViewTransformer::~GLViewer_ViewTransformer()
{
    if ( myType == GLViewer_Viewer::Pan ||
         myType == GLViewer_Viewer::Zoom ||
         myType == GLViewer_Viewer::PanGlobal ||
         myType == GLViewer_Viewer::FitRect )
    {
        /* 'long' transforms need this */
        initTransform( false );
    }

    //QAD_Application::getDesktop()->clearInfo();

    endDrawRect();
}

/*!
    Inits long transformations ( Zoom, Pan etc ) [ protected ]

    Saves viewport state( cursor etc ) and installs event filter to process
    mouse events if 'init' is true. Restores viewport state if 'init' is false.
*/
void GLViewer_ViewTransformer::initTransform( bool init )
{
    GLViewer_ViewPort* avp = myViewer->getActiveView()->getViewPort();
    if ( init )
    {
        mySavedCursor = avp->cursor();
        mySavedMouseTrack = avp->hasMouseTracking();
        avp->setMouseTracking( false );
        avp->installEventFilter( this );
    }
    else
    {
        avp->removeEventFilter( this );
        avp->setMouseTracking( mySavedMouseTrack );
        avp->setCursor( mySavedCursor );
    }
}

/*!
    Runs transfomation. Redefine to add your own 'instant' transforms
    ( for ex., 'FitAll' is an instant transform ). [ virtual public ]
*/
void GLViewer_ViewTransformer::exec()
{
    GLViewer_ViewPort* avp = myViewer->getActiveView()->getViewPort();
    if( !avp )
        return;

    switch( myType )
    {
        case GLViewer_Viewer::Zoom:
            myMajorBtn = zoomButton();
            avp->setCursor( *avp->getZoomCursor() );
            break;
        case GLViewer_Viewer::Pan:
            myMajorBtn = panButton();
            avp->setCursor( *avp->getPanCursor() );
            break;
        case GLViewer_Viewer::PanGlobal:
            myMajorBtn = panGlobalButton();
            avp->setCursor( *avp->getPanglCursor() );
            avp->fitAll( true, false );  /* view is ready now */
            break;
        case GLViewer_Viewer::FitRect:
            myMajorBtn = fitRectButton();
            avp->setCursor( *avp->getHandCursor() );
            break;
        case GLViewer_Viewer::Reset:
            avp->reset(); onTransform( Fin );
            break;
        case GLViewer_Viewer::FitAll:
            avp->fitAll(); onTransform( Fin );
            break;
        case GLViewer_Viewer::FitSelect:
            avp->fitSelect(); onTransform( Fin );
            break;
        default: break;
    }
}

/*!
    Catches mouse events for the viewport. [ virtual protected ]
*/
bool GLViewer_ViewTransformer::eventFilter( QObject* o, QEvent* e )
{
    switch ( e->type() )
    {
        case QEvent::MouseMove:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        {
            TransformState state = EnTrain;
            QMouseEvent* me = ( QMouseEvent* )e;

            myButtonState = me->buttons();
            if ( e->type() == QEvent::MouseButtonPress )
                myButtonState |= me->button();  /* add pressed button */

            if ( e->type() == QEvent::MouseButtonRelease )
                myButtonState |= me->button();  /* add release button */

            int mouseOnlyState = ( myButtonState & ( Qt::LeftButton | Qt::MidButton | Qt::RightButton ) );
            if ( myStart.isNull() )
            {
                state = Debut;
                myStart = me->pos();
                myMajorBtn = mouseOnlyState;
            }

            if ( e->type() == QEvent::MouseButtonRelease )
            {
                state = Fin;
            }
            myCurr = me->pos();
            onTransform( state );
            return true;
        }
        default: break;
    }
    return QObject::eventFilter( o, e );
}

/*!
    Transforms the viewport. Used for 'non-instant' transforms
    only( ex. Rotate, Pan etc ). [ virtual protected ]
*/
void GLViewer_ViewTransformer::onTransform( TransformState state )
{
    GLViewer_ViewPort* avp = myViewer->getActiveView()->getViewPort();
    bool doTrsf = ( myButtonState & myMajorBtn );
    switch ( myType )
    {
        case GLViewer_Viewer::Zoom:
            if ( state != Fin && doTrsf )
            {
                avp->zoom( myStart.x(), myStart.y(), myCurr.x(), myCurr.y() );
                myStart = myCurr;
            }
            break;
        case GLViewer_Viewer::Pan:
            if ( state != Fin && doTrsf )
            {
                avp->pan( myCurr.x() - myStart.x(), myStart.y() - myCurr.y() );
                myStart = myCurr;
            }
            break;
        case GLViewer_Viewer::PanGlobal:
        {
            if ( state == Fin )
            avp->setCenter( myCurr.x(), myCurr.y() );
            break;
        }
        case GLViewer_Viewer::FitRect:
        {
            if ( doTrsf )
            {
                QRect rect( qMin( myStart.x(), myCurr.x() ), qMin( myStart.y(), myCurr.y() ),
                            qAbs( myStart.x() - myCurr.x() ), qAbs( myStart.y() - myCurr.y() ) );
                if ( !rect.isEmpty() )
                {
                    switch ( state )
                    {
                        case Fin:
                            avp->fitRect( rect );
                            break;
                        default:
                        {
                            drawRect( rect );
                            break;
                        }
                    }
                }
            }
            break;
        }
        default:
            break;
    }

    if ( state == Fin )
        myViewer->activateTransform( GLViewer_Viewer::NoTransform );
}

/*!
  Draws rectangle by starting and current points
*/
void GLViewer_ViewTransformer::drawRect(const QRect& theRect)
{
  if ( !myRectBand ) {
    myRectBand = new QRubberBand( QRubberBand::Rectangle, myViewer->getActiveView()->getViewPort() );
    QPalette palette;
    palette.setColor(myRectBand->foregroundRole(), Qt::white);
    myRectBand->setPalette(palette);
  }
  myRectBand->hide();

  myRectBand->setGeometry( theRect );
  myRectBand->setVisible( theRect.isValid() );
}

/*!
  \brief Delete rubber band on the end on the dragging operation.
*/
void GLViewer_ViewTransformer::endDrawRect()
{
  if ( myRectBand ) myRectBand->hide();

  delete myRectBand;
  myRectBand = 0;
}

/*!
    Returns the type of the transformer. [ public ]
*/
int GLViewer_ViewTransformer::type() const
{
    return myType;
}


int GLViewer_ViewSketcher::sketchBtn = Qt::LeftButton;

/*!
    Constructor
*/
GLViewer_ViewSketcher::GLViewer_ViewSketcher( GLViewer_Viewer* viewer, int type )
: QObject( 0 ),
myViewer( viewer ),
myData( 0 ),
myType( type ),
myRectBand( 0 )
{
    if( !myViewer )
        return;
    GLViewer_ViewPort* avp = myViewer->getActiveView()->getViewPort();
    if( !avp )
        return;

    mySavedCursor = avp->cursor();
    avp->setCursor( *GLViewer_ViewPort::getHandCursor() );
    avp->installEventFilter( this );

    if ( myType == GLViewer_Viewer::Rect )
        myData = new QRect();
}

/*!
    Destructor
*/
GLViewer_ViewSketcher::~GLViewer_ViewSketcher()
{
    GLViewer_ViewPort* avp = myViewer->getActiveView()->getViewPort();
    avp->removeEventFilter( this );
    avp->setCursor( mySavedCursor );

    if ( myType == GLViewer_Viewer::Rect )
        delete ( QRect* ) myData;

    endDrawRect();
}

/*!
    Catches mouse events for the viewport. [ virtual protected ]
*/
bool GLViewer_ViewSketcher::eventFilter( QObject* o, QEvent* e )
{
    switch ( e->type() )
    {
        case QEvent::MouseMove:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        {
            SketchState state = EnTrain;
            QMouseEvent* me = (QMouseEvent*)e;

            myButtonState = me->buttons();
            if ( e->type() == QEvent::MouseButtonPress )
                myButtonState |= me->button();  /* add pressed button */

            if ( myStart.isNull() )
            {
                state = Debut;
                myStart = me->pos();
            }

            int mouseOnlyState = ( myButtonState & ( Qt::LeftButton | Qt::MidButton | Qt::RightButton ) );
            if ( e->type() == QEvent::MouseButtonRelease && mouseOnlyState == sketchButton() )
            {
                state = Fin;
            }
            myCurr = me->pos();
            onSketch( state );
            return true;
        }
        default: break;
    }
    return QObject::eventFilter( o, e );
}

/*!
    Draws in the viewport. [ virtual protected ]
*/
void GLViewer_ViewSketcher::onSketch( SketchState state )
{
    GLViewer_ViewPort* avp = myViewer->getActiveView()->getViewPort();
    if( !avp )
        return;

    if( myType == GLViewer_Viewer::Rect )
    {
        QRect* sketchRect = ( QRect* )data();
        if ( myButtonState & sketchButton() )
        {
            QRect rect( qMin( myStart.x(), myCurr.x() ), qMin( myStart.y(), myCurr.y() ),
                        qAbs( myStart.x() - myCurr.x() ), qAbs( myStart.y() - myCurr.y() ) );
            if ( !rect.isEmpty() )
            {
                if ( !sketchRect->isEmpty() && myRectBand )
                    myRectBand->hide();    /* erase */
                *sketchRect = rect;
                if ( state != Fin )
                    drawRect( *sketchRect );
            }
        }
    }

    if ( state == Fin )
    {
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    	QApplication::syncX();  /* force rectangle redrawing */
#endif
        myViewer->activateSketching( GLViewer_Viewer::NoSketching );
    }
}

/*!
  Draws rectangle by starting and current points
*/
void GLViewer_ViewSketcher::drawRect(const QRect& theRect)
{
  if ( !myRectBand ) {
    myRectBand = new QRubberBand( QRubberBand::Rectangle, myViewer->getActiveView()->getViewPort() );
    QPalette palette;
    palette.setColor(myRectBand->foregroundRole(), Qt::white);
    myRectBand->setPalette(palette);
  }
  myRectBand->hide();

  myRectBand->setGeometry( theRect );
  myRectBand->setVisible( theRect.isValid() );
}

/*!
  \brief Delete rubber band on the end on the dragging operation.
*/
void GLViewer_ViewSketcher::endDrawRect()
{
  if ( myRectBand ) myRectBand->hide();

  delete myRectBand;
  myRectBand = 0;
}
