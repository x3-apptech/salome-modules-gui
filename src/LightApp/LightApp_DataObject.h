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

// File   : LightApp_DataObject.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#ifndef LIGHTAPP_DATAOBJECT_H
#define LIGHTAPP_DATAOBJECT_H

#include "LightApp.h"
#include <CAM_DataObject.h>

class CAM_DataModel;
class LightApp_Study;

class LIGHTAPP_EXPORT LightApp_DataObject : public virtual CAM_DataObject
{
  class Key;

public:
  //! Column id
  enum { 
    EntryId = VisibilityId + 1,    //!< entry column
    RefEntryId                     //!< reference entry column
  };

public:
  LightApp_DataObject( SUIT_DataObject* = 0 );
  virtual ~LightApp_DataObject();

  virtual SUIT_DataObjectKey*     key() const;
  virtual QString                 entry() const;

  virtual QString                 refEntry() const;
  virtual bool                    isReference() const;

  virtual QString                 text( const int = NameId ) const;
  virtual QColor                  color( const ColorRole, const int = NameId ) const;

  virtual SUIT_DataObject*        componentObject() const;
  virtual QString                 componentDataType() const;

  virtual bool                    customSorting( const int = NameId ) const;
  virtual bool                    compare( const QVariant&, const QVariant&, const int = NameId ) const;
  virtual int                     groupId() const;
  virtual QVariant                customData(Qtx::CustomDataType type);

  virtual bool                    isVisible() const;
  virtual bool                    isDraggable() const;
  virtual bool                    isDropAccepted() const;
  virtual bool                    renameAllowed( const int = NameId ) const;
  virtual bool                    setName( const QString& );

protected:
  QString                         myCompDataType;
  SUIT_DataObject*                myCompObject;
};

class LIGHTAPP_EXPORT LightApp_ModuleObject
: public virtual LightApp_DataObject, public CAM_ModuleObject
{
public:
  LightApp_ModuleObject( SUIT_DataObject* = 0 );
  LightApp_ModuleObject( CAM_DataModel*, SUIT_DataObject* = 0 );

  virtual ~LightApp_ModuleObject();

  virtual QString        name() const;
  QPixmap                icon( const int = NameId ) const;
  QString                toolTip( const int = NameId ) const;

  virtual void           insertChild( SUIT_DataObject*, int );
};

class LIGHTAPP_EXPORT LightApp_RootObject : public virtual LightApp_DataObject
{
public:
  LightApp_RootObject( LightApp_Study* );

  virtual ~LightApp_RootObject();

  void                   setStudy( LightApp_Study* );
  LightApp_Study*        study() const;

private:
  LightApp_Study*        myStudy;
};

#endif  // LIGHTAPP_DATAOBJECT_H
