// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "MyDataModel.hxx"

//
// =================================================================
// MyDataObject implementation
// =================================================================
//

const string MyDataObject::PROPERTY_KEY_TYPE    = "type";
const string MyDataObject::PROPERTY_KEY_CIRCUIT ="circuit";
const string MyDataObject::PROPERTY_KEY_REPFONC ="rf";

MyDataObject::MyDataObject() : DataObject() {
  this->setProperty(PROPERTY_KEY_TYPE, "Tuyauterie");
  this->setProperty(PROPERTY_KEY_CIRCUIT,"RRA");
  this->setProperty(PROPERTY_KEY_REPFONC,"RF_01");
}

/*! This function specified the localization of the object in the
 * hierarchical organization
 */
string MyDataObject::getPath() {
  // We choose here a convention for organizing the path for this
  // class of object.
  /*
  string path = getProperty(PROPERTY_KEY_CIRCUIT) + pathsep
    + getProperty(PROPERTY_KEY_REPFONC) + pathsep
    + getProperty(PROPERTY_KEY_TYPE);
  */
  string path = getProperty(PROPERTY_KEY_TYPE) + pathsep
    + getProperty(PROPERTY_KEY_CIRCUIT) + pathsep
    + getProperty(PROPERTY_KEY_REPFONC);
    
  return path;
}

//
// =================================================================
// MyDataModel implementation
// =================================================================
//
MyDataModel::MyDataModel() : DataModel() {
}

DataObject * MyDataModel::newDataObject() {
  MyDataObject * dataObject = new MyDataObject();
  return dataObject;
}
