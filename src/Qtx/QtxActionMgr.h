// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
// File:      QtxActionMgr.h
// Author:    Alexander SOLOVYEV, Sergey TELKOV

#ifndef QTXACTIONMGR_H
#define QTXACTIONMGR_H

#include "Qtx.h"

#include <qmap.h>
#include <qobject.h>
#include <qguardedptr.h>

class QAction;
class QDomNode;


#ifdef WIN32
#pragma warning( disable:4251 )
#endif


/*!
  \class QtxActionMgr
  Contains set of actions accessible by id.
  Base class for menu, popup creators and other action containers.
*/
class QTX_EXPORT QtxActionMgr : public QObject
{
  Q_OBJECT 

  class SeparatorAction;

public:
  class Reader;
  class XMLReader;

protected:
  class Creator;

public:
  QtxActionMgr( QObject* parent );
  virtual ~QtxActionMgr();

  virtual int      registerAction( QAction*, const int = -1 );
  virtual void     unRegisterAction( const int );

  QAction*         action( const int ) const;
  int              actionId( const QAction* ) const;
  bool             contains( const int ) const;

  int              count() const;
  bool             isEmpty() const;
  void             idList( QIntList& ) const;

  bool             isUpdatesEnabled() const;
  virtual void     setUpdatesEnabled( const bool );

  virtual bool     isVisible( const int, const int ) const;
  virtual void     setVisible( const int, const int, const bool );

  void             update();

  virtual bool     isEnabled( const int ) const;
  virtual void     setEnabled( const int, const bool );

  static QAction*  separator( const bool = false );

protected:
  virtual void     internalUpdate();
  int              generateId() const;

private:
  typedef QGuardedPtr<QAction> ActionPtr;
  typedef QMap<int, ActionPtr> ActionMap;

private:
  bool             myUpdate;
  ActionMap        myActions;
};


QTX_EXPORT typedef QMap<QString, QString> ItemAttributes;

/*!
  \class QtxActionMgr::Creator
  Allows to fill automatically action manager with actions created by data from file
*/
class QtxActionMgr::Creator
{
public:
  Creator( QtxActionMgr::Reader* );
  virtual ~Creator();

  Reader* reader() const;

  virtual int append( const QString&, const bool,
                      const ItemAttributes&, const int ) = 0;
  virtual void connect( QAction* ) const;

  virtual bool loadPixmap( const QString&, QPixmap& ) const;

protected:
  static int     intValue( const ItemAttributes&, const QString&, const int );
  static QString strValue( const ItemAttributes&, const QString&,
                                      const QString& = QString::null );
private:
  QtxActionMgr::Reader*  myReader;
};

/*!
  \class QtxActionMgr::Reader
  This class is used to read files of some format
  to create actions and to fill action manager automatically
*/
class QtxActionMgr::Reader
{
public:
  QTX_EXPORT Reader();
  QTX_EXPORT virtual ~Reader();

  QTX_EXPORT QStringList  options() const;
  QTX_EXPORT QString      option( const QString&, const QString& = QString::null ) const;
  QTX_EXPORT void         setOption( const QString&, const QString& );

  QTX_EXPORT virtual bool read( const QString&, Creator& ) const = 0;

private:
  QMap< QString, QString > myOptions;
};

/*!
  \class QtxActionMgr::Reader
  This class is used to read files of XML format
  to create actions and to fill action manager automatically
*/
class QtxActionMgr::XMLReader : public Reader
{
public:
  QTX_EXPORT XMLReader( const QString&, const QString&, const QString& );
  QTX_EXPORT virtual ~XMLReader();

  QTX_EXPORT virtual bool read( const QString&, Creator& ) const;

protected:
  QTX_EXPORT virtual void read( const QDomNode&, const int, Creator& ) const;
  QTX_EXPORT virtual bool isNodeSimilar( const QDomNode&, const QString& ) const;
};


#endif
