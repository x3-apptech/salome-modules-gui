// Copyright (C) 2007-2021  CEA/DEN, EDF R&D, OPEN CASCADE
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

// Author: Guillaume Boulant (EDF/R&D)

#include "DataModel.hxx"

DataModel::DataModel() {

}

DataModel::~DataModel() {
  _mapDataObjects.clear();
}

bool DataModel::addDataObject(DataObject * dataObject) {
  _mapDataObjects[dataObject->getNameId()] = dataObject;
  return true;
}
bool DataModel::removeDataObject(std::string nameId) {
  _mapDataObjects.erase(nameId);
  return true;
}
bool DataModel::removeDataObject(DataObject * dataObject) {
  if ( dataObject == NULL ) return false;
  return removeDataObject(dataObject->getNameId());
}

DataObject * DataModel::getDataObject(std::string id) {
  return _mapDataObjects[id];
}



std::map<std::string, DataObject *>::iterator DataModel::begin() {
  return _mapDataObjects.begin();
}

std::map<std::string, DataObject *>::iterator DataModel::end() {
  return _mapDataObjects.end();
}
