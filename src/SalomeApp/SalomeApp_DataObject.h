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
// See http://www.salome-platform.org/
//
#ifndef SALOMEAPP_DATAOBJECT_H
#define SALOMEAPP_DATAOBJECT_H

#include "SalomeApp.h"

#include "LightApp_DataObject.h"
#include "CAM_RootObject.h"

#include "SALOMEDSClient.hxx"

class SalomeApp_Study;

class SALOMEAPP_EXPORT SalomeApp_DataObject : public LightApp_DataObject
{

public:
  enum { CT_Value, CT_Entry, CT_IOR, CT_RefEntry };

public:
  SalomeApp_DataObject( SUIT_DataObject* = 0 );
  SalomeApp_DataObject( const _PTR(SObject)&, SUIT_DataObject* = 0 );
  virtual ~SalomeApp_DataObject();
    
  virtual QString                 name() const;
  virtual QPixmap                 icon() const;
  virtual QString                 toolTip() const;

  virtual QString                 text( const int ) const;
  virtual QColor                  color( const ColorRole ) const;

  virtual QString                 entry() const;

  /*! location of corresponding SALOMEDS::SObject  */
  virtual _PTR(SObject)           object() const;

  bool                            isReference() const;
  _PTR(SObject)                   referencedObject() const;

  /*! GEOM, SMESH, VISU, etc.*/
  virtual QString                 componentDataType() const;

private:
  QString                         ior( const _PTR(SObject)& ) const;
  QString                         entry( const _PTR(SObject)& ) const;
  QString                         value( const _PTR(SObject)& ) const;

private:
  _PTR(SObject)                   myObject;
  QString                         myEntry;
  QString                         myName;
};

/*!
 * SalomeApp_ModuleObject - class for optimized access to DataModel from
 * SalomeApp_DataObject instances - see also CAM_RootObject.h
 */

class SALOMEAPP_EXPORT SalomeApp_ModuleObject : public SalomeApp_DataObject,
                                                public CAM_RootObject
{
public:
  SalomeApp_ModuleObject( SUIT_DataObject* = 0 );
  SalomeApp_ModuleObject( const _PTR(SObject)&, SUIT_DataObject* = 0 );
  SalomeApp_ModuleObject( CAM_DataModel*, const _PTR(SObject)&, SUIT_DataObject* = 0 );
  virtual ~SalomeApp_ModuleObject();

  virtual QString        name() const;
};

#endif
