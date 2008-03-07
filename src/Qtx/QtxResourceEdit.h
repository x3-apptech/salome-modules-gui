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
// File:      QtxResourceEdit.h
// Author:    Sergey TELKOV

#ifndef QTXRESOURCEEDIT_H
#define QTXRESOURCEEDIT_H

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

#include "Qtx.h"

class QString;
class QtxResourceMgr;

#include <qcolor.h>
#include <qvariant.h>
#include <qfont.h>

/*!
  \class QtxResourceEdit
  Class for managing preferences items
*/
class QTX_EXPORT QtxResourceEdit
{
public:
  class Item;

public:
  QtxResourceEdit( QtxResourceMgr* );
  virtual ~QtxResourceEdit();

  QtxResourceMgr*      resourceMgr() const;

  virtual int          addItem( const QString& label, const int pId = -1, const int = -1,
                                const QString& section = QString::null,
                                const QString& param = QString::null );

  QVariant             itemProperty( const int, const QString& ) const;
  virtual void         setItemProperty( const int, const QString&, const QVariant& );

  void                 resource( const int, QString&, QString& ) const;

  virtual void         store();
  virtual void         retrieve();

  virtual void         update();

  virtual void         toBackup();
  virtual void         fromBackup();

protected:
  Item*                item( const int ) const;
  Item*                item( const QString& ) const;
  Item*                item( const QString&, const int ) const;

  virtual Item*        createItem( const QString&, const int ) = 0;

  void                 resourceValues( QMap<int, QString>& ) const;
  void                 resourceValues( QMap<Item*, QString>& ) const;

  void                 setResourceValues( QMap<int, QString>& ) const;
  void                 setResourceValues( QMap<Item*, QString>& ) const;
  
  void                 differentValues( const QMap<int, QString>&, const QMap<int, QString>&,
                                        QMap<int, QString>&, const bool fromFirst = false ) const;
  void                 differentValues( const QMap<Item*, QString>&, const QMap<Item*, QString>&,
                                        QMap<Item*, QString>&, const bool fromFirst = false ) const;

  virtual void         changedResources( const QMap<Item*, QString>& );

  virtual void         itemAdded( Item* );
  virtual void         itemRemoved( Item* );

  void                 childItems( QPtrList<Item>& ) const;

private:
  void                 removeItem( Item* );
  Item*                createItem( const QString&, const int, const int );

private:
  typedef QMap<int, Item*> ItemMap;

private:
  ItemMap              myItems;
  QtxResourceMgr*      myResMgr;
  QMap<Item*, QString> myBackup;
  QPtrList<Item>       myChildren;

  friend class QtxResourceEdit::Item;
};

/*!
  \class QtxResourceEditor::Item
  Class for incapsulation of one preference item
*/

class QTX_EXPORT QtxResourceEdit::Item
{
public:
  Item( QtxResourceEdit*, Item* = 0 );
  virtual ~Item();

  int                  id() const;
  virtual int          type() const = 0;

  Item*                parentItem() const;
  void                 childItems( QPtrList<Item>& ) const;

  virtual bool         isEmpty() const;

  QString              title() const;
  void                 resource( QString&, QString& ) const;

  virtual void         setTitle( const QString& );
  virtual void         setResource( const QString&, const QString& );

  virtual void         update();

  virtual QVariant     property( const QString& ) const;
  virtual void         setProperty( const QString&, const QVariant& );

  virtual void         store() = 0;
  virtual void         retrieve() = 0;

  virtual Item*        createItem( const QString&, const int ) = 0;

  QString              resourceValue() const;
  void                 setResourceValue( const QString& );

  virtual void         insertChild( Item* );
  virtual void         removeChild( Item* );

protected:
  QtxResourceMgr*      resourceMgr() const;
  QtxResourceEdit*     resourceEdit() const;

  int                  getInteger( const int = 0 ) const;
  double               getDouble( const double = 0.0 ) const;
  bool                 getBoolean( const bool = false ) const;
  QColor               getColor( const QColor& = QColor() ) const;
  QFont                getFont( const QFont& = QFont() ) const;
  QString              getString( const QString& = QString::null ) const;

  void                 setInteger( const int );
  void                 setDouble( const double );
  void                 setBoolean( const bool );
  void                 setColor( const QColor& );
  void                 setFont( const QFont& );  
  void                 setString( const QString& );

  Item*                item( const int ) const;
  Item*                item( const QString& ) const;
  Item*                item( const QString&, const int ) const;

protected:
  typedef QPtrList<Item>         ItemList;
  typedef QPtrListIterator<Item> ItemListIterator;

private:
  static int           generateId();

private:
  int                  myId;
  Item*                myParent;
  ItemList             myChildren;

  QString              myTitle;
  QString              myResSection;
  QString              myResParameter;

  QtxResourceEdit*     myEdit;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
