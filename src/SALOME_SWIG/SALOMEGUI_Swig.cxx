//  SALOME SALOMEGUI : implementation of desktop and GUI kernel
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : SALOMEGUI_Swig.cxx
//  Author : Vadim SANDLER
//  Module : SALOME
//  $Header$

#include "SALOMEGUI_Swig.hxx"

#include "SUIT_Session.h"
#include "SUIT_Desktop.h"
#include "SUIT_DataObjectIterator.h"
#include "OB_Browser.h"
#include "SalomeApp_Application.h"
#include "SalomeApp_Study.h"
#include "SalomeApp_Module.h"
#include "SalomeApp_DataObject.h"
#include "LightApp_SelectionMgr.h"
#include "SALOME_Prs.h"
#include "SOCC_ViewModel.h"
#include "SVTK_ViewModel.h"
#include "SVTK_ViewWindow.h"
#include "SOCC_ViewWindow.h"
#include "SPlot2d_ViewWindow.h"

#include "SALOME_Event.hxx"
#include "SALOME_ListIO.hxx"
#include "SALOME_InteractiveObject.hxx"
#include "SALOME_ListIteratorOfListIO.hxx"

//#include "utilities.h"

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOME_ModuleCatalog)

using namespace std;

/*!
  asv : 3.12.04 : added checking for NULL GUI objects in almost all methods.
  In the scope of fixing bug PAL6869.

  (PR : modify comments)
  Instance of this class is created every time "import salome" line is typed 
  - in IAPP embedded Python interpretor  (SALOME_Session_Server executable),
  - in inline Python nodes in Supervisor (in SALOME_Container executable),
  - in stand-alone Python console outside any executable.
  SALOME GUI(desktop and other objects) is only available in SALOME_Session_Server

  VSR : 19.04.05 : Reimplemented for new SALOME GUI (SUIT-based)
  All methods are implemeted using Event mechanism.
  Display/Erase methods use SALOME_Prs/SALOME_View mechanism. It is currently
  implemented only for OCC and VTK viewers.
*/

/*!
  getApplication()
  Returns active application object [ static ]
*/
static SalomeApp_Application* getApplication() {
  if ( SUIT_Session::session() )
    return dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  return NULL;
}

/*!
  getActiveStudy()
  Gets active study or 0 if there is no study opened [ static ]
*/
static SalomeApp_Study* getActiveStudy()
{
  if ( getApplication() )
    return dynamic_cast<SalomeApp_Study*>( getApplication()->activeStudy() );
  return 0;
}

/*!
  SALOMEGUI_Swig::SALOMEGUI_Swig
  Constructor
*/
SALOMEGUI_Swig::SALOMEGUI_Swig()
{
}

/*!
  SALOMEGUI_Swig::~SALOMEGUI_Swig
  Destructor
*/
SALOMEGUI_Swig::~SALOMEGUI_Swig()
{
}

/*!
  SALOMEGUI_Swig::hasDesktop
  Returns TRUE if GUI is available.
*/
class THasDesktopEvent: public SALOME_Event {
public:
  typedef bool TResult;
  TResult myResult;
  THasDesktopEvent() : myResult( false ) {}
  virtual void Execute() {
    myResult = (bool)( getApplication() && getApplication()->desktop() );
  }
};

/*!
  \return true if GUI is available.
*/
bool SALOMEGUI_Swig::hasDesktop()
{
  return ProcessEvent( new THasDesktopEvent() );
}

/*!
  SALOMEGUI_Swig::updateObjBrowser
  Updates active study's Object Browser.
  VSR: updateSelection parameter is currently not used. Will be implemented or removed lately.
*/
void SALOMEGUI_Swig::updateObjBrowser( bool /*updateSelection*/ )
{
  class TEvent: public SALOME_Event {
  public:
    TEvent() {}
    virtual void Execute() {
      if ( SalomeApp_Application* anApp = getApplication() ) {
	anApp->updateObjectBrowser();
	anApp->updateActions(); //SRN: added in order to update the toolbar
      }
    }
  };
  ProcessVoidEvent( new TEvent() );
}

