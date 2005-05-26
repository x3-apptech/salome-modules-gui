// File:      QtxResourceEditor.h
// Created:   
// Author:    Sergey TELKOV
// Copyright: 

#ifndef QTXPREFERENCESEDITOR_H
#define QTXPREFERENCESEDITOR_H

#ifdef WIN32
#pragma warning( disable:4251 )
#endif


#include "Qtx.h"
#include "QtxResourceMgr.h"


class QString;

#include <qlist.h>
#include <qstringlist.h>

/*
  Class: QtxResourceEditor
  Descr: 
*/

class QTX_EXPORT QtxResourceEditor
{
public:

  class Tab;
  class Item;
  class Group;
  class Category;

  enum { Space, Bool, Color, String, List, RealSpin, IntegerSpin, RealEdit, IntegerEdit };

public:
  QtxResourceEditor( QtxResourceMgr* );
  virtual ~QtxResourceEditor();

  int                  Add( const QString&, const QString&, const QString&,
                            const QString&, const QString&, const int, const QString& );

  void                 SetList( const int, const QValueList<int>&, const QStringList& );
  void                 SetRange( const int, const double, const double, const double = 3, const double = -1 );
  bool                 IsExist( const int, QString&, QString&, QString& ) const;

  void                 SetColumns( const QString&, const QString&, const QString&, const int );

  void                 SetTitle( const QString&, const QString& );
  void                 SetTitle( const QString&, const QString&, const QString& );
  void                 SetTitle( const QString&, const QString&, const QString&, const QString& );

  void                 Store();
  void                 Retrieve();

  static int           GenerateId();

  virtual void         Update();

  QtxResourceMgr*      resMgr() const;

protected:
  typedef QPtrList<Category> CategoryList;

protected:
  virtual Category*    createCategory( const QString& ) = 0;
  Category*            getCategory( const QString& ) const;
  const CategoryList&  categories() const;

  //typedef QPtrList<QtxResourceMgr>   ResMgrList;

private:
  QtxResourceMgr*      myResMgr;
  CategoryList         myCategories;
};

/*
  Class: QtxResourceEditor::Item
  Descr: Class for incapsulation of one preference item
*/

class QtxResourceEditor::Item
{
public:
  Item( QtxResourceEditor* );
  virtual ~Item();

  QString              Get( QString&, int&, QString& ) const;
  virtual void         Set( const QString&, const QString&, const int, const QString& );
  virtual void         Update();

  int                  GetId() const;
  int                  GetType() const;

  virtual void         SetList( const QValueList<int>&, const QStringList& );
  virtual void         SetRange( const double, const double, const double = 3, const double = -1 );

  virtual void         Store() = 0;
  virtual void         Retrieve() = 0;

  bool                 GetInteger( const QString&, const QString&, int& ) const;
  bool                 GetBoolean( const QString&, const QString&, bool& ) const;
  bool                 GetDouble( const QString&, const QString&, double& ) const;
  bool                 GetString( const QString&, const QString&, QString& ) const;
  bool                 GetColor( const QString&, const QString&, QColor& ) const;
  void                 SetInteger( const QString&, const QString&, const int );
  void                 SetBoolean( const QString&, const QString&, const bool );
  void                 SetDouble( const QString&, const QString&, const double );
  void                 SetString( const QString&, const QString&, const QString& );
  void                 SetColor( const QString&, const QString&, const QColor& );

private:
  QtxResourceMgr*      resMgr() const;

private:
  int                  myId, myType;
  QString              myName, myLabel, mySection;
  QtxResourceEditor*   myEditor;
};

/*
  Class: QtxResourceEditor::Group
  Descr: Class for incapsulation of one group of preferences (container of items)
*/

class QtxResourceEditor::Group
{
public:
  Group( const QString& );
  virtual ~Group();

  QString              GetName() const;
  virtual void         SetName( const QString& );

  virtual void         SetColumns( const int );
  virtual void         SetTitle( const QString& );
  virtual void         Update();

  int                  Add( const QString&, const QString&, const int, const QString& );

  void                 SetList( const int, const QValueList<int>&, const QStringList& );
  void                 SetRange( const int, const double, const double, const double = 3, const double = -1 );

  bool                 IsExist( const int ) const;

  void                 Store();
  void                 Retrieve();

protected:
  virtual Item*        createItem( const int, const QString& = QString::null ) = 0;
  Item*                getItem( const int ) const;

private:
  QString              myName;
  QPtrList<Item>       myItems;
};

/*
  Class: QtxResourceEditor::Tab
  Descr: Class for incapsulation of one set of preferences (container of groups)
*/

class QtxResourceEditor::Tab
{
public:
  Tab( const QString& );
  virtual ~Tab();

  QString              GetName() const;
  int                  Add( const QString&, const QString&,
                            const QString&, const int, const QString& );

  virtual void         SetTitle( const QString& );
  void                 SetTitle( const QString&, const QString& );

  void                 SetColumns( const QString&, const int );

  void                 SetList( const int, const QValueList<int>&, const QStringList& );
  void                 SetRange( const int, const double, const double, const double = 3, const double = -1 );

  bool                 IsExist( const int, QString& ) const;

  void                 Store();
  void                 Retrieve();

  virtual void         Update();

protected:
  virtual Group*       createGroup( const QString& ) = 0;
  Group*               getGroup( const QString& ) const;

private:
  QString              myName;
  QPtrList<Group>      myGroups;
};

/*
  Class: QtxResourceEditor::Category
  Descr: Class for incapsulation of one set of tabs
*/

class QtxResourceEditor::Category
{
public:
  Category( const QString& );
  virtual ~Category();

  QString              GetName() const;
  int                  Add( const QString&, const QString&, const QString&,
                            const QString&, const int, const QString& );

  virtual void         SetTitle( const QString& );
  void                 SetTitle( const QString&, const QString& );
  void                 SetTitle( const QString&, const QString&, const QString& );

  void                 SetColumns( const QString&, const QString&, const int );

  void                 SetList( const int, const QValueList<int>&, const QStringList& );
  void                 SetRange( const int, const double, const double, const double = 3, const double = -1 );

  bool                 IsExist( const int, QString&, QString& ) const;

  void                 Store();
  void                 Retrieve();

  virtual void         Update();

protected:
  virtual Tab*         createTab( const QString& ) = 0;
  Tab*                 getTab( const QString& ) const;

private:
  QString              myName;
  QPtrList<Tab>        myTabs;
};

#ifdef WNT
#pragma warning( default:4251 )
#endif

#endif
