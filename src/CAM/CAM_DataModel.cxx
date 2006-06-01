// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#include "CAM_RootObject.h"

/*!Constructor. Initialise module by \a module.*/
CAM_DataModel::CAM_DataModel( CAM_Module* module )
: myRoot( 0 ),
myModule( module )
{
}

/*!Destructor. Do nothing.*/
CAM_DataModel::~CAM_DataModel()
{
}

/*!
  Default implementation, does nothing.
  Can be used for creation of root object.
*/
void CAM_DataModel::initialize()
{
}

/*!Get root object.
 *\retval CAM_DataObject pointer - root object.
 */
CAM_DataObject* CAM_DataModel::root() const
{
  return myRoot;
}

/*!Sets root object to \a newRoot.\n
 *Emit root changed, if it was.
 *\param newRoot - new root object
 */
void CAM_DataModel::setRoot( const CAM_DataObject* newRoot )
{
  if ( myRoot == newRoot )
    return;

  if ( myRoot )
    myRoot->disconnect( this, SLOT( onDestroyed( SUIT_DataObject* ) ) );

  myRoot = (CAM_DataObject*)newRoot;

  if ( myRoot )
    myRoot->connect( this, SLOT( onDestroyed( SUIT_DataObject* ) ) );

  emit rootChanged( this );
}

/*!Gets module.
 *\retval CAM_Module pointer - module.
 */
CAM_Module* CAM_DataModel::module() const
{
  return myModule;
}

/*!Nullify root, if \a obj equal root.*/
void CAM_DataModel::onDestroyed( SUIT_DataObject* obj )
{
  if ( myRoot == obj )
    myRoot = 0;
}
