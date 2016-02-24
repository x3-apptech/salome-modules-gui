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

// File   : SalomeApp_ListView.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#ifndef SALOMEAPP_LISTVIEW_H
#define SALOMEAPP_LISTVIEW_H

//#include <QtxListView.h>

#include <SUIT_PopupClient.h>

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QString>
#include <QLineEdit>
#include <QComboBox>

#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListOfReal.hxx>

class QToolButton; 

// enumeration for ListView updating mode
enum UpdateType {
  utCancel        = -1,           // cancel updating
  utNone          =  0,           // needs no any update
  utUpdateItem,                   // update one item
  utUpdateParent,                 // update parent item too
  utUpdateViewer,                 // update all viewer
  utUpdateAll                     // strong update
};

class SalomeApp_ListViewItem;
class SalomeApp_EntityEdit;

/*!
  \class SalomeApp_ListView
  parent class for Data Viewer and Properties Viewer
*/
class SalomeApp_ListView : public QTreeWidget/*QtxListView*/ , public SUIT_PopupClient  {
  
  Q_OBJECT
    
public:
// constructor  
  SalomeApp_ListView(QWidget *parent);
// destructor  
  virtual ~SalomeApp_ListView();

// updates list viewer
  virtual void updateViewer();
// updtes currently selected item(s)
  virtual void updateSelected();

// fills popup with items
  virtual QString popupClientType() const;
  virtual void    contextMenuPopup( QMenu* );

// setting editing of items availbale/not available
  void enableEditing(bool theFlag);
// says if editing is enabled
  bool isEnableEditing();
// accepts user input by calling finishEditing(true)
  void accept();

// clears view
  void clear();

// event filter
  bool eventFilter(QObject* object, QEvent* event) ;

// gets current tooltip for list view
// returns valid rect in success
  QRect tip(QPoint aPos, QString& aText, QRect& dspRect, QFont& dspFnt) const;

protected:
// handler for resize event
  void resizeEvent(QResizeEvent* e);
// handler for paint event
  void viewportPaintEvent(QPaintEvent* e);
// finishes editing of entity
  virtual UpdateType finishEditing(bool ok);

// returns true if mouse events are enabled
  bool isMouseEnabled();
// enables/disables mouse events (excluding MouseMove)
  void enableMouse(bool enable);
  
protected slots:
// called when selection changed in list
  virtual void onSelectionChanged();
// called when user finishes in editing of item
  void onEditOk();
// called when user cancels item editing
  void onEditCancel();
// called when columns sizes are changed
  void onHeaderSizeChange(int, int, int);

protected:
  SalomeApp_EntityEdit*   myEdit;
  SalomeApp_ListViewItem* myEditedItem;
  bool                    myEditingEnabled;
  bool                    myMouseEnabled;
};


class SalomeApp_EditBox: public QLineEdit
{
  Q_OBJECT

public:
  SalomeApp_EditBox(QWidget* parent);
 
protected:
  void keyPressEvent(QKeyEvent* e);
  
signals:
  void escapePressed();
};

class SalomeApp_ComboBox: public QComboBox 
{
  Q_OBJECT

public:
  SalomeApp_ComboBox(bool rw, QWidget* parent = 0, const char* name = 0);

  int findItem(const QString& theText);
  void insertItem(const QString& theValue, int theIndex = -1);
  void insertList(const QStringList& theList);
  void insertItem(const int theValue);
  void insertList(const TColStd_ListOfInteger& theList);
  void insertItem(const double theValue);
  void insertList(const TColStd_ListOfReal& theList);
};

class SalomeApp_EntityEdit : public QWidget 
{
  Q_OBJECT

public:
  // enum for edit control type
  enum {
    etLineEdit,           // simple edit box
    etComboBox,           // non-editable combo box
    etComboEdit           // editable combo box
  };
  // enum for value type
  enum {
    vtString,             // string
    vtInteger,            // integer value
    vtDouble              // double value
  };
  // enum for insertion order
  enum {
    atBottom,
    atTop,
    atBeforeCurrent,
    atAfterCurrent
  };
  // enum for edit box buttons (Apply = Cancel)
  enum {
    btApply  = 0x0001,
    btCancel = 0x0002
  };

public:
  SalomeApp_EntityEdit( QWidget* parent, 
                        int      controlType = etLineEdit, 
                        int      valueType   = vtString,
                        bool     butApply    = false, 
                        bool     butCancel   = false);
  ~SalomeApp_EntityEdit();