/*!
  SALOMEGUI_Swig::getActiveStudyId
  Returns active study's ID or 0 if there is no active study.
*/
class TGetActiveStudyIdEvent: public SALOME_Event {
public:
  typedef int TResult;
  TResult myResult;
  TGetActiveStudyIdEvent() : myResult( 0 ) {}
  virtual void Execute() {
    if ( SalomeApp_Study* aStudy = getActiveStudy() ) {
      myResult = aStudy->studyDS()->StudyId();
    }
  }
};

/*!
  \return active study's ID or 0 if there is no active study.
*/
int SALOMEGUI_Swig::getActiveStudyId()
{
  return ProcessEvent( new TGetActiveStudyIdEvent() );
}

/*!
  SALOMEGUI_Swig::getActiveStudyName
  Returns active study's name or NULL if there is no active study.
*/
class TGetActiveStudyNameEvent: public SALOME_Event {
public:
  typedef string TResult;
  TResult myResult;
  TGetActiveStudyNameEvent() {}
  virtual void Execute() {
    if ( SalomeApp_Study* aStudy = getActiveStudy() ) {
      myResult = aStudy->studyDS()->Name();
    }
  }
};

/*!
  \return active study's name or NULL if there is no active study.
*/
const char* SALOMEGUI_Swig::getActiveStudyName()
{
  string result = ProcessEvent( new TGetActiveStudyNameEvent() );
  return result.empty() ? NULL : result.c_str();
}

/*!
  SALOMEGUI_Swig::getComponentName
  Returns the name of the component by its user name.
*/
class TGetModulCatalogEvent: public SALOME_Event {
public:
  typedef CORBA::Object_var TResult;
  TResult myResult;
  TGetModulCatalogEvent() : myResult(CORBA::Object::_nil()) {}
  virtual void Execute() {
    if (SalomeApp_Application* anApp = getApplication())
      myResult = anApp->namingService()->Resolve("/Kernel/ModulCatalog");
  }
};

/*!
  \return the name of the component by its user name.
*/
const char* SALOMEGUI_Swig::getComponentName( const char* componentUserName )
{
  CORBA::Object_var anObject = ProcessEvent(new TGetModulCatalogEvent());
  if (!CORBA::is_nil(anObject)) {
    SALOME_ModuleCatalog::ModuleCatalog_var aCatalogue =
      SALOME_ModuleCatalog::ModuleCatalog::_narrow( anObject );
    SALOME_ModuleCatalog::ListOfIAPP_Affich_var aModules = aCatalogue->GetComponentIconeList();
    for ( unsigned int ind = 0; ind < aModules->length(); ind++ ) {
      CORBA::String_var aModuleName     = aModules[ ind ].modulename;
      CORBA::String_var aModuleUserName = aModules[ ind ].moduleusername;
      if ( strcmp(componentUserName, aModuleUserName.in()) == 0 )
        return aModuleName._retn();
    }
  }
  return 0;
}

/*!
  SALOMEGUI_Swig::getComponentUserName
  Returns the user name of the component by its name.
*/
const char* SALOMEGUI_Swig::getComponentUserName( const char* componentName )
{
  CORBA::Object_var anObject = ProcessEvent(new TGetModulCatalogEvent());
  if (!CORBA::is_nil(anObject)) {
    SALOME_ModuleCatalog::ModuleCatalog_var aCatalogue =
      SALOME_ModuleCatalog::ModuleCatalog::_narrow( anObject );
    SALOME_ModuleCatalog::ListOfIAPP_Affich_var aModules = aCatalogue->GetComponentIconeList();
    for ( unsigned int ind = 0; ind < aModules->length(); ind++ ) {
      CORBA::String_var aModuleName     = aModules[ ind ].modulename;
      CORBA::String_var aModuleUserName = aModules[ ind ].moduleusername;
      if ( strcmp(componentName, aModuleName.in()) == 0 )
        return aModuleUserName._retn();
    }
  }
  return 0;
}

/*!
  SALOMEGUI_Swig::SelectedCount
  Returns the number of selected objects.
*/
class TSelectedCountEvent: public SALOME_Event {
public:
  typedef int TResult;
  TResult myResult;
  TSelectedCountEvent() : myResult( 0 ) {}
  virtual void Execute() {
    if ( SalomeApp_Application* anApp = getApplication() ) {
      SalomeApp_Study*       aStudy  = dynamic_cast<SalomeApp_Study*>( anApp->activeStudy() ); // for sure!
      LightApp_SelectionMgr* aSelMgr = anApp->selectionMgr(); 
      if ( aStudy && aSelMgr ) {
	SALOME_ListIO anIOList;
	aSelMgr->selectedObjects( anIOList );
	myResult = anIOList.Extent();
      }
    }
  }
};

