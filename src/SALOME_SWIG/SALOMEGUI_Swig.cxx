//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  SALOME SALOMEGUI : implementation of desktop and GUI kernel
// File   : SALOMEGUI_Swig.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#include "SALOMEGUI_Swig.hxx"

#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>
#include <SUIT_DataObjectIterator.h>
#include <CAM_DataModel.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_Module.h>
#include <SalomeApp_DataObject.h>
#include <LightApp_SelectionMgr.h>
#include <LightApp_DataOwner.h>
#include <SALOME_Prs.h>
#include <SOCC_ViewModel.h>
#include <SVTK_ViewModel.h>
#include <SVTK_ViewWindow.h>
#include <SOCC_ViewWindow.h>
#include <SPlot2d_ViewWindow.h>

#include <SALOME_Event.h>
#include <SALOME_ListIO.hxx>
#include <SALOME_InteractiveObject.hxx>
#include <SALOME_ListIteratorOfListIO.hxx>

/*!
  \class SALOMEGUI_Swig
  \brief Python interface module for SALOME GUI.

  This module provides an access to the SALOME GUI implementing set of functions
  which can be used from Python. This module is implemented using SWIG wrappings
  for some GUI functionality:
  - getActiveStudyId(), getActiveStudyName() : get active study identifier and name
  - updateObjBrowser() : update contents of the Object Browser
  - SelectedCount() : get number of currently selected items
  - getSelected() : get entry of the speicified selected item
  - ClearIObjects() : clear selection
  - Display(), DisplayOnly(), Erase() : display/erase objects
  - etc.

  Instance of this class is created every time "import salome" line is typed 
  - in IAPP embedded Python interpretor  (SALOME_Session_Server executable)
  - in inline Python nodes in Supervisor (in SALOME_Container executable)
  - in stand-alone Python console outside any executable

  SALOME GUI (desktop and other objects) is only available in SALOME_Session_Server.
  It means that it can not be accessed from the external Python console.

  The usage in Python:
  \code
  import libSALOME_Swig
  sg = libSALOME_Swig.SALOMEGUI_Swig()
  if sg.hasDesktop():
      selcount = sg.SelectedCount()
      if selcount > 0:
          sg.Erase( sg.getSelected( 0 ) )
      pass
  \endcode
*/

/*
  --- INTERNAL COMMENTS SECTION ---

  ASV : 03.12.04 : added checking for NULL GUI objects in almost all methods.
  In the scope of fixing bug PAL6869.

  VSR : 19.04.05 : Reimplemented for new SALOME GUI (SUIT-based)
  All methods are implemeted using Event mechanism.
  Display/Erase methods use SALOME_Prs/SALOME_View mechanism. It is currently
  implemented only for OCC and VTK viewers.
*/

/*!
  \brief Get active application object
  \internal
  \return active application or 0 if there is no any
*/
static SalomeApp_Application* getApplication()
{
  if ( SUIT_Session::session() )
    return dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  return 0;
}

/*!
  \brief Get active study object
  \internal
  \return active study or 0 if there is no study opened
*/
static SalomeApp_Study* getActiveStudy()
{
  if ( getApplication() )
    return dynamic_cast<SalomeApp_Study*>( getApplication()->activeStudy() );
  return 0;
}

/*!
  \brief Constructor.
*/
SALOMEGUI_Swig::SALOMEGUI_Swig()
{
}

/*!
  \brief Destructor
*/
SALOMEGUI_Swig::~SALOMEGUI_Swig()
{
}

/*!
  \fn bool SALOMEGUI_Swig::hasDesktop()
  \brief Check GUI availability.
  \return \c true if GUI is available
*/

class THasDesktopEvent: public SALOME_Event
{
public:
  typedef bool TResult;
  TResult myResult;
  THasDesktopEvent() : myResult( false ) {}
  virtual void Execute()
  {
    myResult = (bool)( getApplication() && getApplication()->desktop() );
  }
};
bool SALOMEGUI_Swig::hasDesktop()
{
  return ProcessEvent( new THasDesktopEvent() );
}

