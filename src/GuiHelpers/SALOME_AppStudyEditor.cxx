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

#include "SALOME_AppStudyEditor.hxx"

#include <SalomeApp_Study.h>
#include <SALOME_ListIO.hxx>
#include <LightApp_SelectionMgr.h>

SALOME_AppStudyEditor::SALOME_AppStudyEditor(SalomeApp_Application * salomeApp)
  : SALOME_StudyEditor()
{
  _salomeApp = salomeApp;
  updateActiveStudy();
}

/**
 * This updates the editor with the current active study. If the
 * active study id is identical to the study id currently associated
 * to this object, then no update is performed.
 */
int SALOME_AppStudyEditor::updateActiveStudy() {
  int activeStudyId = SALOME_AppStudyEditor::getActiveStudyId(_salomeApp);
  if ( activeStudyId != this->getStudyId() ) {
    this->setStudyById(activeStudyId);
  }
  return activeStudyId;
}

// GUI context only
int SALOME_AppStudyEditor::getActiveStudyId(SalomeApp_Application * salomeApp) {
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*> (salomeApp->activeStudy());
  _PTR(Study) aCStudy = appStudy->studyDS();
  int studyId = aCStudy->StudyId();
  return studyId;
}

SALOMEDS::SObject_ptr SALOME_AppStudyEditor::IObjectToSObject(const Handle(SALOME_InteractiveObject)& iobject) {
  if (!iobject.IsNull()) {
    if (iobject->hasEntry()) {
      SALOMEDS::SObject_var sobject = _study->FindObjectID(iobject->getEntry());
      return sobject._retn();
    }
  }
  return SALOMEDS::SObject::_nil();
}

/**
 * This function creates a list of all the "study objects" (SObject)
 * that map with the selection in the object browser (the "interactive
 * objects", IObjects). Please note that the objects belongs to the
 * active study.
 */
SALOME_StudyEditor::SObjectList * SALOME_AppStudyEditor::getSelectedObjects() {

  // _GBO_: please note that the use of this function can be
  // underoptimal in the case where the number of selected objects is
  // high, because we create a vector list of SObjects while the list
  // of IObject can be processed directly. In the case where a high
  // number of objects is selected (~1000), it's certainly better to
  // use directly the SALOME_ListIO and iterators on it.

  LightApp_SelectionMgr* aSelectionMgr = _salomeApp->selectionMgr();
  SALOME_ListIO selectedIObjects;
  aSelectionMgr->selectedObjects(selectedIObjects);
  /* 
   * This returns a list containing the selected objects of the objects
   * browser. Please note that the objects of the browser ARE NOT the
   * objects of the study. What is returned by this function is a list
   * of "interactive objects" (IObject) which are objects of the
   * graphical context. Each of this IObject is characterized by an
   * entry string that corresponds to the entry string of an associated
   * "study object" (SObject) in the active study. The mapping can be
   * done using the function IObjectToSObject.
   */

  SObjectList * listOfSObject = new SObjectList();
  SALOME_ListIteratorOfListIO It (selectedIObjects);
  for (; It.More(); It.Next()) {
    SALOMEDS::SObject_ptr sobject = this->IObjectToSObject(It.Value());
    if (!sobject->_is_nil()) {
      listOfSObject->push_back(sobject);
    }
  }

  return listOfSObject;
}