/*!
  \return the number of selected objects.
*/
int SALOMEGUI_Swig::SelectedCount()
{
  return ProcessEvent( new TSelectedCountEvent() );
}

/*!
  SALOMEGUI_Swig::getSelected
  Returns the selected object entry by the given index.
*/
class TGetSelectedEvent: public SALOME_Event {
public:
  typedef QString TResult;
  TResult myResult;
  int     myIndex;
  TGetSelectedEvent( int theIndex ) : myIndex( theIndex ) {}
  virtual void Execute() {
    if ( SalomeApp_Application* anApp = getApplication() ) {
      SalomeApp_Study*       aStudy  = dynamic_cast<SalomeApp_Study*>( anApp->activeStudy() ); // for sure!
      LightApp_SelectionMgr* aSelMgr = anApp->selectionMgr(); 
      if ( aStudy && aSelMgr ) {
	SALOME_ListIO anIOList;
	aSelMgr->selectedObjects( anIOList );
	if ( myIndex < anIOList.Extent() ) {
	  int index = 0;
	  SALOME_ListIteratorOfListIO anIter( anIOList );
	  for( ; anIter.More(); anIter.Next(), index++ ) {
	    Handle(SALOME_InteractiveObject) anIO = anIter.Value();
	    if ( myIndex == index ) {
	      myResult = anIO->getEntry();
	      return;
	    }
	  }
	}
      }
    }
  }
};

/*!
  \return the selected object entry by the given index.
*/
const char* SALOMEGUI_Swig::getSelected( int index )
{
  QString result = ProcessEvent( new TGetSelectedEvent( index ) );
  return result.isEmpty() ? NULL : strdup(result.latin1());
}

/*!
  Adds an object with the given entry to the selection.
*/
void SALOMEGUI_Swig::AddIObject( const char* theEntry )
{
  class TEvent: public SALOME_Event {
  public:
    QString myEntry;
    TEvent( const char* theEntry ) : myEntry( theEntry ) {}
    virtual void Execute() {
      if ( SalomeApp_Application* anApp = getApplication() ) {
	SalomeApp_Study*       aStudy  = dynamic_cast<SalomeApp_Study*>( anApp->activeStudy() ); // for sure!
	LightApp_SelectionMgr* aSelMgr = anApp->selectionMgr(); 
	if ( aStudy && aSelMgr ) {
	  SALOME_ListIO anIOList;
	  anIOList.Append( new SALOME_InteractiveObject( myEntry, "", "" ) );
	  aSelMgr->setSelectedObjects( anIOList, true );
	}
      }
    }
  };
  ProcessVoidEvent( new TEvent( theEntry ) );
}

/*!
  Removes the object with the given entry from the selection.
*/
void SALOMEGUI_Swig::RemoveIObject( const char* theEntry )
{
  class TEvent: public SALOME_Event {
  public:
    QString myEntry;
    TEvent( const char* theEntry ) : myEntry( theEntry ) {}
    virtual void Execute() {
      if ( SalomeApp_Application* anApp = getApplication() ) {
	SalomeApp_Study*       aStudy  = dynamic_cast<SalomeApp_Study*>( anApp->activeStudy() ); // for sure!
	LightApp_SelectionMgr* aSelMgr = anApp->selectionMgr(); 
	if ( aStudy && aSelMgr ) {
	  SALOME_ListIO anIOList;
	  // VSR: temporary solution, until LightApp_SelectionMgr::unsetSelectedObjects() method appears
	  // Lately this should be replaced by the following:
	  // anIOList.Append( new SALOME_InteractiveObject( myEntry, "", "" ) );
	  // aSelMgr->unsetSelectedObjects( anIOList );
	  ///////////////////////////////////////////////
	  aSelMgr->selectedObjects( anIOList );
	  SALOME_ListIteratorOfListIO anIter( anIOList );
	  for( ; anIter.More(); anIter.Next() ) {
	    if ( anIter.Value()->isSame( new SALOME_InteractiveObject( myEntry, "", "" ) ) ) { 
	      anIOList.Remove( anIter );
	      aSelMgr->setSelectedObjects( anIOList, true );
	      return;
	    }
	  }
	}
      }
    }
  };
  ProcessVoidEvent( new TEvent( theEntry ) );
}

