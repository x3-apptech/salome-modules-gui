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

// File:      QtxPagePrefMgr.h
// Author:    Sergey TELKOV
//
#ifndef QTXPAGEPREFMGR_H
#define QTXPAGEPREFMGR_H

#include "QtxPreferenceMgr.h"

#include "QtxPathEdit.h"
#include "QtxPathListEdit.h"

#include <QFrame>
#include <QLabel>
#include <QPointer>
#include <QIcon>

class QtxGridBox;
class QtxFontEdit;
class QtxGroupBox;
class QtxComboBox;
class QtxColorButton;
class QtxBiColorTool;
class QtxShortcutEdit;
class QtxShortcutTree;
class QtxBackgroundTool;

class QToolBox;
class QLineEdit;
class QTextEdit;
class QCheckBox;
class QTabWidget;
class QToolButton;
class QListWidget;
class QFileDialog;
class QDateTimeEdit;
class QStackedWidget;
class QSlider;
class QTreeWidget;

class QTX_EXPORT QtxPagePrefMgr : public QFrame, public QtxPreferenceMgr
{
  Q_OBJECT

public:
  QtxPagePrefMgr( QtxResourceMgr*, QWidget* = 0 );
  virtual ~QtxPagePrefMgr();

  virtual QSize    sizeHint() const;
  virtual QSize    minimumSizeHint() const;

  virtual void     updateContents();

signals:
  void             resourceChanged( int );
  void             resourceChanged( QString&, QString& );
  void             resourcesChanged( const QMap<int, QString>& );

public slots:
  virtual void     setVisible( bool );

protected:
  virtual void     itemAdded( QtxPreferenceItem* );
  virtual void     itemRemoved( QtxPreferenceItem* );
  virtual void     itemChanged( QtxPreferenceItem* );

  virtual QVariant optionValue( const QString& ) const;
  virtual void     setOptionValue( const QString&, const QVariant& );

private:
  void             initialize() const;
  void             initialize( QtxPreferenceItem* );

private:
  QtxGridBox*      myBox;
  bool             myInit;
};

class QTX_EXPORT QtxPagePrefItem : public QtxPreferenceItem
{
  class Listener;

public:
  QtxPagePrefItem( const QString&, QtxPreferenceItem* = 0,
                   const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefItem();

  QWidget*          widget() const;

  virtual void      activate();

protected:
  void              setWidget( QWidget* );

  virtual void      itemAdded( QtxPreferenceItem* );
  virtual void      itemRemoved( QtxPreferenceItem* );
  virtual void      itemChanged( QtxPreferenceItem* );

  void              pageChildItems( QList<QtxPagePrefItem*>&, const bool = false ) const;

  virtual void      store();
  virtual void      retrieve();

  virtual void      widgetShown();
  virtual void      widgetHided();

  virtual void      ensureVisible( QtxPreferenceItem* );

private:
  virtual void      contentChanged();

private:
  QPointer<QWidget> myWidget;
  Listener*         myListener;
};

class QTX_EXPORT QtxPageNamedPrefItem : public QtxPagePrefItem
{
public:
  QtxPageNamedPrefItem( const QString&, QtxPreferenceItem* = 0,
                        const QString& = QString(), const QString& = QString() );
  virtual ~QtxPageNamedPrefItem();

  virtual void      setTitle( const QString& );

  static void       adjustLabels( QtxPagePrefItem* );

protected:
  QLabel*           label() const;
  QWidget*          control() const;

