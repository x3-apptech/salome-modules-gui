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
// File:      QtxListResourceEdit.h
// Author:    Sergey TELKOV

#ifndef QTXLISTRESOURCEEDIT_H
#define QTXLISTRESOURCEEDIT_H

#include "QtxResourceEdit.h"

#include <qmap.h>
#include <qhbox.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qvalidator.h>

class QLabel;
class QListBox;
class QLineEdit;
class QCheckBox;
class QComboBox;
class QTabWidget;
class QWidgetStack;

class QtxIntSpinBox;
class QtxDblSpinBox;

class QtxDirListEditor;

/*!
  \class QtxListResourceEdit
  GUI implementation of QtxResourceEdit - manager of resources
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
  class ColorItem;
  class StateItem;
  class SelectItem;
  class StringItem;
  class DoubleSpinItem;
  class DoubleEditItem;
  class IntegerSpinItem;
  class IntegerEditItem;
  class FontItem;
  class FileItem;
  class DirListItem;

  enum { Space, Bool, Color, String, Selector, DblSpin, IntSpin, Double, Integer, GroupBox, Font, DirList, File, User };

public:
  QtxListResourceEdit( QtxResourceMgr*, QWidget* = 0 );
  virtual ~QtxListResourceEdit();

  virtual void  setItemProperty( const int, const QString&, const QVariant& );

signals:
  void          resourceChanged( int );
  void          resourceChanged( QString&, QString& );
  void          resourcesChanged( const QMap<int, QString>& );

private slots:
  void          onSelectionChanged();

protected:
  virtual void  itemAdded( Item* );
  virtual Item* createItem( const QString&, const int );
  virtual void  changedResources( const QMap<Item*, QString>& );

private:
  void          updateState();
  void          updateVisible();

private:
  QListBox*     myList;
  QWidgetStack* myStack;
};

/*!
  \class QtxListResourceEdit::Category
  GUI implementation of 'Category' frame
*/

class QtxListResourceEdit::Category : public QFrame, public Item
{
public:
  Category( QtxListResourceEdit*, QWidget* = 0 );
  virtual ~Category();

  virtual bool     isEmpty() const;

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

/*!
  \class QtxListResourceEdit::Tab
  GUI implementation of resources tab.
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
  QWidget*      myMainFrame;
};

/*!
  \class QtxListResourceEdit::Group
  GUI implementation of resources group.
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

/*!
  \class QtxListResourceEdit::PrefItem
  Base class for preferences items.
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

/*!
  \class QtxListResourceEdit::Spacer
  GUI implementation of resources spacer.
*/

class QtxListResourceEdit::Spacer : public PrefItem
{
public:
  Spacer( QtxResourceEdit*, Item*, QWidget* = 0 );
  virtual ~Spacer();

  virtual void store();
  virtual void retrieve();
};

/*!
  \class QtxListResourceEdit::SelectItem
  GUI implementation of resources selector item.
*/

class QtxListResourceEdit::SelectItem : public PrefItem
{
public:
  SelectItem( const QString&, QtxResourceEdit*, Item*, QWidget* = 0 );
  virtual ~SelectItem();

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

/*!
  \class  QtxListResourceEdit::StateItem
  GUI implementation of resources bool item.
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

/*!
  \class  QtxListResourceEdit::StringItem
  GUI implementation of resources string item.
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

/*!
  \class  QtxListResourceEdit::IntegerEditItem
  GUI implementation of resources integer item.
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

/*!
  \class  QtxListResourceEdit::IntegerSpinItem
  GUI implementation of resources integer item.
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

/*!
  \class  QtxListResourceEdit::DoubleEditItem
  GUI implementation of resources double item.
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

/*!
  \class  QtxListResourceEdit::DoubleSpinItem
  GUI implementation of resources double item.
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

/*!
  \class  QtxListResourceEdit::ColorItem
  GUI implementation of resources color item.
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

class QtxComboBox;
class QToolButton;

/*!
  \class  QtxListResourceEdit::FontItem
  GUI implementation of resources font item.
*/
class QtxListResourceEdit::FontItem : public PrefItem
{
  Q_OBJECT

public:
  typedef enum
  {
    Family    = 0x01,
    Size      = 0x02,
    UserSize  = 0x04,
    Bold      = 0x08,
    Italic    = 0x10,
    Underline = 0x20,
    Preview   = 0x40,

    All = Family | Size | UserSize | Bold | Italic | Underline | Preview
    
  } WidgetFlags;
  
public:
  FontItem( const QString&, QtxResourceEdit*, Item*, QWidget* = 0 );
  virtual ~FontItem();

  virtual void store();
  virtual void retrieve();

  virtual QVariant property( const QString& ) const;
  virtual void     setProperty( const QString&, const QVariant& );

private slots:
  void onActivateFamily( int );
  void onPreview();
  
private:
  void       setFamily( const QString& );
  QString    family() const;
  void       setSize( const int );
  int        size() const;
  void       setParams( const bool, const bool, const bool );
  void       params( bool&, bool&, bool& );
  void       internalUpdate();
  
private:
  int            myFlags;
  bool           myIsSystem;
  QtxComboBox   *myFamilies, *mySizes;
  QCheckBox     *myBold, *myItalic, *myUnderline;
  QToolButton   *myPreview;
  QMap<QString, QVariant>   myProperties;
};


/*!
  \class QtxListResourceEdit
  \brief GUI implementation of resources directory list item.
*/
class QtxListResourceEdit::DirListItem : public PrefItem
{
  Q_OBJECT
  
public:

  /*!
   * \brief Constructor
   */
  DirListItem( const QString&, QtxResourceEdit*, Item*, QWidget* = 0 );
  /*!
   * \brief Destructor
   */
  virtual ~DirListItem();

  /*!
   * \brief Stores the data
   */
  virtual void     store();

  /*!
   * \brief Retrieves the data
   */
  virtual void     retrieve();

private:
  QtxDirListEditor* myDirListEditor; //!< The widget wich implements in GUI the list of directories
};

class QtxComboBox;
class QToolButton;
class QFileDialog;

/*!
  \class QtxListResourceEdit::FontItem
  GUI implementation of resources font item.
*/

class QtxListResourceEdit::FileItem : public PrefItem
{
  Q_OBJECT

private:
  /*!
    \class QtxListResourceEdit::FileItem::FileValidator
    custom file validator: checks files on some rights
  */

  class FileValidator : public QValidator
  {
  public:
    FileValidator( FileItem*, QObject* );
    ~FileValidator();

    virtual QValidator::State validate( QString&, int& ) const;

  private:
    FileItem* myItem;
  };

public:
  FileItem( const QString&, QtxResourceEdit*, Item*, QWidget* = 0 );
  virtual ~FileItem();

  virtual void store();
  virtual void retrieve();
  
  virtual QVariant property( const QString& ) const;
  virtual void     setProperty( const QString&, const QVariant& );

  virtual bool isFileCorrect( const QString& ) const;

private slots:
  void onOpenFile();
  void onFileSelected( const QString& );

private:
  uint          myFlags;
  bool          myIsReadOnly;
  QStringList   myFilter;
  bool          myIsExisting;
  QLineEdit*    myFile;
  QToolButton*  myOpenFile;
  QFileDialog*  myFileDlg;
};


#endif
