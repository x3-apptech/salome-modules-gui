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


#include "DataProcessor.hxx"
#include <Basics_Utils.hxx>
#include "QtHelper.hxx"

DataProcessor::DataProcessor(DataModel * dataModel) {
  _dataModel = dataModel;
}

/*!
 * This function retrieves in the data model all the DataObject
 * associated to the item nameIds contained in the specified list. The
 * input list is what the TreeView sends via the notification signal.
 */
DataObjectVector * DataProcessor::extract(QStringList itemNameIdList) {
  if ( _dataModel == NULL ) {
    STDLOG("No data model associated to this processor");
    return NULL;
  }

  DataObjectVector * dataObjectList = new DataObjectVector();

  // We can request the dataModel to obtain the dataObject associated
  // to each of the item (iteNameId is a TreeView id, Qt stuff only).
  QStringList::const_iterator it;
  for (it = itemNameIdList.constBegin(); it != itemNameIdList.constEnd(); ++it) {
    QString itemNameId = *it;
    DataObject * dataObject = _dataModel->getDataObject(QS2S(itemNameId));
    
    if ( dataObject != NULL ) {
      dataObjectList->push_back(dataObject);
    } else {
      LOG("No data object associated to the item "<<itemNameId);
    } 
  }
  return dataObjectList;
}


void DataProcessor::process(QStringList itemNameIdList) {
  if ( _dataModel == NULL ) {
    STDLOG("No data model");
    return;
  }

  this->preprocess(itemNameIdList);

  // We can request the dataModel to obtain the dataObject associated
  // to each of the item (iteNameId is a TreeView id, Qt stuff only).
  QStringList::const_iterator it;
  for (it = itemNameIdList.constBegin(); it != itemNameIdList.constEnd(); ++it) {
    QString itemNameId = *it;
    DataObject * dataObject = _dataModel->getDataObject(QS2S(itemNameId));
    
    if ( dataObject != NULL ) {
      this->processDataObject(dataObject);
    } else {
      STDLOG("No data object associated to the item "<<QS2S(itemNameId));
    } 
  }

  this->postprocess(itemNameIdList);
}