  void              setControl( QWidget* );

private:
  QPointer<QLabel>  myLabel;
  QPointer<QWidget> myControl;
};

class QTX_EXPORT QtxPagePrefListItem : public QObject, public QtxPagePrefItem
{
  Q_OBJECT

public:
  QtxPagePrefListItem( const QString&, QtxPreferenceItem* = 0,
                       const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefListItem();

  virtual void     updateContents();

  QString          emptyInfo() const;
  void             setEmptyInfo( const QString& );

  bool             isFixedSize() const;
  void             setFixedSize( const bool );

private slots:
  void             onItemSelectionChanged();

protected:
  virtual QVariant optionValue( const QString& ) const;
  virtual void     setOptionValue( const QString&, const QVariant& );
  virtual void     ensureVisible( QtxPreferenceItem* );

  virtual void     widgetShown();

private:
  void             updateInfo();
  void             updateGeom();
  void             updateState();
  void             updateVisible();

  int              selected() const;
  QtxPagePrefItem* selectedItem() const;
  void             setSelected( const int );

private:
  bool             myFix;
  QListWidget*     myList;
  QStackedWidget*  myStack;

  QString          myInfText;
  QLabel*          myInfLabel;
};

class QTX_EXPORT QtxPagePrefToolBoxItem : public QtxPagePrefItem
{
public:
  QtxPagePrefToolBoxItem( const QString&, QtxPreferenceItem* = 0,
                          const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefToolBoxItem();

  virtual void     updateContents();

protected:
  virtual void     ensureVisible( QtxPreferenceItem* );

private:
  void             updateToolBox();

private:
  QToolBox*        myToolBox;
};

class QTX_EXPORT QtxPagePrefTabsItem : public QtxPagePrefItem
{
public:
  QtxPagePrefTabsItem( const QString&, QtxPreferenceItem* = 0,
                       const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefTabsItem();

  virtual void     updateContents();

  int              tabPosition() const;
  void             setTabPosition( const int );

  int              tabShape() const;
  void             setTabShape( const int );

  QSize            tabIconSize() const;
  void             setTabIconSize( const QSize& );

protected:
  virtual QVariant optionValue( const QString& ) const;
  virtual void     setOptionValue( const QString&, const QVariant& );
  virtual void     ensureVisible( QtxPreferenceItem* );

private:
  void             updateTabs();

private:
  QTabWidget*      myTabs;
};

class QTX_EXPORT QtxPagePrefFrameItem : public QtxPagePrefItem
{
public:
  QtxPagePrefFrameItem( const QString&, QtxPreferenceItem* = 0,
                        const QString& = QString(), const QString& = QString(), const bool = false );
  virtual ~QtxPagePrefFrameItem();

  virtual void     updateContents();

  bool             stretch() const;
  void             setStretch( const bool );

  int              margin() const;
  void             setMargin( const int );

  int              spacing() const;
  void             setSpacing( const int );

  int              columns() const;
  void             setColumns( const int );

  Qt::Orientation  orientation() const;
  void             setOrientation( const Qt::Orientation );

protected:
  virtual QVariant optionValue( const QString& ) const;
  virtual void     setOptionValue( const QString&, const QVariant& );

  virtual void     widgetShown();

private:
  void             updateFrame();

private:
  QtxGridBox*      myBox;
};

class QTX_EXPORT QtxPagePrefGroupItem : public QtxPagePrefItem
{
public:
  QtxPagePrefGroupItem( const QString&, QtxPreferenceItem* = 0,
                        const QString& = QString(), const QString& = QString() );
  QtxPagePrefGroupItem( const int, const QString&, QtxPreferenceItem* = 0,
                        const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefGroupItem();

  virtual void     updateContents();

  int              margin() const;
  void             setMargin( const int );

  int              spacing() const;
  void             setSpacing( const int );

  int              columns() const;
  void             setColumns( const int );

  Qt::Orientation  orientation() const;
  void             setOrientation( const Qt::Orientation );

  bool             isFlat() const;
  void             setFlat( const bool );

  virtual void     setResource( const QString&, const QString& );

  virtual void     store();
  virtual void     retrieve();

  QtxGridBox*      gridBox() const;

protected:
  virtual QVariant optionValue( const QString& ) const;
  virtual void     setOptionValue( const QString&, const QVariant& );

  virtual void     widgetShown();

private:
  void             updateState();
  void             updateGroup();

private:
  QtxGridBox*      myBox;
  QtxGroupBox*     myGroup;
};

class QTX_EXPORT QtxPagePrefLabelItem : public QtxPagePrefItem
{
public:
  QtxPagePrefLabelItem( const QString&, QtxPreferenceItem* = 0 );
  QtxPagePrefLabelItem( Qt::Alignment, const QString&, QtxPreferenceItem* = 0 );
  virtual ~QtxPagePrefLabelItem();

  virtual void      setTitle( const QString& );

  Qt::Alignment     alignment() const;
  void              setAlignment( Qt::Alignment );

protected:
  virtual QVariant optionValue( const QString& ) const;
  virtual void     setOptionValue( const QString&, const QVariant& );

private:
  QPointer<QLabel>  myLabel;
};

class QTX_EXPORT QtxPagePrefSpaceItem : public QtxPagePrefItem
{
public:
  QtxPagePrefSpaceItem( QtxPreferenceItem* = 0 );
  QtxPagePrefSpaceItem( Qt::Orientation, QtxPreferenceItem* = 0 );
  QtxPagePrefSpaceItem( const int, const int, QtxPreferenceItem* = 0 );
  virtual ~QtxPagePrefSpaceItem();