/*!
  Clears selection.
*/
void SALOMEGUI_Swig::ClearIObjects()
{
  class TEvent: public SALOME_Event {
  public:
    TEvent() {}
    virtual void Execute() {
      if ( SalomeApp_Application* anApp = getApplication() ) {
	SalomeApp_Study*       aStudy  = dynamic_cast<SalomeApp_Study*>( anApp->activeStudy() ); // for sure!
	LightApp_SelectionMgr* aSelMgr = anApp->selectionMgr(); 
	if ( aStudy && aSelMgr )
	  aSelMgr->clearSelected();
      }
    }
  };
  ProcessVoidEvent( new TEvent() );
}

/*!
  Displays an object in the current view window
  (the presentable object should be previously created and displayed in this viewer).
  VSR: For the current moment implemented for OCC and VTK viewers only.
*/		
void SALOMEGUI_Swig::Display( const char* theEntry )
{
  class TEvent: public SALOME_Event {
    QString myEntry;
  public:
    TEvent( const char* theEntry ) : myEntry( theEntry ) {}
    virtual void Execute() {
      if ( SalomeApp_Application* anApp = getApplication() ) {
	SUIT_ViewWindow* window = anApp->desktop()->activeWindow();
	if ( window ) {
	  SALOME_View* view = dynamic_cast<SALOME_View*>( window->getViewManager()->getViewModel() );
	  if ( view )
	    view->Display( view->CreatePrs( myEntry ) );
	}
      }
    }
  };
  ProcessVoidEvent( new TEvent( theEntry ) );
}

/*!
  Displays an object in the current view window and erases all other
  (the presentable object should be previously created and displayed in this viewer).
  VSR: For the current moment implemented for OCC and VTK viewers only.
*/
void SALOMEGUI_Swig::DisplayOnly( const char* theEntry )
{
  class TEvent: public SALOME_Event {
    QString myEntry;
  public:
    TEvent( const char* theEntry ) : myEntry( theEntry ) {}
    virtual void Execute() {
      if ( SalomeApp_Application* anApp = getApplication() ) {
	SUIT_ViewWindow* window = anApp->desktop()->activeWindow();
	if ( window ) {
	  SALOME_View* view = dynamic_cast<SALOME_View*>( window->getViewManager()->getViewModel() );
	  if ( view ) {
	    view->EraseAll( false );
	    view->Display( view->CreatePrs( myEntry ) );
	  }
	}
      }
    }
  };
  ProcessVoidEvent( new TEvent( theEntry ) );
}

/*!
  Erases an object in the current view window
  (the presentable object should be previously created and displayed in this viewer).
  VSR: For the current moment implemented for OCC and VTK viewers only.
*/		
void SALOMEGUI_Swig::Erase( const char* theEntry )
{
  class TEvent: public SALOME_Event {
    QString myEntry;
  public:
    TEvent( const char* theEntry ) : myEntry( theEntry ) {}
    virtual void Execute() {
      if ( SalomeApp_Application* anApp = getApplication() ) {
	SUIT_ViewWindow* window = anApp->desktop()->activeWindow();
	if ( window ) {
	  SALOME_View* view = dynamic_cast<SALOME_View*>( window->getViewManager()->getViewModel() );
	  if ( view )
	    view->Erase( view->CreatePrs( myEntry ) );
	}
      }
    }
  };
  ProcessVoidEvent( new TEvent( theEntry ) );
}

/*!
  Displays all active module's child objects in the current view window
  (the presentable objects should be previously created and displayed in this viewer).
  VSR: For the current moment implemented for OCC and VTK viewers only.
*/
void SALOMEGUI_Swig::DisplayAll()
{
  class TEvent: public SALOME_Event {
  public:
    TEvent() {}
    virtual void Execute() {
      if ( SalomeApp_Application* anApp = getApplication() ) {
	SalomeApp_Study*  study        = dynamic_cast<SalomeApp_Study*>( anApp->activeStudy() ); // for sure!
	SUIT_ViewWindow*  window       = anApp->desktop()->activeWindow();
	SalomeApp_Module* activeModule = dynamic_cast<SalomeApp_Module*>( anApp->activeModule() );
	if ( study && window && activeModule ) {
	  SALOME_View* view = dynamic_cast<SALOME_View*>( window->getViewManager()->getViewModel() );
	  if ( view ) {
	    for ( SUIT_DataObjectIterator it( activeModule->dataModel()->root(), SUIT_DataObjectIterator::DepthLeft ); it.current(); ++it ) {
	      SalomeApp_DataObject* obj = dynamic_cast<SalomeApp_DataObject*>( it.current() );
	      if ( obj && !obj->entry().isEmpty() )
		view->Display( view->CreatePrs( obj->entry() ) );
	    }
	  }
	}
      }
    }
  };
  ProcessVoidEvent( new TEvent() );
}

