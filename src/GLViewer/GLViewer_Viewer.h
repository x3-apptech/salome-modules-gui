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
// File:      GLViewer_Viewer.h
// Created:   November, 2004
//
#ifndef GLVIEWER_VIEWER_H
#define GLVIEWER_VIEWER_H

#include "GLViewer.h"
#include "GLViewer_Defs.h"
#include <SUIT_ViewModel.h>

#include <QCursor>
#include <QObject>

class QMouseEvent;
class QKeyEvent;
class QWheelEvent;
class QRect;
class QRubberBand;

class GLViewer_Selector;
class GLViewer_ViewSketcher;
class GLViewer_ViewTransformer;
class GLViewer_ViewFrame;

//class SUIT_Desktop;
class SUIT_ViewWindow;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

/*! 
  \class GLViewer_Object
  Base Viewer for GLViewer
*/
class GLVIEWER_API GLViewer_Viewer: public SUIT_ViewModel
{
    Q_OBJECT

public:
    enum SelectionMode { NoSelection, Single, Multiple };
    enum TransformType { NoTransform, Reset, FitAll, FitRect, FitSelect,
                         Zoom, PanGlobal, Pan, Rotate, UserTransform = 100 };
    enum SketchingType { NoSketching, Rect, UserSketching = 100 };

public:
    GLViewer_Viewer( const QString& title );
    ~GLViewer_Viewer();

public:
    virtual void                 setViewManager( SUIT_ViewManager* theViewManager );
    virtual QString              getType() const { return Type(); }
    static QString               Type() { return "GLViewer_ViewModel";  }

    virtual void                 contextMenuPopup( QMenu* );

public:
    void                         setSelectionMode( SelectionMode );
    SelectionMode                getSelectionMode() const;
    GLViewer_Selector*           getSelector() const;

    virtual void                 update( int = 0 );

    void                         activateTransform( int );
    void                         activateSketching( int );

    GLViewer_ViewFrame*          getActiveView() const;

signals:
    void                         selectionChanged( SelectionChangeStatus );

protected:
    virtual void                 onSketchingStarted();
    virtual void                 onSketchingFinished();
    virtual void                 onTransformationStarted();
    virtual void                 onTransformationFinished();
    virtual void                 onSelectionModeChanged();

    virtual void                 unhilightDetected();
    virtual bool                 eventFilter( QObject*, QEvent* );

    /* virtual constructors */
    virtual GLViewer_ViewTransformer* createTransformer( int );
    virtual GLViewer_ViewSketcher*    createSketcher( int );
    virtual GLViewer_Selector*        createSelector();

    virtual void                 startOperations( QMouseEvent* ) {}
    virtual bool                 updateOperations( QMouseEvent* ) { return false; }
    virtual void                 finishOperations( QMouseEvent* ) {}
    virtual void                 startOperations( QWheelEvent* ) {}

protected slots:
    virtual void                 onKeyEvent( SUIT_ViewWindow*, QKeyEvent* );
    virtual void                 onMouseEvent( SUIT_ViewWindow*, QMouseEvent* );
    virtual void                 onWheelEvent( SUIT_ViewWindow*, QWheelEvent* );

    virtual void                 onSelectionCancel();
    virtual void                 onSelectionDone( bool add, SelectionChangeStatus status );

    virtual void                 onChangeBgColor();

private:
    void                         handleMouseMove( QMouseEvent* );
    void                         handleMousePress( QMouseEvent* );
    void                         handleMouseRelease( QMouseEvent* );
    void                         handleWheel( QWheelEvent* );

protected:
    GLViewer_Selector*           mySelector;        /* selector */
    SelectionMode                mySelMode;         /* current selection mode */
    GLViewer_ViewSketcher*       mySketcher;        /* sketch manipulator */
    GLViewer_ViewTransformer*    myTransformer;     /* transform manipulator */
};

class GLVIEWER_API GLViewer_ViewTransformer : public QObject
{
public:
    GLViewer_ViewTransformer( GLViewer_Viewer*, int type );
    ~GLViewer_ViewTransformer();

public:
    /*! Returns transformer type */
    int                          type() const;

    /*! Sets/returns acceleration key ( CTRL by default ) */
    static int                   accelKey() { return acccelKey; }
    static void                  setAccelKey( int k ) { acccelKey = k; }

    /*! Sets/returns mouse button used for zooming ( MB1 by default ) */
    static int                   zoomButton() { return zoomBtn; }
    static void                  setZoomButton( int b ) { zoomBtn = b; }

    /*! Sets/returns mouse button used for panning ( MB2 by default ) */
    static int                   panButton() { return panBtn; }
    static void                  setPanButton( int b ) { panBtn = b; }

    /*! Sets/returns mouse button used for global pan ( MB1 by default ) */
    static int                   panGlobalButton() { return panGlobalBtn; }
    static void                  setPanGlobalButton( int b ) { panGlobalBtn = b; }

    /*! Sets/returns mouse button used for fit area ( MB1 by default ) */
    static int                   fitRectButton() { return fitRectBtn; }
    static void                  setFitRectButton( int b ) { fitRectBtn = b; }

    virtual void                 exec();
    virtual bool                 eventFilter( QObject*, QEvent* );

protected:
    enum TransformState { Debut, EnTrain, Fin };
    virtual void                 onTransform( TransformState );
    void                         initTransform( bool );

    void                         drawRect(const QRect& theRect);
    void                         endDrawRect();

protected:
    static int                   panBtn;
    static int                   zoomBtn;
    static int                   fitRectBtn;
    static int                   panGlobalBtn;

    static int                   acccelKey;

    GLViewer_Viewer*             myViewer;
    int                          myType;
    QCursor                      mySavedCursor;
    bool                         mySavedMouseTrack;
    QPoint                       myStart, myCurr;
    int                          myButtonState;
    int                          myMajorBtn;

    QRubberBand*                 myRectBand; //!< selection rectangle rubber band
};

class GLVIEWER_API GLViewer_ViewSketcher : public QObject
{
public:
    GLViewer_ViewSketcher( GLViewer_Viewer*, int type );
    ~GLViewer_ViewSketcher();

public:
    /*! Returns sketcher type */
    int                          type() const { return myType; }

    /*! Returns result of sketching */
    void*                        data() const { return myData; }

    /*! Returns current state of mouse/sys kbd buttons */
    int                          buttonState() const { return myButtonState; }

    /*! Sets/returns mouse button used for sketching ( MB1 by default ) */
    static int                   sketchButton() { return sketchBtn; }
    static void                  setSketchButton( int b ) { sketchBtn = b; }

    virtual bool                 eventFilter( QObject*, QEvent* );

protected:
    enum SketchState { Debut, EnTrain, Fin };
    virtual void                 onSketch( SketchState );

    void                         drawRect(const QRect& theRect);
    void                         endDrawRect();

protected:
    static int                   sketchBtn;
    GLViewer_Viewer*             myViewer;
    int                          myType;
    void*                        myData;
    QCursor                      mySavedCursor;
    QPoint                       myStart, myCurr;
    int                          myButtonState;

    QRubberBand*                 myRectBand; //!< selection rectangle rubber band
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
