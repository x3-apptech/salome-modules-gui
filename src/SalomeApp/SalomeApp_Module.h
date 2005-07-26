// File:      SalomeApp_Module.h
// Created:   10/25/2004 11:33:06 AM
// Author:    Sergey LITONIN
// Copyright (C) CEA 2004

#ifndef SALOMEAPP_MODULE_H
#define SALOMEAPP_MODULE_H

#include "SalomeApp.h"
#include "SalomeApp_Selection.h"

#include <CAM_Module.h>

#include <string>

class QDockWindow;

class CAM_Study;

class QtxPopupMgr;

class SUIT_Operation;
class SUIT_Convertor;
class SUIT_ViewModel;
class SUIT_DataObject;

class SalomeApp_DataModel;
class SalomeApp_Application;
class SalomeApp_Preferences;
class SalomeApp_SelectionManager;

/*!
  Description : Base class for all salome modules
*/

class SALOMEAPP_EXPORT SalomeApp_Module : public CAM_Module
{
  Q_OBJECT

public:
  SalomeApp_Module( const QString& );
  virtual ~SalomeApp_Module();

  virtual void                        initialize( CAM_Application* );
  virtual void                        windows( QMap<int, int>& ) const;
  virtual void                        viewManagers( QStringList& ) const;

  /*! engineIOR() should be a pure virtual method, to avoid logical errors!\n
   * Implementation in derived classes can return the following values:\n
   * module`s engine IOR - means that this is a standard SALOME module with a CORBA engine
   * \li "" (empty string)   - means that this is a light module, default engine should be used for interaction with SALOMEDS persistence
   * \li "-1"                - means that this is a light module, SALOMEDS persistence is not used at all\n
   */
  virtual QString                     engineIOR() const = 0;

  virtual void                        contextMenuPopup( const QString&, QPopupMenu*, QString& );

  virtual void                        createPreferences();

  /*! Convenient shortcuts*/
  SalomeApp_Application*              getApp() const;
  
  void                                updateObjBrowser( bool = true, SUIT_DataObject* = 0 );

  virtual void                        selectionChanged();
  virtual void                        preferencesChanged( const QString&, const QString& );

  virtual void                        studyActivated() {};

public slots:
  virtual bool                        activateModule( SUIT_Study* );
  virtual bool                        deactivateModule( SUIT_Study* );

  void                                MenuItem();

protected slots:
  virtual void                        onModelSaved();
  virtual void                        onModelOpened();
  virtual void                        onModelClosed();

protected:
  QtxPopupMgr*                        popupMgr();
  SalomeApp_Preferences*              preferences() const;

  virtual CAM_DataModel*              createDataModel();
  virtual SalomeApp_Selection*        createSelection() const;

  int                                 addPreference( const QString& label );
  int                                 addPreference( const QString& label, const int pId, const int = -1,
				                     const QString& section = QString::null,
				                     const QString& param = QString::null );
  QVariant                            preferenceProperty( const int, const QString& ) const;
  void                                setPreferenceProperty( const int, const QString&, const QVariant& );

private:
  QtxPopupMgr*                        myPopupMgr;
};

#endif
