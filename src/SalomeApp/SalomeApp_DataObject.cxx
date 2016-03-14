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
// File   : SalomeApp_DataObject.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#include "SalomeApp_DataObject.h"
#include "SalomeApp_Study.h"
#include "SalomeApp_Application.h"

#include <CAM_DataObject.h>

#include <SUIT_Session.h>
#include <SUIT_Application.h>
#include <SUIT_ResourceMgr.h>

#include <SALOME_LifeCycleCORBA.hxx>
#include <Basics_Utils.hxx>

#include <QObject>
#include <QVariant>

//VSR: uncomment below macro to support unicode text properly in SALOME
//     current commented out due to regressions
//#define PAL22528_UNICODE

namespace
{
  QString fromUtf8( const char* txt )
  {
#ifdef PAL22528_UNICODE
    return QString::fromUtf8( txt );
#else
    return QString( txt );
#endif
  }

  QString fromUtf8( const std::string& txt )
  {
    return fromUtf8( txt.c_str() );
  }
}

/*!
  \class SalomeApp_DataObject
  \brief Implementation of the data object for use in CORBA-based
  SALOME modules.
*/

/*!
  \brief Constructor. 
  \param parent parent data object
*/
SalomeApp_DataObject::SalomeApp_DataObject( SUIT_DataObject* parent )
: CAM_DataObject( parent ),
  LightApp_DataObject( parent )
{
}

/*!
  \brief Constructor. 
  \param sobj SALOMEDS object
  \param parent parent data object
*/
SalomeApp_DataObject::SalomeApp_DataObject( const _PTR(SObject)& sobj, 
                                            SUIT_DataObject* parent )
: CAM_DataObject( parent ),
  LightApp_DataObject( parent )
{
  myObject = sobj;
}

/*!
  \brief Destructor.
*/
SalomeApp_DataObject::~SalomeApp_DataObject()
{
}

/*!
  \brief Get data object name.
  \return object name
*/
QString SalomeApp_DataObject::name() const
{
  QString str;
  if ( myObject )
    str = fromUtf8( myObject->GetName() );
  
  if ( str.isEmpty() ) {
    _PTR(SObject) refObj = referencedObject();
    if ( refObj )
      str = fromUtf8( refObj->GetName() );
  }
  
  if ( isReference() ) {
    if ( !(QString(referencedObject()->GetName().c_str()).isEmpty()) )
      str = QString( "* " ) + str;
    else
      str = QString( "<Invalid Reference>" );
  }
  return str;
}

/*!
  \brief Get object string identifier.
  \return object ID
*/
QString SalomeApp_DataObject::entry() const
{
  return entry( myObject );
}

/*!
  \brief Get object text data for the specified column.

  This method returns the data according to the specufied column \a id:
  - NameId     : object name (by calling name() method)
  - EntryId    : object entry (by calling entry() method)
  - ValueId    : object value
  - IORId      : object IOR
  - RefEntryId : object reference entry

  \param id column id
  \return object text data
*/
QString SalomeApp_DataObject::text( const int id ) const
{
  QString txt;

  // Text for "Value" and "IOR" columns
  switch ( id )
  {
  case ValueId:
#ifndef WIN32
    if ( componentObject() != this )
#else
    if ( componentObject() != (SUIT_DataObject*)this )
#endif
      txt = value( object() );
      if ( txt.isEmpty() )
        txt = value( referencedObject() );
    break;
  case IORId:
    txt = ior( referencedObject() );
    break;
  default:
    // Issue 21379: LightApp_DataObject::text() treats "Entry"
    // and "Reference Entry" columns
    txt = LightApp_DataObject::text( id );
    break;
  }
  return txt;
}