  int              size( Qt::Orientation ) const;
  void             setSize( Qt::Orientation, const int );

  int              stretch( Qt::Orientation ) const;
  void             setStretch( Qt::Orientation, const int );

protected:
  virtual QVariant optionValue( const QString& ) const;
  virtual void     setOptionValue( const QString&, const QVariant& );

private:
  void             initialize( const int, const int, const int, const int );
};

class QTX_EXPORT QtxPagePrefCheckItem : public QtxPagePrefItem
{
public:
  QtxPagePrefCheckItem( const QString&, QtxPreferenceItem* = 0,
                        const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefCheckItem();

  virtual void     setTitle( const QString& );

  virtual void     store();
  virtual void     retrieve();

private:
  QCheckBox*       myCheck;
};

class QTX_EXPORT QtxPagePrefEditItem : public QtxPageNamedPrefItem
{
public:
  typedef enum { String, Integer, Double } InputType;

public:
  QtxPagePrefEditItem( const QString&, QtxPreferenceItem* = 0,
                       const QString& = QString(), const QString& = QString() );
  QtxPagePrefEditItem( const int, const QString&, QtxPreferenceItem* = 0,
                       const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefEditItem();

  int              inputType() const;
  void             setInputType( const int );

  int              decimals() const;
  void             setDecimals( const int );
  
  int              echoMode() const;
  void             setEchoMode( const int );

  virtual void     store();
  virtual void     retrieve();

protected:
  virtual QVariant optionValue( const QString& ) const;
  virtual void     setOptionValue( const QString&, const QVariant& );

private:
  void             updateEditor();

private:
  int              myType;
  int              myDecimals;
  int              myEchoMode;
  QLineEdit*       myEditor;
};

class QTX_EXPORT QtxPagePrefSliderItem : public QObject,public QtxPageNamedPrefItem
{
  Q_OBJECT

public:
  QtxPagePrefSliderItem( const QString&, QtxPreferenceItem* = 0,
                         const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefSliderItem();

  int              singleStep() const;
  int              pageStep() const;
  int              minimum() const;
  int              maximum() const;
  QList<QIcon>     icons() const; 

  void             setSingleStep( const int& );
  void             setPageStep( const int& );
  void             setMinimum( const int& );
  void             setMaximum( const int& );
  void             setIcons( const QList<QIcon>& );

  virtual void     store();
  virtual void     retrieve();

protected:
  virtual QVariant optionValue( const QString& ) const;
  virtual void     setOptionValue( const QString&, const QVariant& );

private slots:
  void             setIcon( int );

private:
  void             updateSlider();
  void             setIcons( const QVariant& );

private:
  QSlider*         mySlider;
  QLabel*          myLabel;
  QList<QIcon>     myIcons;
};

class QTX_EXPORT QtxPagePrefSelectItem : public QtxPageNamedPrefItem
{
public:
  typedef enum { NoInput, String, Integer, Double } InputType;

public:
  QtxPagePrefSelectItem( const QString&, QtxPreferenceItem* = 0,
                         const QString& = QString(), const QString& = QString() );
  QtxPagePrefSelectItem( const int, const QString&, QtxPreferenceItem* = 0,
                         const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefSelectItem();

  int              inputType() const;
  void             setInputType( const int );

  QStringList      strings() const;
  QList<QVariant>  numbers() const;
  QList<QIcon>     icons() const;

  void             setStrings( const QStringList& );
  void             setNumbers( const QList<QVariant>& );
  void             setIcons( const QList<QIcon>& );

  virtual void     store();
  virtual void     retrieve();

protected:
  virtual QVariant optionValue( const QString& ) const;
  virtual void     setOptionValue( const QString&, const QVariant& );

private:
  void             updateSelector();
  void             setStrings( const QVariant& );
  void             setNumbers( const QVariant& );
  void             setIcons( const QVariant& );

private:
  int              myType;
  QtxComboBox*     mySelector;
};

class QTX_EXPORT QtxPagePrefSpinItem : public QtxPageNamedPrefItem
{
public:
  typedef enum { Integer, Double } InputType;

public:
  QtxPagePrefSpinItem( const QString&, QtxPreferenceItem* = 0,
                       const QString& = QString(), const QString& = QString() );
  QtxPagePrefSpinItem( const int, const QString&, QtxPreferenceItem* = 0,
                       const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefSpinItem();

  QVariant         step() const;
  QVariant         precision() const;
  QVariant         minimum() const;
  QVariant         maximum() const;

  QString          prefix() const;
  QString          suffix() const;
  QString          specialValueText() const;

  void             setStep( const QVariant& );
  void             setPrecision( const QVariant& );
  void             setMinimum( const QVariant& );
  void             setMaximum( const QVariant& );

  void             setPrefix( const QString& );
  void             setSuffix( const QString& );
  void             setSpecialValueText( const QString& );

  int              inputType() const;
  void             setInputType( const int );

  virtual void     store();
  virtual void     retrieve();

protected:
  virtual QVariant optionValue( const QString& ) const;
  virtual void     setOptionValue( const QString&, const QVariant& );

private:
  void             updateSpinBox();

private:
  int              myType;
};

class QTX_EXPORT QtxPagePrefTextItem : public QtxPageNamedPrefItem
{
public:
  QtxPagePrefTextItem( QtxPreferenceItem* = 0,
                       const QString& = QString(), const QString& = QString() );
  QtxPagePrefTextItem( const QString&, QtxPreferenceItem* = 0,
                       const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefTextItem();

  virtual void     store();
  virtual void     retrieve();

private:
  QTextEdit*       myEditor;
};

class QTX_EXPORT QtxPagePrefColorItem : public QtxPageNamedPrefItem
{
public:
  QtxPagePrefColorItem( const QString&, QtxPreferenceItem* = 0,
                        const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefColorItem();

  virtual void     store();
  virtual void     retrieve();

private:
  QtxColorButton*  myColor;
};

class QTX_EXPORT QtxPagePrefBiColorItem : public QtxPageNamedPrefItem
{
public:
  QtxPagePrefBiColorItem( const QString&, QtxPreferenceItem* = 0,
			   const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefBiColorItem();

  virtual QString  text() const;
  virtual void     setText( const QString& );

  virtual void     store();
  virtual void     retrieve();

protected:
  virtual QVariant optionValue( const QString& ) const;
  virtual void     setOptionValue( const QString&, const QVariant& );

private:
  QtxBiColorTool*  myColors;
};

class QTX_EXPORT QtxPagePrefFontItem : public QObject, public QtxPageNamedPrefItem
{
  Q_OBJECT

public:
  QtxPagePrefFontItem( const int, const QString&, QtxPreferenceItem* = 0,
                       const QString& = QString(), const QString& = QString() );
  QtxPagePrefFontItem( const QString&, QtxPreferenceItem* = 0,
                       const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefFontItem();

  int              features() const;
  void             setFeatures( const int );

  void             setMode( const int );
  int              mode() const;

  void             setFonts( const QStringList& );
  QStringList      fonts() const;

  void             setSizes( const QList<int>& = QList<int>() );
  QList<int>       sizes() const;

  virtual void     store();
  virtual void     retrieve();

protected:
  virtual QVariant optionValue( const QString& ) const;
  virtual void     setOptionValue( const QString&, const QVariant& );

private:
  QtxFontEdit*     myFont;
};

class QTX_EXPORT QtxPagePrefPathItem : public QtxPageNamedPrefItem
{
public:
  QtxPagePrefPathItem( const Qtx::PathType, const QString&, QtxPreferenceItem* = 0,
                       const QString& = QString(), const QString& = QString() );
  QtxPagePrefPathItem( const QString&, QtxPreferenceItem* = 0,
                       const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefPathItem();

  Qtx::PathType    pathType() const;
  void             setPathType( const Qtx::PathType );

  QString          pathFilter() const;
  void             setPathFilter( const QString& );

  virtual void     store();
  virtual void     retrieve();

protected:
  virtual QVariant optionValue( const QString& ) const;
  virtual void     setOptionValue( const QString&, const QVariant& );

private:
  QtxPathEdit*     myPath;
};

class QTX_EXPORT QtxPagePrefPathListItem : public QtxPageNamedPrefItem
{
public:
  QtxPagePrefPathListItem( QtxPreferenceItem* = 0,
                           const QString& = QString(), const QString& = QString() );
  QtxPagePrefPathListItem( const QString&, QtxPreferenceItem* = 0,
                           const QString& = QString(), const QString& = QString() );
  QtxPagePrefPathListItem( const Qtx::PathType, const QString&, QtxPreferenceItem* = 0,
                           const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefPathListItem();

  Qtx::PathType    pathType() const;
  void             setPathType( const Qtx::PathType );

  virtual void     store();
  virtual void     retrieve();

protected:
  virtual QVariant optionValue( const QString& ) const;
  virtual void     setOptionValue( const QString&, const QVariant& );

private:
  QtxPathListEdit* myPaths;
};

class QTX_EXPORT QtxPagePrefDateTimeItem : public QtxPageNamedPrefItem
{
public:
  typedef enum { Date, Time, DateTime } InputType;

public:
  QtxPagePrefDateTimeItem( const QString&, QtxPreferenceItem* = 0,
                           const QString& = QString(), const QString& = QString() );
  QtxPagePrefDateTimeItem( const int, const QString&, QtxPreferenceItem* = 0,
                           const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefDateTimeItem();

  int              inputType() const;
  void             setInputType( const int );

  bool             calendar() const;
  void             setCalendar( const bool );

  QDate            maximumDate() const;
  QTime            maximumTime() const;
  QDate            minimumDate() const;
  QTime            minimumTime() const;

  void             setMaximumDate( const QDate& );
  void             setMaximumTime( const QTime& );
  void             setMinimumDate( const QDate& );
  void             setMinimumTime( const QTime& );

  virtual void     store();
  virtual void     retrieve();

protected:
  virtual QVariant optionValue( const QString& ) const;
  virtual void     setOptionValue( const QString&, const QVariant& );

private:
  void             updateDateTime();

private:
  int              myType;
  QDateTimeEdit*   myDateTime;
};

class QTX_EXPORT QtxPagePrefShortcutBtnsItem : public QtxPageNamedPrefItem
{
public:
  QtxPagePrefShortcutBtnsItem( const QString&, QtxPreferenceItem* = 0,
                               const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefShortcutBtnsItem();
  virtual void     store();
  virtual void     retrieve();

private:
  QtxShortcutEdit* myShortcut;
};

class QTX_EXPORT QtxPagePrefShortcutTreeItem : public QtxPageNamedPrefItem
{
public:
  QtxPagePrefShortcutTreeItem( const QString&, QtxPreferenceItem* = 0, 
                               const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefShortcutTreeItem();
  virtual void     store();
  virtual void     retrieve();
								   
private:
  QtxShortcutTree* myShortcutTree;
  QString          mySection;
};

class QTX_EXPORT QtxPagePrefBackgroundItem : public QObject, public QtxPageNamedPrefItem
{
  Q_OBJECT

public:
  QtxPagePrefBackgroundItem( const QString&, QtxPreferenceItem* = 0,
			     const QString& = QString(), const QString& = QString() );
  virtual ~QtxPagePrefBackgroundItem();

