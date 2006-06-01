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
// File:      SalomeApp_DataModel.h
// Created:   10/25/2004 10:32:33 AM
// Author:    Sergey LITONIN
// Copyright (C) CEA 2004

#ifndef SALOMEAPP_DATAMODEL_H
#define SALOMEAPP_DATAMODEL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SalomeApp.h"
#include "LightApp_DataModel.h"

#include "SALOMEDSClient.hxx"

class SalomeApp_Module;
class SalomeApp_Study;
class SalomeApp_DataObject;

//   Class       : SalomeApp_DataModel
///  Description : Base class of data model
class SALOMEAPP_EXPORT SalomeApp_DataModel : public LightApp_DataModel
{
  Q_OBJECT

public:
  static SUIT_DataObject*             synchronize( const _PTR( SComponent )&, SalomeApp_Study* );

  SalomeApp_DataModel ( CAM_Module* theModule );
  virtual ~SalomeApp_DataModel();

  virtual bool                        open( const QString&, CAM_Study*, QStringList );
  virtual bool                        create( CAM_Study* );
  virtual void                        update( LightApp_DataObject* = 0, LightApp_Study* = 0 );

  QString                             getRootEntry( SalomeApp_Study* ) const;
  SalomeApp_Module*                   getModule() const;

protected:
  SalomeApp_Study*                    getStudy() const;
  virtual void                        updateTree( const _PTR( SComponent )&, SalomeApp_Study* );
};


#endif 
