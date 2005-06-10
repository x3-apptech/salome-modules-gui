// File:      QtxListResourceEdit.h
// Author:    Sergey TELKOV

#ifndef QTXLISTRESOURCEEDIT_H
#define QTXLISTRESOURCEEDIT_H

#include "QtxResourceEdit.h"

#include <qmap.h>
#include <qhbox.h>
#include <qframe.h>
#include <qgroupbox.h>

class QLabel;
class QListBox;
class QLineEdit;
class QCheckBox;
class QComboBox;
class QTabWidget;
class QWidgetStack;

class QtxIntSpinBox;
class QtxDblSpinBox;

/*
  Class: QtxListResourceEdit
  Descr: GUI implementation of QtxResourceEdit - manager of resources
*/

class QTX_EXPORT QtxListResourceEdit : public QFrame, public QtxResourceEdit
{
  Q_OBJECT

public:
  class Tab;
  class Group;
  class Category;
  class PrefItem;

  class Spacer;
  class ListItem;
  class ColorItem;
  class StateItem;
  class StringItem;
  class DoubleSpinItem;
  class DoubleEditItem;
  class IntegerSpinItem;
  class IntegerEditItem;

public:
  QtxListResourceEdit( QtxResourceMgr*, QWidget* = 0 );
  virtual ~QtxListResourceEdit();

signals:
  void          resourceChanged( int );
  void          resourceChanged( QString&, QString& );
  void          resourcesChanged( const QMap<int, QString>& );

private slots:
  void          onSelectionChanged();

protected:
  virtual Item* createItem( const QString&, const int );
  virtual void  changedResources( const QMap<Item*, QString>& );

private:
  void          updateState();

private:
  QListBox*     myList;
  QWidgetStack* myStack;
};

/*
  Class: QtxListResourceEdit::Category
  Descr: GUI implementation of 'Category' frame
*/

class QtxListResourceEdit::Category : public QFrame, public Item
{
public:
  Category( QtxListResourceEdit*, QWidget* = 0 );
  virtual ~Category();

  virtual int      type() const;
  virtual void     store();
  virtual void     retrieve();

  virtual QVariant property( const QString& ) const;
  virtual void     setProperty( const QString&, const QVariant& );

protected:
  virtual Item*    createItem( const QString&, const int );

private:
  void             updateState();

private:
  QLabel*          myInfo;
  QTabWidget*      myTabs;
};

/*
  Class: QtxListResourceEdit::Tab
  Descr: GUI implementation of resources tab.
*/

class QtxListResourceEdit::Tab : public QFrame, public Item
{
public:
  Tab( QtxResourceEdit*, Item*, QWidget* = 0 );
  virtual ~Tab();

  virtual int   type() const;
  virtual void  store();
  virtual void  retrieve();

public:
  virtual void  polish();

protected:
  virtual Item* createItem( const QString&, const int );

private:
  void          adjustLabels();

private:
  QGroupBox*    myMainFrame;
};

/*
  Class: QtxListResourceEdit::Group
  Descr: GUI implementation of resources group.
*/

class QtxListResourceEdit::Group : public QGroupBox, public Item
{
public:
  Group( const QString&, QtxResourceEdit*, Item*, QWidget* = 0 );
  virtual ~Group();

  virtual int      type() const;
  virtual void     store();
  virtual void     retrieve();

  virtual QVariant property( const QString& ) const;
  virtual void     setProperty( const QString&, const QVariant& );

  virtual void     setTitle( const QString& );

protected:
  virtual Item*    createItem( const QString&, const int );
};

/*
  Class: QtxListResourceEdit::PrefItem
  Descr: Base class for preferences items.
*/

class QtxListResourceEdit::PrefItem : public QHBox, public Item
{
public:
  PrefItem( const int, QtxResourceEdit*, Item* = 0, QWidget* = 0 );
  virtual ~PrefItem();

  virtual int   type() const;

protected:
  virtual Item* createItem( const QString&, const int );

private:
  int           myType;
};

/*
  Class: QtxListResourceEdit::Spacer
  Descr: GUI implementation of resources spacer.
*/

