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
// File:      LightApp_Module.h
// Created:   6/20/2005 16:25:06 AM
// Author:    OCC team
// Copyright (C) CEA 2005

#ifndef LIGHTAPP_MODULE_H
#define LIGHTAPP_MODULE_H

#include "LightApp.h"
#include <CAM_Module.h>

class LightApp_Application;
class LightApp_Preferences;
class LightApp_Selection;
class LightApp_Operation;
class LightApp_SwitchOp;
class LightApp_ShowHideOp;
class LightApp_Displayer;

class SUIT_Study;
class SUIT_DataObject;
class SUIT_Operation;
class SUIT_ViewManager;
class CAM_Application;

class QtxPopupMgr;

class QString;
class QVariant;

#ifdef WIN32
#pragma warning ( disable:4251 )
#endif

/*!
  \class LightApp_Module
  Base class for all light modules
*/

class LIGHTAPP_EXPORT LightApp_Module : public CAM_Module
{
  Q_OBJECT

public:
  LightApp_Module( const QString& );
  virtual ~LightApp_Module();

  virtual void                        initialize( CAM_Application* );
  virtual void                        windows( QMap<int, int>& ) const;
  virtual void                        viewManagers( QStringList& ) const;

  virtual void                        contextMenuPopup( const QString&, QPopupMenu*, QString& );

  virtual void                        createPreferences();

  LightApp_Application*               getApp() const;

  virtual void                        update( const int );
  // Update viewer or/and object browser etc. in accordance with update flags
  // ( see SalomeApp_UpdateFlags enumeration ). Derived modules can redefine this method
  // for their own purposes

  virtual void                        updateObjBrowser( bool = true, SUIT_DataObject* = 0 );
  // Update object bropwser ( for updating model or whole object browser use update() method
  // can be used )

  virtual void                        selectionChanged();
  virtual void                        preferencesChanged( const QString&, const QString& );

  virtual void                        studyActivated() {};

  virtual LightApp_Displayer*         displayer();
  virtual LightApp_Selection*         createSelection() const;

public slots:
  virtual bool                        activateModule( SUIT_Study* );
  virtual bool                        deactivateModule( SUIT_Study* );

  void                                MenuItem();

protected slots:
  virtual void                        onModelSaved();
  virtual void                        onModelOpened();
  virtual void                        onModelClosed();

  virtual void                        onOperationStopped( SUIT_Operation* );
  virtual void                        onOperationDestroyed();
  virtual void                        onShowHide();
  virtual void                        onViewManagerAdded( SUIT_ViewManager* );
  virtual void                        onViewManagerRemoved( SUIT_ViewManager* );

protected:
  virtual QtxPopupMgr*                popupMgr();
  LightApp_Preferences*               preferences() const;

  virtual CAM_DataModel*              createDataModel();

  int                                 addPreference( const QString& label );
  int                                 addPreference( const QString& label, const int pId, const int = -1,
				                     const QString& section = QString::null,
				                     const QString& param = QString::null );
  QVariant                            preferenceProperty( const int, const QString& ) const;
  void                                setPreferenceProperty( const int, const QString&, const QVariant& );

  /*! Module stores operations in map. This method starts operation by id.
   *  If operation isn't in map, then it will be created by createOperation method
   *  and will be inserted to map
   */
  void                                startOperation( const int );
  /*! Create operation by its id. You must not call this method, it will be called automatically
   *  by startOperation. Please redefine this method in current module
   */
  virtual LightApp_Operation*         createOperation( const int ) const;

  virtual void                        updateControls();

private:
  typedef QMap<int,LightApp_Operation*> MapOfOperation;

private:
  QtxPopupMgr*          myPopupMgr;
  MapOfOperation        myOperations;
  LightApp_SwitchOp*    mySwitchOp;

protected:
  int                   myDisplay, myErase, myDisplayOnly, myEraseAll;
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