/*!
  Erases all objects from the current view window
  VSR: For the current moment implemented for OCC and VTK viewers only.
*/
void SALOMEGUI_Swig::EraseAll()
{
  class TEvent: public SALOME_Event {
  public:
    TEvent() {}
    virtual void Execute() {
      if ( SalomeApp_Application* anApp = getApplication() ) {
	SUIT_ViewWindow* window = anApp->desktop()->activeWindow();
	if ( window ) {
	  SALOME_View* view = dynamic_cast<SALOME_View*>( window->getViewManager()->getViewModel() );
	  if ( view )
	    view->EraseAll( false );
	}
      }
    }
  };
  ProcessVoidEvent( new TEvent() );
}

/*!
  Returns TRUE if the object with given entry is in the current viewer.
  VSR: For the current moment implemented for OCC and VTK viewers only.
*/
class TIsInViewerEvent: public SALOME_Event {
  QString myEntry;
public:
  typedef bool TResult;
  TResult myResult;
  TIsInViewerEvent( const char* theEntry ) : myEntry( theEntry ), myResult( false ) {}
  virtual void Execute() {
    if ( SalomeApp_Application* anApp = getApplication() ) {
      SUIT_ViewWindow* window = anApp->desktop()->activeWindow();
      if ( window ) {
	SALOME_View* view = dynamic_cast<SALOME_View*>( window->getViewManager()->getViewModel() );
	if ( view ) {
	  SALOME_Prs* aPrs = view->CreatePrs( myEntry );
	  myResult = !aPrs->IsNull();
	}
      }
    }
  }
};

/*!
  \return TRUE if the object with given entry is in the current viewer.
  VSR: For the current moment implemented for OCC and VTK viewers only.
*/
bool SALOMEGUI_Swig::IsInCurrentView( const char* theEntry )
{
  return ProcessEvent( new TIsInViewerEvent( theEntry ) );
}

/*!
  Updates (repaint) current view
*/
void SALOMEGUI_Swig::UpdateView()
{
  class TEvent: public SALOME_Event {
  public:
    TEvent() {}
    virtual void Execute() {
      if ( SalomeApp_Application* anApp = getApplication() ) {
	SUIT_ViewWindow* window = anApp->desktop()->activeWindow();
	if ( window ) {
	  SALOME_View* view = dynamic_cast<SALOME_View*>( window->getViewManager()->getViewModel() );
	  if ( view )
	    view->Repaint();
	}
      }
    }
  };
  ProcessVoidEvent( new TEvent() );
}

/*!
  Fit all the contents of the current view window
 */
void SALOMEGUI_Swig::FitAll()
{
  class TEvent: public SALOME_Event {
  public:
    TEvent() {}
    virtual void Execute() {
      if ( SalomeApp_Application* anApp = getApplication() ) {
	SUIT_ViewWindow* window = anApp->desktop()->activeWindow();
	if ( window ) {
	  if ( dynamic_cast<SVTK_ViewWindow*>( window ) )
	    (dynamic_cast<SVTK_ViewWindow*>( window ))->onFitAll();
	  else if ( dynamic_cast<SOCC_ViewWindow*>( window ) )
	    (dynamic_cast<SOCC_ViewWindow*>( window ))->onFitAll();
	  else if ( dynamic_cast<SPlot2d_ViewWindow*>( window ) )
	    (dynamic_cast<SPlot2d_ViewWindow*>( window ))->onFitAll();
	}
      }
    }
  };
  ProcessVoidEvent( new TEvent() );
}

/*!
  Reset current view window to the default state.
 */
