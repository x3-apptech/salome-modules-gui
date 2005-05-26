// File:      GLViewer_Widget.h
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

/***************************************************************************
**  Class:   GLViewer_Widget
**  Descr:   OpenGL Widget for GLViewer
**  Module:  GLViewer
**  Created: UI team, 27.03.03
****************************************************************************/
#ifndef GLVIEWER_WIDGET_H
#define GLVIEWER_WIDGET_H

#include "GLViewer.h"

#include <qgl.h>
#include <qfile.h>

class GLViewer_ViewPort2d;
class GLViewer_CoordSystem;

class GLVIEWER_EXPORT GLViewer_Widget : public QGLWidget
{
  Q_OBJECT

public:
    GLViewer_Widget( QWidget*, const char* = 0 );
    ~GLViewer_Widget();

    GLViewer_ViewPort2d*   getViewPort() const { return myViewPort; }
    GLint                  getWidth() const { return myWidth; }
    GLint                  getHeight() const { return myHeight; }

    void                   getScale( GLfloat&, GLfloat&, GLfloat& );
    void                   setScale( GLfloat, GLfloat, GLfloat );

    void                   getPan( GLfloat&, GLfloat&, GLfloat& );
    void                   setPan( GLfloat, GLfloat, GLfloat );

    GLfloat                getRotationAngle() const { return myRotationAnglePrev; }
    void                   setRotationAngle( GLfloat a ) { myRotationAnglePrev = a; }
    void                   getRotationStart( GLfloat&, GLfloat&, GLfloat& );
    void                   setRotationStart( GLfloat, GLfloat, GLfloat );
    void                   getRotation( GLfloat&, GLfloat&, GLfloat&, GLfloat& );
    void                   setRotation( GLfloat, GLfloat, GLfloat, GLfloat );
    void                   setBackground( QString );

    void                   addToolTip( QString, QRect );
    void                   removeToolTip();

    void                   copyBuffers();
    virtual void           translateBackgroundToPS( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPSCS );

    void                   exportRepaint();

#ifdef WIN32
    virtual void           translateBackgroundToEMF( HDC dc, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aEMFCS );
#endif

private:
    void                   getBackgroundRectInViewerCS( double& left, double& top, double& right, double& bottom );

protected:
    virtual void           initializeGL();
    virtual void           paintGL();
    virtual void           resizeGL( int, int );

    virtual void           paintEvent( QPaintEvent* );
    virtual void           mouseMoveEvent( QMouseEvent* );
    virtual void           mousePressEvent( QMouseEvent* );
    virtual void           mouseReleaseEvent( QMouseEvent* );
    virtual void           enterEvent( QEvent* );
    virtual void           leaveEvent( QEvent* );

private:
    GLint                  myWidth;
    GLint                  myHeight;

    GLfloat                myXScale;
    GLfloat                myYScale;
    GLfloat                myZScale;

    GLfloat                myXPan;
    GLfloat                myYPan;
    GLfloat                myZPan;

    GLfloat                myRotationStartX;
    GLfloat                myRotationStartY;
    GLfloat                myRotationStartZ;
    GLfloat                myRotationAngle;
    GLfloat                myRotationCenterX;
    GLfloat                myRotationCenterY;
    GLfloat                myRotationCenterZ;
    GLfloat                myRotationAnglePrev;

    GLboolean              myStart;
    GLViewer_ViewPort2d*   myViewPort;

    //new code
    bool                   isLoadBackground;
    QString                myBackgroundFile;
    GLuint                 texName;
    int                    myIW;
    int                    myIH;
    int                    myBackgroundSize;

    //GLubyte***             pixels;
    QRect                  myToolTipRect;

    //for export repaint
    bool                   isExportMode;
};

#endif // GLVIEWER_WIDGET_H
