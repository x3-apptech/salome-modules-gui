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

// File:      QtxActionToolMgr.h
// Author:    Alexander SOLOVYOV, Sergey TELKOV
//
#ifndef QTXACTIONTOOLMGR_H
#define QTXACTIONTOOLMGR_H

#include "Qtx.h"
#include "QtxActionMgr.h"

#include <QMap>
#include <QList>

class QToolBar;
class QMainWindow;
class QAction;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxActionToolMgr : public QtxActionMgr
{
  Q_OBJECT

  class ToolNode
  {
  public:
    ToolNode() : id( -1 ), visible( true ) {};
    ToolNode( const int _id ) : id( _id ), visible( true ) {};

    int       id;          //!< tool node ID
    bool      visible;     //!< visibility status
  };

  typedef QList<ToolNode> NodeList;    //!< toolbar nodes list

protected:
  class ToolCreator;

public:
  QtxActionToolMgr( QMainWindow* );
  virtual ~QtxActionToolMgr();

  QMainWindow*    mainWindow() const;

  int             createToolBar( const QString&, int = -1, QMainWindow* = 0, bool = true );
  int             createToolBar( const QString&, const QString&, int = -1, QMainWindow* = 0, bool = true );
  int             createToolBar( const QString&, bool, Qt::ToolBarAreas = Qt::AllToolBarAreas, 
                                 int = -1, QMainWindow* = 0, bool = true );
  int             createToolBar( const QString&, const QString&, bool, Qt::ToolBarAreas = Qt::AllToolBarAreas, 
                                 int = -1, QMainWindow* = 0, bool = true );
  void            removeToolBar( const QString& );
  void            removeToolBar( const int );

  int             insert( const int, const int, const int = -1 );
  int             insert( QAction*, const int, const int = -1 );
  int             insert( const int, const QString&, const int = -1 );
  int             insert( QAction*, const QString&, const int = -1 );

  int             append( const int, const int );
  int             append( QAction*, const int );
  int             append( const int, const QString& );
  int             append( QAction*, const QString& );

  int             prepend( const int, const int );
  int             prepend( QAction*, const int );
  int             prepend( const int, const QString& );
  int             prepend( QAction*, const QString& );

  virtual bool    isVisible( const int, const int ) const;
  virtual void    setVisible( const int, const int, const bool );

  void            show( const int );
  void            hide( const int );
  bool            isShown( const int ) const;
  void            setShown( const int, const bool );

  void            remove( const int, const int );
  void            remove( const int, const QString& );

  QToolBar*       toolBar( const int ) const;
  QToolBar*       toolBar( const QString& ) const;
  QIntList        toolBarsIds() const;
  
  bool            hasToolBar( const int ) const;
  bool            hasToolBar( const QString& ) const;

  bool            containsAction( const int, const int = -1 ) const;
  int             index( const int, const int ) const;

  virtual bool    load( const QString&, QtxActionMgr::Reader& );

  int             find( QToolBar* ) const;

protected slots:
  void            onToolBarDestroyed();

protected:
  int             find( const QString& ) const;
  QToolBar*       find( const QString&, QMainWindow* ) const;

  virtual void    internalUpdate();
  void            updateToolBar( const int );

  virtual void    updateContent();

private:
  void            simplifySeparators( QToolBar* );
  void            triggerUpdate( const int );

private:
  typedef struct { NodeList nodes; QToolBar* toolBar; } ToolBarInfo;   //!< toolbar info
  typedef QMap<int, ToolBarInfo>                        ToolBarMap;    //!< toolbars map

private:
  ToolBarMap      myToolBars;      //!< toobars map
  QMainWindow*    myMainWindow;    //!< parent main window
  QMap<int,int>   myUpdateIds;     //!< list of actions ID being updated
};

class QtxActionToolMgr::ToolCreator : public QtxActionMgr::Creator
{
public:
  ToolCreator( QtxActionMgr::Reader*, QtxActionToolMgr* );
  virtual ~ToolCreator();

  virtual int append( const QString&, const bool,
                      const ItemAttributes&, const int );

private:
  QtxActionToolMgr* myMgr;         //!< toolbar manager
};

#endif
