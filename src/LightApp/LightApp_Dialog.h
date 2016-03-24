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

// File:      LightApp_Dialog.h
// Author:    Alexander SOLOVYOV
//
#ifndef LIGHTAPP_DIALOG_H
#define LIGHTAPP_DIALOG_H

#include "LightApp.h"
#include <QtxDialog.h>

#include <QList>
#include <QMap>
#include <QPixmap>

class QLineEdit;
class QAbstractButton;
class QLabel;

class SUIT_ResourceMgr;

#ifdef WIN32
#pragma warning ( disable:4251 )
#endif

/*!
  \class LightApp_Dialog
  Base class for all LightApp dialogs.
  Provides standard widget for object selection: line edit, button;
  it is necessary to call corresponding methods on selection change.
  Standard dialog provides filtering, selection string representation,
  possibility to indicate necessary selection by text with list of ids.
*/
class LIGHTAPP_EXPORT LightApp_Dialog : public QtxDialog
{
  Q_OBJECT
  
public:
  typedef QList<int>             TypesList;
  typedef QMap<int,QStringList>  SelectedObjects;

  enum ObjectWg
  {
    Label   = 0x00000001,
    Btn     = 0x00000002,
    Control = 0x00000004
  };
  
  typedef enum
  {
    OneName,           //<! only one object can be selected and it's name is shown
    OneNameOrCount,    //<! if one object is selected, it's name is shown otherwise 
                       // "<count> <type>" is shown
    ListOfNames,       //! list of all names is shown
    Count              //! In every case "<count> <type>" is shown
    
  } NameIndication;
  //! The enumeration describing how names of selected objects will be shown in line edit
  //! For more details see above

public:
  LightApp_Dialog( QWidget* = 0, const char* = 0, bool = false,
                    bool = false, const int = Standard, Qt::WindowFlags = 0 );
  virtual ~LightApp_Dialog();
  
  virtual void    show();

  //! Check if buttons is exclusive (as radiobuttons)
  bool isExclusive() const;
  
  //! Set exclusive state  
  void setExclusive( const bool );

  //! Check if operation according to dialog will be resumed automatically when mouse enter the dialog
  bool isAutoResumed() const;

  //! Set auto resumed state
  void setAutoResumed( const bool );

  //! Show widgets corresponding to id
  void showObject( const int );

  //! Hide widgets corresponding to id
  void hideObject( const int );

  //! Change the shown state of widgets corresponding to id  
  void setObjectShown( const int, const bool );

  //! Check the shown state
  bool isObjectShown( const int ) const;

  //! Change the enabled state of widgets corresponding to id
  void setObjectEnabled( const int, const bool );

  //! Check the enabled state
  bool isObjectEnabled( const int ) const;
  
  //! Get widget of object (see ObjectWg enumeration)
  QWidget* objectWg( const int theId, const int theWgId ) const;
  
  //! Pass to all active widgets name, type and id of selected object          
  void selectObject( const QString&, const int, const QString&, const bool = true );

  /*!
      Pass to all active widgets list of names, types and ids of selected objects
      Every active widget filters list and accept only objects with possible types
  */
  void selectObject( const QStringList&, const TypesList&, const QStringList&, const bool = true );
  
  //! Get text of object's control
  QString objectText( const int ) const;
  
  //! Set text of object's control
  void setObjectText( const int, const QString& );

  //! Select in certain widget avoiding check if there is active widget
  void selectObject( const int, const QString&, const int, const QString&, const bool = true );
  void selectObject( const int, const QStringList&, const TypesList&, const QStringList&, const bool = true );
  
  //! Check if certain widget has selection  
  bool hasSelection( const int ) const;

  //! Clear selection in widgets. If parameter is -1, then selection in all widgets will be cleared
  void clearSelection( const int = -1 );

  //! Get ids list of object selected in certain widget
  void selectedObject( const int, QStringList& ) const;
  
  //! Get ids list of object selected in certain widget
  QString selectedObject( const int ) const;

  //! Get map "widget id -> ids list"
  void objectSelection( SelectedObjects& ) const;
  
  //! Activate object selection button
  void activateObject( const int );

