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

class QTX_EXPORT QtxResourceEdit
{
public:
  class Item;

  enum { Space, Bool, Color, String, List, RealSpin, IntegerSpin, RealEdit, IntegerEdit };

public:
  QtxResourceEdit( QtxResourceMgr* );
  virtual ~QtxResourceEdit();

  QtxResourceMgr*      resourceMgr() const;

  int                  addItem( const QString& label, const int pId = -1, const int = -1,
                                const QString& section = QString::null,
                                const QString& param = QString::null );

  QVariant             property( const int, const QString& ) const;
  void                 setProperty( const int, const QString&, const QVariant& );

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

private:
  void                 removeItem( Item* );
  Item*                createItem( const QString&, const int, const int );

private:
  typedef QMap<int, Item*> ItemMap;

private:
  ItemMap              myItems;
  QtxResourceMgr*      myResMgr;
  QMap<Item*, QString> myBackup;

  friend class QtxResourceEdit::Item;
};

/*
  Class: QtxResourceEditor::Item
  Descr: Class for incapsulation of one preference item
*/

class QtxResourceEdit::Item
{
public:
  Item( QtxResourceEdit*, Item* = 0 );
  virtual ~Item();

  int                  id() const;
  virtual int          type() const = 0;

  Item*                parentItem() const;
  void                 childItems( QPtrList<Item>& ) const;

  QString              title() const;
  void                 resource( QString&, QString& ) const;

  virtual void         setTitle( const QString& );
  virtual void         setResource( const QString&, const QString& );

  virtual void         update();

  QVariant             property( const QString& ) const;
  virtual void         setProperty( const QString&, const QVariant& );

  virtual void         store() = 0;
  virtual void         retrieve() = 0;

  virtual Item*        createItem( const QString&, const int ) = 0;

  QString              resourceValue() const;
  void                 setResourceValue( const QString& );

protected:
  QtxResourceMgr*      resourceMgr() const;
  QtxResourceEdit*     resourceEdit() const;

  int                  getInteger( const int = 0 ) const;
  double               getDouble( const double = 0.0 ) const;
  bool                 getBoolean( const bool = false ) const;
  QColor               getColor( const QColor& = QColor() ) const;
  QString              getString( const QString& = QString::null ) const;

  void                 setInteger( const int );
  void                 setDouble( const double );
  void                 setBoolean( const bool );
  void                 setColor( const QColor& );
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