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

#ifndef __GUI_SERVICES_H__
#define __GUI_SERVICES_H__

#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <LightApp_SelectionMgr.h>
#include <SUIT_ResourceMgr.h>

#include "SALOMEconfig.h"
#include CORBA_SERVER_HEADER(SALOMEDS)
#include <SALOME_InteractiveObject.hxx>
#include <SALOME_KernelServices.hxx>
#include "SalomeGuiHelpers.hxx"
namespace GUI {

  // SALOME GUI main services
  SALOMEGUIHELPERS_EXPORT SalomeApp_Application * getSalomeApplication();
  SALOMEGUIHELPERS_EXPORT LightApp_SelectionMgr * getSelectionManager();
  SALOMEGUIHELPERS_EXPORT SUIT_ResourceMgr      * getResourcesManager();

  // Helper functions to deal with the study in a GUI context.
  //
  // First of all, in a GUI context, there is one of the opened
  // studies that is considered as active. Please note that the active
  // study is a GUI concept and it can be obtained only in the
  // SALOME_SessionServer process, i.e. the process that embeds the
  // SalomeApp_Application. The concept of active study doesn't make
  // sense outside of the GUI context.
  //
  // The active study is associated with an GUI Objects Browser that
  // displays a graphical representation of the study. The items that
  // can be selected in the Objects Browser are name Interactive
  // Objects (instance of class SALOME_InteractiveObject). To work
  // with data, we have to retrieve from this Interactive Object (IO)
  // the underlying Study Object (instance of class SALOMEDS::SObject)
  // and from this last the data object that can be anything (that
  // depends of the SALOME module technical choices). In general, on
  // of the attribute of a SObject is a CORBA servant that handles the
  // data to work with
  SALOMEGUIHELPERS_EXPORT SALOMEDS::Study_ptr getActiveStudy();
  SALOMEGUIHELPERS_EXPORT int                 getActiveStudyId();

  // Another way to get the active study (to be converted in
  // SALOMEDS::Study):
  SALOMEGUIHELPERS_EXPORT SalomeApp_Study   * getSalomeAppActiveStudy();

  SALOMEGUIHELPERS_EXPORT SALOMEDS::SObject_ptr IObjectToSObject(const Handle(SALOME_InteractiveObject)& iobject);

  template<class TInterface> typename TInterface::_var_type
  IObjectToInterface(const Handle(SALOME_InteractiveObject)& iobject) {
    SALOMEDS::SObject_ptr sobject = IObjectToSObject(iobject);
    return KERNEL::SObjectToInterface<TInterface>(sobject);
  }
  // _MEM_: note that template functions have to be declared AND
  // implemented in the header because they are not compiled in this
  // library but in every client library that includes this header
  // file. The effective compilation depends on the particular class
  // used for TInterface.


}

#endif // GUI_SERVICES
