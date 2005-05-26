// File:      GLViewer_Selector.h
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

/****************************************************************************
**  Class:   GLViewer_Selector
**  Descr:   Base class for object selection in QAD-based application
**  Module:  GLViewer
**  Created: UI team, 22.09.00
*****************************************************************************/
#ifndef GLVIEWER_SELECTOR_H
#define GLVIEWER_SELECTOR_H

#include "GLViewer.h"
#include "GLViewer_Defs.h"

#include <qsize.h>
#include <qobject.h>

#include <Quantity_NameOfColor.hxx>

class GLViewer_Viewer;

class GLVIEWER_EXPORT GLViewer_Selector : public QObject
{
    Q_OBJECT

public:
    GLViewer_Selector( GLViewer_Viewer* );
    ~GLViewer_Selector();

public:
    void                  lock( bool );
    void                  setMinRectSize( const QSize& minSize );

    virtual void          detect( int, int ) = 0;
    virtual void          select( bool append = false ) = 0;
    virtual void          select( const QRect&, bool append = false ) = 0;
    virtual void          unselectAll() = 0;
    virtual int           numSelected() const = 0;

    virtual void          setHilightColor( Quantity_NameOfColor ) = 0;
    virtual void          setSelectColor( Quantity_NameOfColor ) = 0;

    virtual void          checkSelection( int, bool, int ) = 0;

    /*! Sets/returns the key for appending selected objects ( SHIFT by default ) */
    static int            appendKey() { return apppendKey; }
    static void           setAppendKey( int k ) { apppendKey = k; }

signals:
    void                  selSelectionCancel();
    void                  selSelectionDone( bool append, SelectionChangeStatus status );

protected:
    GLViewer_Viewer*      myViewer;
    bool                  myLocked;
    QSize                 myMinRectSize;

private:
    static int            apppendKey;
};

#endif
