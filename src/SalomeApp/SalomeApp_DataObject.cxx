#include "SalomeApp_DataObject.h"

#include "SalomeApp_Study.h"
#include "LightApp_RootObject.h"

#include <SUIT_Application.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_DataObjectKey.h>

#include <qobject.h>

#include <SALOMEDSClient_AttributeReal.hxx>
#include <SALOMEDSClient_AttributeInteger.hxx>
#include <SALOMEDSClient_AttributeComment.hxx>
#include <SALOMEDSClient_AttributeTableOfReal.hxx>
#include <SALOMEDSClient_AttributeTableOfInteger.hxx>

/*
	Class: SalomeApp_DataObject
	Level: Public
*/
/*!Constructor. Initialize by \a parent*/
SalomeApp_DataObject::SalomeApp_DataObject( SUIT_DataObject* parent )
: LightApp_DataObject( parent ),
  CAM_DataObject( parent )
{
}

/*!Constructor. Initialize by \a parent and SObject*/
SalomeApp_DataObject::SalomeApp_DataObject( const _PTR(SObject)& sobj, SUIT_DataObject* parent )
: LightApp_DataObject( parent ),
  CAM_DataObject( parent )
{
  myObject = sobj;
}

/*!Destructor. Do nothing.*/
SalomeApp_DataObject::~SalomeApp_DataObject()
{
}

/*!Gets object ID.
 *\retval QString
 */
QString SalomeApp_DataObject::entry() const
{
  if ( myObject )
    return myObject->GetID().c_str();
  return QString::null;
}

/*!Gets name of object.*/
QString SalomeApp_DataObject::name() const
{
  QString str;

  if ( myObject )
    str = myObject->GetName().c_str();

  if ( str.isEmpty() )
  {
    _PTR(SObject) refObj = referencedObject();
    if ( refObj )
      str = refObj->GetName().c_str();
  }

  if ( isReference() )
    {
      if ( !(QString(referencedObject()->GetName().c_str()).isEmpty()) )
	str = QString( "* " ) + str;
      else
	str = QString( "<Invalid Reference>" );
    }

  return str;
}

/*!Gets icon picture of object.*/
QPixmap SalomeApp_DataObject::icon() const
{
  _PTR(GenericAttribute) anAttr;
  if ( myObject && myObject->FindAttribute( anAttr, "AttributePixMap" ) ){
    _PTR(AttributePixMap) aPixAttr ( anAttr );
    if ( aPixAttr->HasPixMap() ){
      QString pixmapName = QObject::tr( aPixAttr->GetPixMap().c_str() );
      LightApp_RootObject* aRoot = dynamic_cast<LightApp_RootObject*>( root() );
      if ( aRoot && aRoot->study() ) {
	QPixmap pixmap = aRoot->study()->application()->resourceMgr()->loadPixmap( componentDataType(), pixmapName, false ); 
	return pixmap;
      }
    }
  }
  return QPixmap();
}

/*!Gets text value for one of entity:
 *\li Value           (id = SalomeApp_DataObject::CT_Value)
 *\li Entry           (id = SalomeApp_DataObject::CT_Entry)
 *\li IOR             (id = SalomeApp_DataObject::CT_IOR)
 *\li Reference entry (id = SalomeApp_DataObject::CT_RefEntry)
 */
QString SalomeApp_DataObject::text( const int id ) const
{
  QString txt;
  switch ( id )
  {
  case CT_Value:
#ifndef WNT
    if ( componentObject() != this )
#else
    if ( componentObject() != (SUIT_DataObject*)this )
#endif
      txt = value( referencedObject() );
    break;
  case CT_Entry:
    txt = entry( referencedObject() );
    break;
  case CT_IOR:
    txt = ior( referencedObject() );
    break;
  case CT_RefEntry:
    if ( isReference() )
      txt = entry( object() );
    break;
  }
  return txt;
}

/*!Get color value for one of entity:
 *\li Text color
 *\li Highlight color
 *\li Higlighted text color
 */
QColor SalomeApp_DataObject::color( const ColorRole cr ) const
{
  QColor clr;
  switch ( cr )
  {
  case Text:
    if ( isReference() )
      {
	if ( !(QString(referencedObject()->GetName().c_str()).isEmpty()) )
	  clr = QColor( 255, 0, 0 );
	else
	  clr = QColor( 200, 200, 200 );
      }
    else if ( myObject )
    {
      _PTR(GenericAttribute) anAttr;
      if ( myObject->FindAttribute( anAttr, "AttributeTextColor" ) )
      {
	_PTR(AttributeTextColor) aColAttr = anAttr;
	clr = QColor( (int)aColAttr->TextColor().R, (int)aColAttr->TextColor().G, (int)aColAttr->TextColor().B );
      }
    }
    break;
  case Highlight:
    if ( isReference() )
      {
	if ( !(QString(referencedObject()->GetName().c_str()).isEmpty()) )
	  clr = QColor( 255, 0, 0 );
	else
	  clr = QColor( 200, 200, 200 );
      }
    break;
  case HighlightedText:
    if ( isReference() )
      clr = QColor( 255, 255, 255 );
    break;
  }
  return clr;
}