/*!
  \brief Update active study's Object Browser.
  \param updateSelection this parameter is obsolete
*/
void SALOMEGUI_Swig::updateObjBrowser( bool /*updateSelection*/ )
{
  class TEvent: public SALOME_Event
  {
  public:
    TEvent() {}
    virtual void Execute()
    {
      if ( SalomeApp_Application* anApp = getApplication() ) {
	anApp->updateObjectBrowser();
	anApp->updateActions(); //SRN: added in order to update the toolbar
      }
    }
  };
  ProcessVoidEvent( new TEvent() );
}

/*!
  \fn int SALOMEGUI_Swig::getActiveStudyId()
  \brief Get active study identifier
  \return active study's ID or 0 if there is no active study
*/

class TGetActiveStudyIdEvent: public SALOME_Event
{
public:
  typedef int TResult;
  TResult myResult;
  TGetActiveStudyIdEvent() : myResult( 0 ) {}
  virtual void Execute()
  {
    if ( SalomeApp_Study* aStudy = getActiveStudy() ) {
      myResult = aStudy->studyDS()->StudyId();
    }
  }
};
int SALOMEGUI_Swig::getActiveStudyId()
{
  return ProcessEvent( new TGetActiveStudyIdEvent() );
}

/*!
  \fn const char* SALOMEGUI_Swig::getActiveStudyName()
  \brief Get active study name
  \return active study's name or null string if there is no active study
*/

class TGetActiveStudyNameEvent: public SALOME_Event
{
public:
  typedef std::string TResult;
  TResult myResult;
  TGetActiveStudyNameEvent() {}
  virtual void Execute()
  {
    if ( SalomeApp_Study* aStudy = getActiveStudy() ) {
      myResult = aStudy->studyDS()->Name();
    }
  }
};
const char* SALOMEGUI_Swig::getActiveStudyName()
{
  std::string result = ProcessEvent( new TGetActiveStudyNameEvent() );
  return result.empty() ? 0 : result.c_str();
}

/*!
  \fn const char* SALOMEGUI_Swig::getComponentName( const char* componentUserName )
  \brief Get name of the component by its title (user name)
  \param componentUserName component title (user name)
  \return component name or null string if component title is invalid
*/

/*!
  \fn const char* SALOMEGUI_Swig::getComponentUserName( const char* componentName )
  \brief Get title (user name) of the component by its name
  \param componentName component name
  \return component title or null string if component name is invalid
*/

class TGetComponentNameEvent: public SALOME_Event
{
public:
  typedef QString TResult;
  TResult myResult;
  QString myName;
  bool    myIsUserName;
  TGetComponentNameEvent( const QString& name, bool isUserName )
    : myName( name ), myIsUserName( isUserName ) {}
  virtual void Execute()
  {
    if ( SalomeApp_Application* app = getApplication() ) {
      myResult = myIsUserName ? app->moduleTitle( myName ) : app->moduleName( myName );
    }
  }
};
const char* SALOMEGUI_Swig::getComponentName( const char* componentUserName )
{
  QString result = ProcessEvent( new TGetComponentNameEvent( componentUserName, false ) );
  return result.isEmpty() ? 0 : strdup( result.toLatin1().constData() );
}
const char* SALOMEGUI_Swig::getComponentUserName( const char* componentName )
{
  QString result = ProcessEvent( new TGetComponentNameEvent( componentName, true ) );
  return result.isEmpty() ? 0 : strdup( result.toLatin1().constData() );
}

/*!
  \fn int SALOMEGUI_Swig::SelectedCount()
  \brief Get number of selected items
  \return number of selected items in the active study
*/

/*!
  \fn const char* SALOMEGUI_Swig::getSelected( int index )
  \brief Get entry of the specified selected item
  \param index selected object index
  \return selected object entry (null string if index is invalid)
*/

