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

// File   : LightApp_DataObject.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#include "LightApp_DataObject.h"
#include "LightApp_Study.h"
#include "LightApp_DataModel.h"
#include "LightApp_Module.h"
#include "LightApp_Application.h"

#include <CAM_Module.h>
#include <SUIT_DataObjectKey.h>

#include <QVariant>

#include <iostream>

/*!
  \class LightApp_DataObject::Key
  \brief Represents unique data object key for the LightApp_DataObject
  class instances.
  \internal
*/

class LightApp_DataObject::Key : public SUIT_DataObjectKey
{
public:
  Key( const QString& );
  virtual ~Key();

  virtual bool isLess( const SUIT_DataObjectKey* ) const;
  virtual bool isEqual( const SUIT_DataObjectKey* ) const;

private:
  QString myEntry;
};

/*!
  \brief Constructor.
  \internal
  \param entry data object entry
*/
LightApp_DataObject::Key::Key( const QString& entry )
: SUIT_DataObjectKey(),
  myEntry( entry )
{
}

/*!
  \brief Destructor.
  \internal
*/
LightApp_DataObject::Key::~Key()
{
}

/*!
  \brief Compares this key with the another one.
  \internal
  \param other another data object key
  \return \c true if this key is less than \a other.
*/
bool LightApp_DataObject::Key::isLess( const SUIT_DataObjectKey* other ) const
{
  Key* that = (Key*)other;
  return myEntry < that->myEntry;
}

/*!
  \brief Compares this key with the another one.
  \internal
  \param other another data object key
  \return \c true if this key is equal to the \a other.
*/
bool LightApp_DataObject::Key::isEqual( const SUIT_DataObjectKey* other ) const
{
  Key* that = (Key*)other;
  return myEntry == that->myEntry;
}

/*!
  \class LightApp_DataObject
  \brief Base data object class to build the data model for all the SALOME modules.
*/

/*!
  \brief Constructor.
  \param parent parent data object
*/
LightApp_DataObject::LightApp_DataObject( SUIT_DataObject* parent )
: CAM_DataObject( parent ),
  myCompObject( 0 ),
  myCompDataType( "" )
{
}

/*!
  \brief Destructor.
*/
LightApp_DataObject::~LightApp_DataObject()
{
}

int LightApp_DataObject::groupId() const
{
  LightApp_DataModel* m = dynamic_cast<LightApp_DataModel*>( dataModel() );
  return m ? m->groupId() : CAM_DataObject::groupId();
}

/*!
  \brief return custom data for data object
*/
QVariant LightApp_DataObject::customData(Qtx::CustomDataType type) {
  switch(type) {
  case Qtx::IdType:
    return EntryId;
    break;
  default:
    return QVariant();
    break;
  }
}

/*!
  \brief Check if the object is visible.
  \return \c true if this object is displayable or \c false otherwise
*/
bool LightApp_DataObject::isVisible() const
{
  LightApp_RootObject* r = dynamic_cast<LightApp_RootObject*>( root() );
  return r && r->study() && componentDataType() != r->study()->getVisualComponentName();
}

/*!
  \brief Check if the object is draggable.

  This method can be re-implemented in the subclasses.

  \return \c true if it is possible to drag this object
*/
bool LightApp_DataObject::isDraggable() const
{
  LightApp_Module* aModule = dynamic_cast<LightApp_Module*>(module());
  if (aModule) {
    return aModule->isDraggable(this);
  }
  return false;
}

/*!
  \brief Check if the drop operation fo this object is possible.

  This method can be re-implemented in the subclasses.

  \param obj object being dropped
  \return \c true if it is possible to drop an object \c obj
          to this object
*/
bool LightApp_DataObject::isDropAccepted() const
{
  LightApp_Module* aModule = dynamic_cast<LightApp_Module*>(module());
  if (aModule) {
    return aModule->isDropAccepted(this);
  }
  return false;
}

/*!
  \brief Check if this object is can't be renamed in place

  This method can be re-implemented in the subclasses.
  Default implementation returns \c false (all objects can not be renamed).

  \param id column id
  \return \c true if the item can be renamed by the user in place (e.g. in the Object browser)
*/
bool LightApp_DataObject::renameAllowed( const int id ) const
{
  if ( id == NameId ) {
    LightApp_Module* m = dynamic_cast<LightApp_Module*>( module() );
    LightApp_Application* app = 0;
    LightApp_RootObject* r = dynamic_cast<LightApp_RootObject*>( root() );
    if(r && r->study())
      app  = dynamic_cast<LightApp_Application*>(r->study()->application());

    return ( m && m->renameAllowed( entry() ) ) ||
      ( app && app->renameAllowed( entry() ) );
  }
  return CAM_DataObject::renameAllowed( id );
}


