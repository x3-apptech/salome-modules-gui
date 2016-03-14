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

#ifndef __APP_STUDY_EDITOR_HXX__
#define __APP_STUDY_EDITOR_HXX__

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOMEDS)
#include <SalomeApp_Application.h>
#include <SALOME_InteractiveObject.hxx>

#include "SALOME_StudyEditor.hxx"
#include "SalomeGuiHelpers.hxx"
/*!
 * This class is a specialization of the KERNEL StudyEditor for
 * the GUI context. The concept of active study is introduced here
 * while it does not exist in the pure KERNEL context.
 */
class SALOMEGUIHELPERS_EXPORT SALOME_AppStudyEditor: public SALOME_StudyEditor {
public:
  SALOME_AppStudyEditor(SalomeApp_Application * salomeApp);
  int updateActiveStudy();
  
  SALOMEDS::SObject_ptr IObjectToSObject(const Handle(SALOME_InteractiveObject)& iobject);
  SALOME_StudyEditor::SObjectList * getSelectedObjects();

  static int getActiveStudyId(SalomeApp_Application * salomeApp);

private:
  SalomeApp_Application * _salomeApp;

};

#endif // __APP_STUDY_EDITOR_HXX__
