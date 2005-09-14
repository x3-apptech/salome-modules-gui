// File:      SalomeApp_Module.h
// Created:   10/25/2004 11:33:06 AM
// Author:    Sergey LITONIN
// Copyright (C) CEA 2004

#ifndef SALOMEAPP_MODULE_H
#define SALOMEAPP_MODULE_H

#include "SalomeApp.h"

#include <LightApp_Module.h>

#include <string>

class CAM_DataModel;
class SUIT_DataObject;
class SUIT_Operation;
class SalomeApp_Application;

class SalomeApp_Operation;
class SalomeApp_SwitchOp;

/*!
 * \brief Base class for all salome modules
*/
class SALOMEAPP_EXPORT SalomeApp_Module : public LightApp_Module
{
  Q_OBJECT

public:
  SalomeApp_Module( const QString& );
  virtual ~SalomeApp_Module();

  /*! engineIOR() should be a pure virtual method, to avoid logical errors!\n
   * Implementation in derived classes can return the following values:\n
   * module`s engine IOR - means that this is a standard SALOME module with a CORBA engine
   * \li "" (empty string)   - means that this is a light module, default engine should be used for interaction with SALOMEDS persistence
   * \li "-1"                - means that this is a light module, SALOMEDS persistence is not used at all\n
   */
  virtual QString                     engineIOR() const = 0;
  
  /*! Convenient shortcuts*/
 
  SalomeApp_Application*              getApp() const;

  virtual void                        update( const int );
  // Update viewer or/and object browser etc. in accordance with update flags
  // ( see SalomeApp_UpdateFlags enumeration ). Derived modules can redefine this method
  // for their own purposes
    
public slots:
  virtual bool                        activateModule( SUIT_Study* );
  virtual bool                        deactivateModule( SUIT_Study* );

protected slots:
  virtual void                        onOperationStopped( SUIT_Operation* );
  virtual void                        onOperationDestroyed();

 protected:
  virtual CAM_DataModel*              createDataModel();

  virtual void                        updateControls();

  /*! Module stores operations in map. This method starts operation by id.
   *  If operation isn't in map, then it will be created by createOperation method
   *  and will be inserted to map
   */
  void                                startOperation( const int );

  /*! Create operation by its id. You must not call this method, it will be called automatically
   *  by startOperation. Please redefine this method in current module
   */
  virtual SalomeApp_Operation*        createOperation( const int ) const;

private:
  typedef QMap<int,SalomeApp_Operation*> MapOfOperation;
  
private:
  MapOfOperation        myOperations;
  SalomeApp_SwitchOp*    mySwitchOp;
};

#endif