  void finishEditing();
  void clear();
  QString getText();
  void setText(const QString& theText );
  void insertItem( const QString& theValue, 
                   bool           theSetCurrent = false,
                   int            theOrder = atBottom );
  void insertList( const QStringList& theList, 
                   const int          theCurrent = -1 );
  void insertItem( const int theValue, 
                   bool  theSetCurrent = false );
  void insertList( const TColStd_ListOfInteger& theList, 
                   const int                    theCurrent = -1 );
  void insertItem( const double theValue, 
                   bool         theSetCurrent = false );
  void insertList( const TColStd_ListOfReal& theList, 
                   const int                 theCurrent = -1 );
  QWidget* getControl();
  void setFocus();
  void setValidator(const QValidator*);
  void showButtons(bool show);
  void setDuplicatesEnabled(bool enabled);

protected:
  void keyPressEvent ( QKeyEvent * e);

private slots:
  void onComboActivated(const QString&);
  void onTextChanged(const QString&);
  void onApply();
  void onCancel();
signals:
  void returnPressed();
  void escapePressed();

private:
  // Widgets
  SalomeApp_EditBox*   myEdit;
  SalomeApp_ComboBox*  myCombo;
  QToolButton*         myApplyBtn;
  QToolButton*         myCancelBtn;
  QString              myString;
};

class SalomeApp_ListViewItem : public QTreeWidgetItem
{
public:
  SalomeApp_ListViewItem( SalomeApp_ListView* );
  SalomeApp_ListViewItem( SalomeApp_ListView*, 
                          SalomeApp_ListViewItem* );
  SalomeApp_ListViewItem( SalomeApp_ListView*,
                          const QStringList&,
                          const bool = false );
  SalomeApp_ListViewItem( SalomeApp_ListViewItem*,
                          const QStringList&,
                          const bool = false );
  SalomeApp_ListViewItem( SalomeApp_ListView*,
                          SalomeApp_ListViewItem*,
                          const QString&,
                          const bool = false );
  SalomeApp_ListViewItem( SalomeApp_ListViewItem*,
                          SalomeApp_ListViewItem*,
                          const QString&,
                          const bool = false);
  SalomeApp_ListViewItem( SalomeApp_ListView*,
                          SalomeApp_ListViewItem*,
                          const QString& theName,
                          const QString& theValue, 
                          const bool = false);
  SalomeApp_ListViewItem( SalomeApp_ListViewItem*,
                          SalomeApp_ListViewItem*,
                          const QString&,
                          const QString&, 
                          const bool = false);
  ~SalomeApp_ListViewItem();

  QString            fullName();
  void               openAllLevels();
  virtual void       updateAllLevels();
  bool               isEditable() const;
  void               setEditable(bool theEditable);

  // returns true if entitiy is accepted after editing
  bool               isAccepted() const;
  // set entity accepted or not after editing
  void               setAccepted(bool theAccepted);

  // returns name of entity (as default it is text in first column)
  virtual QString    getName() const;
  // sets name of entity (as default it is text in first column)
  virtual UpdateType setName(const QString& theName);

  // returns value of entity (as default it is text in second column)
  virtual QString    getValue() const;
  // sets value of entity (as default it is text in second column)
  virtual UpdateType setValue(const QString& theValue);

  // creates control for editing and fills it with values
  SalomeApp_EntityEdit*    startEditing();
  // fills widget with initial values (list or single value)
  virtual void       fillWidgetWithValues(SalomeApp_EntityEdit* theWidget);
  // finishes editing of entity
  virtual UpdateType finishEditing(SalomeApp_EntityEdit* theWidget);

  // returns type of edit control (0 - edit box, 1 - combo box, 2 - editable combo box)
  virtual int        getEditingType();
  // sets type of edit control (0 - edit box, 1 - combo box, 2 - editable combo box)
  virtual void       setEditingType(const int);
  // returns edited column
  virtual int        getEditedColumn();
  // returns type of edited value (string, int, double)
  virtual int        getValueType();
  // sets type of edited value (string, int, double)
  virtual void       setValueType(const int);

  // gets user type 
  virtual int        getUserType();
  // sets user type 
  virtual void       setUserType(const int);

  // returns buttons for editing widget
  virtual int        getButtons();
  // sets  buttons for editing widget
  virtual void       setButtons(const int);
  // returns text for tooltip
  QString            tipText();
  // calculates rectangle which should contain items tip
  QRect              tipRect();
  // calculates rect of item text in viewport coordinates
  QRect              textRect(const int column) const;
  // calculates full rect of item data in viewport coordinates
  QRect              itemRect(const int column) const;

protected:
  // initialization
  void               init();
  int                depth() const;

private:
  bool myEditable;
  bool myAccepted;
  int  myEditingType;
  int  myValueType;
  int  myButtons;
  int  myUserType; // user are welcome to put additional data here and use it in fillWidgetWithValues()
};


#endif
