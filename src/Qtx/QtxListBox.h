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

// File:      QtxListBox.h
// Author:    Sergey TELKOV
//
#ifndef QTXLISTBOX_H
#define QTXLISTBOX_H

#include "Qtx.h"

// This file isn't yet ported to Qt4 => there are some corrections for OCCViewer porting  -->
//#include <qlistbox.h>
#include <QListWidget>
#include <QListWidgetItem>
// <--

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QLineEdit;
class QValidator;

class QTX_EXPORT QtxListBox : public QListWidget//QListBox // This file isn't yet ported to Qt4 => there are some corrections for OCCViewer porting
{
    Q_OBJECT

public:
    QtxListBox( QWidget* = 0, const char* = 0, Qt::WindowFlags = 0 );
    virtual ~QtxListBox();

    bool              isEditEnabled() const;
    bool              defaultEditAction() const;
    bool              isModificationEnabled() const;

    QListWidgetItem*  editedItem() const;
    int               editedIndex() const;

    void              startEdition( const int );
    void              startEdition( const QListWidgetItem* );
    void              endEdition( const bool );

    void              ensureItemVisible( const int );
    void              ensureItemVisible( const QListWidgetItem* );

    virtual bool      eventFilter( QObject*, QEvent* );

    const QValidator* validator() const;

    void              moveItemToTop( const int );
    void              moveItemToBottom( const int );
    void              moveItem( const int, const int );

    void              createItem( const int = -1 );
    void              deleteItem( const int = -1 );

signals:
    void              itemEdited( int );
    void              itemEdited( QListWidgetItem* );
    void              itemMoved( int, int );

public slots:
    virtual void      setEditEnabled( bool );
    virtual void      setDefaultEditAction( bool );
    virtual void      setModificationEnabled( bool );

    virtual void      clearValidator();
    virtual void      setValidator( const QValidator* );
    virtual void      setContentsPos( int, int );

private slots:
    void              onContentsMoving( int, int );

protected:
    virtual void      keyPressEvent( QKeyEvent* );
    virtual void      viewportResizeEvent( QResizeEvent* );
    virtual void      mouseDoubleClickEvent( QMouseEvent* );

private:
    QLineEdit*        editor() const;
    void              createEditor();
    void              updateEditor();

private:
    QLineEdit*        myEditor;
    int               myEditIndex;
    bool              myEditState;
    bool              myEditDefault;
    bool              myModifEnabled;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