/*!
  \brief Set name of the this object.

  This method can be re-implemented in the subclasses.
  Default implementation returns \c false.

  \return \c true if rename operation finished successfully, \c false otherwise.
*/
bool LightApp_DataObject::setName(const QString& name)
{
    LightApp_Module* m = dynamic_cast<LightApp_Module*>( module() );
    LightApp_RootObject* r = dynamic_cast<LightApp_RootObject*>( root() );
    LightApp_Application* app =
      (r && r->study()) ? dynamic_cast<LightApp_Application*>(r->study()->application()) : 0;

    return ( m && m->renameObject( entry(), name ) ) ||
           ( app && app->renameObject( entry(), name ) );
  return CAM_DataObject::setName(name);
}


/*!
  \brief Get object string identifier.

  This method should be reimplemented in the subclasses.
  Default implementation returns null string.

  \return object ID
*/
QString LightApp_DataObject::entry() const
{
  return QString();
}

/*!
  \brief Returns the string identifier of the data objects referenced by this one.

  This method should be reimplemented in the subclasses.
  Default implementation returns null string.

  \return ID string of the referenced data object
*/
QString LightApp_DataObject::refEntry() const
{
  return QString();
}

/*!
  \brief Tells if this data objects is a reference to some other or not.

  The base implementation retuns true, if refEntry() returns non-empty string.

  \return true if refEntry() is a non-empty string.
*/
bool LightApp_DataObject::isReference() const
{
  return !refEntry().isEmpty();
}

/*!
  \brief Get the data object unique key.
  \return data object key
*/
SUIT_DataObjectKey* LightApp_DataObject::key() const
{
  QString str = entry();
  return new Key( str );
}

/*!
  \brief Get object text data for the specified column.

  Column with \a id == NameId is supposed to be used
  to get the object name.
  Column with \a id == EntryId is supposed to be used
  to get the object entry.
  Column with \a id == RefEntryId is supposed to be used
  to show the entry of the object referenced by this one.

  \param id column id
  \return object text data
*/
QString LightApp_DataObject::text( const int id ) const
{
  QString txt;
  
  switch ( id )
  {
  case EntryId:
    txt = entry();
    break;
  case RefEntryId:
    // Issue 21379: reference support at LightApp level
    if ( isReference() )
      txt = refEntry();
    break;
  default:
    // Issue 21379: Note that we cannot return some specially decorated
    // name string (like "* ref_obj_name") when isReference() returns true, 
    // since there is no generic way at LightApp level
    // to query the object name using refEntry() up to now.
    // TODO: Think how to make reference name generation
    // more generic at move it here from SalomeApp level...
    txt = CAM_DataObject::text( id );
    break;
  }

  return txt;
}

/*!
  \brief Get data object color for the specified column.
  \param role color role
  \param id column id (not used)
  \return object color for the specified column
*/
QColor LightApp_DataObject::color( const ColorRole role, const int id) const
{
  QColor c;

  // Issue 21379: reference support at LightApp level
  // Centralized way for choosing text/background color for references.
  // Colors for "normal" objects should be chosen by sub-classes.
  switch ( role )
  {
  case Text:
  case Foreground:
    // text color (not selected item)
    // TODO: think how to detect invalid references...
    if ( isReference() )
      c = QColor( 255, 0, 0 );      // valid reference (red)
    break;

  case Highlight:
    // background color for the highlighted item
    // TODO: think how to detect invalid references...
    if ( isReference() ) 
      c = QColor( 255, 0, 0 );      // valid reference (red)
    break;

  case HighlightedText:
    // text color for the highlighted item
    if ( isReference() )
      c = QColor( 255, 255, 255 );   // white
    break;

  default:
    break;
  }

  if ( !c.isValid() )
    c = CAM_DataObject::color( role, id );

  return c;
}

/*!
  \brief Get the component object.
  \return component data object
*/
SUIT_DataObject* LightApp_DataObject::componentObject() const
{
  if ( !myCompObject ) {
    SUIT_DataObject* compObj = (SUIT_DataObject*)this;

    while ( compObj && compObj->parent() && compObj->parent() != root() ) {
      compObj = compObj->parent();
    }
    LightApp_DataObject* that = (LightApp_DataObject*)this;
    that->myCompObject = compObj;
  }
  return myCompObject;
}

/*!
  \brief Get component type.
  \return component type
*/
QString LightApp_DataObject::componentDataType() const
{
  if ( myCompDataType.isEmpty() ) {
    SUIT_DataObject* aCompObj = componentObject();
    CAM_ModuleObject* anObj = dynamic_cast<CAM_ModuleObject*>( aCompObj );
    if ( anObj ) {
      CAM_DataModel* aModel = anObj->dataModel();
      if ( aModel ) {
        LightApp_DataObject* that = (LightApp_DataObject*)this;
        that->myCompDataType = aModel->module()->name();
      }
    }
  }
  return myCompDataType;
}

