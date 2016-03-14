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


#ifndef DATAMODEL_H
#define DATAMODEL_H

#include "TreeData.hxx"

#include <map>
#include "DataObject.hxx"

class TREEDATA_EXPORT DataModel {

public:
  DataModel();
  ~DataModel();

  /*!
   * This function can be used to create a specific instance of
   * DataObject. Note that this function is a pure virtual method and
   * then no default behavior is done. In particular, the newly
   * created object is not automatically added to the data model. This
   * behavior should be implemented in a dedicated version of this
   * class.
   */
  virtual DataObject * newDataObject() = 0;

  /*! Function to add data object to the model */
  bool addDataObject(DataObject * dataObject);
  /*! Functions to remove data object from the model */
  bool removeDataObject(string nameId);
  bool removeDataObject(DataObject * dataObject);
  /*! Function to retrieve a data object in the model */
  DataObject * getDataObject(string nameId);

  map<string, DataObject *>::iterator begin();
  map<string, DataObject *>::iterator end();

private:
  map<string, DataObject *> _mapDataObjects;
  

};

#endif // DATAMODEL_H
