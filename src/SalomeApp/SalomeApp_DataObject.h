#ifndef SALOMEAPP_DATAOBJECT_H
#define SALOMEAPP_DATAOBJECT_H

#include "SalomeApp.h"

#include "CAM_DataObject.h"
#include "CAM_RootObject.h"

#include "SALOMEDSClient.hxx"

class SalomeApp_Study;

class SALOMEAPP_EXPORT SalomeApp_DataObject : public virtual CAM_DataObject
{
  class Key;

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

  virtual SUIT_DataObjectKey*     key() const;
  virtual QString                 entry() const;

  virtual _PTR(SObject)           object() const; // location of corresponding SALOMEDS::SObject  

  bool                            isReference() const;
  _PTR(SObject)                   referencedObject() const;

  SUIT_DataObject*                componentObject() const;
  QString                         componentDataType() const; // GEOM, SMESH, VISU, etc.

private:
  QString                         ior( const _PTR(SObject)& ) const;
  QString                         entry( const _PTR(SObject)& ) const;
  QString                         value( const _PTR(SObject)& ) const;

private:
  _PTR(SObject)                   myObject;
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
};

#endif