/*!
  \brief Check if the specified column supports custom sorting.
  \param id column id
  \return \c true if column sorting should be customized
  \sa compare()
*/
bool LightApp_DataObject::customSorting( const int id ) const
{
  // perform custom sorting for the "Entry" column
  return id == EntryId ? true : CAM_DataObject::customSorting( id );
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
bool LightApp_DataObject::compare( const QVariant& left, const QVariant& right, const int id ) const
{
  if ( id == EntryId )
  {
    // perform custom sorting for the "Entry" column
    QString leftStr  = left.toString();
    QString rightStr = right.toString();
    QStringList idsLeft  = leftStr.split( ":", QString::SkipEmptyParts );
    QStringList idsRight = rightStr.split( ":", QString::SkipEmptyParts );
    if ( idsLeft.count() > 1 || idsRight.count() > 1 ) {
      bool result = true;
      bool calculated = false;
      for ( int i = 0; i < idsLeft.count() || i < idsRight.count(); i++ ) {
        bool okLeft = true, okRight = true;
        int lid = 0, rid = 0;
        if ( i < idsLeft.count() )
          lid = idsLeft[i].toInt( &okLeft );
        if ( i < idsRight.count() )
          rid = idsRight[i].toInt( &okRight );
        if ( okLeft && okRight ) {
          // both seem to be correct integer ID
	  if ( lid < rid ) return true;
        }
        else if ( okLeft || okRight ) {
          // objects with correct (int) ID have higher priority
          return okLeft;
        }
        else {
          // both not integer ID
          int r = QString::localeAwareCompare( idsLeft[i], idsRight[i] );
          if ( !calculated && r != 0 ) {
            result = r < 0;
            calculated = true;
          }
        }
      }
      // we should reach this if the entries are exactly equal
      return result;
    }
    return QString::localeAwareCompare( leftStr, rightStr ) < 0;
  }
  return CAM_DataObject::compare( left, right, id );
}

/*!
  \class LightApp_ModuleObject
  \brief Used for optimized access to the data model from the data objects.
  \sa CAM_ModuleObject class
*/

/*!
  \brief Constructor.
  \param parent parent data object
*/
LightApp_ModuleObject::LightApp_ModuleObject( SUIT_DataObject* parent )
: CAM_DataObject( parent ),
  LightApp_DataObject( parent ),
  CAM_ModuleObject( parent )
{
}

/*!
  \brief Constructor.
  \param dm data model
  \param parent parent data object
*/
LightApp_ModuleObject::LightApp_ModuleObject( CAM_DataModel* dm, SUIT_DataObject* parent )
: CAM_DataObject( parent ),
  LightApp_DataObject( parent ),
  CAM_ModuleObject( dm, parent )
{
}

/*
  \brief Destructor.
*/
LightApp_ModuleObject::~LightApp_ModuleObject()
{
}

/*!
  \brief Get module name.
  \return module name
*/
QString LightApp_ModuleObject::name() const
{
  return CAM_ModuleObject::name();
}

/*!
  \brief Get data object icon for the specified column.
  \param id column id
  \return object icon for the specified column
*/
QPixmap LightApp_ModuleObject::icon( const int id ) const
{
  return CAM_ModuleObject::icon( id );
}

/*!
  \brief Get data object tooltip for the specified column.
  \param id column id
  \return object tooltip for the specified column
*/
QString LightApp_ModuleObject::toolTip( const int id ) const
{
  return CAM_ModuleObject::toolTip( id );
}

/*!
  \brief Insert new child object to the children list at specified position.

  Adds component in the study for this module object if it is not done yet.

  \param obj object to be inserted
  \param pos position at which data object should be inserted
*/
void LightApp_ModuleObject::insertChild( SUIT_DataObject* obj, int pos )
{
  LightApp_DataObject::insertChild( obj, pos );

  CAM_DataModel* aModel = dataModel();

  LightApp_RootObject* aRoot = dynamic_cast<LightApp_RootObject*>( parent() );

  if ( aRoot )
    aRoot->study()->addComponent( aModel );
}

/*!
  \class LightApp_RootObject
  \brief Root data object for the light (without CORBA) SALOME application.

  This class is to be instanciated by only one object - the root object
  of the LightApp data object tree. This object is not shown in the object browser.
  The goal of this class is to provide a unified access to LightApp_Study
  object from LightApp_DataObject instances.
*/

/*
  \brief Constructor.
  \param study study
*/
LightApp_RootObject::LightApp_RootObject( LightApp_Study* study )
: CAM_DataObject( 0 ),
  LightApp_DataObject( 0 ),
  myStudy( study )
{
}

/*
  \brief Destructor.
*/
LightApp_RootObject::~LightApp_RootObject()
{
}

/*
  \brief Set study.
  \param study pointer to the study
*/
void LightApp_RootObject::setStudy( LightApp_Study* study )
{
  myStudy = study;
}

/*
  \brief Get study
  \return pointer to the study
*/
LightApp_Study* LightApp_RootObject::study() const
{
  return myStudy;
}
