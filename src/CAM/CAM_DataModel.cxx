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

#include "CAM_DataModel.h"

#include "CAM_Module.h"
#include "CAM_DataObject.h"

/*!
  \class CAM_DataModel
  \brief Base class for all data models used in CAM-based applications.
  
  Represents data model of the CAM module. Provides necessary interface
  (default implementation is empty).
*/

/*!
  \brief Constructor.
  
  Initialise data module by specified \a module.
*/
CAM_DataModel::CAM_DataModel( CAM_Module* module )
: myRoot( 0 ),
  myModule( module )
{
}

/*!
  \brief Destructor.

  Does nothing.
*/
CAM_DataModel::~CAM_DataModel()
{
}

/*!
  \brief Initialize data model.

  This method should be re-implemented in the successor classes 
  and can be used for creation of root data object.
  Default implementation does nothing.
*/
void CAM_DataModel::initialize()
{
}

/*!
  \brief Get data model root object.
  \return root object
  \sa setRoot()
*/
CAM_DataObject* CAM_DataModel::root() const
{
  return myRoot;
}

/*!
  \brief Set data model root object.

  This method should be used to specify custom root object instance.

  Root object can be created in several ways, depending on application or module needs:
  - in initialize() method
  - while the data model is being loaded
  - when the data model is updated and becomes non-empty 

  If root object is changed, this method emits rootChanged() signal.

  \param newRoot new root object
*/
void CAM_DataModel::setRoot( const CAM_DataObject* newRoot )
{
  if ( myRoot == newRoot )
    return;

  if ( myRoot )
    myRoot->disconnect( SIGNAL( destroyed( SUIT_DataObject* ) ), 
                        this, SLOT( onDestroyed( SUIT_DataObject* ) ) );

  myRoot = (CAM_DataObject*)newRoot;

  if ( myRoot )
    myRoot->connect( SIGNAL( destroyed( SUIT_DataObject* ) ), 
                     this, SLOT( onDestroyed( SUIT_DataObject* ) ) );

  emit rootChanged( this );
}

/*!
  \brief Get module.
  \return module owning this data model
*/
CAM_Module* CAM_DataModel::module() const
{
  return myModule;
}

/*!
  \brief Called when data object is destroyed.

  Nullifies the root object if it is detroyed to avoid crashes.
  
  \param obj object being destroyed
*/
void CAM_DataModel::onDestroyed( SUIT_DataObject* obj )
{
  if ( myRoot == obj )
    myRoot = 0;
}

/*!
  \brief Load data model.

  This method should be re-implemented in the successor classes.
  Default implementation returns \c true.

  \param name study name
  \param study study
  \param files list of file names from which data should be loaded
  \return \c true if data model is loaded successfully
*/
bool CAM_DataModel::open( const QString& /*name*/, 
                          CAM_Study*     /*study*/, 
                          QStringList    /*files*/ )
{
  return true;
}

/*!
  \brief Save data model.

  This method should be re-implemented in the successor classes.
  Default implementation returns \c true.

  \param files list of file names to which data should be saved
  \return \c true if data model is saved successfully
*/
bool CAM_DataModel::save( QStringList& )
{ 
  return true; 
}

/*!
  \brief Save data to the new file.

  This method should be re-implemented in the successor classes.
  Default implementation returns \c true.

  \param name study name
  \param study study
  \param files resulting list of file names to which data is saved
  \return \c true if data model is saved successfully
*/
bool CAM_DataModel::saveAs( const QString& /*name*/,
                            CAM_Study*     /*study*/,
                            QStringList&   /*files*/ )
{
  return true;
}

/*!
  \brief Close data model.

  This method should be re-implemented in the successor classes.
  Default implementation returns \c true.

  \return \c true if data model is closed successfully
*/
bool CAM_DataModel::close()
{ 
  return true; 
}

/*!
  \brief Create empty data model.

  This method should be re-implemented in the successor classes.
  Default implementation returns \c true.

  \return \c true if data model is created successfully
*/
bool CAM_DataModel::create( CAM_Study* )
{ 
  return true; 
}

/*!
  \fn void CAM_DataModel::rootChanged( const CAM_DataModel* root );
  \brief Emitted when the root data object is changed.
  \param root new root data object
*/
