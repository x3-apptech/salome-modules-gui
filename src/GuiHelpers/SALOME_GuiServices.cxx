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

#include "SALOME_GuiServices.hxx"
#include <SUIT_Session.h>

namespace GUI {
  /*!
   * Get the SALOME application, i.e. the main GUI framework of
   * SALOME. This function returns a pointer to the singleton instance
   * of the SalomeApp_Application.
   * 
   * The SALOME application can be used to get reference to
   * the SALOME KERNEL services such that the naming service, the
   * lifeCycleCORBA, ... Theses services can be retrieved using static
   * functions (not required to get the singleton instance):
   *
   *   SALOME_NamingService *aNamingService = SalomeApp_Application::namingService();
   *
   * Please note that this usage can be done only from within the
   * SALOME session server process, i.e. the process that embed the
   * SALOME application.
   */
  SalomeApp_Application * getSalomeApplication() {
    static SalomeApp_Application * app;
    if ( app == NULL ) {
      app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
    }
    return app;
  }

  /*!
   * Get the selection manager of the SALOME application. The
   * selection manager can be used to get the selected items in the
   * objects browser that is a GUI display of the active study.
   * The function returns a pointer to the selectionMgr attribute of
   * the SalomeApp_Application singleton instance.
   */
  LightApp_SelectionMgr * getSelectionManager() {
    SalomeApp_Application* anApp = GUI::getSalomeApplication();
    if ( anApp == NULL ) return NULL;
    return dynamic_cast<LightApp_SelectionMgr*>( anApp->selectionMgr() );
  }


  SUIT_ResourceMgr * getResourcesManager() {
    return SUIT_Session::session()->resourceMgr();
  }
  // Note that the resource manager can be also retrieved from the
  // SALOME application using the resourceMgr() method:
  // 

  /**
   * This returns the current active study id if an active study is
   * defined in the SALOME session, returns -1 otherwise. Note that
   * the active study doesn't make sense outside of the GUI SALOME
   * process, i.e. the SALOME_SessionServer embedding the
   * SalomeApp_Application.
   */
  int getActiveStudyId() {
    SALOME::Session_var aSession = KERNEL::getSalomeSession();
    if ( CORBA::is_nil(aSession) ) {
      INFOS("ERR: can't request for active study because the session is NULL");
      return -1;
    }
    return aSession->GetActiveStudyId();
  }

  /**
   * This returns the current active study if an active study is
   * defined in the SALOME session, returns null otherwise.
   */
  SALOMEDS::Study_ptr getActiveStudy() {
    return KERNEL::getStudyById(getActiveStudyId());
  }


  // __GBO__ Question: what is the difference between a
  // SALOMEDS::Study and a SalomeApp_Study?
  SalomeApp_Study* getSalomeAppActiveStudy() {
    SUIT_Application* app = getSalomeApplication();
    if( app )
      return dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
    else
      return NULL;
  }


  SALOMEDS::SObject_ptr IObjectToSObject(const Handle(SALOME_InteractiveObject)& iobject) {
    if (!iobject.IsNull()) {
      if (iobject->hasEntry()) {
        SalomeApp_Study* appStudy = getSalomeAppActiveStudy();
        if ( appStudy != NULL ) {
          //
          // IMPORTANT NOTE:
          //
          // Note that the SalomeApp_Study give acces to shared
          // pointer (i.e. _PTR(<BaseClassName>)) that points to proxy
          // objects (i.e. SALOMEDSClien_<BaseClassName>) that embeds
          // the CORBA servants (i.e. SALOMEDS::<BaseClassName>_ptr).
          // Then to retrieve the corba servant, a method is to
          // retrieve the SALOMEDS::Study servant first and the to
          // request this servant to get the SObject given its entry.
          //
          _PTR(Study) studyClient = appStudy->studyDS();
          SALOMEDS::Study_var study = KERNEL::getStudyManager()->GetStudyByID(studyClient->StudyId());
          SALOMEDS::SObject_ptr sobject = study->FindObjectID(iobject->getEntry());
          return sobject;
        }
      }
    }
    return SALOMEDS::SObject::_nil();
  }

}

