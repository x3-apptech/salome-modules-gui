// File:      GLViewer_Viewer2d.h
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

/***************************************************************************
**  Class:   GLViewer_Viewer2d
**  Descr:   OpenGL Viewer 2D
**  Module:  GLViewer
**  Created: UI team, 04.09.02
****************************************************************************/
#ifndef GLVIEWER_VIEWER2D_H
#define GLVIEWER_VIEWER2D_H

#ifdef WNT
#include <windows.h>
#endif

#include <qmap.h>
#include <qrect.h>

#include "GLViewer_Viewer.h"
#include "GLViewer_ViewFrame.h"
#include "GLViewer_Drawer.h"

#include <TColStd_SequenceOfInteger.hxx>

#include <GL/gl.h>

class GLViewer_Object;
class GLViewer_Rect;
class GLViewer_Context;
class GLViewer_Selector2d;
//class GLViewer_Sketcher;

class SUIT_Desktop;
class SUIT_ViewWindow;

const double Sizes[2*5] = { 
    /* A1 */ 594.0, 840.0,
    /* A2 */ 420.0, 594.0,
    /* A3 */ 297.0, 420.0,
    /* A4 */ 210.0, 297.0,
    /* A5 */ 148.5, 210.0
};

#ifdef WNT
#pragma warning( disable:4251 )
#endif

class GLVIEWER_EXPORT GLViewer_Viewer2d : public GLViewer_Viewer
{
    Q_OBJECT

public:
    enum GLSketchingType { None, Polyline, Arc, Curve, Scribble, Oval, Rectangle };
    enum VectorFileType { POST_SCRIPT, HPGL
#ifdef WIN32
        , ENH_METAFILE
#endif
    };

    enum PaperType { A1=0, A2, A3, A4, A5 };

public:
    GLViewer_Viewer2d( const QString& title );
    ~GLViewer_Viewer2d();

public:
    SUIT_ViewWindow*     createView( SUIT_Desktop* );

    virtual void         contextMenuPopup( QPopupMenu* );

    //void                activateGLSketching( int );

    GLViewer_Context*    getGLContext() const { return myGLContext; }
    void                 updateColors( QColor colorH, QColor colorS );
    void                 updateBorders( const QRect& rect );
    void                 updateBorders();

    void                 updateAll();
    void                 updateDrawers( GLboolean update, GLfloat scX = 0.0, GLfloat scY = 0.0 );
    void                 activateDrawers( TColStd_SequenceOfInteger& list, bool onlyUpdate,
                                          GLboolean swap = GL_TRUE );
    void                 activateDrawers( QValueList<GLViewer_Object*>& theObjects, bool onlyUpdate, GLboolean swap = GL_FALSE );
    void                 activateDrawer( int index, bool onlyUpdate, GLboolean swap = GL_FALSE );
    void                 activateDrawer( GLViewer_Object*, bool onlyUpdate, GLboolean swap = GL_FALSE );
    void                 activateAllDrawers( bool onlyUpdate, GLboolean swap = GL_FALSE );

    void                 transPoint( GLfloat& x, GLfloat& y );
    QRect*               getWinObjectRect( GLViewer_Object* theObject );

    GLViewer_Rect        getGLVRect( const QRect& ) const;
    QRect                getQRect( const GLViewer_Rect& ) const;

    virtual void         insertHeader( VectorFileType aType, QFile& hFile );
    virtual void         insertEnding( VectorFileType aType, QFile& hFile );
    virtual bool         translateTo( VectorFileType aType, QString FileName, PaperType aPType, 
                                      double mmLeft, double mmRight, double mmTop, double mmBottom );
    
    //bool                 isSketchingActive();
    //int                  getSketchingType();

    //virtual void         startSketching();
    //virtual void         finishSketching();

    void                 repaintView( GLViewer_ViewFrame* theView = NULL, bool makeCurrent = false );

public slots:
    //void                 onSketchDelObject();
    //void                 onSketchUndoLast();
    //void                 onSketchFinish();
    void                 onChangeBgColor();
    void                 onCreateGLMarkers( int = 1000, int = 5 );
    void                 onCreateGLPolyline( int = 100, int = 10, int = 100 );
    void                 onCreateGLText( QString = "Text", int = 1 );

protected:
    GLViewer_Selector*        createSelector();
    GLViewer_ViewTransformer* createTransformer( int );

    void                 transformCoordsToPS( double& x, double& y );
    void                 transformCoordsToHPGL( double& x, double& y );

    virtual void         startOperations( QMouseEvent* );
    virtual bool         updateOperations( QMouseEvent* );
    virtual void         finishOperations( QMouseEvent* );
    virtual void         startOperations( QWheelEvent* );

protected slots:
    void                 onMouseEvent( SUIT_ViewWindow*, QMouseEvent* );
    void                 onDumpView();

private:
    bool                 testRotation( QMouseEvent* );

protected:    
    GLViewer_Context*    myGLContext;
    QValueList<GLViewer_Drawer*> myDrawers;

    //GLViewer_Sketcher*   myGLSketcher;
};

/****************************************************************
**  Class: GLViewer_View2dTransformer
**
*****************************************************************/
class GLVIEWER_EXPORT GLViewer_View2dTransformer : public GLViewer_ViewTransformer
{
public:
    GLViewer_View2dTransformer( GLViewer_Viewer*, int );
    ~GLViewer_View2dTransformer();

public:
    virtual void         exec();

    /*! Sets/returns mouse butto which will be used for rotation ( MB1 by default ) */
    static int           rotateButton() { return rotateBtn; }
    static void          setRotateButton( int b ) { rotateBtn = b; }

protected:
    void                 onTransform( TransformState );

protected:
    static  int          rotateBtn;
};

#ifdef WNT
#pragma warning ( default:4251 )
#endif

#endif
