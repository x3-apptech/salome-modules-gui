// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File:      QtxPreferenceMgr.h
// Author:    Sergey TELKOV
//
#ifndef QTXPREFERENCEMGR_H
#define QTXPREFERENCEMGR_H

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

#include "Qtx.h"

#include <QFont>
#include <QIcon>
#include <QColor>
#include <QVariant>

class QtxResourceMgr;
class QtxPreferenceMgr;

class QTX_EXPORT QtxPreferenceItem
{
  class Updater;

public:
  QtxPreferenceItem( QtxPreferenceItem* = 0 );
  QtxPreferenceItem( const QString&, QtxPreferenceItem* );
  QtxPreferenceItem( const QString&, const QString&, const QString&, QtxPreferenceItem* );
  virtual ~QtxPreferenceItem();

  int                       id() const;
  virtual int               rtti() const;

  QtxPreferenceItem*        rootItem() const;
  QtxPreferenceItem*        parentItem() const;
  QList<QtxPreferenceItem*> childItems( const bool = false ) const;

  int                       depth() const;
  int                       count() const;
  virtual bool              isEmpty() const;

  void                      appendItem( QtxPreferenceItem* );
  void                      removeItem( QtxPreferenceItem* );
  void                      insertItem( QtxPreferenceItem*, QtxPreferenceItem* = 0 );

  QIcon                     icon() const;
  QString                   title() const;
  void                      resource( QString&, QString& ) const;

  virtual void              setIcon( const QIcon& );
  virtual void              setTitle( const QString& );
  virtual void              setResource( const QString&, const QString& );

  virtual void              updateContents();

  QVariant                  option( const QString& ) const;
  void                      setOption( const QString&, const QVariant& );

  bool                      isEvaluateValues() const;
  void                      setEvaluateValues( const bool );

  bool                      isRestartRequired() const;
  void                      setRestartRequired( const bool );

  virtual void              store() = 0;
  virtual void              retrieve() = 0;

  QString                   resourceValue() const;
  void                      setResourceValue( const QString& );

  QtxPreferenceItem*        findItem( const int, const bool = false ) const;
  QtxPreferenceItem*        findItem( const QString&, const bool = false ) const;
  QtxPreferenceItem*        findItem( const QString&, const int, const bool = false ) const;

  virtual QtxResourceMgr*   resourceMgr() const;
  virtual QtxPreferenceMgr* preferenceMgr() const;

  virtual void              activate();
  void                      ensureVisible();

  static int                RTTI();

protected:
  int                       getInteger( const int = 0 ) const;
  double                    getDouble( const double = 0.0 ) const;
  bool                      getBoolean( const bool = false ) const;
  QColor                    getColor( const QColor& = QColor() ) const;
  QFont                     getFont( const QFont& = QFont() ) const;
  QString                   getString( const QString& = QString() ) const;

  void                      setInteger( const int );
  void                      setDouble( const double );
  void                      setBoolean( const bool );
  void                      setColor( const QColor& );
  void                      setFont( const QFont& );
  void                      setString( const QString& );

  virtual void              itemAdded( QtxPreferenceItem* );
  virtual void              itemRemoved( QtxPreferenceItem* );
  virtual void              itemChanged( QtxPreferenceItem* );

  virtual void              ensureVisible( QtxPreferenceItem* );

  void                      sendItemChanges();

  virtual void              triggerUpdate();

  virtual QVariant          optionValue( const QString& ) const;
  virtual void              setOptionValue( const QString&, const QVariant& );

protected:
  typedef QList<QtxPreferenceItem*> ItemList;

private:
  static int                generateId();

private:
  int                       myId;
  QtxPreferenceItem*        myParent;
  ItemList                  myChildren;

  bool                      myEval;
  bool                      myRestartNeeded;
  QIcon                     myIcon;
  QString                   myTitle;
  QString                   mySection;
  QString                   myParameter;
};

class QTX_EXPORT QtxPreferenceMgr : public QtxPreferenceItem
{
public:
  QtxPreferenceMgr( QtxResourceMgr* );
  virtual ~QtxPreferenceMgr();

  virtual QtxResourceMgr*   resourceMgr() const;
  virtual QtxPreferenceMgr* preferenceMgr() const;

  QVariant                  option( const int, const QString& ) const;
  void                      setOption( const int, const QString&, const QVariant& );

  virtual void              store();
  virtual void              retrieve();

  virtual void              update();

  virtual void              toBackup();
  virtual void              fromBackup();

protected:
  typedef QMap<QtxPreferenceItem*, QString> ResourceMap;

  void                      resourceValues( QMap<int, QString>& ) const;
  void                      resourceValues( ResourceMap& ) const;

  void                      setResourceValues( QMap<int, QString>& ) const;
  void                      setResourceValues( ResourceMap& ) const;

  void                      differentValues( const QMap<int, QString>&, const QMap<int, QString>&,
                                             QMap<int, QString>&, const bool fromFirst = false ) const;
  void                      differentValues( const ResourceMap&, const ResourceMap&,
                                             ResourceMap&, const bool fromFirst = false ) const;

  virtual void              changedResources( const ResourceMap& );

private:
  QtxResourceMgr*           myResMgr;
  ResourceMap               myBackup;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
