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
#include "CAM_RootObject.h"

#include "CAM_DataModel.h"
#include "CAM_Module.h"

/*!Constructor. Initialize by \a parent.
 * Set data model to 0.
 */
CAM_RootObject::CAM_RootObject( SUIT_DataObject* parent )
: CAM_DataObject( parent ),
myDataModel( 0 )
{
}

/*!Constructor. Initialize by \a parent and \a data - data object
 *\param data - data object
 *\param parent - parent data object
 */
CAM_RootObject::CAM_RootObject( CAM_DataModel* data, SUIT_DataObject* parent )
: CAM_DataObject( parent ),
myDataModel( data )
{
}

/*!Destructor. Do nothing.*/
CAM_RootObject::~CAM_RootObject()
{
}

/*!
    Returns module name
*/
QString CAM_RootObject::name() const
{
  QString aName = "";
  if (myDataModel)
    aName = myDataModel->module()->moduleName();
  return aName;
}

/*!Get data model
 *\retval const CAM_DataModel pointer to data model.
 */
CAM_DataModel* CAM_RootObject::dataModel() const
{
  return myDataModel;
}

/*!Set data model.
 *\param dm - data model to set.
 */
void CAM_RootObject::setDataModel( CAM_DataModel* dm )
{
  myDataModel = dm;
}