/*!
  \brief Get data object icon for the specified column.
  \param id column id
  \return object icon for the specified column
*/
QPixmap SalomeApp_DataObject::icon( const int id ) const
{
  // we display icon only for the first (NameId ) column
  if ( id == NameId ) {
    _PTR(GenericAttribute) anAttr;
    if ( myObject && myObject->FindAttribute( anAttr, "AttributePixMap" ) ){
      _PTR(AttributePixMap) aPixAttr ( anAttr );
      if ( aPixAttr->HasPixMap() ) {
        QString componentType = componentDataType();
        QString pixmapID      = fromUtf8( aPixAttr->GetPixMap() );
        // select a plugin within a component
        QStringList plugin_pixmap = pixmapID.split( "::", QString::KeepEmptyParts );
        if ( plugin_pixmap.size() == 2 ) {
          componentType = plugin_pixmap.front();
          pixmapID      = plugin_pixmap.back();
        }
        QString pixmapName = QObject::tr( pixmapID.toLatin1().constData() );
        LightApp_RootObject* aRoot = dynamic_cast<LightApp_RootObject*>( root() );
        if ( aRoot && aRoot->study() ) {
          SUIT_ResourceMgr* mgr = aRoot->study()->application()->resourceMgr();
          return mgr->loadPixmap( componentType, pixmapName, false ); 
        }
      }
    }
  }
  return LightApp_DataObject::icon( id );
}

/*!
  \brief Get data object color for the specified column.
  \param role color role
  \param id column id (not used)
  \return object color for the specified column
*/
QColor SalomeApp_DataObject::color( const ColorRole role, const int id ) const
{
  // we ignore parameter <id> in order to use the same colors for 
  // all columns
  QColor c;
  switch ( role )
  {
  case Text:
  case Foreground:
    // text color (not selected item)
    if ( isReference() ) {
      if ( QString(referencedObject()->GetName().c_str()).isEmpty() )
        c = QColor( 200, 200, 200 );  // invalid reference (grayed)
    }
    else if ( myObject ) {
      // get color atrtribute value
      _PTR(GenericAttribute) anAttr;
      if ( myObject->FindAttribute( anAttr, "AttributeTextColor" ) ) {
        _PTR(AttributeTextColor) aColAttr = anAttr;
        c = QColor( (int)aColAttr->TextColor().R, (int)aColAttr->TextColor().G, (int)aColAttr->TextColor().B );
      }
    }
    break;

  case Highlight:
    // background color for the highlighted item
    if ( isReference() ) {
      if ( QString(referencedObject()->GetName().c_str()).isEmpty() )
        c = QColor( 200, 200, 200 );  // invalid reference (grayed)
    }
    else if ( myObject ) {
      // get color atrtribute value
      _PTR(GenericAttribute) anAttr;
      if( myObject->FindAttribute ( anAttr, "AttributeTextHighlightColor") ) {
        _PTR(AttributeTextHighlightColor) aHighColAttr = anAttr;
        c = QColor( (int)(aHighColAttr->TextHighlightColor().R), 
                    (int)(aHighColAttr->TextHighlightColor().G), 
                    (int)(aHighColAttr->TextHighlightColor().B));
      }
    }
    break;
  default:
    break;
  }

  // Issue 21379: LightApp_DataObject::color() defines colors for valid references
  if ( !c.isValid() )
    c = LightApp_DataObject::color( role, id );

  return c;
}

/*!
  \brief Get data object tooltip for the specified column.
  \param id column id (not used)
  \return object tooltip for the specified column
*/
QString SalomeApp_DataObject::toolTip( const int /*id*/ ) const
{
  // we ignore parameter <id> in order to use the same tooltip for 
  // all columns
  
  // Get customized tooltip in case of it exists
  const SalomeApp_DataObject* compObj = dynamic_cast<SalomeApp_DataObject*>( componentObject() );
  // Check if the component has been loaded.
  // In order to avoid loading the component only for getting a custom tooltip.
  if ( compObj && compObj != this && !ior(compObj->object()).isEmpty() ) {
    SalomeApp_Application* app = 
      dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
    if ( app ) {
      // --- try to find (and not load) the component instance, like GEOM instance,
      //     registered in naming service under Containers/<hostname>/...
      //     with any container name, on every machine available
      Engines::ContainerParameters params;
      app->lcc()->preSet(params); // --- any container name, anywhere
      Engines::EngineComponent_var aComponent =
        app->lcc()->FindComponent(params, componentDataType().toLatin1().constData() );
      
      if ( !CORBA::is_nil(aComponent) && aComponent->hasObjectInfo() ) {
        LightApp_RootObject* aRoot = dynamic_cast<LightApp_RootObject*>( root() );
        if ( aRoot && aRoot->study() ) {
          CORBA::String_var data = aComponent->getObjectInfo( aRoot->study()->id(), entry().toLatin1().constData());
          QString objInfo = data.in();
          QStringList l;
          l << name();
          if ( !objInfo.isEmpty() )
            l << objInfo;
          return l.join("\n");
        }
      }
    }
  }
  
  return QString( "Object \'%1\', module \'%2\', ID=%3" ).arg( name() ).arg( componentDataType() ).arg( entry() );
}

