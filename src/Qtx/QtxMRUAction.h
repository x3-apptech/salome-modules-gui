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

// File:      QtxMRUAction.h
// Author:    Sergey TELKOV
//
#ifndef QTXMRUACTION_H
#define QTXMRUACTION_H

#include "QtxAction.h"

#include <QStringList>

class QtxResourceMgr;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxMRUAction : public QtxAction
{
  Q_OBJECT

public:
  //! Items insertion policy
  typedef enum { MoveFirst,   //!< put the specified item to the beginning
                 MoveLast,    //!< put the specified item to the end
                 AddFirst,    //!< if specified item doesn't exist, add it to the beginning
                 AddLast      //!< if specified item doesn't exist, add it to the end
  } InsertionMode;

  typedef enum { LinkAuto,    //!< put the full path of link into the menu if link file names of severals link are same
                 LinkShort,   //!< put the only file name of link into the menu
                 LinkFull     //!< put the full path of link into the menu
  } LinkType;

public:
  QtxMRUAction( QObject* = 0 );
  QtxMRUAction( const QString&, const QString&, QObject* = 0 );
  QtxMRUAction( const QString&, const QIcon&, const QString&, QObject* = 0 );
  virtual ~QtxMRUAction();

  int          insertMode() const;
  void         setInsertMode( const int );

  int          linkType() const;
  void         setLinkType( const int );

  int          count() const;
  bool         isEmpty() const;

  int          visibleCount() const;
  void         setVisibleCount( const int );

  bool         isClearPossible() const;
  void         setClearPossible( const bool );

  int          historyCount() const;
  void         setHistoryCount( const int );

  void         remove( const int );
  void         remove( const QString& );
  void         insert( const QString& );

  QString      item( const int ) const;
  int          find( const QString& ) const;
  bool         contains( const QString& ) const;

  virtual void loadLinks( QtxResourceMgr*, const QString&, const bool = true );
  virtual void saveLinks( QtxResourceMgr*, const QString&, const bool = true ) const;

public slots:
  void         clear();

signals:
  void         activated( const QString& );

private slots:
  void         onActivated();
  void         onAboutToShow();
  void         onCleared( bool );

private:
  void         updateMenu();

private:
  QStringList  myLinks;        //!< most recent used items
  QAction*     myClear;        //!< clear item
  int          myVisCount;     //!< number of visible MRU items
  int          myHistoryCount; //!< number of stored MRU items
  int          myLinkType;     //!< type of link names in menu
  int          myInsertMode;   //!< items insertion policy
};

#endif
