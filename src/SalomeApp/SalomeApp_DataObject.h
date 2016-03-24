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

// File   : SalomeApp_DataObject.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#ifndef SALOMEAPP_DATAOBJECT_H
#define SALOMEAPP_DATAOBJECT_H

#include "SalomeApp.h"
#include <LightApp_DataObject.h>
#include <SALOMEDSClient.hxx>

class SalomeApp_Study;

class SALOMEAPP_EXPORT SalomeApp_DataObject : public virtual LightApp_DataObject
{

public:
  //! Column id
  enum {
    ValueId = RefEntryId + 1,    //!< value column
    IORId,                       //!< IOR column
    LastId                       //!< indicates last Id value
  };

public:
  SalomeApp_DataObject( SUIT_DataObject* = 0 );
  SalomeApp_DataObject( const _PTR(SObject)&, SUIT_DataObject* = 0 );
  virtual ~SalomeApp_DataObject();

  virtual QString        name() const;
  virtual QString        entry() const;

  virtual QString        text( const int = NameId ) const;
  virtual QPixmap        icon( const int = NameId ) const;
  virtual QColor         color( const ColorRole, const int = NameId ) const;
  virtual QString        toolTip( const int = NameId ) const;
  virtual QFont          font( const int = NameId ) const;

  virtual _PTR(SObject)  object() const;

  virtual QString        refEntry() const;
  virtual bool           isReference() const;
  _PTR(SObject)          referencedObject() const;

  bool                   hasChildren() const;
  bool                   expandable() const;

  virtual bool           isVisible() const;

  virtual QString        componentDataType() const;

  virtual bool           customSorting( const int = NameId ) const;
  virtual bool           compare( const QVariant&, const QVariant&, const int = NameId ) const;

  virtual void           insertChildAtTag( SalomeApp_DataObject*, int );
  virtual void           updateItem();

private:
  QString                ior( const _PTR(SObject)& ) const;
  QString                entry( const _PTR(SObject)& ) const;
  QString                value( const _PTR(SObject)& ) const;

private:
  _PTR(SObject)          myObject;
};

class SALOMEAPP_EXPORT SalomeApp_ModuleObject : public SalomeApp_DataObject,
                                                public CAM_ModuleObject
{
public:
  SalomeApp_ModuleObject( SUIT_DataObject* = 0 );
  SalomeApp_ModuleObject( const _PTR(SObject)&, SUIT_DataObject* = 0 );
  SalomeApp_ModuleObject( CAM_DataModel*, const _PTR(SObject)&, SUIT_DataObject* = 0 );
  virtual ~SalomeApp_ModuleObject();

  virtual QString        name() const;
  QPixmap                icon( const int = NameId ) const;
  QString                toolTip( const int = NameId ) const;
};

class SALOMEAPP_EXPORT SalomeApp_RootObject : public SalomeApp_DataObject,
                                              public LightApp_RootObject
{
public:
  SalomeApp_RootObject( LightApp_Study* );
  virtual ~SalomeApp_RootObject();

  QString                name() const;
  QString                entry() const;
  QString                text( const int = NameId ) const;
  QPixmap                icon( const int = NameId ) const;
  QColor                 color( const ColorRole, const int = NameId ) const;
  QString                toolTip( const int = NameId ) const;
  void                   setToSynchronize(bool value){_toSynchronize=value;};
  bool                   toSynchronize() const {return _toSynchronize;};
protected:
  bool _toSynchronize;
};

class SALOMEAPP_EXPORT SalomeApp_SavePointObject : public virtual LightApp_DataObject
{
public:
  SalomeApp_SavePointObject( SUIT_DataObject*, const int, SalomeApp_Study* );
  virtual ~SalomeApp_SavePointObject();

  virtual QString        name() const;
  virtual QString        entry() const;

  virtual QPixmap        icon( const int = NameId ) const;
  virtual QString        toolTip( const int = NameId ) const;

  int                    getId() const;

private:
  int                    myId;
  SalomeApp_Study*       myStudy;
};

class SALOMEAPP_EXPORT SalomeApp_SavePointRootObject : public SUIT_DataObject
{
public:
  SalomeApp_SavePointRootObject( SUIT_DataObject* );

  virtual QString        name() const;
  virtual QString        toolTip( const int = NameId ) const;
};

#endif