class TGetSelectedEvent: public SALOME_Event
{
public:
  typedef QStringList TResult;
  TResult myResult;
  TGetSelectedEvent() {}
  virtual void Execute()
  {
    if ( SalomeApp_Application* anApp = getApplication() ) {
      SalomeApp_Study* aStudy  = dynamic_cast<SalomeApp_Study*>( anApp->activeStudy() ); // for sure!
      LightApp_SelectionMgr* aSelMgr = anApp->selectionMgr(); 
      if ( aStudy && aSelMgr ) {
	SUIT_DataOwnerPtrList aList;
	aSelMgr->selected( aList );

	for ( SUIT_DataOwnerPtrList::const_iterator itr = aList.begin(); 
	      itr != aList.end(); ++itr ) {
	  const LightApp_DataOwner* owner = 
	    dynamic_cast<const LightApp_DataOwner*>( (*itr).operator->() );
	  if( !owner )
	    continue;
	  QString entry = owner->entry();
	  if( !myResult.contains( entry ) )
	    myResult.append( entry );
	}
      }
    }
  }
};
int SALOMEGUI_Swig::SelectedCount()
{
  QStringList selected = ProcessEvent( new TGetSelectedEvent() );
  return selected.count();
}
const char* SALOMEGUI_Swig::getSelected( int index )
{
  QStringList selected = ProcessEvent( new TGetSelectedEvent() );
  return index >= 0 && index < selected.count() ? 
    strdup( selected[ index ].toLatin1().constData() ) : 0;
}

/*!
  \brief Add an object to the current selection.
  \param theEntry object entry
*/
void SALOMEGUI_Swig::AddIObject( const char* theEntry )
{
  class TEvent: public SALOME_Event
  {
  public:
    QString myEntry;
    TEvent( const char* theEntry ) : myEntry( theEntry ) {}
    virtual void Execute()
    {
      if ( SalomeApp_Application* anApp = getApplication() ) {
	SalomeApp_Study*       aStudy  = dynamic_cast<SalomeApp_Study*>( anApp->activeStudy() ); // for sure!
	LightApp_SelectionMgr* aSelMgr = anApp->selectionMgr(); 
	if ( aStudy && aSelMgr ) {
	  SALOME_ListIO anIOList;
	  anIOList.Append( new SALOME_InteractiveObject( myEntry.toLatin1(), "", "" ) );
	  aSelMgr->setSelectedObjects( anIOList, true );
	}
      }
    }
  };
  ProcessVoidEvent( new TEvent( theEntry ) );
}

