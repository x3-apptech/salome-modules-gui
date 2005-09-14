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
  QString                         componentDataType() const;

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
