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

#ifndef _DATAPROCESSOR_H_
#define _DATAPROCESSOR_H_

#include "TreeData.hxx"

#include "DataModel.hxx"
#include "DataObject.hxx"
#include <QStringList>
#include <QString>

#include <vector>
typedef std::vector<DataObject *> DataObjectVector;

//
// =================================================================
// This class can be used to automize processing on data objects.
// The inputs of public functions are list of selected items in the
// tree view. The function automize the extraction of associated
// DataObject and can automize the processing of these object if the
// virtual function processDataObject is implemented.
// =================================================================
//

class TREEDATA_EXPORT DataProcessor {

public:
  DataProcessor(DataModel * dataModel);

  DataObjectVector * extract(QStringList itemNameIdList);
  void               process(QStringList itemNameIdList);

  
protected:
  virtual void preprocess(QStringList itemNameIdList) {
      // Implement something to be executed at the begining of the process function
  };
  virtual void postprocess(QStringList itemNameIdList) {
      // Implement something to be executed at the end of the process function
  };
  // Implement what must be done with each DataObject during the process function
  virtual void processDataObject(DataObject * dataObject) = 0;

private:
  DataModel * _dataModel;
};

#endif
