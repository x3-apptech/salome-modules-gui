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

#include "CAM_DataObject.h"

#include "CAM_Module.h"
#include "CAM_DataModel.h"

#include <Qtx.h>

/*!
  \class CAM_DataObject
  \brief CAM-based implementation of the data object.
  
  In addition to base implementation provides integration
  with CAM_DataModel.
*/

/*!
  \brief Constructor.
  \param parent parent data object
*/
CAM_DataObject::CAM_DataObject( SUIT_DataObject* parent )
: SUIT_DataObject( parent )
{
}

/*!
  \brief Destructor.

  Does nothing.
*/
CAM_DataObject::~CAM_DataObject()
{
}

/*!
  \brief Get CAM module.
  \return parent module object pointer
*/
CAM_Module* CAM_DataObject::module() const
{ 
  CAM_DataModel* dm = dataModel();
  return dm ? dm->module() : 0;
}

/*!
  \brief Get CAM data model.
  \return data model or 0 if it is not set
  \sa CAM_ModuleObject class
*/
CAM_DataModel* CAM_DataObject::dataModel() const
{
  CAM_DataObject* parentObj = dynamic_cast<CAM_DataObject*>( parent() );
  return parentObj ? parentObj->dataModel() : 0;
}

/*!
  \class CAM_ModuleObject
  \brief CAM data model root object.
  
  This class is intended for optimized access to CAM_DataModel instance
  from CAM_DataObject instances.

  To take advantage of this class in a specific application, 
  custom data model root object class should be derived from both CAM_ModuleObject
  and application-specific DataObject implementation using virtual inheritance.
*/

/*!
  \brief Constructor.
  \param parent parent data object
*/
CAM_ModuleObject::CAM_ModuleObject( SUIT_DataObject* parent )
: CAM_DataObject( parent ),
  myDataModel( 0 )
{
}

/*!
  \brief Constructor.
  \param data data model
  \param parent parent data object
*/
CAM_ModuleObject::CAM_ModuleObject( CAM_DataModel* data, SUIT_DataObject* parent )
: CAM_DataObject( parent ),
  myDataModel( data )
{
}

/*!
  \brief Destructor.

  Does nothing.
*/
CAM_ModuleObject::~CAM_ModuleObject()
{
}

/*!
  \brief Get root object name.

  If the data model is set, this method returns module name.
  Otherwise returns empty string.

  \return root object name
*/
QString CAM_ModuleObject::name() const
{
  return myDataModel ? myDataModel->module()->moduleName() : QString();
}

/*!
  \brief Get data object icon for the specified column.

  The parameter \a id specifies the column identificator

  \param id column id
  \return object icon for the specified column
*/
QPixmap CAM_ModuleObject::icon( const int id ) const
{
  QPixmap p;
  // show icon only for the "Name" column
  if ( id == NameId && dataModel() && dataModel()->module() )
    p = dataModel()->module()->moduleIcon();
  if ( !p.isNull() )
    p = Qtx::scaleIcon( p, 16 );
  return p;
}

/*!
  \brief Get data object tooltip for the specified column.

  The parameter \a id specifies the column identificator

  \param id column id
  \return object tooltip for the specified column
*/
QString CAM_ModuleObject::toolTip( const int /*id*/ ) const
{
  // show the same tooltip for all columns
  QString tip;
  if ( dataModel() && dataModel()->module() )
    tip = QObject::tr( "MODULE_ROOT_OBJECT_TOOLTIP" ).arg( dataModel()->module()->moduleName() );
  return tip;
}

/*!
  \brief Get data model.
  \return data model pointer or 0 if it is not set
*/
CAM_DataModel* CAM_ModuleObject::dataModel() const
{
  return myDataModel;
}

/*!
  \brief Set data model.
  \param dm data model
*/
void CAM_ModuleObject::setDataModel( CAM_DataModel* dm )
{
  myDataModel = dm;
}