/*!
  \brief Remove the object from the selection.
  \param theEntry object entry
*/
void SALOMEGUI_Swig::RemoveIObject( const char* theEntry )
{
  class TEvent: public SALOME_Event
  {
  public:
    QString myEntry;
    TEvent( const char* theEntry ) : myEntry( theEntry ) {}
    virtual void Execute()
    {
      if ( SalomeApp_Application* anApp = getApplication() ) {
	SalomeApp_Study* aStudy  = dynamic_cast<SalomeApp_Study*>( anApp->activeStudy() ); // for sure!
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
	    if ( anIter.Value()->isSame( new SALOME_InteractiveObject( myEntry.toLatin1(), "", "" ) ) ) { 
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
  \brief Clear selection (unselect all objects).
*/
void SALOMEGUI_Swig::ClearIObjects()
{
  class TEvent: public SALOME_Event
  {
  public:
    TEvent() {}
    virtual void Execute()
    {
      if ( SalomeApp_Application* anApp = getApplication() ) {
	SalomeApp_Study* aStudy  = dynamic_cast<SalomeApp_Study*>( anApp->activeStudy() ); // for sure!
	LightApp_SelectionMgr* aSelMgr = anApp->selectionMgr(); 
	if ( aStudy && aSelMgr )
	  aSelMgr->clearSelected();
      }
    }
  };
  ProcessVoidEvent( new TEvent() );
}

/*!
  \brief Display an object in the current view window.

  The presentable object should be previously created and
  displayed in this viewer.

  For the current moment implemented for OCC and VTK viewers only.

  \param theEntry object entry
*/		
void SALOMEGUI_Swig::Display( const char* theEntry )
{
  class TEvent: public SALOME_Event
  {
    QString myEntry;
  public:
    TEvent( const char* theEntry ) : myEntry( theEntry ) {}
    virtual void Execute() {
      if ( SalomeApp_Application* anApp = getApplication() ) {
	SUIT_ViewWindow* window = anApp->desktop()->activeWindow();
	if ( window ) {
	  SALOME_View* view = dynamic_cast<SALOME_View*>( window->getViewManager()->getViewModel() );
	  if ( view )
	    view->Display( view->CreatePrs( myEntry.toLatin1() ) );
	}
      }
    }
  };
  ProcessVoidEvent( new TEvent( theEntry ) );
}

/*!
  \brief Displays an object in the current view window and 
  erases all other ones.

  The presentable object should be previously created and 
  displayed in this viewer.

  For the current moment implemented for OCC and VTK viewers only.
  
  \param theEntry object entry
*/
void SALOMEGUI_Swig::DisplayOnly( const char* theEntry )
{
  class TEvent: public SALOME_Event
  {
    QString myEntry;
  public:
    TEvent( const char* theEntry ) : myEntry( theEntry ) {}
    virtual void Execute()
    {
      if ( SalomeApp_Application* anApp = getApplication() ) {
	SUIT_ViewWindow* window = anApp->desktop()->activeWindow();
	if ( window ) {
	  SALOME_View* view = dynamic_cast<SALOME_View*>( window->getViewManager()->getViewModel() );
	  if ( view ) {
	    view->EraseAll( false );
	    view->Display( view->CreatePrs( myEntry.toLatin1() ) );
	  }
	}
      }
    }
  };
  ProcessVoidEvent( new TEvent( theEntry ) );
}

/*!
  \brief Erase an object in the current view window.

  The presentable object should be previously created and 
  displayed in this viewer.

  For the current moment implemented for OCC and VTK viewers only.

  \param theEntry object entry
*/		
void SALOMEGUI_Swig::Erase( const char* theEntry )
{
  class TEvent: public SALOME_Event
  {
    QString myEntry;
  public:
    TEvent( const char* theEntry ) : myEntry( theEntry ) {}
    virtual void Execute()
    {
      if ( SalomeApp_Application* anApp = getApplication() ) {
	SUIT_ViewWindow* window = anApp->desktop()->activeWindow();
	if ( window ) {
	  SALOME_View* view = dynamic_cast<SALOME_View*>( window->getViewManager()->getViewModel() );
	  if ( view )
	    view->Erase( view->CreatePrs( myEntry.toLatin1() ) );
	}
      }
    }
  };
  ProcessVoidEvent( new TEvent( theEntry ) );
}

/*!
  \brief Display all active module's presentable 
  child objects in the current view window.
  
  The presentable objects should be previously created and
  displayed in this viewer.

  For the current moment implemented for OCC and VTK viewers only.
*/
void SALOMEGUI_Swig::DisplayAll()
{
  class TEvent: public SALOME_Event
  {
  public:
    TEvent() {}
    virtual void Execute()
    {
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
		view->Display( view->CreatePrs( obj->entry().toLatin1() ) );
	    }
	  }
	}
      }
    }
  };
  ProcessVoidEvent( new TEvent() );
}

/*!
  \brief Erase all objects from the current view window.
  
  For the current moment implemented for OCC and VTK viewers only.
*/
void SALOMEGUI_Swig::EraseAll()
{
  class TEvent: public SALOME_Event
  {
  public:
    TEvent() {}
    virtual void Execute()
    {
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
  \fn bool SALOMEGUI_Swig::IsInCurrentView( const char* theEntry )
  \brief Check it the object is displayed in the current view window.

  VSR: For the current moment implemented for OCC and VTK viewers only.

  \param theEntry object entry
  \return \c true if the object with given entry is displayed 
          in the current viewer
*/

class TIsInViewerEvent: public SALOME_Event
{
  QString myEntry;
public:
  typedef bool TResult;
  TResult myResult;
  TIsInViewerEvent( const char* theEntry ) : myEntry( theEntry ), myResult( false ) {}
  virtual void Execute()
  {
    if ( SalomeApp_Application* anApp = getApplication() ) {
      SUIT_ViewWindow* window = anApp->desktop()->activeWindow();
      if ( window ) {
	SALOME_View* view = dynamic_cast<SALOME_View*>( window->getViewManager()->getViewModel() );
	if ( view ) {
	  SALOME_Prs* aPrs = view->CreatePrs( myEntry.toLatin1() );
	  myResult = !aPrs->IsNull();
	}
      }
    }
  }
};
bool SALOMEGUI_Swig::IsInCurrentView( const char* theEntry )
{
  return ProcessEvent( new TIsInViewerEvent( theEntry ) );
}

/*!
  \brief Update (repaint) current view window.
*/
void SALOMEGUI_Swig::UpdateView()
{
  class TEvent: public SALOME_Event
  {
  public:
    TEvent() {}
    virtual void Execute()
    {
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
  \brief Fit current view window to display all its contents.
*/
void SALOMEGUI_Swig::FitAll()
{
  class TEvent: public SALOME_Event
  {
  public:
    TEvent() {}
    virtual void Execute()
    {
      if ( SalomeApp_Application* anApp = getApplication() ) {
	SUIT_ViewWindow* window = anApp->desktop()->activeWindow();
	if ( window ) {
	  if ( dynamic_cast<SVTK_ViewWindow*>( window ) )
	    ( dynamic_cast<SVTK_ViewWindow*>( window ) )->onFitAll();
	  else if ( dynamic_cast<SOCC_ViewWindow*>( window ) )
	    ( dynamic_cast<SOCC_ViewWindow*>( window ) )->onFitAll();
	  else if ( dynamic_cast<SPlot2d_ViewWindow*>( window ) )
	    ( dynamic_cast<SPlot2d_ViewWindow*>( window ) )->onFitAll();
	}
      }
    }
  };
  ProcessVoidEvent( new TEvent() );
}

/*!
  \brief Reset current view window to the default state.
*/
void SALOMEGUI_Swig::ResetView()
{
  class TEvent: public SALOME_Event
  {
  public:
    TEvent() {}
    virtual void Execute()
    {
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

/*!
  \brief View operation type.
  \internal
*/
enum {
  __ViewTop,          //!< view top side
  __ViewBottom,       //!< view bottom side
  __ViewLeft,         //!< view left side
  __ViewRight,        //!< view right side
  __ViewFront,        //!< view front side
  __ViewBack          //!< view back side
};

/*!
  \brief Change the view of the current view window.
  \internal
  \param view view operation type
*/
static void setView( int view )
{
  class TEvent: public SALOME_Event
  {
  private:
    int myView;
  public:
    TEvent( int view ) : myView( view ) {}
    virtual void Execute()
    {
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
  \brief Switch current view window to show the top view.
*/
void SALOMEGUI_Swig::ViewTop()
{
  setView( __ViewTop );
}

/*!
  \brief Switch current view window to show the bottom view
*/
void SALOMEGUI_Swig::ViewBottom()
{
  setView( __ViewBottom );
}

/*!
  \brief Switch current view window to show the left view
*/
void SALOMEGUI_Swig::ViewLeft()
{
  setView( __ViewLeft );
}

/*!
  \brief Switch current view window to show the right view
*/
void SALOMEGUI_Swig::ViewRight()
{
  setView( __ViewRight );
}

/*!
  \brief Switch current view window to show the front view
*/
void SALOMEGUI_Swig::ViewFront()
{
  setView( __ViewFront );
}

/*!
  \brief Switch current view window to show the back view
*/
void SALOMEGUI_Swig::ViewBack()
{
  setView( __ViewBack );
}
