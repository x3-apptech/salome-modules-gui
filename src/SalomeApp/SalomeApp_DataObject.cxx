#include "SalomeApp_DataObject.h"
#include "SalomeApp_RootObject.h"
#include "SalomeApp_Study.h"

#include <SUIT_DataObjectKey.h>
#include <SUIT_Application.h>
#include <SUIT_ResourceMgr.h>

#include <qobject.h>

/*!
	Class: SalomeApp_DataObject::Key
	Level: Internal
*/

class SalomeApp_DataObject::Key : public SUIT_DataObjectKey
{
public:
  Key( const QString& );
  virtual ~Key();

  virtual bool isLess( const SUIT_DataObjectKey* ) const;
  virtual bool isEqual( const SUIT_DataObjectKey* ) const;

private:
  QString myEntry;
};

SalomeApp_DataObject::Key::Key( const QString& entry )
: SUIT_DataObjectKey(),
  myEntry( entry )
{
}

SalomeApp_DataObject::Key::~Key()
{
}

bool SalomeApp_DataObject::Key::isLess( const SUIT_DataObjectKey* other ) const
{
  Key* that = (Key*)other;
  return myEntry < that->myEntry;
}

bool SalomeApp_DataObject::Key::isEqual( const SUIT_DataObjectKey* other ) const
{
  Key* that = (Key*)other;
  return myEntry == that->myEntry;
}

/*!
	Class: SalomeApp_DataObject
	Level: Public
*/

SalomeApp_DataObject::SalomeApp_DataObject( SUIT_DataObject* parent )
: CAM_DataObject( parent )
{
}

SalomeApp_DataObject::SalomeApp_DataObject( const _PTR(SObject)& sobj, SUIT_DataObject* parent )
: CAM_DataObject( parent )
{
  myObject = sobj;
}

SalomeApp_DataObject::~SalomeApp_DataObject()
{
}

QString SalomeApp_DataObject::entry() const
{
  if ( myObject )
    return myObject->GetID().c_str();
  return QString::null;
}

SUIT_DataObjectKey* SalomeApp_DataObject::key() const
{
  QString str = entry();
  return new Key( str );
}

QString SalomeApp_DataObject::name() const
{
  if ( myObject )
    return myObject->GetName().c_str();
  return QString::null;
  //return QString( "%1 [%2]" ).arg( myName ).arg( myEntry ); // for debug
}

QPixmap SalomeApp_DataObject::icon() const
{
  _PTR(GenericAttribute) anAttr;
  if ( myObject && myObject->FindAttribute( anAttr, "AttributePixMap" ) ){
    _PTR(AttributePixMap) aPixAttr ( anAttr );
    if ( aPixAttr->HasPixMap() ){
      QString pixmapName = QObject::tr( aPixAttr->GetPixMap().c_str() );
      SalomeApp_RootObject* aRoot = dynamic_cast<SalomeApp_RootObject*>( root() );
      if ( aRoot && aRoot->study() ) {
	QPixmap pixmap = aRoot->study()->application()->resourceMgr()->loadPixmap( componentDataType(), pixmapName ); 
	return pixmap;
      }
    }
  }
  return QPixmap();
}

QColor SalomeApp_DataObject::color() const
{
  _PTR(GenericAttribute) anAttr;
  if ( myObject && myObject->FindAttribute( anAttr, "AttributeTextColor" ) ){
    _PTR(AttributeTextColor) aColAttr (  anAttr );
    QColor color( (int)aColAttr->TextColor().R, (int)aColAttr->TextColor().G, (int)aColAttr->TextColor().B );
    return color;
  }
  return QColor();
}

QString SalomeApp_DataObject::toolTip() const
{
  //return object()->Name();
  return QString( "Object \'%1\', module \'%2\', ID=%3" ).arg( name() ).arg( componentDataType() ).arg( entry() );
}

SUIT_DataObject* SalomeApp_DataObject::componentObject() const
{
  SUIT_DataObject* compObj = 0;  // for root object (invisible SALOME_ROOT_OBJECT) 

  if ( parent() && parent() == root() ) 
    compObj = (SUIT_DataObject*)this; // for component-level objects
  else 
  {
    compObj = parent(); // for lower level objects
    while ( compObj && compObj->parent() != root() )
      compObj = compObj->parent();
  }

  return compObj;
}

QString SalomeApp_DataObject::componentDataType() const
{
  const SalomeApp_DataObject* compObj = dynamic_cast<SalomeApp_DataObject*>( componentObject() );
  if ( compObj && compObj->object() )
  {
    _PTR(SComponent) aComp ( compObj->object() );
    if ( aComp )
      return aComp->ComponentDataType().c_str();
  }

  return "";
}

_PTR(SObject) SalomeApp_DataObject::object() const
{
  return myObject;
}


/*!
	Class: SalomeApp_ModuleObject
	Level: Public
*/

SalomeApp_ModuleObject::SalomeApp_ModuleObject( SUIT_DataObject* parent )
: SalomeApp_DataObject( parent ), 
  CAM_RootObject( parent ),
  CAM_DataObject( parent )
{
}

SalomeApp_ModuleObject::SalomeApp_ModuleObject( const _PTR(SObject)& sobj, SUIT_DataObject* parent )
: SalomeApp_DataObject( sobj, parent ), 
  CAM_RootObject( 0, parent ),
  CAM_DataObject( parent )
{
}

SalomeApp_ModuleObject::SalomeApp_ModuleObject( CAM_DataModel* dm, const _PTR(SObject)& sobj, SUIT_DataObject* parent )
: SalomeApp_DataObject( sobj, parent ), 
  CAM_RootObject( dm, parent ),
  CAM_DataObject( parent )  
{
}

SalomeApp_ModuleObject::~SalomeApp_ModuleObject()
{
}
