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

// Author: Guillaume Boulant (EDF/R&D)

#ifndef DATAOBJECT_H
#define DATAOBJECT_H

#include "TreeData.hxx"

#include <map>
#include <string>
using namespace std;

class TREEDATA_EXPORT DataObject {

public:
  DataObject();
  ~DataObject();

  void setLabel(string label);
  string getLabel();
  void setProperty(string key, string value);
  string getProperty(string key);

  /*!
   * This function specifies the localization of the object in the
   * hierarchical organization that can be defined by the DataModel it
   * belongs to.
   */
  virtual string getPath() = 0;
  string getPathName();
  string getNameId();

  static const string pathsep;

  /*! This function can be used for debug */
  string toString();

private:
  /*! The name this object can be displayed with */
  string _label;
  /*! The identifier of this object. An identifier is invariant all the session long */
  string _nameId;
  /*! The dictionnary of properties that characterize this object */
  map<string, string> _properties;

  static int _lastId;
  static const string _BASENAME;
};

#endif // DATAOBJECT_H
