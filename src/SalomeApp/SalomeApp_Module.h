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

// File:      SalomeApp_Module.h
// Created:   10/25/2004 11:33:06 AM
// Author:    Sergey LITONIN

#ifndef SALOMEAPP_MODULE_H
#define SALOMEAPP_MODULE_H

#include "SalomeApp.h"

#include <LightApp_Module.h>

class CAM_DataModel;
class SalomeApp_Application;
class LightApp_Selection;
class SALOME_ListIO;
class QString;
class SUIT_DataObject;

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
  virtual LightApp_Selection*         createSelection() const;
  
protected:
  virtual CAM_DataModel*              createDataModel();
  virtual void                        extractContainers( const SALOME_ListIO&, SALOME_ListIO& ) const;
};

#endif
