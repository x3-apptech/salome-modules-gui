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
#include "CAM_DataObject.h"

#include "CAM_Module.h"
#include "CAM_DataModel.h"

/*!Constructor. Sets parent object.*/
CAM_DataObject::CAM_DataObject( SUIT_DataObject* parent )
: SUIT_DataObject( parent )
{
}

/*!Destructor.Do nothing*/
CAM_DataObject::~CAM_DataObject()
{
}

/*!Get module.
 *\retval const CAM_Module pointer - module
 */
CAM_Module* CAM_DataObject::module() const
{ 
  CAM_Module* mod = 0;

  CAM_DataModel* data = dataModel();
  if ( data )
    mod = data->module();

  return mod;
}

/*!Get data model.
 *Return 0 - if no parent obbject.
 *\retval const CAM_DataModel pointer - data model
 */
CAM_DataModel* CAM_DataObject::dataModel() const
{
  CAM_DataObject* parentObj = dynamic_cast<CAM_DataObject*>( parent() );

  if ( !parentObj )
    return 0;

  return parentObj->dataModel();
}
