//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File   : LightApp_DataObject.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#include "LightApp_DataObject.h"
#include "LightApp_Study.h"
#include "LightApp_DataModel.h"

#include <CAM_Module.h>
#include <SUIT_DataObjectKey.h>

#include <QVariant>

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

  Column with \a id = 0 (NameId) is supposed to be used
  to get the object name.
  Column with \a id = 1 (EntryId) is supposed to be used
  to get the object entry.

  \param id column id
  \return object text data
*/
QString LightApp_DataObject::text( const int id ) const
{
  return id == EntryId ? entry() : CAM_DataObject::text( id );
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
    LightApp_ModuleObject* anObj = dynamic_cast<LightApp_ModuleObject*>( aCompObj );
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
	  return lid < rid;
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
