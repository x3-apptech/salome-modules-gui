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

#include "DataObject.hxx"
#include <Basics_Utils.hxx>

// Static assignement
int DataObject::_lastId=0;
const std::string DataObject::_BASENAME = std::string("object_");
const std::string DataObject::pathsep = std::string("/");

DataObject::DataObject() {
  _nameId = _BASENAME+ToString(_lastId);
  _lastId++;
  // The default label is set to the nameId, but it can be modified
  // using setLabel whereas the nameId can't be modified.
  _label  = _nameId;
}

DataObject::~DataObject() {
  _properties.clear();
}

void DataObject::setLabel(std::string label) {
  _label = label;
}
std::string DataObject::getLabel() {
  return _label;
}

std::string DataObject::getPathName() {
  std::string pathName;
  pathName = this->getPath() + pathsep + this->getLabel();
  return pathName;
}


std::string DataObject::getNameId() {
  return _nameId;
}

void DataObject::setProperty(std::string key, std::string value) {
  _properties[key] = value;
}
std::string DataObject::getProperty(std::string key) {
  return _properties[key];
}

std::string DataObject::toString() {
  std::string serialize="\n";
  serialize+="nameId = "+getNameId()+"\n";
  serialize+="label  = "+getLabel()+"\n";
  serialize+="path   = "+getPath();
  return serialize;
}