  //! Set all object selection buttons to inactive state
  void deactivateAll();

signals:
  //! selection in certain widget is changed
  void selectionChanged ( int );

  //! selection in certain widget is on
  void objectActivated  ( int );

  //! selection in certain widget is off  
  void objectDeactivated( int );

  /*
     text representation of selection is changed
     it is emitted only if "read only" state of line edit is false
  */
  void objectChanged( int, const QStringList& );
                                                   
protected:
  //! Finds and returns resource manager
  SUIT_ResourceMgr* resMgr() const;
  
  /*! Create label, button and line edit for object selection
   *  If passed id is negative, then id will be calculated automatically (first free id)
   *  Returns the same id (if id>=0) or calculated
  */
  int  createObject    ( const QString&, QWidget*, const int = -1 );

  //! Set pixmap as icon for all selection buttons
  void setObjectPixmap ( const QPixmap& );

  //! Load pixmap with section, name using resource manager and set as icon for all selection buttons
  void setObjectPixmap ( const QString&, const QString& );

  //! Change label
  void renameObject    ( const int, const QString& );

  //! Set possible types for certain id. The list of arguments must be finished by negative integer
  void setObjectType   ( const int, const int, ... );

  //! Set list as possible types for object selection
  void setObjectType   ( const int, const TypesList& );

  /*!
      Add types to list of possible types
      The list of arguments must be finished by negative integer
  */
  void addObjectType   ( const int, const int, const int, ... );

  //! Add types to list of possible types
  void addObjectType   ( const int, const TypesList& );

  //! Add type to list of possible types
  void addObjectType   ( const int, const int );

  //! Clear list of possible types (it means, that all types are welcome)  
  void removeObjectType( const int );

  //! Remove types in list from list of possible types
  void removeObjectType( const int, const TypesList& );

  //! Remove a type from list of possible types
  void removeObjectType( const int, const int );
  
  //! Check if list of possible types contains this one
  bool hasObjectType   ( const int, const int ) const;

  //! Return list of possible types
  void objectTypes     ( const int, TypesList& ) const;
  
  //!Change and get type name for indicating in selection widget
  QString& typeName( const int );
  const QString typeName( const int ) const;
  
  //! Create string contains selection list by list of names, list of types and current name indication state
  virtual QString selectionDescription( const QStringList&, const TypesList&, const NameIndication ) const;
  
  //! Create string by pattern "<count> <type>" for current list of types
  virtual QString countOfTypes( const TypesList& ) const;

  //! Get and set name indication for certain widget
  NameIndication nameIndication( const int ) const;
  void           setNameIndication( const int, const NameIndication );

  //! Check using name indication if multiple selection in possible
  bool           multipleSelection( const int ) const;

  //! Set the "read only" state of object selection line edit
  //! The "read only" will be false only if name indication is ListOfNames
  void           setReadOnly( const int, const bool );

  //! Check the "read only" state of object selection line edit
  bool           isReadOnly( const int ) const;
  
private slots:
  //! emits if the object selection button changes state
  void onToggled( bool );

  //! text in some line edit is changed
  void onTextChanged( const QString& );

private:
  /*!
     If buttons are exclusive, set to "off" all buttons except one with id
     If id=-1, then all buttons, except first with "on" state, will be set to "off"
  */
  void    updateButtons( const int = -1 );

  /*!
      Filter types and update selection string in line edit
      If bool is true, then signal is emitted
  */
  void    updateObject( const int, bool = true );

  //! Remove from list not possible types and remove from names and ids lists the corresponding items
  void    filterTypes( const int, QStringList&, TypesList&, QStringList& ) const;
  
private:
  typedef struct
  {
    QLineEdit*       myEdit;
    QAbstractButton* myBtn;
    QLabel*          myLabel;
    QStringList      myNames, myIds;
    TypesList        myTypes, myPossibleTypes;
    NameIndication   myNI;
    
  } Object;
  
  typedef QMap<int, Object> ObjectMap;
  
private:
  ObjectMap           myObjects;
  
  QMap<int,QString>   myTypeNames;
  bool                myIsExclusive, myIsBusy;
  QPixmap             myPixmap;
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
