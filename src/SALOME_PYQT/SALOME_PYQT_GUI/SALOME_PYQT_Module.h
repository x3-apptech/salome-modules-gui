// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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
// File   : SALOME_PYQT_Module.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#ifndef SALOME_PYQT_MODULE_H
#define SALOME_PYQT_MODULE_H

#include "SALOME_PYQT_GUI.h"
#include "SALOME_PYQT_PyInterp.h" // this include must be first (see PyInterp_base.h)!*/
#include "SALOME_PYQT_ModuleLight.h"
#include "SalomeApp_Module.h"
#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOME_Component)



class SALOME_PYQT_EXPORT SALOME_PYQT_Module: public SalomeApp_Module,
  public SALOME_PYQT_ModuleLight
{
  Q_OBJECT;

public:
  SALOME_PYQT_Module();
  ~SALOME_PYQT_Module();

  /* get module engine IOR */
  virtual QString            engineIOR() const;

public slots:
  virtual bool               activateModule( SUIT_Study* );
  void                       preferenceChanged( const QString&, 
                                                const QString&, 
                                                const QString& );
  void                       onGUIEvent();
  void                       onActiveViewChanged( SUIT_ViewWindow* );
  void                       onViewClosed( SUIT_ViewWindow* );
  void                       onViewTryClose( SUIT_ViewWindow* );
  void                       onViewCloned( SUIT_ViewWindow* );

protected:
  /* create data model */
  virtual CAM_DataModel*     createDataModel();

  Engines::EngineComponent_var getEngine() const;

private:
  void                       getEngineIOR();

private:
  QString                    myIOR;
};

#endif // SALOME_PYQT_MODULE_H