/*!
  \brief Get font to be used for data object rendering in the item views
  \param id column id
  \return font to be used for the item rendering
*/
QFont SalomeApp_DataObject::font( const int id ) const
{
  QFont f = LightApp_DataObject::font( id );
  if ( id == NameId ) {
    if ( !expandable() && hasChildren() ) {
      // set bold font to highlight the item which is non-expandable but has children
      f.setBold( true );
    }
  }
  return f;
}

/*!
  \brief Get component type.
  \return component type
*/
QString SalomeApp_DataObject::componentDataType() const
{
  //  if ( myCompDataType.isEmpty() ) {
    const SalomeApp_DataObject* compObj = dynamic_cast<SalomeApp_DataObject*>( componentObject() );
    if ( compObj && compObj->object() )
    {
      _PTR(SComponent) aComp( compObj->object() );
      if ( aComp ) {
        SalomeApp_DataObject* that = (SalomeApp_DataObject*)this;
        that->myCompDataType = aComp->ComponentDataType().c_str();
      }
    }
    //  }
  return myCompDataType;
}

/*!
  \brief Get SALOMEDS object.
  \return SALOMEDS object
*/
_PTR(SObject) SalomeApp_DataObject::object() const
{
  return myObject;
}

/*!
  \brief Returns the string identifier of the data objects referenced by this one.

  Re-implemented from LightApp_DataObject using SALOMEDS API.

  \return ID string of the referenced SObject
*/
QString SalomeApp_DataObject::refEntry() const
{
  return entry( referencedObject() );
}

/*!
  \brief Check if the data object is a reference.

  Re-implemented from LightApp_DataObject using SALOMEDS API.

  \return \c true if this data object actually refers to another one
*/
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

/*!
  \brief Get the object referenced by this one.
  \return referenced object
*/
_PTR(SObject) SalomeApp_DataObject::referencedObject() const
{
  _PTR(SObject) refObj;
  _PTR(SObject) obj = myObject;
  while ( obj && obj->ReferencedObject( refObj ) )
    obj = refObj;

  return obj;
}

/*!
  \brief Check if object has children
  \return \c true if object has at least one child sub-object and \c false otherwise
*/
bool SalomeApp_DataObject::hasChildren() const
{
  bool ok = false;

  // tmp??
  _PTR(UseCaseBuilder) aUseCaseBuilder = myObject->GetStudy()->GetUseCaseBuilder();
  if (aUseCaseBuilder->IsUseCaseNode(myObject)) {
    ok = aUseCaseBuilder->HasChildren(myObject);
    // TODO: check name as below?
  }
  else {
    _PTR(ChildIterator) it ( myObject->GetStudy()->NewChildIterator( myObject ) );
    for ( ; it->More() && !ok; it->Next() ) {
      _PTR(SObject) obj = it->Value();
      if ( obj ) {
        _PTR(SObject) refObj;
        //if ( obj->ReferencedObject( refObj ) ) continue; // omit references
        if ( obj->GetName() != "" ) ok = true;
      }
    }
  }
  return ok;
}

