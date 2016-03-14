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

#include "QtHelper.hxx"

//
// =================================================================
// Helper functions for DataObject and DataModel classes
// =================================================================
//

// ----
// A DataObject can't be used as is but must be specialized to
// specify the behavior in the hierarchic model.
#include "MyDataModel.hxx"

void TEST_DataObject() {
  // In this test, the object id should increase at each instance
  DataObject * dataObject;
  for (int i=0; i<100; i++) {
    dataObject = new MyDataObject();
    QLOG("object nameId = " << dataObject->getNameId().c_str());
  }
  QLOG("path     = " << dataObject->getPath().c_str());
  QLOG("pathname = " << dataObject->getPathName().c_str());

  QLOG("serialize= " << dataObject->toString().c_str());

}

void TEST_DataModel() {
  MyDataModel * dataModel = new MyDataModel();

  int refIter = 53;
  string refNameId;

  DataObject * dataObject;
  for (int i=0; i<100; i++) {
    // We can either create the data object using its constructor or
    // using the factory of the model (the prefered way):
    // dataObject = new MyDataObject();
    dataObject = dataModel->newDataObject();
    dataObject->setLabel("myobject"+ToString(i));
    if ( i == refIter ) {
      refNameId = dataObject->getNameId();
    }
    dataModel->addDataObject(dataObject);
  }

  dataObject = dataModel->getDataObject(refNameId);
  QLOG("object nameId = " << dataObject->getNameId().c_str());
  QLOG("path     = " << dataObject->getPath().c_str());
  QLOG("pathname = " << dataObject->getPathName().c_str());
}

//
// =================================================================
//
int main(int argc, char * argv[ ])
{
  TEST_DataObject();
  //TEST_DataModel();
}
