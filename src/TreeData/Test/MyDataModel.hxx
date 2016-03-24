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

#ifndef _MYDATAMODEL_H_
#define _MYDATAMODEL_H_

//
// =================================================================
// Definition of an atom in the data model as an implementation of
// the virtual class DataObject
// =================================================================
//

#include "DataObject.hxx"
class MyDataObject: public DataObject {
public:
  MyDataObject();
  virtual string getPath();
  static const string PROPERTY_KEY_TYPE;
  static const string PROPERTY_KEY_CIRCUIT;
  static const string PROPERTY_KEY_REPFONC;
};


//
// =================================================================
// Definition of the data model as an implementation of the virtual
// class DataModel. It implements the DataObject factory.
// =================================================================
//
#include "DataModel.hxx"
class MyDataModel: public DataModel {
public:
  MyDataModel();
  virtual DataObject * newDataObject();
};

#endif // _MYDATAMODEL_H_
