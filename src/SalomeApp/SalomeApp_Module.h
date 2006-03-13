// File:      SalomeApp_Module.h
// Created:   10/25/2004 11:33:06 AM
// Author:    Sergey LITONIN
// Copyright (C) CEA 2004

#ifndef SALOMEAPP_MODULE_H
#define SALOMEAPP_MODULE_H

#include "SalomeApp.h"

#include <LightApp_Module.h>

class CAM_DataModel;
class SalomeApp_Application;
class LightApp_Operation;
class LightApp_Selection;
class SALOME_ListIO;
class QString;

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
   * \li "" (empty string)   - means that this not correct SALOME module
   */
  virtual QString                     engineIOR() const = 0;
  
  /*! Convenient shortcuts*/

  SalomeApp_Application*              getApp() const;

  virtual void                        storeVisualParameters(int savePoint);
  virtual void                        restoreVisualParameters(int savePoint);

protected:
  virtual CAM_DataModel*              createDataModel();
  virtual LightApp_Selection*         createSelection() const;
  virtual void                        extractContainers( const SALOME_ListIO&, SALOME_ListIO& ) const;
};

#endif