void SALOMEGUI_Swig::ResetView()
{
  class TEvent: public SALOME_Event {
  public:
    TEvent() {}
    virtual void Execute() {
      if ( SalomeApp_Application* anApp = getApplication() ) {
	SUIT_ViewWindow* window = anApp->desktop()->activeWindow();
	if ( window ) {
	  if ( dynamic_cast<SVTK_ViewWindow*>( window ) )
	    (dynamic_cast<SVTK_ViewWindow*>( window ))->onResetView();
	  else if ( dynamic_cast<SOCC_ViewWindow*>( window ) )
	    (dynamic_cast<SOCC_ViewWindow*>( window ))->onResetView();
	  else if ( dynamic_cast<SPlot2d_ViewWindow*>( window ) )
	    (dynamic_cast<SPlot2d_ViewWindow*>( window ))->onFitAll();
	  // VSR: there is no 'ResetView' functionality for Plot2d viewer,
	  // so we use 'FitAll' instead.
	}
      }
    }
  };
  ProcessVoidEvent( new TEvent() );
}

enum {
  __ViewTop,
  __ViewBottom,
  __ViewLeft,
  __ViewRight,
  __ViewFront,
  __ViewBack
};

void setView( int view )
{
  class TEvent: public SALOME_Event {
  private:
    int myView;
  public:
    TEvent( int view ) : myView( view ) {}
    virtual void Execute() {
      if ( SalomeApp_Application* anApp = getApplication() ) {
	SUIT_ViewWindow* window = anApp->desktop()->activeWindow();
	if ( window ) {
	  if ( dynamic_cast<SVTK_ViewWindow*>( window ) ) {
	    switch( myView ) {
	    case __ViewTop:
	      (dynamic_cast<SVTK_ViewWindow*>( window ))->onTopView(); break;
	    case __ViewBottom:
	      (dynamic_cast<SVTK_ViewWindow*>( window ))->onBottomView(); break;
	    case __ViewLeft:
	      (dynamic_cast<SVTK_ViewWindow*>( window ))->onLeftView(); break;
	    case __ViewRight:
	      (dynamic_cast<SVTK_ViewWindow*>( window ))->onRightView(); break;
	    case __ViewFront:
	      (dynamic_cast<SVTK_ViewWindow*>( window ))->onFrontView(); break;
	    case __ViewBack:
	      (dynamic_cast<SVTK_ViewWindow*>( window ))->onBackView(); break;
	    default:
	      break;
	    }
	  }
	  else if ( dynamic_cast<SOCC_ViewWindow*>( window ) ) {
	    switch( myView ) {
	    case __ViewTop:
	      (dynamic_cast<SOCC_ViewWindow*>( window ))->onTopView(); break;
	    case __ViewBottom:
	      (dynamic_cast<SOCC_ViewWindow*>( window ))->onBottomView(); break;
	    case __ViewLeft:
	      (dynamic_cast<SOCC_ViewWindow*>( window ))->onLeftView(); break;
	    case __ViewRight:
	      (dynamic_cast<SOCC_ViewWindow*>( window ))->onRightView(); break;
	    case __ViewFront:
	      (dynamic_cast<SOCC_ViewWindow*>( window ))->onFrontView(); break;
	    case __ViewBack:
	      (dynamic_cast<SOCC_ViewWindow*>( window ))->onBackView(); break;
	    default:
	      break;
	    }
	  }
	}
      }
    }
  };
  ProcessVoidEvent( new TEvent( view ) );
}

/*!
  Switch current view window to show top view
 */
void SALOMEGUI_Swig::ViewTop()
{
  setView( __ViewTop );
}

/*!
  Switch current view window to show bottom view
 */
void SALOMEGUI_Swig::ViewBottom()
{
  setView( __ViewBottom );
}

/*!
  Switch current view window to show left view
 */
void SALOMEGUI_Swig::ViewLeft()
{
  setView( __ViewLeft );
}

/*!
  Switch current view window to show right view
 */
void SALOMEGUI_Swig::ViewRight()
{
  setView( __ViewRight );
}

/*!
  Switch current view window to show front view
 */
void SALOMEGUI_Swig::ViewFront()
{
  setView( __ViewFront );
}

/*!
  Switch current view window to show back view
 */
void SALOMEGUI_Swig::ViewBack()
{
  setView( __ViewBack );
}