/*!
  \brief Check if the object is expandable (e.g. in the data tree view)
  \return \c true if object is expandable and \c false otherwise
*/
bool SalomeApp_DataObject::expandable() const
{
  bool exp = true;
  _PTR(GenericAttribute) anAttr;
  if ( myObject && myObject->FindAttribute( anAttr, "AttributeExpandable" ) ) {
    _PTR(AttributeExpandable) aAttrExp = anAttr;
    exp = aAttrExp->IsExpandable();
  }
  return exp;
}

/*!
  \brief Check if the object is visible.
  \return \c true if this object is displayable or \c false otherwise
*/
bool SalomeApp_DataObject::isVisible() const
{
  bool isDraw = true;
  _PTR(GenericAttribute) anAttr;
  if ( myObject && myObject->FindAttribute(anAttr, "AttributeDrawable") ) 
  {
    _PTR(AttributeDrawable) aAttrDraw = anAttr;
    isDraw = aAttrDraw->IsDrawable(); 
  }
  return isDraw && LightApp_DataObject::isVisible() && ( !name().isEmpty() || isReference() );
}

/*!
  \brief Check if the specified column supports custom sorting.
  \param id column id
  \return \c true if column sorting should be customized
  \sa compare()
*/
bool SalomeApp_DataObject::customSorting( const int id ) const
{
  // perform custom sorting for the "Entry" and "Reference Entry" columns
  return id == EntryId  || id == RefEntryId  ? true 
    : LightApp_DataObject::customSorting( id );
}

/*!
  \brief Compares data from two items for sorting purposes.

  This method is called only for those columns for which customSorting()
  method returns \c true.

  \param left first data to compare
  \param right second data to compare
  \param id column id
  \return result of the comparison
  \sa customSorting()
*/
bool SalomeApp_DataObject::compare( const QVariant& left, const QVariant& right, const int id ) const
{
  // use the same custom sorting for the "Reference Entry" column as for the
  // "Entry" column (call base implementation)
  return LightApp_DataObject::compare( left, right, id == RefEntryId ? EntryId : id );
}

/*!
  \brief Get data object IOR.
  \param obj data object
  \return data object IOR or null string if IOR is empty
*/
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

/*!
  \brief Get data object entry identifier.
  \param obj data object
  \return data object entry identifier or empty object does not have entry
*/
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

/*!
  \brief Get data object value.
  \param obj data object
  \return data object value or empty string if there is no 
  value associated to the object
*/
QString SalomeApp_DataObject::value( const _PTR(SObject)& obj ) const
{
  if ( !obj )
    return QString();

  QString val;
  _PTR(GenericAttribute) attr;

  if ( obj->FindAttribute( attr, "AttributeString" ) )
  {
    _PTR(AttributeString) strAttr = attr;
    std::string str = strAttr->Value();
    QString aStrings = fromUtf8( str );
    
    //Special case to show NoteBook variables in the "Value" column of the OB 
    if ( LightApp_RootObject* aRoot = dynamic_cast<LightApp_RootObject*>( root() ) )
    {
      if ( SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( aRoot->study() ) )
      {
        _PTR(Study) studyDS( aStudy->studyDS() );

        bool ok = false;
        QStringList aSectionList = aStrings.split( "|" );
        if ( !aSectionList.isEmpty() )
        {
          QString aLastSection = aSectionList.last();
          QStringList aStringList = aLastSection.split( ":" );
          if ( !aStringList.isEmpty() )
          {
            ok = true;
            for ( int i = 0, n = aStringList.size(); i < n; i++ )
            {
              QString aStr = aStringList[i];
              if ( studyDS->IsVariable( aStr.toStdString() ) )
                val.append( aStr + ", " );
            }

            if ( !val.isEmpty() )
              val.remove( val.length() - 2, 2 );
          }
        }
        if( !ok )
          val = aStrings;
      }
    }
  }
  else if ( obj->FindAttribute( attr, "AttributeInteger" ) )
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
  else if ( obj->FindAttribute( attr, "AttributeComment" ) )
  {
    _PTR(AttributeComment) comm = attr;
    std::string str = comm->Value();
    val = fromUtf8( str );
  }

  return val;
}

