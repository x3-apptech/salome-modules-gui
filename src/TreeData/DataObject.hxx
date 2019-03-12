// Copyright (C) 2007-2019  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef DATAOBJECT_H
#define DATAOBJECT_H

#include "TreeData.hxx"

#include <map>
#include <string>

class TREEDATA_EXPORT DataObject {

public:
  DataObject();
  ~DataObject();

  void setLabel(std::string label);
  std::string getLabel();
  void setProperty(std::string key, std::string value);
  std::string getProperty(std::string key);

  /*!
   * This function specifies the localization of the object in the
   * hierarchical organization that can be defined by the DataModel it
   * belongs to.
   */
  virtual std::string getPath() = 0;
  std::string getPathName();
  std::string getNameId();

  static const std::string pathsep;

  /*! This function can be used for debug */
  std::string toString();

private:
  /*! The name this object can be displayed with */
  std::string _label;
  /*! The identifier of this object. An identifier is invariant all the session long */
  std::string _nameId;
  /*! The dictionnary of properties that characterize this object */
  std::map<std::string, std::string> _properties;

  static int _lastId;
  static const std::string _BASENAME;
};

#endif // DATAOBJECT_H
