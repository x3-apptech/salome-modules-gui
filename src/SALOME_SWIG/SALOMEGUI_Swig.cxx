// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
#include <LightApp_Application.h>
#include <LightApp_Displayer.h>
#include <LightApp_Study.h>
#include <LightApp_Module.h>
#include <LightApp_DataObject.h>
#include <LightApp_SelectionMgr.h>
#include <LightApp_DataOwner.h>
#include <SALOME_Prs.h>
#include <SALOME_Event.h>

#ifndef DISABLE_SALOMEOBJECT
  #include <SALOME_ListIO.hxx>
  #include <SALOME_InteractiveObject.hxx>
#ifndef DISABLE_OCCVIEWER
    #include <SOCC_ViewModel.h>
    #include <SOCC_ViewWindow.h>
#endif
#ifndef DISABLE_VTKVIEWER
    #include <SVTK_ViewModel.h>
    #include <SVTK_ViewWindow.h>
    #include <SVTK_Renderer.h>
    
    #include <vtkCamera.h>
    #include <vtkRenderer.h>
#endif
#ifndef DISABLE_PLOT2DVIEWER
    #include <SPlot2d_ViewWindow.h>
#endif
#endif


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
static LightApp_Application* getApplication()
{
  if ( SUIT_Session::session() )
    return dynamic_cast<LightApp_Application*>( SUIT_Session::session()->activeApplication() );
  return 0;
}

