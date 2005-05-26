// File:      SalomeApp_ResourceEditor.h
// Created:   
// Author:    Alexander Sladkov
// Copyright: 

#ifndef SALOMEAPP_RESOURCEEDITOR_H
#define SALOMEAPP_RESOURCEEDITOR_H

#include <qmap.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qtabwidget.h>
#include <qpushbutton.h>

class QLabel;
class QListBox;
class QSpinBox;
class QLineEdit;
class QCheckBox;
class QComboBox;
class QWidgetStack;

class QtxDblSpinBox;

/*
  Class: SalomeApp_ResourceEditor
  Descr: GUI implementation of QtxResourceEditor - manager of resources
*/

#include <SalomeApp.h>
#include <QtxDialog.h>
#include <QtxResourceEditor.h>

class SALOMEAPP_EXPORT SalomeApp_ResourceEditor :
  public QtxDialog, public QtxResourceEditor
{
  Q_OBJECT

public:
  class PrefTab;
  class PrefGroup;
  class PrefCategory;

  class Spacing;
  class ListItem;
  class ColorItem;
  class StateItem;
  class StringItem;
  class DoubleSpinItem;
  class DoubleEditItem;
  class DataDoubleEditItem;
  class IntegerSpinItem;
  class IntegerEditItem;

public:
  SalomeApp_ResourceEditor( QtxResourceMgr*, QWidget* = 0 );
  virtual ~SalomeApp_ResourceEditor();

  virtual void           show();
  virtual void           accept();
  virtual void           reject();

private slots:
  void                   onHelp();
  void                   onApply();
  //void                   onDefault();
  void                   onSelectionChanged();

protected:
  virtual Category*      createCategory( const QString& );
  virtual void           updateViewer();

  void                   setTabOrder( QObject*, ... );
  void                   setTabOrder( const QObjectList& );

private:
  QListBox*              mypCatList;
  QWidgetStack*          mypCatStack;
};

class SalomeApp_ResourceEditor::Spacing : public Item
{
public:
  Spacing( SalomeApp_ResourceEditor*, QWidget* = 0 );
  virtual ~Spacing();

  virtual void           Store();
  virtual void           Retrieve();
};

class SalomeApp_ResourceEditor::ListItem : public Item
{
public:
  ListItem( SalomeApp_ResourceEditor*, QWidget* = 0 );
  virtual ~ListItem();

  virtual void           Set( const QString&, const QString&, const int, const QString& );
  virtual void           SetList( const QValueList<int>&, const QStringList& );
  virtual void           Store();
  virtual void           Retrieve();

private:
  QLabel*                mypName;
  QComboBox*             mypList;
  QMap<int, int>         myIndex;
};


class SalomeApp_ResourceEditor::ColorItem : public Item
{
public:
  ColorItem( SalomeApp_ResourceEditor*, QWidget* = 0 );
  virtual ~ColorItem();

  virtual void           Set( const QString&, const QString&, const int, const QString& );
  virtual void           Store();
  virtual void           Retrieve();

private:
  QLabel*                mypName;
  QPushButton*           mypColor;
};


class SalomeApp_ResourceEditor::StateItem : public Item
{
public:
  StateItem( SalomeApp_ResourceEditor*, QWidget* = 0 );
  virtual ~StateItem();

  virtual void           Set( const QString&, const QString&, const int, const QString& );
  virtual void           Store();
  virtual void           Retrieve();

private:
  QLabel*                mypName;
  QCheckBox*             mypState;
};


class SalomeApp_ResourceEditor::StringItem : public Item
{
public:
  StringItem( SalomeApp_ResourceEditor*, QWidget* = 0 );
  virtual ~StringItem();

  virtual void           Set( const QString&, const QString&, const int, const QString& );
  virtual void           Store();
  virtual void           Retrieve();

private:
  QLabel*                mypName;
  QLineEdit*             mypString;
};


class SalomeApp_ResourceEditor::DoubleSpinItem : public Item
{
public:
  DoubleSpinItem( SalomeApp_ResourceEditor*, QWidget* = 0 );
  virtual ~DoubleSpinItem();

  virtual void           Set( const QString&, const QString&, const int, const QString& );
  virtual void           SetRange( const double, const double, const double = 3, const double = -1 );
  virtual void           Store();
  virtual void           Retrieve();

private:
  QLabel*                mypName;
  QtxDblSpinBox*         mypDouble;
};


class SalomeApp_ResourceEditor::DoubleEditItem : public Item
{
public:
  DoubleEditItem( SalomeApp_ResourceEditor*, QWidget* = 0);
  virtual ~DoubleEditItem();

  virtual void           Set( const QString&, const QString&, const int, const QString& );
  virtual void           Store();
  virtual void           Retrieve();

private:
  QLabel*                mypName;
  QLineEdit*             mypDouble;
};


class SalomeApp_ResourceEditor::IntegerSpinItem : public Item
{
public:
  IntegerSpinItem( SalomeApp_ResourceEditor*, QWidget* = 0 );
  virtual ~IntegerSpinItem();

  virtual void           Set( const QString&, const QString&, const int, const QString& );
  virtual void           SetRange( const double, const double, const double = 3, const double = -1 );
  virtual void           Store();
  virtual void           Retrieve();

private:
  QLabel*                mypName;
  QSpinBox*              mypInteger;
};


class SalomeApp_ResourceEditor::IntegerEditItem : public Item
{
public:
  IntegerEditItem( SalomeApp_ResourceEditor*, QWidget* = 0 );
  virtual ~IntegerEditItem();

  virtual void           Set( const QString&, const QString&, const int, const QString& );
  virtual void           Store();
  virtual void           Retrieve();

private:
  QLabel*                mypName;
  QLineEdit*             mypInteger;
};

class SalomeApp_ResourceEditor::PrefGroup : public QGroupBox, public Group
{
public:
  PrefGroup( const QString&, SalomeApp_ResourceEditor*, QWidget* = 0 );
  virtual ~PrefGroup();

  virtual void           SetColumns( const int );
  virtual void           SetTitle( const QString& );

protected:
  virtual Item*          createItem( const int, const QString& = QString::null );

private:
  SalomeApp_ResourceEditor* myEditor;
};


class SalomeApp_ResourceEditor::PrefTab : public QFrame, public Tab
{
public:
  PrefTab( const QString&, SalomeApp_ResourceEditor*, QTabWidget* );
  virtual ~PrefTab();

  virtual void           SetTitle( const QString& );

  void                   adjustLabels();

protected:
  virtual Group*         createGroup( const QString& );

private:
  QTabWidget*               mypTabWidget;
  QGroupBox*                mypMainFrame;
  SalomeApp_ResourceEditor* myEditor;
};


class SalomeApp_ResourceEditor::PrefCategory : public QTabWidget, public Category
{
public:
  PrefCategory( const QString&, SalomeApp_ResourceEditor*, QListBox*, QWidgetStack* );
  virtual ~PrefCategory();

  virtual void           SetTitle( const QString& );

  void                   adjustLabels();

protected:
  virtual Tab*           createTab( const QString& );

private:
  QListBox*                 mypListBox;
  QWidgetStack*             mypWidgetStack;
  SalomeApp_ResourceEditor* myEditor;
};


class ColorButton : public QPushButton
{
  Q_OBJECT

public:
  ColorButton( QWidget* = 0 );
  virtual ~ColorButton();

private slots:
  void            onColor();
};

#endif