  void               gradients( QStringList&, QIntList& ) const;
  void               setGradients( const QStringList&, const QIntList& = QIntList() );

  bool               isModeAllowed( Qtx::BackgroundMode ) const;
  void               setModeAllowed( Qtx::BackgroundMode, bool );

  bool               isTextureModeAllowed( Qtx::TextureMode ) const;
  void               setTextureModeAllowed( Qtx::TextureMode, bool );

  bool               isTextureAllowed() const;
  void               setTextureAllowed( bool );

  QString            imageFormats() const;
  void               setImageFormats( const QString& );

  Qt::Orientation    orientation() const;
  void               setOrientation( Qt::Orientation );

  virtual void       store();
  virtual void       retrieve();

protected:
  virtual QVariant   optionValue( const QString& ) const;
  virtual void       setOptionValue( const QString&, const QVariant& );

private:
  QtxBackgroundTool* myBgTool;
};

class QtxUserDefinedContent: public QWidget
{
 public:
  QtxUserDefinedContent(QWidget* parent = 0, Qt::WindowFlags f = 0 ):QWidget(parent, f) {};
  virtual void store(QtxResourceMgr* theRes, QtxPreferenceMgr* thePref) = 0;
  virtual void retrieve(QtxResourceMgr* theRes, QtxPreferenceMgr* thePref) = 0;
};


class QTX_EXPORT QtxUserDefinedItem : public QtxPageNamedPrefItem
{
public:
  QtxUserDefinedItem( QtxPreferenceItem* parent );

  void setContent( QtxUserDefinedContent* theContent );

  virtual void store();
  virtual void retrieve();

protected:
  virtual QVariant optionValue( const QString& theName ) const;
  virtual void     setOptionValue( const QString& theName, const QVariant& theVal );

private:
  QtxUserDefinedContent* myContent;
};

#endif