/*!
  \brief Get active study object
  \internal
  \return active study or 0 if there is no study opened
*/
static LightApp_Study* getActiveStudy()
{
  if ( getApplication() )
    return dynamic_cast<LightApp_Study*>( getApplication()->activeStudy() );
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
      if ( LightApp_Application* anApp = getApplication() ) {
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
    if ( LightApp_Study* aStudy = getActiveStudy() ) {
      myResult = aStudy->id();
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
    if ( LightApp_Study* aStudy = getActiveStudy() ) {
      myResult = aStudy->studyName().toUtf8().constData();
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
    if ( LightApp_Application* app = getApplication() ) {
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
    if ( LightApp_Application* anApp = getApplication() ) {
      LightApp_Study* aStudy  = dynamic_cast<LightApp_Study*>( anApp->activeStudy() ); // for sure!
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
      if ( LightApp_Application* anApp = getApplication() ) {
        LightApp_Study*       aStudy  = dynamic_cast<LightApp_Study*>( anApp->activeStudy() ); // for sure!
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
      if ( LightApp_Application* anApp = getApplication() ) {
        LightApp_Study* aStudy  = dynamic_cast<LightApp_Study*>( anApp->activeStudy() ); // for sure!
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
      if ( LightApp_Application* anApp = getApplication() ) {
        LightApp_Study* aStudy  = dynamic_cast<LightApp_Study*>( anApp->activeStudy() ); // for sure!
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
      LightApp_Application* anApp  = getApplication();
      LightApp_Study*       aStudy = getActiveStudy();
      if ( anApp && aStudy ) {
	QString mname = anApp->moduleTitle( aStudy->componentDataType( myEntry ) );
	LightApp_Displayer* d = LightApp_Displayer::FindDisplayer( mname, true );
	if ( d ) {
	  QStringList entries;
	  if( aStudy->isComponent( myEntry ) )
	    aStudy->children( myEntry, entries );
	  else
	    entries.append( myEntry );
	  foreach( QString entry, entries )
	    d->Display( aStudy->referencedToEntry( entry ), false, 0 );
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
      LightApp_Application* anApp  = getApplication();
      LightApp_Study*       aStudy = getActiveStudy();
      if ( anApp && aStudy ) {
	QStringList comps;
	aStudy->components( comps );
	foreach( QString comp, comps ) {
	  LightApp_Displayer* d = LightApp_Displayer::FindDisplayer( anApp->moduleTitle( comp ), true );
	  if ( d ) d->EraseAll( false, false, 0 );
	}

	QString mname = anApp->moduleTitle( aStudy->componentDataType( myEntry ) );
	LightApp_Displayer* d = LightApp_Displayer::FindDisplayer( mname, true );
	if ( d ) {
	  QStringList entries;
	  if( aStudy->isComponent( myEntry ) )
	    aStudy->children( myEntry, entries );
	  else
	    entries.append( myEntry );
	  foreach( QString entry, entries )
	    d->Display( aStudy->referencedToEntry( entry ), false, 0 );
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
      LightApp_Application* anApp  = getApplication();
      LightApp_Study*       aStudy = getActiveStudy();
      if ( anApp && aStudy ) {
	QString mname = anApp->moduleTitle( aStudy->componentDataType( myEntry ) );
	LightApp_Displayer* d = LightApp_Displayer::FindDisplayer( mname, true );
	if ( d ) {
	  QStringList entries;
	  if( aStudy->isComponent( myEntry ) )
	    aStudy->children( myEntry, entries );
	  else
	    entries.append( myEntry );
	  foreach( QString entry, entries )
	    d->Erase( aStudy->referencedToEntry( entry ), false, false, 0 );
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
*/
void SALOMEGUI_Swig::DisplayAll()
{
  class TEvent: public SALOME_Event
  {
  public:
    TEvent() {}
    virtual void Execute()
    {
      LightApp_Application* anApp  = getApplication();
      LightApp_Study*       aStudy = getActiveStudy();
      if ( anApp && aStudy ) {
	QStringList comps;
	aStudy->components( comps );
	foreach( QString comp, comps ) {
	  LightApp_Displayer* d = LightApp_Displayer::FindDisplayer( anApp->moduleTitle( comp ), true );
	  if ( d ) {
	    QStringList entries;
	    aStudy->children( aStudy->centry( comp ), entries );
	    foreach( QString entry, entries )
	      d->Display( aStudy->referencedToEntry( entry ), false, 0 );
	  }
	}
      }
    }
  };
  ProcessVoidEvent( new TEvent() );
}

/*!
  \brief Erase all objects from the current view window.
*/
void SALOMEGUI_Swig::EraseAll()
{
  class TEvent: public SALOME_Event
  {
  public:
    TEvent() {}
    virtual void Execute()
    {
      LightApp_Application* anApp  = getApplication();
      LightApp_Study*       aStudy = getActiveStudy();
      if ( anApp && aStudy ) {
	QStringList comps;
	aStudy->components( comps );
	foreach( QString comp, comps ) {
	  LightApp_Displayer* d = LightApp_Displayer::FindDisplayer( anApp->moduleTitle( comp ), true );
	  if ( d ) d->EraseAll( false, false, 0 );
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
    if ( LightApp_Application* anApp = getApplication() ) {
      SUIT_ViewWindow* window = anApp->desktop()->activeWindow();
      if ( window ) {
        SALOME_View* view = dynamic_cast<SALOME_View*>( window->getViewManager()->getActiveView() );
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
      if ( LightApp_Application* anApp = getApplication() ) {
        SUIT_ViewWindow* window = anApp->desktop()->activeWindow();
        if ( window ) {
          SALOME_View* view = dynamic_cast<SALOME_View*>( window->getViewManager()->getActiveView() );
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
      if ( LightApp_Application* anApp = getApplication() ) {
        SUIT_ViewWindow* window = anApp->desktop()->activeWindow();
        if ( window ) {
#ifndef DISABLE_SALOMEOBJECT
#ifndef DISABLE_VTKVIEWER
          if ( dynamic_cast<SVTK_ViewWindow*>( window ) )
            ( dynamic_cast<SVTK_ViewWindow*>( window ) )->onFitAll();
#endif
#ifndef DISABLE_PLOT2DVIEWER
          if ( dynamic_cast<SPlot2d_ViewWindow*>( window ) )
            ( dynamic_cast<SPlot2d_ViewWindow*>( window ) )->onFitAll();
#endif
#endif
#ifndef DISABLE_OCCVIEWER
          if ( dynamic_cast<OCCViewer_ViewWindow*>( window ) )
            ( dynamic_cast<OCCViewer_ViewWindow*>( window ) )->onFitAll();
#endif
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
      if ( LightApp_Application* anApp = getApplication() ) {
        SUIT_ViewWindow* window = anApp->desktop()->activeWindow();
        if ( window ) {
#ifndef DISABLE_SALOMEOBJECT
#ifndef DISABLE_VTKVIEWER
          if ( dynamic_cast<SVTK_ViewWindow*>( window ) )
            (dynamic_cast<SVTK_ViewWindow*>( window ))->onResetView();
#endif
#ifndef DISABLE_PLOT2DVIEWER
          if ( dynamic_cast<SPlot2d_ViewWindow*>( window ) )
            (dynamic_cast<SPlot2d_ViewWindow*>( window ))->onFitAll();
          // VSR: there is no 'ResetView' functionality for Plot2d viewer,
          // so we use 'FitAll' instead.
#endif
#endif
#ifndef DISABLE_OCCVIEWER
          if ( dynamic_cast<OCCViewer_ViewWindow*>( window ) )
            (dynamic_cast<OCCViewer_ViewWindow*>( window ))->onResetView();
#endif
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
      if ( LightApp_Application* anApp = getApplication() ) {
        SUIT_ViewWindow* window = anApp->desktop()->activeWindow();
        if ( window ) {
#ifndef DISABLE_SALOMEOBJECT
#ifndef DISABLE_VTKVIEWER
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
#endif
#endif
#ifndef DISABLE_OCCVIEWER
          if ( dynamic_cast<OCCViewer_ViewWindow*>( window ) ) {
            switch( myView ) {
            case __ViewTop:
              (dynamic_cast<OCCViewer_ViewWindow*>( window ))->onTopView(); break;
            case __ViewBottom:
              (dynamic_cast<OCCViewer_ViewWindow*>( window ))->onBottomView(); break;
            case __ViewLeft:
              (dynamic_cast<OCCViewer_ViewWindow*>( window ))->onLeftView(); break;
            case __ViewRight:
              (dynamic_cast<OCCViewer_ViewWindow*>( window ))->onRightView(); break;
            case __ViewFront:
              (dynamic_cast<OCCViewer_ViewWindow*>( window ))->onFrontView(); break;
            case __ViewBack:
              (dynamic_cast<OCCViewer_ViewWindow*>( window ))->onBackView(); break;
            default:
              break;
            }
          }
#endif
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

/*
  \fn bool SALOMEGUI_Swig::getViewParameters()
  \brief Get camera parameters of the active view.

  NOTE: For the current moment implemented for VTK viewer only.

  \return \c string with the view parameters
*/

class TGetViewParameters: public SALOME_Event
{
public:
  typedef QString TResult;
  TResult myResult;
  TGetViewParameters() : myResult( "" ) {}
  virtual void Execute() {  
    if ( LightApp_Application* anApp = getApplication() ) {
      if ( SUIT_ViewWindow* window = anApp->desktop()->activeWindow() ) {
#ifndef DISABLE_VTKVIEWER
	if ( SVTK_ViewWindow* svtk = dynamic_cast<SVTK_ViewWindow*>( window ) ) {	  
	  if ( vtkRenderer* ren = svtk->getRenderer()) {		    
	    if ( vtkCamera* camera = ren->GetActiveCamera() ) {
   	      double pos[3], focalPnt[3], viewUp[3], scale[3], parScale;	    
	      
	      // save position, focal point, viewUp, scale
	      camera->GetPosition( pos );
	      camera->GetFocalPoint( focalPnt );
	      camera->GetViewUp( viewUp );
	      parScale = camera->GetParallelScale();
	      svtk->GetRenderer()->GetScale( scale );

	      myResult += QString("sg.setCameraPosition( %1, %2, %3 )\n").arg(pos[0]).arg(pos[1]).arg(pos[2]);
	      myResult += QString("sg.setCameraFocalPoint( %1, %2, %3 )\n").arg(focalPnt[0]).arg(focalPnt[1]).arg(focalPnt[2]);
	      myResult += QString("sg.setCameraViewUp( %1, %2, %3 )\n").arg(viewUp[0]).arg(viewUp[1]).arg(viewUp[2]);
	      myResult += QString("sg.setViewScale(%1, %2, %3, %4 )\n").arg(parScale).arg(scale[0]).arg(scale[1]).arg(scale[2]);
	    }
	  }
        }
#endif
      }
    }
  }
};
	
const char* SALOMEGUI_Swig::getViewParameters() {
  QString result = ProcessEvent( new TGetViewParameters() );
  return result.isEmpty() ? 0 : strdup( result.toLatin1().constData() );  
}


/*!
  \brief View parameter type.
  \internal
*/
enum {
  __CameraPosition,   //!< position of the active camera
  __CameraFocalPoint, //!< focal point of the active camera      
  __CameraViewUp,     //!< view up of the active camera         
  __ViewScale         //!< scale of the view
};


/*!
  \brief Change the camera parameters of the current view window.
  \internal

  NOTE: For the current moment implemented for VTK viewer only.

  \param parameter type of the parameter
  \param values value of the parameter
*/
static void setViewParameter( int parameter, QList<double>& values ) {
  class TEvent: public SALOME_Event {
  private:
    int           myParameter;
    QList<double> myValues;
  public:
    TEvent( int parameter , QList<double>& values ) : myParameter(parameter), myValues( values ) {}

    virtual void Execute() {
      if ( LightApp_Application* anApp = getApplication() ) {
	if ( SUIT_ViewWindow* window = anApp->desktop()->activeWindow() ) {
#ifndef DISABLE_VTKVIEWER
	  if ( SVTK_ViewWindow* svtk = dynamic_cast<SVTK_ViewWindow*>( window ) ) {	  
	    if ( vtkRenderer* ren = svtk->getRenderer()) {		    
	      if ( vtkCamera* camera = ren->GetActiveCamera() ) {
	        switch(myParameter) {	    
                  case __CameraPosition : {
	            if ( myValues.size() == 3 ) {  
	              camera->SetPosition( myValues[0], myValues[1], myValues[2] );
                    }
                    break;
                  }
                  case __CameraFocalPoint : {
	            if ( myValues.size() == 3 ) {  
	              camera->SetFocalPoint( myValues[0], myValues[1], myValues[2] );
                    }
                    break;
                  }
                  case __CameraViewUp : {
	            if ( myValues.size() == 3 ) {  
	              camera->SetViewUp( myValues[0], myValues[1], myValues[2] );
                    }
                    break;
                  }
                  case __ViewScale : {
	            if ( myValues.size() == 4 ) {  
	              camera->SetParallelScale( myValues[0] );
                      double scale[] = { myValues[1], myValues[2], myValues[3] };
                      svtk->GetRenderer()->SetScale( scale );
                    }
                    break;
                  }
                  default: break;
                }
              }
            }
	    svtk->Repaint();
          }
#endif
        }
      }
    }
  };
  ProcessVoidEvent( new TEvent( parameter, values ) );
}

/*!
  \brief Set camera position of the active view .
  \param x - X coordinate of the camera
  \param y - Y coordinate of the camera
  \param z - Z coordinate of the camera
*/
void SALOMEGUI_Swig::setCameraPosition( double x, double y, double z ) {
  QList<double> lst;
  lst.push_back( x );
  lst.push_back( y );
  lst.push_back( z );
  setViewParameter( __CameraPosition, lst );
}

/*!
  \brief Set camera focal point of the active view.
  \param x - X coordinate of the focal point
  \param y - Y coordinate of the focal point
  \param z - Z coordinate of the focal point
*/
void SALOMEGUI_Swig::setCameraFocalPoint( double x, double y, double z ) {
  QList<double> lst;
  lst.push_back( x );
  lst.push_back( y );
  lst.push_back( z );
  setViewParameter( __CameraFocalPoint, lst );
}

/*!
  \brief Set the view up direction for the camera.
  \param x - X component of the direction vector
  \param y - Y component of the direction vector
  \param z - Z component of the direction vector
*/
void SALOMEGUI_Swig::setCameraViewUp( double x, double y, double z ) {
  QList<double> lst;
  lst.push_back( x );
  lst.push_back( y );
  lst.push_back( z );
  setViewParameter( __CameraViewUp, lst );
}

/*!
  \brief Set view scale.
  \param parallelScale  - scaling used for a parallel projection.
  \param x - X scale
  \param y - Y scale
  \param z - Z scale
*/
void SALOMEGUI_Swig::setViewScale( double parallelScale, double x, double y, double z ) {
  QList<double> lst;
  lst.push_back( parallelScale );
  lst.push_back( x );
  lst.push_back( y );
  lst.push_back( z );
  setViewParameter( __ViewScale, lst );
}

     