void SalomeApp_DataObject::insertChildAtTag( SalomeApp_DataObject* obj, int tag )
{
  int pos = 0;
  int npos = qMin( tag-1,childCount() );
  for ( int i = npos; i > 0; i-- )
  {
    if ( (dynamic_cast<SalomeApp_DataObject*>( childObject( i-1 ) ) )->object()->Tag() < tag )
    {
      pos = i;
      break;
    }
  }
  insertChildAtPos( obj, pos );
}

void SalomeApp_DataObject::updateItem()
{
  if ( modified() ) return;
  setModified( true );
}

/*!
  \class SalomeApp_ModuleObject
  \brief This class is used for optimized access to the SALOMEDS-based 
  data model from SalomeApp_DataObject class instances.
  \sa CAM_ModuleObject class
*/

/*!
  \brief Constructor.
  \param parent parent data object
*/
SalomeApp_ModuleObject::SalomeApp_ModuleObject( SUIT_DataObject* parent )
: CAM_DataObject( parent ),
  LightApp_DataObject( parent ),
  SalomeApp_DataObject( parent ),
  CAM_ModuleObject( parent )
{
}

/*!
  \brief Constructor.
  \param sobj SALOMEDS object
  \param parent parent data object
*/
SalomeApp_ModuleObject::SalomeApp_ModuleObject( const _PTR(SObject)& sobj, 
                                                SUIT_DataObject* parent )
: CAM_DataObject( parent ),
  LightApp_DataObject( parent ),
  SalomeApp_DataObject( sobj, parent ),
  CAM_ModuleObject( parent )
{
}

/*!
  \brief Constructor.
  \param dm data model
  \param sobj SALOMEDS object
  \param parent parent data object
*/
SalomeApp_ModuleObject::SalomeApp_ModuleObject( CAM_DataModel* dm, 
                                                const _PTR(SObject)& sobj, 
                                                SUIT_DataObject* parent )
: CAM_DataObject( parent ),
  LightApp_DataObject( parent ),
  SalomeApp_DataObject( sobj, parent ),
  CAM_ModuleObject( dm, parent )
{
}

/*!
  \brief Destructor.
*/
SalomeApp_ModuleObject::~SalomeApp_ModuleObject()
{
}

/*!
  \brief Get module name.
  \return module name
*/
QString SalomeApp_ModuleObject::name() const
{
  return SalomeApp_DataObject::name();
}

/*!
  \brief Get data object icon for the specified column.
  \param id column id
  \return object icon for the specified column
  \sa CAM_ModuleObject class
*/
QPixmap SalomeApp_ModuleObject::icon( const int id ) const
{
  QPixmap p = SalomeApp_DataObject::icon( id );
  // The module might not provide a separate small icon
  // for Obj. Browser representation -> always try to scale it
  // See CAM_ModuleObject::icon()
  if ( !p.isNull() )
    p = Qtx::scaleIcon( p, 16 );
  return p;
}

/*!
  \brief Get data object tooltip for the specified column.
  \param id column id
  \return object tooltip for the specified column
*/
QString SalomeApp_ModuleObject::toolTip( const int id ) const
{
  return SalomeApp_DataObject::toolTip( id );
}

/*!
  \class SalomeApp_RootObject
  \brief Root data object for the CORBA-based SALOME application.

  This class is to be instanciated by only one object - the root object
  of the SalomeApp data object tree. This object is not shown in the object browser.
  The goal of this class is to provide a unified access to SalomeApp_Study
  object from SalomeApp_DataObject instances.
*/

/*!
  \brief Constructor.
  \param study pointer to the study
*/
SalomeApp_RootObject::SalomeApp_RootObject( LightApp_Study* study )
: CAM_DataObject( 0 ),
  LightApp_DataObject( 0 ),
  SalomeApp_DataObject( 0 ),
  LightApp_RootObject( study ),
  _toSynchronize(true)
{
}

/*!
  \brief Destructor.
*/
SalomeApp_RootObject::~SalomeApp_RootObject()
{
}