/*!Gets tooltip.*/
QString SalomeApp_DataObject::toolTip() const
{
  //return object()->Name();
  return QString( "Object \'%1\', module \'%2\', ID=%3" ).arg( name() ).arg( componentDataType() ).arg( entry() );
}

/*!Get component type.*/
QString SalomeApp_DataObject::componentDataType() const
{
  const SalomeApp_DataObject* compObj = dynamic_cast<SalomeApp_DataObject*>( componentObject() );
  if ( compObj && compObj->object() )
  {
    _PTR(SComponent) aComp( compObj->object() );
    if ( aComp )
      return aComp->ComponentDataType().c_str();
  }

  return "";
}

/*!Gets object.*/
_PTR(SObject) SalomeApp_DataObject::object() const
{
  return myObject;
}

/*!Checks: Is object reference.*/
bool SalomeApp_DataObject::isReference() const
{
  bool isRef = false;
  if ( myObject )
  {
    _PTR(SObject) refObj;
    isRef = myObject->ReferencedObject( refObj );
  }
  return isRef;
}

/*!Gets reference object.*/
_PTR(SObject) SalomeApp_DataObject::referencedObject() const
{
  _PTR(SObject) refObj;
  _PTR(SObject) obj = myObject;
  while ( obj && obj->ReferencedObject( refObj ) )
    obj = refObj;

  return obj;
}

/*!Gets IOR*/
QString SalomeApp_DataObject::ior( const _PTR(SObject)& obj ) const
{
  QString txt;
  if ( obj )
  {
    _PTR(GenericAttribute) attr;
    if ( obj->FindAttribute( attr, "AttributeIOR" ) )
    {
      _PTR(AttributeIOR) iorAttr = attr;
      if ( iorAttr )
      {
	std::string str = iorAttr->Value();
	txt = QString( str.c_str() );
      }
    }
  }
  return txt;
}

/*!Gets Entry*/
QString SalomeApp_DataObject::entry( const _PTR(SObject)& obj ) const
{
  QString txt;
  if ( obj )
  {
    std::string str = obj->GetID();
    txt = QString( str.c_str() );
  }
  return txt;
}

/*!Value*/
QString SalomeApp_DataObject::value( const _PTR(SObject)& obj ) const
{
  if ( !obj )
    return QString::null;

  QString val;
  _PTR(GenericAttribute) attr;

  if ( obj->FindAttribute( attr, "AttributeInteger" ) )
  {
    _PTR(AttributeInteger) intAttr = attr;
    if ( intAttr )
      val = QString::number( intAttr->Value() );
  }
  else if ( obj->FindAttribute( attr, "AttributeReal" ) )
  {
    _PTR(AttributeReal) realAttr = attr;
    if ( realAttr )
      val = QString::number( realAttr->Value() );
  }
  else if ( obj->FindAttribute( attr, "AttributeTableOfInteger" ) )
  {
    _PTR(AttributeTableOfInteger) tableAttr = attr;
    std::string title = tableAttr->GetTitle();
    val = QString( title.c_str() );
    if ( !val.isEmpty() )
      val += QString( " " );
    val += QString( "[%1,%2]" ).arg( tableAttr->GetNbRows() ).arg( tableAttr->GetNbColumns() );
  }
  else if ( obj->FindAttribute( attr, "AttributeTableOfReal" ) )
  {
    _PTR(AttributeTableOfReal) tableAttr = attr;
    std::string title = tableAttr->GetTitle();
    val = QString( title.c_str() );
    if ( !val.isEmpty() )
      val += QString( " " );
    val += QString( "[%1,%2]" ).arg( tableAttr->GetNbRows() ).arg( tableAttr->GetNbColumns() );
  }
  else if ( obj->FindAttribute( attr, "AttributeComment") )
  {
    _PTR(AttributeComment) comm = attr;
    std::string str = comm->Value();
    val = QString( str.c_str() );
  }

  return val;
}

/*
	Class: SalomeApp_ModuleObject
	Level: Public
*/

/*!Constructor.Initialize by \a parent.*/
SalomeApp_ModuleObject::SalomeApp_ModuleObject( SUIT_DataObject* parent )
: SalomeApp_DataObject( parent ), 
  CAM_RootObject( parent ),
  CAM_DataObject( parent )
{
}

/*!Constructor.Initialize by \a parent and SObject.*/
SalomeApp_ModuleObject::SalomeApp_ModuleObject( const _PTR(SObject)& sobj, SUIT_DataObject* parent )
: SalomeApp_DataObject( sobj, parent ), 
  CAM_RootObject( 0, parent ),
  CAM_DataObject( parent )
{
}

/*!Constructor.Initialize by \a parent and CAM_DataModel.*/
SalomeApp_ModuleObject::SalomeApp_ModuleObject( CAM_DataModel* dm, const _PTR(SObject)& sobj, SUIT_DataObject* parent )
: SalomeApp_DataObject( sobj, parent ), 
  CAM_RootObject( dm, parent ),
  CAM_DataObject( parent )  
{
}

/*!Destructor. Do nothing.*/
SalomeApp_ModuleObject::~SalomeApp_ModuleObject()
{
}