class QtxListResourceEdit::Spacer : public PrefItem
{
public:
  Spacer( QtxResourceEdit*, Item*, QWidget* = 0 );
  virtual ~Spacer();

  virtual void store();
  virtual void retrieve();
};

/*
  Class: QtxListResourceEdit::ListItem
  Descr: GUI implementation of resources list item.
*/

class QtxListResourceEdit::ListItem : public PrefItem
{
public:
  ListItem( const QString&, QtxResourceEdit*, Item*, QWidget* = 0 );
  virtual ~ListItem();

  virtual void     store();
  virtual void     retrieve();

  virtual QVariant property( const QString& ) const;
  virtual void     setProperty( const QString&, const QVariant& );

private:
  void             setStrings( const QVariant& );
  void             setIndexes( const QVariant& );

  void             setStrings( const QStringList& );
  void             setIndexes( const QValueList<int>& );

private:
  QComboBox*       myList;
  QMap<int, int>   myIndex;
};

/*
  Class: QtxListResourceEdit::StateItem
  Descr: GUI implementation of resources bool item.
*/

class QtxListResourceEdit::StateItem : public PrefItem
{
public:
  StateItem( const QString&, QtxResourceEdit*, Item*, QWidget* = 0 );
  virtual ~StateItem();

  virtual void     store();
  virtual void     retrieve();

private:
  QCheckBox*       myState;
};

/*
  Class: QtxListResourceEdit::StringItem
  Descr: GUI implementation of resources string item.
*/

class QtxListResourceEdit::StringItem : public PrefItem
{
public:
  StringItem( const QString&, QtxResourceEdit*, Item*, QWidget* = 0 );
  virtual ~StringItem();

  virtual void     store();
  virtual void     retrieve();

private:
  QLineEdit*       myString;
};

/*
  Class: QtxListResourceEdit::IntegerEditItem
  Descr: GUI implementation of resources integer item.
*/

class QtxListResourceEdit::IntegerEditItem : public PrefItem
{
public:
  IntegerEditItem( const QString&, QtxResourceEdit*, Item*, QWidget* = 0 );
  virtual ~IntegerEditItem();

  virtual void     store();
  virtual void     retrieve();

private:
  QLineEdit*       myInteger;
};

/*
  Class: QtxListResourceEdit::IntegerSpinItem
  Descr: GUI implementation of resources integer item.
*/

class QtxListResourceEdit::IntegerSpinItem : public PrefItem
{
public:
  IntegerSpinItem( const QString&, QtxResourceEdit*, Item*, QWidget* = 0 );
  virtual ~IntegerSpinItem();

  virtual void     store();
  virtual void     retrieve();

  virtual QVariant property( const QString& ) const;
  virtual void     setProperty( const QString&, const QVariant& );

private:
  QtxIntSpinBox*  myInteger;
};

/*
  Class: QtxListResourceEdit::DoubleEditItem
  Descr: GUI implementation of resources double item.
*/

class QtxListResourceEdit::DoubleEditItem : public PrefItem
{
public:
  DoubleEditItem( const QString&, QtxResourceEdit*, Item*, QWidget* = 0 );
  virtual ~DoubleEditItem();

  virtual void     store();
  virtual void     retrieve();

private:
  QLineEdit*       myDouble;
};

/*
  Class: QtxListResourceEdit::DoubleSpinItem
  Descr: GUI implementation of resources double item.
*/

class QtxListResourceEdit::DoubleSpinItem : public PrefItem
{
public:
  DoubleSpinItem( const QString&, QtxResourceEdit*, Item*, QWidget* = 0 );
  virtual ~DoubleSpinItem();

  virtual void     store();
  virtual void     retrieve();

  virtual QVariant property( const QString& ) const;
  virtual void     setProperty( const QString&, const QVariant& );

private:
  QtxDblSpinBox*   myDouble;
};

/*
  Class: QtxListResourceEdit::ColorItem
  Descr: GUI implementation of resources color item.
*/

class QtxListResourceEdit::ColorItem : public PrefItem
{
public:
  ColorItem( const QString&, QtxResourceEdit*, Item*, QWidget* = 0 );
  virtual ~ColorItem();

  virtual void     store();
  virtual void     retrieve();

private:
  QWidget*         myColor;
};

#endif