/*!
  \brief Get data object name.
  \return object name
*/
QString SalomeApp_RootObject::name() const
{
  return LightApp_RootObject::name();
}
 
/*!
  \brief Get object string identifier.
  \return object ID
*/
QString SalomeApp_RootObject::entry() const
{
  return LightApp_RootObject::entry();
}

/*!
  \brief Get object text data for the specified column.
  \param id column id
  \return object text data
*/
QString SalomeApp_RootObject::text( const int id ) const
{
  return LightApp_RootObject::text( id );
}

/*!
  \brief Get data object icon for the specified column.
  \param id column id
  \return object icon for the specified column
*/
QPixmap SalomeApp_RootObject::icon( const int id ) const
{
  return LightApp_RootObject::icon( id );
}

/*!
  \brief Get data object color for the specified column.
  \param role color role
  \param id column id (not used)
  \return object color for the specified column
*/
QColor SalomeApp_RootObject::color( const ColorRole role, const int id ) const
{
  return LightApp_RootObject::color( role, id );
}

/*!
  \brief Get data object tooltip for the specified column.
  \param id column id (not used)
  \return object tooltip for the specified column
*/
QString SalomeApp_RootObject::toolTip( const int id ) const
{
  return LightApp_RootObject::toolTip( id );
}

/*!
  \class SalomeApp_SavePointObject
  \brief Represents persistent visual_state object.

  Save point objects are stored in the data model, but NOT in SObjects
  structure, so they are handled separately using this special class
*/

/*!
  \brief Constructor.
  \param parent parent data object
  \param id save point ID
  \param study study
*/
SalomeApp_SavePointObject::SalomeApp_SavePointObject( SUIT_DataObject* parent, 
                                                      const int id, 
                                                      SalomeApp_Study* study )
: LightApp_DataObject( parent ), 
  CAM_DataObject( parent ),
  myId( id ),
  myStudy( study )
{
}

/*!
  \brief Destructor.
*/
SalomeApp_SavePointObject::~SalomeApp_SavePointObject()
{
}

/*!
  \brief Get save point unique identifier.
  \return save point ID
*/
int SalomeApp_SavePointObject::getId() const
{
  return myId;
}

/*!
  \brief Get object string identifier.
  \return object ID
*/
QString SalomeApp_SavePointObject::entry() const
{
  return QObject::tr( "SAVE_POINT_DEF_NAME" ) + QString::number( myId );
}

/*!
  \brief Get data object name.
  \return object name
*/
QString SalomeApp_SavePointObject::name() const
{
  return myStudy->getNameOfSavePoint( myId );
}

/*!
  \brief Get data object icon for the specified column.
  \param id column id
  \return object icon for the specified column
*/
QPixmap SalomeApp_SavePointObject::icon( const int /*id*/ ) const
{
  return QPixmap();
}

/*!
  \brief Get data object tooltip for the specified column.
  \param id column id (not used)
  \return object tooltip for the specified column
*/
QString SalomeApp_SavePointObject::toolTip( const int /*id*/ ) const
{
  return QObject::tr( "SAVE_POINT_OBJECT_TOOLTIP" ).arg( name() );
}

/*!
  \class SalomeApp_SavePointRootObject
  \brief Represents parent object for visual_state objects.
*/

/*!
  \brief Constructor.
  \param parent parent object
*/
SalomeApp_SavePointRootObject::SalomeApp_SavePointRootObject( SUIT_DataObject* parent )
: SUIT_DataObject( parent )
{
}

/*!
  \brief Get data object name.
  \return object name
*/
QString SalomeApp_SavePointRootObject::name() const
{
  return QObject::tr( "SAVE_POINT_ROOT_NAME" ); 
}

/*!
  \brief Get data object tooltip for the specified column.
  \param id column id (not used)
  \return object tooltip for the specified column
*/
QString SalomeApp_SavePointRootObject::toolTip( const int /*id*/ ) const
{
  return QObject::tr( "SAVE_POINT_ROOT_TOOLTIP" ); 
}
