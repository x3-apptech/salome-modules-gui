// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

// File   : SalomePyQt.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#ifdef WNT
// E.A. : On windows with python 2.6, there is a conflict
// E.A. : between pymath.h and Standard_math.h which define
// E.A. : some same symbols : acosh, asinh, ...
#include <Standard_math.hxx>
#include <pymath.h>
#endif

#include <SALOME_PYQT_ModuleLight.h> // this include must be first!!!
#include <SALOME_PYQT_DataModelLight.h>
#include "SalomePyQt.h"

#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QImage>
#include <QStringList>
#include <QAction>

#include <SALOME_Event.h>

#include <QtxActionMenuMgr.h>
#include <QtxActionGroup.h>
#include <QtxWorkstack.h>
#include <QtxTreeView.h>
#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Tools.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_DataBrowser.h>
#include <STD_TabDesktop.h>
#include <LightApp_Application.h>
#include <LightApp_Study.h>
#include <LightApp_SelectionMgr.h>
#include <LogWindow.h>
#include <OCCViewer_ViewWindow.h>
#include <Plot2d_ViewManager.h>
#include <Plot2d_ViewWindow.h>

/*!
  \brief Get the currently active application.
  \internal
  \return active application object or 0 if there is no any
*/
static LightApp_Application* getApplication()
{
  if ( SUIT_Session::session() )
    return dynamic_cast<LightApp_Application*>( SUIT_Session::session()->activeApplication() );
  return 0;
}

/*!
  \brief Get the currently active study.
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
  \brief Get the currently active module.
  \internal
  This function returns correct result only if Python-based
  module is currently active. Otherwize, 0 is returned.
*/
static SALOME_PYQT_ModuleLight* getActiveModule()
{
  SALOME_PYQT_ModuleLight* module = 0;
  if ( LightApp_Application* anApp = getApplication() ) {
    module = SALOME_PYQT_ModuleLight::getInitModule();
    if ( !module )
      module = dynamic_cast<SALOME_PYQT_ModuleLight*>( anApp->activeModule() );
  }
  return module;
}

/*!
  \class SALOME_Selection
  \brief The class represents selection which can be used in Python.
*/

/*!
  \brief Map of created selection objects.
  \internal
*/
static QMap<LightApp_Application*, SALOME_Selection*> SelMap;

/*!
  \brief Get the selection object for the specified application.

  Finds or creates the selection object (one per study).

  \param app application object
  \return selection object or 0 if \a app is invalid
*/
SALOME_Selection* SALOME_Selection::GetSelection( LightApp_Application* app )
{
  SALOME_Selection* sel = 0;
  if ( app && SelMap.find( app ) != SelMap.end() )
    sel = SelMap[ app ];
  else 
    sel = SelMap[ app ] = new SALOME_Selection( app );
  return sel;
}

/*!
  \brief Constructor.
  \param p parent object
*/
SALOME_Selection::SALOME_Selection( QObject* p ) : QObject( 0 ), mySelMgr( 0 )
{
  LightApp_Application* app = dynamic_cast<LightApp_Application*>( p );
  if ( app ) {
    mySelMgr = app->selectionMgr();
    connect( mySelMgr, SIGNAL( selectionChanged() ), this, SIGNAL( currentSelectionChanged() ) );
    connect( mySelMgr, SIGNAL( destroyed() ),        this, SLOT  ( onSelMgrDestroyed() ) );
  }
}

/*!
  \brief Destructor.
*/
SALOME_Selection::~SALOME_Selection()
{
  LightApp_Application* app = 0;
  QMap<LightApp_Application*, SALOME_Selection*>::Iterator it;
  for ( it = SelMap.begin(); it != SelMap.end() && !app; ++it ) {
    if ( it.value() == this ) app = it.key();
  }
  if ( app ) SelMap.remove( app );
}

/*!
  \brief Called when selection manager is destroyed (usually 
  when the study is closed).
*/
void SALOME_Selection::onSelMgrDestroyed()
{
  mySelMgr = 0;
}

/*!
  \brief Clear the selection.
*/
void SALOME_Selection::Clear()
{
  class TEvent: public SALOME_Event {
    LightApp_SelectionMgr* mySelMgr;
  public:
    TEvent( LightApp_SelectionMgr* selMgr ) 
      : mySelMgr( selMgr ) {}
    virtual void Execute() {
      if ( mySelMgr )
        mySelMgr->clearSelected();
    }
  };
  ProcessVoidEvent( new TEvent( mySelMgr ) );
}

/*!
  \brief Clear the selection.
*/
void SALOME_Selection::ClearIObjects()
{
  Clear();
}

/*!
  Removes all selection filters.
*/
void SALOME_Selection::ClearFilters()
{
  class TEvent: public SALOME_Event {
    LightApp_SelectionMgr* mySelMgr;
  public:
    TEvent( LightApp_SelectionMgr* selMgr ) 
      : mySelMgr( selMgr ) {}
    virtual void Execute() {
      if ( mySelMgr )
        mySelMgr->clearFilters();
    }
  };
  ProcessVoidEvent( new TEvent( mySelMgr ) );
}

/*!
  \class SalomePyQt
  \brief The class provides utility functions which can be used in the Python
  to operate with the SALOME GUI.

  All the functionality of this class is implemented as static methods, so they
  can be called with the class name prefixed or via creation of the class instance.
  For example, next both ways of SalomePyQt class usage are legal:
  \code
  from SalomePyQt import *
  sg = SalomePyQt()
  # using SalomePyQt class instance
  desktop = sg.getDesktop()
  # using SalomePyQt class directly
  menubar = SalomePyQt.getMainMenuBar()
  \endcode
*/

/*!
  \fn QWidget* SalomePyQt::getDesktop();
  \brief Get the active application's desktop window.
  \return desktop window or 0 if there is no any
*/

class TGetDesktopEvent: public SALOME_Event 
{
public:
  typedef QWidget* TResult;
  TResult myResult;
  TGetDesktopEvent() : myResult( 0 ) {}
  virtual void Execute()
  {
    if ( getApplication() )
      myResult = (QWidget*)( getApplication()->desktop() );
  }
};
QWidget* SalomePyQt::getDesktop()
{
  return ProcessEvent( new TGetDesktopEvent() );
}

/*!
  \fn QWidget* SalomePyQt::getMainFrame();
  \brief Get current application's main frame widget [obsolete].

  Main frame widget is an internal widget of the application 
  desktop window (workspace).

  \return workspace widget (0 on any error)
*/

class TGetMainFrameEvent: public SALOME_Event
{
public:
  typedef QWidget* TResult;
  TResult myResult;
  TGetMainFrameEvent() : myResult( 0 ) {}
  virtual void Execute()
  {
    if ( getApplication() ) {
      SUIT_Desktop* aDesktop = getApplication()->desktop();
      myResult = (QWidget*)( aDesktop->centralWidget() );
    }
  }
};
QWidget* SalomePyQt::getMainFrame()
{
  return ProcessEvent( new TGetMainFrameEvent() );
}

/*!
  \fn QMenuBar* SalomePyQt::getMainMenuBar();
  \brief Get current application desktop's main menu.
  \return main menu object (0 on any error)
*/

class TGetMainMenuBarEvent: public SALOME_Event
{
public:
  typedef QMenuBar* TResult;
  TResult myResult;
  TGetMainMenuBarEvent() : myResult( 0 ) {}
  virtual void Execute()
  {
    if ( LightApp_Application* anApp = getApplication() ) {
      myResult = anApp->desktop()->menuBar();
    }
  }
};
QMenuBar* SalomePyQt::getMainMenuBar() 
{
  return ProcessEvent( new TGetMainMenuBarEvent() );
}

/*!
  \fn QMenu* SalomePyQt::getPopupMenu( const MenuName menu );
  \brief Get main menu's child popup submenu by its identifier.
  
  This function is obsolete. 
  Use QMenu* SalomePyQt::getPopupMenu( const QString& menu ) instead.

  \param menu menu identifier
  \return popup submenu object or 0 if it does not exist
*/

/*!
  \fn QMenu* SalomePyQt::getPopupMenu( const QString& menu );
  \brief Get main menu's child popup submenu by its name.
  
  The function creates menu if it does not exist.

  \param menu menu name
  \return popup submenu object (0 on any error)
*/

class TGetPopupMenuEvent: public SALOME_Event
{
public:
  typedef QMenu* TResult;
  TResult myResult;
  QString myMenuName;
  TGetPopupMenuEvent( const QString& menu ) : myResult( 0 ), myMenuName( menu ) {}
  virtual void Execute()
  {
    LightApp_Application* anApp = getApplication();
    if ( anApp && !myMenuName.isEmpty() ) {
      QtxActionMenuMgr* mgr = anApp->desktop()->menuMgr();
      myResult = mgr->findMenu( myMenuName, -1, false ); // search only top menu
    }
  }
};

/*!
  \brief Get menu item title
  \internal
  \param menuId menu identifier
  \return menu title (localized)
*/
static QString getMenuName( const QString& menuId )
{
  QStringList contexts;
  contexts << "SalomeApp_Application" << "LightApp_Application" << "STD_TabDesktop" <<
    "STD_MDIDesktop" << "STD_Application" << "SUIT_Application" << "";
  QString menuName = menuId;
  for ( int i = 0; i < contexts.count() && menuName == menuId; i++ )
    menuName = QApplication::translate( contexts[i].toLatin1().data(), menuId.toLatin1().data() );
  return menuName;
}

QMenu* SalomePyQt::getPopupMenu( const MenuName menu )
{
  QString menuName;
  switch( menu ) {
  case File:
    menuName = getMenuName( "MEN_DESK_FILE" );        break;
  case View:
    menuName = getMenuName( "MEN_DESK_VIEW" );        break;
  case Edit:
    menuName = getMenuName( "MEN_DESK_EDIT" );        break;
  case Preferences:
    menuName = getMenuName( "MEN_DESK_PREFERENCES" ); break;
  case Tools:
    menuName = getMenuName( "MEN_DESK_TOOLS" );       break;
  case Window:
    menuName = getMenuName( "MEN_DESK_WINDOW" );      break;
  case Help:
    menuName = getMenuName( "MEN_DESK_HELP" );        break;
  }
  return ProcessEvent( new TGetPopupMenuEvent( menuName ) );
}
QMenu* SalomePyQt::getPopupMenu( const QString& menu )
{
  return ProcessEvent( new TGetPopupMenuEvent( menu ) );
}

/*!
  \fn QTreeView* SalomePyQt::getObjectBrowser();
  \brief Get object browser
  \return object browser for the active study or 0 in case of error
*/

class TGetObjectBrowserEvent: public SALOME_Event
{
public:
  typedef QTreeView* TResult;
  TResult myResult;
  TGetObjectBrowserEvent() : myResult( 0 ) {}
  virtual void Execute()
  {
    LightApp_Application* anApp = getApplication();
    if ( anApp ) {
      myResult = anApp->objectBrowser()->treeView();
    }
  }
};
QTreeView* SalomePyQt::getObjectBrowser()
{
  return ProcessEvent( new TGetObjectBrowserEvent() );
}

/*!
  \fn int SalomePyQt::getStudyId();
  \brief Get active study's identifier.
  \return active study ID or 0 if there is no active study
*/

class TGetStudyIdEvent: public SALOME_Event
{
public:
  typedef int TResult;
  TResult myResult;
  TGetStudyIdEvent() : myResult( 0 ) {}
  virtual void Execute()
  {
    if ( LightApp_Study* aStudy = getActiveStudy() ) {
      myResult = aStudy->id();
    }
  }
};
int SalomePyQt::getStudyId()
{
  return ProcessEvent( new TGetStudyIdEvent() );
}

/*!
  \fn SALOME_Selection* SalomePyQt::getSelection()
  \brief Get the selection object for the current study.

  Creates a Selection object if it has not been created yet.

  \return selection object (0 on error)
*/

class TGetSelectionEvent: public SALOME_Event 
{
public:
  typedef SALOME_Selection* TResult;
  TResult myResult;
  TGetSelectionEvent() : myResult( 0 ) {}
  virtual void Execute() 
  {
    myResult = SALOME_Selection::GetSelection( getApplication() );
  }
};
SALOME_Selection* SalomePyQt::getSelection()
{
  return ProcessEvent( new TGetSelectionEvent() );
}

/*!
  \fn void SalomePyQt::putInfo( const QString& msg, const int sec )
  \brief Put an information message to the current application's 
  desktop status bar.

  Optional second delay parameter (\a sec) can be used to specify
  time of the message diplaying in seconds. If this parameter is less
  or equal to zero, the constant message will be put.

  \param msg message text 
  \param sec message displaying time in seconds
*/

class TPutInfoEvent: public SALOME_Event
{
  QString myMsg;
  int     mySecs;
public:
  TPutInfoEvent( const QString& msg, const int sec = 0 ) : myMsg( msg ), mySecs( sec ) {}
  virtual void Execute()
  {
    if ( LightApp_Application* anApp = getApplication() ) {
      anApp->putInfo( myMsg, mySecs * 1000 );
    }
  }
};
void SalomePyQt::putInfo( const QString& msg, const int sec )
{
  ProcessVoidEvent( new TPutInfoEvent( msg, sec ) );
}

/*!
  \fn const QString SalomePyQt::getActiveComponent();
  \brief Get the currently active module name (for the current study).
  \return active module name or empty string if there is no active module
*/

class TGetActiveComponentEvent: public SALOME_Event
{
public:
  typedef QString TResult;
  TResult myResult;
  TGetActiveComponentEvent() {}
  virtual void Execute() 
  {
    if ( LightApp_Application* anApp = getApplication() ) {
      if ( CAM_Module* mod = anApp->activeModule() ) {
        myResult = mod->name();
      }
    }
  }
};
const QString SalomePyQt::getActiveComponent()
{
  return ProcessEvent( new TGetActiveComponentEvent() );
}

/*!
  \fn PyObject* SalomePyQt::getActivePythonModule()
  \brief Access to Python module object currently loaded into SALOME_PYQT_ModuleLight container.
  \return Python module object currently loaded into SALOME_PYQT_ModuleLight container
*/

class TGetActivePyModuleEvent: public SALOME_Event
{
public:
  typedef PyObject* TResult;
  TResult myResult;
  TGetActivePyModuleEvent() : myResult( 0 ) {}
  virtual void Execute() 
  {
    SALOME_PYQT_ModuleLight* module = getActiveModule();
    if ( module )
      myResult = (PyObject*)module->getPythonModule();
  }
};
PyObject* SalomePyQt::getActivePythonModule()
{
  return ProcessEvent( new TGetActivePyModuleEvent() );
}

/*!
  \fn bool SalomePyQt::activateModule( const QString& modName )
  \brief Activates SALOME module with the given name
  \return True if the module has been activated and False otherwise.
*/

class TActivateModuleEvent: public SALOME_Event
{
public:
  typedef bool TResult;
  TResult myResult;
  QString myModuleName;
  TActivateModuleEvent( const QString& modName ) 
  : myResult( false ), myModuleName( modName ) {}
  virtual void Execute() 
  {
    if ( LightApp_Application* anApp = getApplication() ) {
      myResult = anApp->activateModule( myModuleName );
    }
  }
};
bool SalomePyQt::activateModule( const QString& modName )
{
  return ProcessEvent( new TActivateModuleEvent( modName ) );
}

/*!
  \brief Update an Object Browser of the specified (by identifier) study.

  If \a studyId <= 0 the active study's object browser is updated.
  The \a updateSelection parameter is obsolete and currently is not used. 
  This parameter will be removed in future, so try to avoid its usage in 
  your code.

  \brief studyId study identifier
  \brief updateSelection update selection flag (not used)
  \sa getActiveStudy()
*/
void SalomePyQt::updateObjBrowser( const int studyId, bool updateSelection )
{  
  class TEvent: public SALOME_Event
  {
    int  myStudyId;
    bool myUpdateSelection;
  public:
    TEvent( const int studyId, bool updateSelection ) 
      : myStudyId( studyId ), myUpdateSelection( updateSelection ) {}
    virtual void Execute()
    {
      if ( SUIT_Session::session() ) {
        if ( getActiveStudy() && myStudyId <= 0 )
          myStudyId = getActiveStudy()->id();
        if ( myStudyId > 0 ) {
          QList<SUIT_Application*> apps = SUIT_Session::session()->applications();
          QList<SUIT_Application*>::Iterator it;
          for( it = apps.begin(); it != apps.end(); ++it ) {
            LightApp_Application* anApp = dynamic_cast<LightApp_Application*>( *it );
            if ( anApp && anApp->activeStudy() && anApp->activeStudy()->id() == myStudyId ) {
              anApp->updateObjectBrowser();
              return;
            }
          }
        }
      }
    }
  };
  ProcessVoidEvent( new TEvent( studyId, updateSelection ) );
}


/*!
  SalomePyQt::isModified()
  \return The modification status of the data model
  for the currently active Python module
  \sa setModified()
*/
class TIsModifiedEvent: public SALOME_Event
{
public:
  typedef bool TResult;
  TResult myResult;
  TIsModifiedEvent() : myResult( false ) {}
  virtual void Execute() 
  {
    SALOME_PYQT_ModuleLight* module = getActiveModule();
    if ( !module )
      return;
    
    SALOME_PYQT_DataModelLight* aModel =
      dynamic_cast<SALOME_PYQT_DataModelLight*>( module->dataModel() );
    if ( aModel )
      myResult = aModel->isModified();
  }
};
bool SalomePyQt::isModified()
{
  return ProcessEvent(new TIsModifiedEvent());
}

/*!
  SalomePyQt::setModified()

  Sets the modification status of the data model for 
  the currently active Python module. This method should be used
  by the Python code in order to enable/disable "Save" operation
  depending on the module's data state.

  \param New modification status of the data model

  \sa isModified()
*/
void SalomePyQt::setModified( bool flag )
{  
  class TEvent: public SALOME_Event
  {
    bool myFlag;
  public:
    TEvent( bool flag ) 
      : myFlag( flag ) {}
    virtual void Execute()
    {
      SALOME_PYQT_ModuleLight* module = getActiveModule();
      if ( !module )
	return;

      SALOME_PYQT_DataModelLight* aModel =
	dynamic_cast<SALOME_PYQT_DataModelLight*>( module->dataModel() );
      LightApp_Application* aLApp = 
	dynamic_cast<LightApp_Application*>( module->application() );
      if ( !aModel || !aLApp )
	return;

      aModel->setModified( myFlag );
      aLApp->updateActions();
    }
  };
  ProcessVoidEvent( new TEvent( flag ) );
}

/*!
  \brief Default resource file section name.
  \internal
*/
static const char* DEFAULT_SECTION = "SalomePyQt";

/*!
  \brief Add string setting to the application preferences.

  The parameter \a autoValue is obsolete parameter and currently is not used.
  This parameter will be removed in future, so try to avoid its usage in 
  your code.

  This function is obsolete. Use one of addSetting() instead.

  \param name setting name (it should be of kind <section:setting> where
  \c section is resources section name and \c setting is setting name)
  \param value new setting value
  \param autoValue (not used)
*/
void SalomePyQt::addStringSetting( const QString& name, const QString& value, bool autoValue )
{
  class TEvent: public SALOME_Event
  {
    QString myName;
    QString myValue;
    bool    myAutoValue;
  public:
    TEvent( const QString& name, const QString& value, bool autoValue ) 
      : myName( name ), myValue( value ), myAutoValue( autoValue ) {}
    virtual void Execute() {
      if ( SUIT_Session::session() ) {
        SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
        QStringList sl = myName.split( ":", QString::SkipEmptyParts );
        QString _sec = sl.count() > 1 ? sl[ 0 ].trimmed() : QString( DEFAULT_SECTION );
        QString _nam = sl.count() > 1 ? sl[ 1 ].trimmed() : sl.count() > 0 ? sl[ 0 ].trimmed() : QString( "" );
        if ( !_sec.isEmpty() && !_nam.isEmpty() )
          resMgr->setValue( _sec, _nam, myValue );
      }
    }
  };
  ProcessVoidEvent( new TEvent( name, value, autoValue ) );
}

/*!
  \brief Add integer setting to the application preferences.

  The parameter \a autoValue is obsolete parameter and currently is not used.
  This parameter will be removed in future, so try to avoid its usage in 
  your code.

  This function is obsolete. Use one of addSetting() instead.

  \param name setting name (it should be of kind <section:setting> where
  \c section is resources section name and \c setting is setting name)
  \param value new setting value
  \param autoValue (not used)
*/
void SalomePyQt::addIntSetting( const QString& name, const int value, bool autoValue)
{
  class TEvent: public SALOME_Event 
  {
    QString myName;
    int     myValue;
    bool    myAutoValue;
  public:
    TEvent( const QString& name, const int value, bool autoValue ) 
      : myName( name ), myValue( value ), myAutoValue( autoValue ) {}
    virtual void Execute()
    {
      if ( SUIT_Session::session() ) {
        SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
        QStringList sl = myName.split( ":", QString::SkipEmptyParts );
        QString _sec = sl.count() > 1 ? sl[ 0 ].trimmed() : QString( DEFAULT_SECTION );
        QString _nam = sl.count() > 1 ? sl[ 1 ].trimmed() : sl.count() > 0 ? sl[ 0 ].trimmed() : QString( "" );
        if ( !_sec.isEmpty() && !_nam.isEmpty() )
          resMgr->setValue( _sec, _nam, myValue );
      }
    }
  };
  ProcessVoidEvent( new TEvent( name, value, autoValue ) );
}

/*!
  \brief Add double setting to the application preferences.

  The parameter \a autoValue is obsolete parameter and currently is not used.
  This parameter will be removed in future, so try to avoid its usage in 
  your code.

  This function is obsolete. Use one of addSetting() instead.

  \param name setting name (it should be of kind <section:setting> where
  \c section is resources section name and \c setting is setting name)
  \param value new setting value
  \param autoValue (not used)
*/
void SalomePyQt::addDoubleSetting( const QString& name, const double value, bool autoValue )
{
  class TEvent: public SALOME_Event 
  {
    QString myName;
    double  myValue;
    bool    myAutoValue;
  public:
    TEvent( const QString& name, const double value, bool autoValue ) 
      : myName( name ), myValue( value ), myAutoValue( autoValue ) {}
    virtual void Execute() 
    {
      if ( SUIT_Session::session() ) {
        SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
        QStringList sl = myName.split( ":", QString::SkipEmptyParts );
        QString _sec = sl.count() > 1 ? sl[ 0 ].trimmed() : QString( DEFAULT_SECTION );
        QString _nam = sl.count() > 1 ? sl[ 1 ].trimmed() : sl.count() > 0 ? sl[ 0 ].trimmed() : QString( "" );
        if ( !_sec.isEmpty() && !_nam.isEmpty() )
          resMgr->setValue( _sec, _nam, myValue );
      }
    }
  };
  ProcessVoidEvent( new TEvent( name, value, autoValue ) );
}

/*!
  \brief Add boolean setting to the application preferences.

  The parameter \a autoValue is obsolete parameter and currently is not used.
  This parameter will be removed in future, so try to avoid its usage in 
  your code.

  This function is obsolete. Use one of addSetting() instead.

  \param name setting name (it should be of kind <section:setting> where
  \c section is resources section name and \c setting is setting name)
  \param value new setting value
  \param autoValue (not used)
*/
void SalomePyQt::addBoolSetting( const QString& name, const bool value, bool autoValue )
{
  class TEvent: public SALOME_Event 
  {
    QString myName;
    bool    myValue;
    bool    myAutoValue;
  public:
    TEvent( const QString& name, const bool value, bool autoValue ) 
      : myName( name ), myValue( value ), myAutoValue( autoValue ) {}
    virtual void Execute() 
    {
      if ( SUIT_Session::session() ) {
        SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
        QStringList sl = myName.split( ":", QString::SkipEmptyParts );
        QString _sec = sl.count() > 1 ? sl[ 0 ].trimmed() : QString( DEFAULT_SECTION );
        QString _nam = sl.count() > 1 ? sl[ 1 ].trimmed() : sl.count() > 0 ? sl[ 0 ].trimmed() : QString( "" );
        if ( !_sec.isEmpty() && !_nam.isEmpty() )
          resMgr->setValue( _sec, _nam, myValue );
      }
    }
  };
  ProcessVoidEvent( new TEvent( name, value, autoValue ) );
}

/*!
  \brief Remove setting from the application preferences.

  This function is obsolete. Use removeSetting() instead.

  \param name setting name (it should be of kind <section:setting> where
  \c section is resources section name and \c setting is setting name)
*/
void SalomePyQt::removeSettings( const QString& name )
{
  class TEvent: public SALOME_Event {
    QString myName;
  public:
    TEvent( const QString& name ) : myName( name ) {}
    virtual void Execute() {
      if ( SUIT_Session::session() ) {
        SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
        QStringList sl = myName.split( ":", QString::SkipEmptyParts );
        QString _sec = sl.count() > 1 ? sl[ 0 ].trimmed() : QString( DEFAULT_SECTION );
        QString _nam = sl.count() > 1 ? sl[ 1 ].trimmed() : sl.count() > 0 ? sl[ 0 ].trimmed() : QString( "" );
        if ( !_sec.isEmpty() && !_nam.isEmpty() )
          resMgr->remove( _sec, _nam );
      }
    }
  };
  ProcessVoidEvent( new TEvent( name ) );
}

/*!
  \fn QString SalomePyQt::getSetting( const QString& name );
  \brief Get application setting value (as string represenation).

  This function is obsolete. Use stringSetting(), integerSetting(), 
  boolSetting(), stringSetting() or colorSetting() instead.

  \param name setting name (it should be of kind <section:setting> where
  \c section is resources section name and \c setting is setting name)
  \return setting name (empty string if setting name is invalid)
*/

class TGetSettingEvent: public SALOME_Event 
{
public:
  typedef QString TResult;
  TResult myResult;
  QString myName;
  TGetSettingEvent( const QString& name ) : myName( name ) {}
  virtual void Execute() 
  {
    if ( SUIT_Session::session() ) {
      SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
      QStringList sl = myName.split( ":", QString::SkipEmptyParts );
      QString _sec = sl.count() > 1 ? sl[ 0 ].trimmed() : QString( DEFAULT_SECTION );
      QString _nam = sl.count() > 1 ? sl[ 1 ].trimmed() : sl.count() > 0 ? sl[ 0 ].trimmed() : QString( "" );
      myResult = ( !_sec.isEmpty() && !_nam.isEmpty() ) ? resMgr->stringValue( _sec, _nam, "" ) : QString( "" );
    }
  }
};
QString SalomePyQt::getSetting( const QString& name )
{
  return ProcessEvent( new TGetSettingEvent( name ) );
}

/*!
  \brief Add double setting to the application preferences.
  \param section resources file section name 
  \param name setting name
  \param value new setting value
*/
void SalomePyQt::addSetting( const QString& section, const QString& name, const double value )
{
  class TEvent: public SALOME_Event 
  {
    QString mySection;
    QString myName;
    double  myValue;
  public:
    TEvent( const QString& section, const QString& name, double value ) 
      : mySection( section ), myName( name ), myValue( value ) {}
    virtual void Execute() 
    {
      if ( SUIT_Session::session() ) {
        SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
        if ( !mySection.isEmpty() && !myName.isEmpty() )
          resMgr->setValue( mySection, myName, myValue );
      }
    }
  };
  ProcessVoidEvent( new TEvent( section, name, value ) );
}

/*!
  \brief Add integer setting to the application preferences.
  \param section resources file section name 
  \param name setting name
  \param value new setting value
*/
void SalomePyQt::addSetting( const QString& section, const QString& name, const int value )
{
  class TEvent: public SALOME_Event 
  {
    QString mySection;
    QString myName;
    int     myValue;
  public:
    TEvent( const QString& section, const QString& name, int value ) 
      : mySection( section ), myName( name ), myValue( value ) {}
    virtual void Execute() 
    {
      if ( SUIT_Session::session() ) {
        SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
        if ( !mySection.isEmpty() && !myName.isEmpty() )
          resMgr->setValue( mySection, myName, myValue );
      }
    }
  };
  ProcessVoidEvent( new TEvent( section, name, value ) );
}

/*!
  \brief Add boolean setting to the application preferences.
  \param section resources file section name 
  \param name setting name
  \param value new setting value
  \param dumb this parameter is used in order to avoid sip compilation error 
  because of conflicting int and bool types
*/
void SalomePyQt::addSetting( const QString& section, const QString& name, const bool value, const int /*dumb*/ )
{
  class TEvent: public SALOME_Event 
  {
    QString mySection;
    QString myName;
    bool    myValue;
  public:
    TEvent( const QString& section, const QString& name, bool value ) 
      : mySection( section ), myName( name ), myValue( value ) {}
    virtual void Execute() 
    {
      if ( SUIT_Session::session() ) {
        SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
        if ( !mySection.isEmpty() && !myName.isEmpty() )
          resMgr->setValue( mySection, myName, myValue );
      }
    }
  };
  ProcessVoidEvent( new TEvent( section, name, value ) );
}

/*!
  \brief Add string setting to the application preferences.
  \param section resources file section name 
  \param name setting name
  \param value new setting value
*/
void SalomePyQt::addSetting( const QString& section, const QString& name, const QString& value )
{
  class TEvent: public SALOME_Event 
  {
    QString mySection;
    QString myName;
    QString myValue;
  public:
    TEvent( const QString& section, const QString& name, const QString& value ) 
      : mySection( section ), myName( name ), myValue( value ) {}
    virtual void Execute() 
    {
      if ( SUIT_Session::session() ) {
        SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
        if ( !mySection.isEmpty() && !myName.isEmpty() )
          resMgr->setValue( mySection, myName, myValue );
      }
    }
  };
  ProcessVoidEvent( new TEvent( section, name, value ) );
}

/*!
  \brief Add color setting to the application preferences.
  \param section resources file section name 
  \param name setting name
  \param value new setting value
*/
void SalomePyQt::addSetting( const QString& section, const QString& name, const QColor& value )
{
  class TEvent: public SALOME_Event 
  {
    QString mySection;
    QString myName;
    QColor  myValue;
  public:
    TEvent( const QString& section, const QString& name, const QColor& value ) 
      : mySection( section ), myName( name ), myValue( value ) {}
    virtual void Execute() 
    {
      if ( SUIT_Session::session() ) {
        SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
        if ( !mySection.isEmpty() && !myName.isEmpty() )
          resMgr->setValue( mySection, myName, myValue );
      }
    }
  };
  ProcessVoidEvent( new TEvent( section, name, value ) );
}

/*!
  \fn int SalomePyQt::integerSetting( const QString& section, 
                                      const QString& name, 
                                      const int def );
  \brief Get integer setting from the application preferences.
  \param section resources file section name 
  \param name setting name
  \param def default value which is returned if the setting is not found
  \return setting value
*/

class TGetIntSettingEvent: public SALOME_Event 
{
public:
  typedef int TResult;
  TResult myResult;
  QString mySection;
  QString myName;
  TResult myDefault;
  TGetIntSettingEvent( const QString& section, const QString& name, const int def ) 
    : mySection( section ), myName( name ), myDefault( def ) {}
  virtual void Execute() 
  {
    if ( SUIT_Session::session() ) {
      SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
      myResult = ( !mySection.isEmpty() && !myName.isEmpty() ) ? resMgr->integerValue( mySection, myName, myDefault ) : myDefault;
    }
  }
};
int SalomePyQt::integerSetting( const QString& section, const QString& name, const int def )
{
  return ProcessEvent( new TGetIntSettingEvent( section, name, def ) );
}

/*!
  \fn double SalomePyQt::doubleSetting( const QString& section, 
                                        const QString& name, 
                                        const double def );
  \brief Get double setting from the application preferences.
  \param section resources file section name 
  \param name setting name
  \param def default value which is returned if the setting is not found
  \return setting value
*/

class TGetDblSettingEvent: public SALOME_Event 
{
public:
  typedef double TResult;
  TResult myResult;
  QString mySection;
  QString myName;
  TResult myDefault;
  TGetDblSettingEvent( const QString& section, const QString& name, const double def ) 
    : mySection( section ), myName( name ), myDefault( def ) {}
  virtual void Execute() 
  {
    if ( SUIT_Session::session() ) {
      SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
      myResult = ( !mySection.isEmpty() && !myName.isEmpty() ) ? resMgr->doubleValue( mySection, myName, myDefault ) : myDefault;
    }
  }
};
double SalomePyQt::doubleSetting( const QString& section, const QString& name, const double def )
{
  return ProcessEvent( new TGetDblSettingEvent( section, name, def ) );
}

/*!
  \fn bool SalomePyQt::boolSetting( const QString& section, 
                                    const QString& name, 
                                    const bool def );
  \brief Get boolean setting from the application preferences.
  \param section resources file section name 
  \param name setting name
  \param def default value which is returned if the setting is not found
  \return setting value
*/

class TGetBoolSettingEvent: public SALOME_Event 
{
public:
  typedef bool TResult;
  TResult myResult;
  QString mySection;
  QString myName;
  TResult myDefault;
  TGetBoolSettingEvent( const QString& section, const QString& name, const bool def ) 
    : mySection( section ), myName( name ), myDefault( def ) {}
  virtual void Execute() 
  {
    if ( SUIT_Session::session() ) {
      SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
      myResult = ( !mySection.isEmpty() && !myName.isEmpty() ) ? resMgr->booleanValue( mySection, myName, myDefault ) : myDefault;
    }
  }
};
bool SalomePyQt::boolSetting( const QString& section, const QString& name, const bool def )
{
  return ProcessEvent( new TGetBoolSettingEvent( section, name, def ) );
}

/*!
  \fn QString SalomePyQt::stringSetting( const QString& section, 
                                         const QString& name, 
                                         const QString& def );
  \brief Get string setting from the application preferences.
  \param section resources file section name 
  \param name setting name
  \param def default value which is returned if the setting is not found
  \return setting value
*/

class TGetStrSettingEvent: public SALOME_Event
{
public:
  typedef QString TResult;
  TResult myResult;
  QString mySection;
  QString myName;
  TResult myDefault;
  TGetStrSettingEvent( const QString& section, const QString& name, const QString& def ) 
    : mySection( section ), myName( name ), myDefault( def ) {}
  virtual void Execute() 
  {
    if ( SUIT_Session::session() ) {
      SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
      myResult = ( !mySection.isEmpty() && !myName.isEmpty() ) ? resMgr->stringValue( mySection, myName, myDefault ) : myDefault;
    }
  }
};
QString SalomePyQt::stringSetting( const QString& section, const QString& name, const QString& def )
{
  return ProcessEvent( new TGetStrSettingEvent( section, name, def ) );
}

/*!
  \fn QColor SalomePyQt::colorSetting( const QString& section, 
                                       const QString& name, 
                                       const QColor def );
  \brief Get color setting from the application preferences.
  \param section resources file section name 
  \param name setting name
  \param def default value which is returned if the setting is not found
  \return setting value
*/

class TGetColorSettingEvent: public SALOME_Event 
{
public:
  typedef QColor TResult;
  TResult myResult;
  QString mySection;
  QString myName;
  TResult myDefault;
  TGetColorSettingEvent( const QString& section, const QString& name, const QColor& def ) 
    : mySection( section ), myName( name ), myDefault( def ) {}
  virtual void Execute() 
  {
    if ( SUIT_Session::session() ) {
      SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
      myResult = ( !mySection.isEmpty() && !myName.isEmpty() ) ? resMgr->colorValue( mySection, myName, myDefault ) : myDefault;
    }
  }
};
QColor SalomePyQt::colorSetting ( const QString& section, const QString& name, const QColor& def )
{
  return ProcessEvent( new TGetColorSettingEvent( section, name, def ) );
}

/*!
  \brief Remove setting from the application preferences.
  \param section resources file section name 
  \param name setting name
*/
void SalomePyQt::removeSetting( const QString& section, const QString& name )
{
  class TEvent: public SALOME_Event 
  {
    QString mySection;
    QString myName;
  public:
    TEvent( const QString& section, const QString& name ) : mySection( section ), myName( name ) {}
    virtual void Execute() 
    {
      if ( SUIT_Session::session() ) {
        SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
        if ( !mySection.isEmpty() && !myName.isEmpty() )
          resMgr->remove( mySection, myName );
      }
    }
  };
  ProcessVoidEvent( new TEvent( section, name ) );
}

/*!
  \fn bool SalomePyQt::hasSetting( const QString& section, const QString& name );
  \brief Check setting existence in the application preferences.
  \param section resources file section name 
  \param name setting name
  \return \c true if setting exists
*/

class THasColorSettingEvent: public SALOME_Event 
{
public:
  typedef bool TResult;
  TResult myResult;
  QString mySection;
  QString myName;
  THasColorSettingEvent( const QString& section, const QString& name ) 
    : mySection( section ), myName( name ) {}
  virtual void Execute() 
  {
    if ( SUIT_Session::session() ) {
      SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
      myResult = resMgr->hasValue( mySection, myName );
    }
  }
};
bool SalomePyQt::hasSetting( const QString& section, const QString& name )
{
  return ProcessEvent( new THasColorSettingEvent( section, name ) );
}

/*!
  \fn QString SalomePyQt::getFileName( QWidget*           parent, 
                                       const QString&     initial, 
                                       const QStringList& filters, 
                                       const QString&     caption,
                                       bool               open );
  \brief Show 'Open/Save file' dialog box for file selection 
         and return a user's choice (selected file name).
  \param parent parent widget
  \param initial initial directory the dialog box to be opened in
  \param filters list of files filters (wildcards)
  \param caption dialog box title
  \param open if \c true, "Open File" dialog box is shown; 
         otherwise "Save File" dialog box is shown
  \return selected file name (null string if user cancels operation)
*/

class TGetFileNameEvent: public SALOME_Event 
{
public:
  typedef QString TResult;
  TResult     myResult;
  QWidget*    myParent;
  QString     myInitial;
  QStringList myFilters;
  QString     myCaption;
  bool        myOpen;
  TGetFileNameEvent( QWidget*           parent, 
                     const QString&     initial, 
                     const QStringList& filters, 
                     const QString&     caption,
                     bool               open ) 
    : myParent ( parent ), 
      myInitial( initial ), 
      myFilters( filters ), 
      myCaption( caption ), 
      myOpen ( open ) {}
  virtual void Execute() 
  {
    if ( LightApp_Application* anApp = getApplication() ) {
      myResult = anApp->getFileName( myOpen, myInitial, myFilters.join(";;"), 
                                     myCaption, myParent );
    }
  }
};
QString SalomePyQt::getFileName( QWidget*           parent, 
                                 const QString&     initial, 
                                 const QStringList& filters, 
                                 const QString&     caption,
                                 bool               open )
{
  return ProcessEvent( new TGetFileNameEvent( parent, initial, filters, caption, open ) );
}

/*!
  \fn QStringList SalomePyQt::getOpenFileNames( QWidget*           parent, 
                                                const QString&     initial, 
                                                const QStringList& filters, 
                                                const QString&     caption );
  \brief Show 'Open files' dialog box for multiple files selection
         and return a user's choice (selected file names list).
  \param parent parent widget
  \param initial initial directory the dialog box to be opened in
  \param filters list of files filters (wildcards)
  \param caption dialog box title
  \return selected file names list (empty list if user cancels operation)
*/

class TGetOpenFileNamesEvent: public SALOME_Event 
{
public:
  typedef QStringList TResult;
  TResult     myResult;
  QWidget*    myParent;
  QString     myInitial;
  QStringList myFilters;
  QString     myCaption;
  TGetOpenFileNamesEvent( QWidget*           parent, 
                          const QString&     initial, 
                          const QStringList& filters, 
                          const QString&     caption ) 
    : myParent ( parent ), 
      myInitial( initial ), 
      myFilters( filters ), 
      myCaption( caption ) {}
  virtual void Execute() 
  {
    if ( LightApp_Application* anApp = getApplication() ) {
      myResult = anApp->getOpenFileNames( myInitial, myFilters.join(";;"), myCaption, myParent );
    }
  }
};
QStringList SalomePyQt::getOpenFileNames( QWidget*           parent, 
                                          const QString&     initial, 
                                          const QStringList& filters, 
                                          const QString&     caption )
{
  return ProcessEvent( new TGetOpenFileNamesEvent( parent, initial, filters, caption ) );
}

/*!
  \fn QString SalomePyQt::getExistingDirectory( QWidget*       parent,
                                                const QString& initial,
                                                const QString& caption );
  \brief Show 'Get Directory' dialog box for the directory selection
         and return a user's choice (selected directory name).
  \param parent parent widget
  \param initial initial directory the dialog box to be opened in
  \param caption dialog box title
  \return selected directory name (null string if user cancels operation)
*/

class TGetExistingDirectoryEvent: public SALOME_Event 
{
public:
  typedef QString TResult;
  TResult     myResult;
  QWidget*    myParent;
  QString     myInitial;
  QString     myCaption;
  TGetExistingDirectoryEvent( QWidget*           parent, 
                              const QString&     initial, 
                              const QString&     caption ) 
    : myParent ( parent ), 
      myInitial( initial ), 
      myCaption( caption ) {}
  virtual void Execute() 
  {
    if ( LightApp_Application* anApp = getApplication() ) {
      myResult = anApp->getDirectory( myInitial, myCaption, myParent );
    }
  }
};
QString SalomePyQt::getExistingDirectory( QWidget*       parent,
                                          const QString& initial,
                                          const QString& caption )
{
  return ProcessEvent( new TGetExistingDirectoryEvent( parent, initial, caption ) );
}

/*!
  \fn QString SalomePyQt::loadIcon( const QString& filename );
  \brief Load an icon from the module resources by the specified file name.
  \param filename icon file name
  \return icon object
*/
class TLoadIconEvent: public SALOME_Event 
{
public:
  typedef QIcon TResult;
  TResult     myResult;
  QString     myModule;
  QString     myFileName;
  TLoadIconEvent( const QString& module, const QString& filename ) 
    : myModule( module ), 
      myFileName ( filename ) {}
  virtual void Execute() 
  {
    if ( LightApp_Application* anApp = getApplication() ) {
      if ( !myFileName.isEmpty() ) {
        QPixmap pixmap = anApp->resourceMgr()->loadPixmap( myModule, 
                         QApplication::translate( myModule.toLatin1().data(), 
                                                  myFileName.toLatin1().data() ) );
        if ( !pixmap.isNull() )
          myResult = QIcon( pixmap );
      }
    }
  }
};
QIcon SalomePyQt::loadIcon( const QString& module, const QString& filename )
{
  return ProcessEvent( new TLoadIconEvent( module, filename ) );
}

/*!
  \brief Open external browser to display context help information.
  \todo

  Current implementation does nothing.

  \param source documentation (HTML) file name
  \param context context (for example, HTML ancor name)
*/
void SalomePyQt::helpContext( const QString& source, const QString& context ) 
{
  class TEvent: public SALOME_Event 
  {
    QString mySource;
    QString myContext;
  public:
    TEvent( const QString& source, const QString& context ) 
      : mySource( source ), myContext( context ) {}
    virtual void Execute() 
    {
      if ( LightApp_Application* anApp = getApplication() ) {
        anApp->onHelpContextModule( "", mySource, myContext );
      }
    }
  };
  ProcessVoidEvent( new TEvent( source, context ) );
}

/*!
  \fn bool SalomePyQt::dumpView( const QString& filename );
  \brief Dump the contents of the currently active view window 
  to the image file in the specified format.

  For the current moment JPEG, PNG and BMP images formats are supported.
  The image format is defined automatically by the file name extension.
  By default, BMP format is used.

  \param filename image file name
  \return operation status (\c true on success)
*/

class TDumpViewEvent: public SALOME_Event 
{
public:
  typedef bool TResult;
  TResult myResult;
  QString myFileName;
  TDumpViewEvent( const QString& filename ) 
    : myResult ( false ), myFileName( filename ) {}
  virtual void Execute() 
  {
    if ( LightApp_Application* anApp = getApplication() ) {
      SUIT_ViewManager* vm = anApp->activeViewManager();
      if ( vm ) { 
        SUIT_ViewWindow* vw = vm->getActiveView();
        if ( vw ) {
          QImage im = vw->dumpView();
          if ( !im.isNull() && !myFileName.isEmpty() ) {
            QString fmt = SUIT_Tools::extension( myFileName ).toUpper();
            if ( fmt.isEmpty() ) fmt = QString( "BMP" ); // default format
            if ( fmt == "JPG" )  fmt = "JPEG";
            myResult = im.save( myFileName, fmt.toLatin1() );
          }
        }
      }
    }
  }
};
bool SalomePyQt::dumpView( const QString& filename )
{
  return ProcessEvent( new TDumpViewEvent( filename ) );
}

/*!
  \fn int SalomePyQt::defaultMenuGroup();
  \brief Get detault menu group identifier which can be used when 
  creating menus (insert custom menu commands).
  \return default menu group ID
*/

class TDefMenuGroupEvent: public SALOME_Event 
{
public:
  typedef int TResult;
  TResult myResult;
  TDefMenuGroupEvent() : myResult( -1 ) {}
  virtual void Execute() 
  {
    myResult = SALOME_PYQT_ModuleLight::defaultMenuGroup();
  }
};
int SalomePyQt::defaultMenuGroup()
{
  return ProcessEvent( new TDefMenuGroupEvent() );
}

class CrTool
{
public:
  CrTool( const QString& tBar ) 
    : myCase( 0 ), myTbName( tBar ) {}
  CrTool( const int id, const int tBar, const int idx ) 
    : myCase( 1 ), myId( id ), myTbId( tBar ), myIndex( idx ) {}
  CrTool( const int id, const QString& tBar, const int idx )
    : myCase( 2 ), myId( id ), myTbName( tBar ), myIndex( idx ) {}
  CrTool( QAction* action, const int tbId, const int id, const int idx )
    : myCase( 3 ), myAction( action ), myTbId( tbId ), myId( id ), myIndex( idx ) {}
  CrTool( QAction* action, const QString& tBar, const int id, const int idx )
    : myCase( 4 ), myAction( action ), myTbName( tBar ), myId( id ), myIndex( idx ) {}

  int execute( SALOME_PYQT_ModuleLight* module ) const
  {
    if ( module ) {
      switch ( myCase ) {
      case 0:
        return module->createTool( myTbName );
      case 1:
        return module->createTool( myId, myTbId, myIndex );
      case 2:
        return module->createTool( myId, myTbName, myIndex );
      case 3:
        return module->createTool( myAction, myTbId, myId, myIndex );
      case 4:
        return module->createTool( myAction, myTbName, myId, myIndex );
      }
    }
    return -1;
  }
private:
   int        myCase;
   QString    myTbName;
   int        myTbId;
   QAction*   myAction;
   int        myId;
   int        myIndex;
};

class TCreateToolEvent: public SALOME_Event 
{
public:
  typedef int TResult;
  TResult myResult;
  const CrTool& myCrTool;
  TCreateToolEvent( const CrTool& crTool ) 
    : myResult( -1 ), myCrTool( crTool ) {}
  virtual void Execute() 
  {
    SALOME_PYQT_ModuleLight* module = getActiveModule();
    if ( module )
      myResult = myCrTool.execute( module );
  }
};

/*!
  \brief Create toolbar with specified name.
  \param tBar toolbar name
  \return toolbar ID or -1 if toolbar creation is failed
*/
int SalomePyQt::createTool( const QString& tBar )
{
  return ProcessEvent( new TCreateToolEvent( CrTool( tBar ) ) );
}

/*! 
  \brief Insert action with specified \a id to the toolbar.
  \param id action ID
  \param tBar toolbar ID
  \param idx required index in the toolbar
  \return action ID or -1 if action could not be added
*/
int SalomePyQt::createTool( const int id, const int tBar, const int idx )
{
  return ProcessEvent( new TCreateToolEvent( CrTool( id, tBar, idx ) ) );
}

/*!
  \brief Insert action with specified \a id to the toolbar.
  \param id action ID
  \param tBar toolbar name
  \param idx required index in the toolbar
  \return action ID or -1 if action could not be added
*/
int SalomePyQt::createTool( const int id, const QString& tBar, const int idx )
{
  return ProcessEvent( new TCreateToolEvent( CrTool( id, tBar, idx ) ) );
}

/*!
  \brief Insert action to the toolbar.
  \param a action
  \param tBar toolbar ID
  \param id required action ID
  \param idx required index in the toolbar
  \return action ID or -1 if action could not be added
*/
int SalomePyQt::createTool( QAction* a, const int tBar, const int id, const int idx )
{
  return ProcessEvent( new TCreateToolEvent( CrTool( a, tBar, id, idx ) ) );
}

/*!
  \brief Insert action to the toolbar.
  \param a action
  \param tBar toolbar name
  \param id required action ID
  \param idx required index in the toolbar
  \return action ID or -1 if action could not be added
*/
int SalomePyQt::createTool( QAction* a, const QString& tBar, const int id, const int idx )
{
  return ProcessEvent( new TCreateToolEvent( CrTool( a, tBar, id, idx ) ) );
}

class CrMenu
{
public:
  CrMenu( const QString& subMenu, const int menu, const int id, const int group, const int idx ) 
    : myCase( 0 ), mySubMenuName( subMenu ), myMenuId( menu ), myId( id ), myGroup( group ), myIndex( idx ) {}
  CrMenu( const QString& subMenu, const QString& menu, const int id, const int group, const int idx ) 
    : myCase( 1 ), mySubMenuName( subMenu ), myMenuName( menu ), myId( id ), myGroup( group ), myIndex( idx ) {}
  CrMenu( const int id, const int menu, const int group, const int idx ) 
    : myCase( 2 ), myId( id ), myMenuId( menu ), myGroup( group ), myIndex( idx ) {}
  CrMenu( const int id, const QString& menu, const int group, const int idx ) 
    : myCase( 3 ), myId( id ), myMenuName( menu ), myGroup( group ), myIndex( idx ) {}
  CrMenu( QAction* action, const int menu, const int id, const int group, const int idx ) 
    : myCase( 4 ), myAction( action ), myMenuId( menu ), myId( id ), myGroup( group ), myIndex( idx ) {}
  CrMenu( QAction* action, const QString& menu, const int id, const int group, const int idx ) 
    : myCase( 5 ), myAction( action ), myMenuName( menu ), myId( id ), myGroup( group ), myIndex( idx ) {}

  int execute( LightApp_Module* module ) const
  {
    if ( module ) {
      switch ( myCase ) {
      case 0:
        return module->createMenu( mySubMenuName, myMenuId, myId, myGroup, myIndex );
      case 1:
        return module->createMenu( mySubMenuName, myMenuName, myId, myGroup, myIndex );
      case 2:
        return module->createMenu( myId, myMenuId, myGroup, myIndex );
      case 3:
        return module->createMenu( myId, myMenuName, myGroup, myIndex );
      case 4:
        return module->createMenu( myAction, myMenuId, myId, myGroup, myIndex );
      case 5:
        return module->createMenu( myAction, myMenuName, myId, myGroup, myIndex );
      }
    }
    return -1;
  }
private:
   int        myCase;
   QString    myMenuName;
   int        myMenuId;
   QString    mySubMenuName;
   int        myGroup;
   QAction*   myAction;
   int        myId;
   int        myIndex;
};

class TCreateMenuEvent: public SALOME_Event
{
public:
  typedef int TResult;
  TResult myResult;
  const CrMenu& myCrMenu;
  TCreateMenuEvent( const CrMenu& crMenu ) 
    : myResult( -1 ), myCrMenu( crMenu ) {}
  virtual void Execute()
  {
    if ( LightApp_Application* anApp = getApplication() ) 
      {
        LightApp_Module* module = dynamic_cast<LightApp_Module*>( anApp->activeModule() );
        if ( module )
          myResult = myCrMenu.execute( module );
      }
  }
};

/*!
  \brief Create main menu.
  \param subMenu menu name
  \param menu parent menu ID
  \param id required menu ID
  \param group menu group ID
  \param idx required index in the menu
  \return menu ID or -1 if menu could not be added
*/
int SalomePyQt::createMenu( const QString& subMenu, const int menu, const int id, const int group, const int idx )
{
  return ProcessEvent( new TCreateMenuEvent( CrMenu( subMenu, menu, id, group, idx ) ) );
}

/*!
  \brief Create main menu.
  \param subMenu menu name
  \param menu parent menu name (list of menu names separated by "|")
  \param id required menu ID
  \param group menu group ID
  \param idx required index in the menu
  \return menu ID or -1 if menu could not be added
*/
int SalomePyQt::createMenu( const QString& subMenu, const QString& menu, const int id, const int group, const int idx )
{
  return ProcessEvent( new TCreateMenuEvent( CrMenu( subMenu, menu, id, group, idx ) ) );
}

/*!
  \brief Insert action to the main menu.
  \param id action ID
  \param menu parent menu ID
  \param group menu group ID
  \param idx required index in the menu
  \return action ID or -1 if action could not be added
*/
int SalomePyQt::createMenu( const int id, const int menu, const int group, const int idx )
{
  return ProcessEvent( new TCreateMenuEvent( CrMenu( id, menu, group, idx ) ) );
}

/*!
  \brief Insert action to the main menu.
  \param id action ID
  \param menu parent menu name (list of menu names separated by "|")
  \param group menu group ID
  \param idx required index in the menu
  \return action ID or -1 if action could not be added
*/
int SalomePyQt::createMenu( const int id, const QString& menu, const int group, const int idx )
{
  return ProcessEvent( new TCreateMenuEvent( CrMenu( id, menu, group, idx ) ) );
}

/*!
  \brief Insert action to the main menu.
  \param a action
  \param menu parent menu ID
  \param group menu group ID
  \param idx required index in the menu
  \return action ID or -1 if action could not be added
*/
int SalomePyQt::createMenu( QAction* a, const int menu, const int id, const int group, const int idx )
{
  return ProcessEvent( new TCreateMenuEvent( CrMenu( a, menu, id, group, idx ) ) );
}

/*!
  \brief Insert action to the main menu.
  \param a action
  \param menu parent menu name (list of menu names separated by "|")
  \param group menu group ID
  \param idx required index in the menu
  \return action ID or -1 if action could not be added
*/
int SalomePyQt::createMenu( QAction* a, const QString& menu, const int id, const int group, const int idx )
{
  return ProcessEvent( new TCreateMenuEvent( CrMenu( a, menu, id, group, idx ) ) );
}

/*!
  \fn QAction* SalomePyQt::createSeparator();
  \brief Create separator action which can be used in the menu or toolbar.
  \return new separator action
*/

class TCreateSepEvent: public SALOME_Event 
{
public:
  typedef QAction* TResult;
  TResult myResult;
  TCreateSepEvent() 
    : myResult( 0 ) {}
  virtual void Execute() 
  {
    SALOME_PYQT_ModuleLight* module = getActiveModule();
    if ( module )
      myResult = (QAction*)module->separator();
  }
};
QAction* SalomePyQt::createSeparator()
{
  return ProcessEvent( new TCreateSepEvent() );
}

/*!
  \fn QAction* SalomePyQt::createAction( const int      id,
                                           const QString& menuText, 
                                           const QString& tipText, 
                                           const QString& statusText, 
                                           const QString& icon,
                                           const int      key, 
                                           const bool     toggle )
  \brief Create an action which can be then used in the menu or toolbar.
  \param id the unique id action to be registered to
  \param menuText action text which should appear in menu
  \param tipText text which should appear in the tooltip
  \param statusText text which should appear in the status bar when action is activated
  \param icon the name of the icon file (the actual icon file name can be coded in the translation files)
  \param key the key accelrator for the action
  \param toggle if \c true the action is checkable
*/

class TCreateActionEvent: public SALOME_Event 
{
public:
  typedef QAction* TResult;
  TResult myResult;
  int     myId;
  QString myMenuText;
  QString myTipText;
  QString myStatusText;
  QString myIcon;
  int     myKey;
  bool    myToggle;
  TCreateActionEvent( const int id, const QString& menuText, const QString& tipText, 
                      const QString& statusText, const QString& icon, const int key, const bool toggle ) 
    : myResult( 0 ), myId( id ), myMenuText( menuText ), myTipText( tipText ),
      myStatusText( statusText ), myIcon( icon ), myKey( key ), myToggle( toggle ) {}
  virtual void Execute()
  {
    SALOME_PYQT_ModuleLight* module = getActiveModule();
    if ( module )
      myResult = (QAction*)module->createAction( myId, myTipText, myIcon, myMenuText, myStatusText, myKey, myToggle );
  }
};
QAction* SalomePyQt::createAction( const int id,           const QString& menuText, 
                                     const QString& tipText, const QString& statusText, 
                                     const QString& icon,    const int key, const bool toggle )
{
  return ProcessEvent( new TCreateActionEvent( id, menuText, tipText, statusText, icon, key, toggle ) );
}

/*!
  \fn QtxActionGroup* SalomePyQt::createActionGroup( const int id, const bool exclusive )
  \brief Create an action group which can be then used in the menu or toolbar
  \param id         : the unique id action group to be registered to
  \param exclusive  : if \c true the action group does exclusive toggling
*/

struct TcreateActionGroupEvent: public SALOME_Event {
  typedef QtxActionGroup* TResult;
  TResult myResult;
  int     myId;
  bool    myExclusive;
  TcreateActionGroupEvent( const int id, const bool exclusive )
    : myId( id ), myExclusive( exclusive ) {}
  virtual void Execute()
  {
    SALOME_PYQT_ModuleLight* module = getActiveModule();
    if ( module )
      myResult = module->createActionGroup( myId, myExclusive );
  }
};
QtxActionGroup* SalomePyQt::createActionGroup(const int id, const bool exclusive)
{
  return ProcessEvent( new TcreateActionGroupEvent( id, exclusive ) );
}

/*!
  \fn QAction* SalomePyQt::action( const int id )
  \brief Get action by specified identifier.
  \return action or 0 if action is not registered
*/

class TActionEvent: public SALOME_Event 
{
public:
  typedef QAction* TResult;
  TResult myResult;
  int     myId;
  TActionEvent( const int id )
    : myResult( 0 ), myId( id ) {}
  virtual void Execute()
  {
    SALOME_PYQT_ModuleLight* module = getActiveModule();
    if ( module )
      myResult = (QAction*)module->action( myId );
  }
};
QAction* SalomePyQt::action( const int id )
{
  return ProcessEvent( new TActionEvent( id ) );
}

/*!
  \fn int SalomePyQt::actionId( const QAction* a );
  \brief Get an action identifier. 
  \return action ID or -1 if action is not registered
*/

class TActionIdEvent: public SALOME_Event 
{
public:
  typedef  int TResult;
  TResult  myResult;
  const QAction* myAction;
  TActionIdEvent( const QAction* action )
    : myResult( -1 ), myAction( action ) {}
  virtual void Execute()
  {
    SALOME_PYQT_ModuleLight* module = getActiveModule();
    if ( module )
      myResult = module->actionId( myAction );
  }
};
int SalomePyQt::actionId( const QAction* a )
{
  return ProcessEvent( new TActionIdEvent( a ) );
}

/*!
  \fn int SalomePyQt::addGlobalPreference( const QString& label );
  \brief Add global (not module-related) preferences group.
  \param label global preferences group name
  \return preferences group identifier
*/

class TAddGlobalPrefEvent: public SALOME_Event
{
public:
  typedef int TResult;
  TResult myResult;
  QString myLabel;
  TAddGlobalPrefEvent( const QString& label )
    : myResult( -1 ), myLabel( label ) {}
  virtual void Execute() 
  {
    SALOME_PYQT_ModuleLight* module = getActiveModule();
    if ( module )
      myResult = module->addGlobalPreference( myLabel );
  }
};
int SalomePyQt::addGlobalPreference( const QString& label )
{
  return ProcessEvent( new TAddGlobalPrefEvent( label ) );
}

/*!
  \fn int SalomePyQt::addPreference( const QString& label );
  \brief Add module-related preferences group.
  \param label preferences group name
  \return preferences group identifier
*/

class TAddPrefEvent: public SALOME_Event 
{
public:
  typedef int TResult;
  TResult myResult;
  QString myLabel;
  TAddPrefEvent( const QString& label )
    : myResult( -1 ), myLabel( label ) {}
  virtual void Execute() 
  {
    SALOME_PYQT_ModuleLight* module = getActiveModule();
    if ( module )
      myResult = module->addPreference( myLabel );
  }
};
int SalomePyQt::addPreference( const QString& label )
{
  return ProcessEvent( new TAddPrefEvent( label ) );
}

/*!
  \fn int SalomePyQt::addPreference( const QString& label, const int pId, const int type,
                                     const QString& section, const QString& param );
  \brief Add module-related preferences.
  \param label preferences group name
  \param pId parent preferences group id
  \param type preferences type
  \param section resources file section name
  \param param resources file setting name
  \return preferences identifier
*/

class TAddPrefParamEvent: public SALOME_Event
{
public:
  typedef int TResult;
  TResult myResult;
  QString myLabel;
  int     myPId;
  int     myType;
  QString mySection;
  QString myParam;
  TAddPrefParamEvent( const QString& label, 
                      const int pId, const int type,
                      const QString& section, 
                      const QString& param )
    : myResult( -1 ),
      myLabel( label ), myPId( pId ), myType( type ), 
      mySection( section ), myParam ( param ) {}
  virtual void Execute()
  {
    SALOME_PYQT_ModuleLight* module = getActiveModule();
    if ( module )
      myResult = module->addPreference( myLabel, myPId, myType, mySection, myParam );
  }
};
int SalomePyQt::addPreference( const QString& label, const int pId, const int type,
                               const QString& section, const QString& param )
{
  return ProcessEvent( new TAddPrefParamEvent( label, pId, type, section, param ) );
}

/*!
  \fn QVariant SalomePyQt::preferenceProperty( const int id, const QString& prop );
  \brief Get the preferences property.
  \param id preferences identifier
  \param prop preferences property name
  \return preferences property value or null QVariant if property is not set
*/

class TPrefPropEvent: public SALOME_Event
{
public:
  typedef QVariant TResult;
  TResult myResult;
  int     myId;
  QString myProp;
  TPrefPropEvent( const int id, const QString& prop )
    : myId( id ), myProp( prop ) {}
  virtual void Execute()
  {
    SALOME_PYQT_ModuleLight* module = getActiveModule();
    if ( module )
      myResult = module->preferenceProperty( myId, myProp );
  }
};
QVariant SalomePyQt::preferenceProperty( const int id, const QString& prop )
{
  return ProcessEvent( new TPrefPropEvent( id, prop ) );
}

/*!
  \brief Set the preferences property.
  \param id preferences identifier
  \param prop preferences property name
  \param var preferences property value
*/
void SalomePyQt::setPreferenceProperty( const int id, 
                                        const QString& prop,
                                        const QVariant& var )
{
  class TEvent: public SALOME_Event
  {
    int      myId;
    QString  myProp;
    QVariant myVar;
  public:
    TEvent( const int id, const QString& prop, const QVariant& var ) 
      : myId( id ), myProp( prop ), myVar( var ) {}
    virtual void Execute() 
    {
      SALOME_PYQT_ModuleLight* module = getActiveModule();
      if ( module )
        module->setPreferenceProperty( myId, myProp, myVar );
    }
  };
  ProcessVoidEvent( new TEvent( id, prop, var) );
}

/*!
  \brief Add the property value to the list of values.

  This method allows creating properties which are QList<QVariant>
  - there is no way to pass such values directly to QVariant parameter with PyQt.

  \param id preferences identifier
  \param prop preferences property name
  \param idx preferences property index
  \param var preferences property value for the index \a idx
*/
void SalomePyQt::addPreferenceProperty( const int id, 
                                        const QString& prop,
                                        const int idx, 
                                        const QVariant& var )
{
  class TEvent: public SALOME_Event
  {
    int      myId;
    QString  myProp;
    int      myIdx;
    QVariant myVar;
  public:
    TEvent( const int id, const QString& prop, const int idx, const QVariant& var ) 
      : myId( id ), myProp( prop ), myIdx( idx), myVar( var ) {}
    virtual void Execute()
    {
      SALOME_PYQT_ModuleLight* module = getActiveModule();
      if ( module ) {
        QVariant var =  module->preferenceProperty( myId, myProp );
        if ( var.isValid() ) {
          if ( var.type() == QVariant::StringList ) {
            QStringList sl = var.toStringList();
            if ( myIdx >= 0 && myIdx < sl.count() ) 
              sl[myIdx] = myVar.toString();
            else
              sl.append( myVar.toString() );
            module->setPreferenceProperty( myId, myProp, sl );
          }
          else if ( var.type() == QVariant::List ) {
            QList<QVariant> vl = var.toList();
            if ( myIdx >= 0 && myIdx < vl.count() ) 
              vl[myIdx] = myVar;
            else
              vl.append( myVar );
            module->setPreferenceProperty( myId, myProp, vl );
          }
        }
        else {
          QList<QVariant> vl;
          vl.append( myVar );
          module->setPreferenceProperty( myId, myProp, vl );
        }
      }
    }
  };
  ProcessVoidEvent( new TEvent( id, prop, idx, var) );
}

/*!
  \brief Put the message to the Log messages output window
  \param msg message text (it can be of simple rich text format)
  \param addSeparator boolean flag which specifies if it is necessary 
         to separate the message with predefined separator
*/
void SalomePyQt::message( const QString& msg, bool addSeparator )
{
  class TEvent: public SALOME_Event
  {
    QString  myMsg;
    bool     myAddSep;
  public:
    TEvent( const QString& msg, bool addSeparator ) 
      : myMsg( msg ), myAddSep( addSeparator ) {}
    virtual void Execute()
    {
      if ( LightApp_Application* anApp = getApplication() ) {
        LogWindow* lw = anApp->logWindow();
        if ( lw )
          lw->putMessage( myMsg, myAddSep );
      }
    }
  };
  ProcessVoidEvent( new TEvent( msg, addSeparator ) );
}

/*!
  \brief Remove all the messages from the Log messages output window.
*/
void SalomePyQt::clearMessages()
{
  class TEvent: public SALOME_Event
  {
  public:
    TEvent() {}
    virtual void Execute()
    {
      if ( LightApp_Application* anApp = getApplication() ) {
        LogWindow* lw = anApp->logWindow();
        if ( lw )
          lw->clear();
      }
    }
  };
  ProcessVoidEvent( new TEvent() );
}

/*!
  \brief Gets window with specified identifier 
  \internal
  \param id window identifier 
  \return pointer on the window
*/
static SUIT_ViewWindow* getWnd( const int id )
{
  SUIT_ViewWindow* resWnd = 0;

  LightApp_Application* app = getApplication();
  if ( app )
  {
    ViewManagerList vmlist = app->viewManagers();
    foreach( SUIT_ViewManager* vm, vmlist )
    {
      QVector<SUIT_ViewWindow*> vwlist = vm->getViews();
      foreach ( SUIT_ViewWindow* vw, vwlist )
      {
        if ( id == vw->getId() )
        {
          resWnd = vw;
          break;
        }
      }
    }
  }

  return resWnd;
}

/*!
  \fn QList<int> SalomePyQt::getViews()
  \brief Get list of integer identifiers of all the currently opened views
  \return list of integer identifiers of all the currently opened views
*/

class TGetViews: public SALOME_Event
{
public:
  typedef QList<int> TResult;
  TResult myResult;
  TGetViews() {}
  virtual void Execute() 
  {
    myResult.clear();
    LightApp_Application* app  = getApplication();
    if ( app )
    {
      STD_TabDesktop* tabDesk = dynamic_cast<STD_TabDesktop*>( app->desktop() );
      if ( tabDesk )
      {
        QList<SUIT_ViewWindow*> wndlist = tabDesk->windows();
        SUIT_ViewWindow* wnd;
        foreach ( wnd, wndlist )
          myResult.append( wnd->getId() );
      }
    }
  }
};
QList<int> SalomePyQt::getViews()
{
  return ProcessEvent( new TGetViews() );
}

/*!
  \fn int SalomePyQt::getActiveView()
  \brief Get integer identifier of the currently active view
  \return integer identifier of the currently active view
*/

class TGetActiveView: public SALOME_Event
{
public:
  typedef int TResult;
  TResult myResult;
  TGetActiveView()
    : myResult( -1 ) {}
  virtual void Execute() 
  {
    LightApp_Application* app = getApplication();
    if ( app )
    {
      SUIT_ViewManager* viewMgr = app->activeViewManager();
      if ( viewMgr )
      {
        SUIT_ViewWindow* wnd = viewMgr->getActiveView();
        if ( wnd )
          myResult = wnd->getId();
      }
    }
  }
};
int SalomePyQt::getActiveView()
{
  return ProcessEvent( new TGetActiveView() );
}

/*!                      
  \fn QString SalomePyQt::getViewType( const int id )
  \brief Get type of the specified view, e.g. "OCCViewer"
  \param id window identifier
  \return view type
*/ 

class TGetViewType: public SALOME_Event
{
public:
  typedef QString TResult;
  TResult myResult;
  int myWndId;
  TGetViewType( const int id )
    : myWndId( id ) {}
  virtual void Execute() 
  {
    SUIT_ViewWindow* wnd = getWnd( myWndId );
    if ( wnd )
    {
      SUIT_ViewManager* viewMgr = wnd->getViewManager();
      if ( viewMgr )
        myResult = viewMgr->getType();
    }
  }
};
QString SalomePyQt::getViewType( const int id )
{
  return ProcessEvent( new TGetViewType( id ) );
}

/*!
  \fn bool SalomePyQt::setViewTitle( const int id, const QString& title )
  \brief Change view caption  
  \param id window identifier
  \param title new window title
  \return \c true if operation is completed successfully and \c false otherwise 
*/

class TSetViewTitle: public SALOME_Event
{
public:
  typedef bool TResult;
  TResult myResult;
  int myWndId;
  QString myTitle;
  TSetViewTitle( const int id, const QString& title )
    : myResult( false ),
      myWndId( id ),
      myTitle( title ) {}
  virtual void Execute() 
  {
    SUIT_ViewWindow* wnd = getWnd( myWndId );
    if ( wnd )
    {
      wnd->setWindowTitle( myTitle );
      myResult = true;
    }
  }
};
bool SalomePyQt::setViewTitle( const int id, const QString& title )
{
  return ProcessEvent( new TSetViewTitle( id, title ) );
}


/*!
  \fn QString SalomePyQt::getViewTitle( const int id )
  \brief Get view caption  
  \param id window identifier
  \return view caption  
*/

class TGetViewTitle: public SALOME_Event
{
public:
  typedef QString TResult;
  TResult myResult;
  int myWndId;
  TGetViewTitle( const int id )
    : myWndId( id ) {}
  virtual void Execute() 
  {
    SUIT_ViewWindow* wnd = getWnd( myWndId );
    if ( wnd )
      myResult = wnd->windowTitle();
  }
};
QString SalomePyQt::getViewTitle( const int id )
{
  return ProcessEvent( new TGetViewTitle( id ) );
}

/*!
  \fn QList<int> SalomePyQt::findViews( const QString& type )
  \brief Get list of integer identifiers of all the 
         currently opened views of the specified type
  \param type viewer type
  \return list of integer identifiers 
*/

class TFindViews: public SALOME_Event
{
public:
  typedef QList<int> TResult;
  TResult myResult;
  QString myType;
  TFindViews( const QString& type )
    : myType( type ) {}
  virtual void Execute() 
  {
    myResult.clear();
    LightApp_Application* app  = getApplication();
    if ( app )
    {
      ViewManagerList vmList;
      app->viewManagers( myType, vmList );
      SUIT_ViewManager* viewMgr;
      foreach ( viewMgr, vmList )
      {
        QVector<SUIT_ViewWindow*> vec = viewMgr->getViews();
        for ( int i = 0, n = vec.size(); i < n; i++ )
        {
          SUIT_ViewWindow* wnd = vec[ i ];
          if ( wnd )
            myResult.append( wnd->getId() );
        }
      }
    }
  }
};
QList<int> SalomePyQt::findViews( const QString& type )
{
  return ProcessEvent( new TFindViews( type ) );
}

/*!
  \fn bool SalomePyQt::activateView( const int id )
  \brief Activate view
  \param id window identifier
  \return \c true if operation is completed successfully and \c false otherwise 
*/

class TActivateView: public SALOME_Event
{
public:
  typedef bool TResult;
  TResult myResult;
  int myWndId;
  TActivateView( const int id )
    : myResult( false ),
      myWndId( id ) {}
  virtual void Execute() 
  {
    SUIT_ViewWindow* wnd = getWnd( myWndId );
    if ( wnd )
    {
      wnd->setFocus();
      myResult = true;
    }
  }
};
bool SalomePyQt::activateView( const int id )
{
  return ProcessEvent( new TActivateView( id ) );
}

/*!
  \fn int SalomePyQt::createView( const QString& type )
  \brief Create new view and activate it
  \param type viewer type
  \return integer identifier of created view (or -1 if view could not be created)
*/

class TCreateView: public SALOME_Event
{
public:
  typedef int TResult;
  TResult myResult;
  QString myType;
  TCreateView( const QString& theType )
    : myResult( -1 ),
      myType( theType ) {}
  virtual void Execute() 
  {
    LightApp_Application* app  = getApplication();
    if ( app )
    {
      SUIT_ViewManager* viewMgr = app->createViewManager( myType );
      if ( viewMgr )
      {
        SUIT_ViewWindow* wnd = viewMgr->getActiveView();
        if ( wnd )
          myResult = wnd->getId();
      }
    }
  }
};
int SalomePyQt::createView( const QString& type )
{
  return ProcessEvent( new TCreateView( type ) );
}

/*!
  \fn int SalomePyQt::createView( const QString& type, QWidget* w )
  \brief Create new view with custom widget embedded and activate it
  \param type viewer type
  \param w custom widget
  \return integer identifier of created view (or -1 if view could not be created)
*/

class TCreateViewWg: public SALOME_Event
{
public:
  typedef int TResult;
  TResult myResult;
  QString myType;
  QWidget* myWidget;
  TCreateViewWg( const QString& theType, QWidget* w )
    : myResult( -1 ),
      myType( theType ),
      myWidget( w ) {}
  virtual void Execute() 
  {
    LightApp_Application* app  = getApplication();
    if ( app )
    {
      SUIT_ViewManager* viewMgr = app->createViewManager( myType, myWidget );
      if ( viewMgr )
      {
        SUIT_ViewWindow* wnd = viewMgr->getActiveView();
        if ( wnd )
          myResult = wnd->getId();
      }
    }
  }
};
int SalomePyQt::createView( const QString& type, QWidget* w )
{
  return ProcessEvent( new TCreateViewWg( type, w ) );
}

/*!
  \fn bool SalomePyQt::closeView( const int id )
  \brief Close view
  \param id window identifier
  \return \c true if operation is completed successfully and \c false otherwise 
*/

class TCloseView: public SALOME_Event
{
public:
  typedef bool TResult;
  TResult myResult;
  int myWndId;
  TCloseView( const int id )
    : myResult( false ),
      myWndId( id ) {}
  virtual void Execute() 
  {
    SUIT_ViewWindow* wnd = getWnd( myWndId );
    if ( wnd )
    {
      SUIT_ViewManager* viewMgr = wnd->getViewManager();
      if ( viewMgr )
      {
        wnd->close();
        myResult = true;
      }
    }
  }
};
bool SalomePyQt::closeView( const int id )
{
  return ProcessEvent( new TCloseView( id ) );
}

/*!
  \fn int SalomePyQt::cloneView( const int id )
  \brief Clone view (if this operation is supported for specified view type)
  \param id window identifier
  \return integer identifier of the cloned view or -1 or operation could not be performed
*/

class TCloneView: public SALOME_Event
{
public:
  typedef int TResult;
  TResult myResult;
  int myWndId;
  TCloneView( const int id )
    : myResult( -1 ),
      myWndId( id ) {}
  virtual void Execute() 
  {
    SUIT_ViewWindow* wnd = getWnd( myWndId );
    if ( wnd )
    {
      SUIT_ViewManager* viewMgr = wnd->getViewManager();
      if ( viewMgr )
      {
        if ( wnd->inherits( "OCCViewer_ViewWindow" ) )
        {
          OCCViewer_ViewWindow* occView = (OCCViewer_ViewWindow*)( wnd );
          occView->onCloneView();

          wnd = viewMgr->getActiveView();
          if ( wnd )
            myResult = wnd->getId();
        }
        else if ( wnd->inherits( "Plot2d_ViewWindow" ) ) 
        {
          Plot2d_ViewManager* viewMgr2d = dynamic_cast<Plot2d_ViewManager*>( viewMgr );
          Plot2d_ViewWindow* srcWnd2d = dynamic_cast<Plot2d_ViewWindow*>( wnd );
          if ( viewMgr2d && srcWnd2d )
          {
            Plot2d_ViewWindow* resWnd = viewMgr2d->cloneView( srcWnd2d );
            myResult = resWnd->getId();
          }
        }
      }
    }
  }
};
int SalomePyQt::cloneView( const int id )
{
  return ProcessEvent( new TCloneView( id ) );
}

/*!
  \fn bool SalomePyQt::isViewVisible( const int id )
  \brief Check whether view is visible ( i.e. it is on the top of the views stack)
  \param id window identifier
  \return \c true if view is visible and \c false otherwise 
*/

class TIsViewVisible: public SALOME_Event
{
public:
  typedef bool TResult;
  TResult myResult;
  int myWndId;
  TIsViewVisible( const int id )
    : myResult( false ),
      myWndId( id ) {}
  virtual void Execute() 
  {
    SUIT_ViewWindow* wnd = getWnd( myWndId );
    if ( wnd )
    {
      QWidget* p = wnd->parentWidget();
      myResult = ( p && p->isVisibleTo( p->parentWidget() ) );
    }
  }
};
bool SalomePyQt::isViewVisible( const int id )
{
  return ProcessEvent( new TIsViewVisible( id ) );
}
  
/*!
  \fn bool SalomePyQt::setViewClosable( const int id, const bool on )
  \brief Set / clear view's "closable" option. By default any view is closable
        (i.e. can be closed by the user).
  \param id window identifier
  \param on new "closable" option's value
*/

void SalomePyQt::setViewClosable( const int id, const bool on )
{
  class TEvent: public SALOME_Event
  {
    int myWndId;
    bool myOn;
  public:
    TEvent( const int id, const bool on )
      : myWndId( id ), myOn( on ) {}
    virtual void Execute()
    {
      SUIT_ViewWindow* wnd = getWnd( myWndId );
      if ( wnd ) wnd->setClosable( myOn );
    }
  };
  ProcessVoidEvent( new TEvent( id, on ) );
}

/*!
  \fn bool SalomePyQt::isViewClosable( const int id )
  \brief Check whether view is closable (i.e. can be closed by the user)
  \param id window identifier
  \return \c true if view is closable or \c false otherwise 
*/

class TIsViewClosable: public SALOME_Event
{
public:
  typedef bool TResult;
  TResult myResult;
  int myWndId;
  TIsViewClosable( const int id )
    : myResult( true ),
      myWndId( id ) {}
  virtual void Execute() 
  {
    SUIT_ViewWindow* wnd = getWnd( myWndId );
    if ( wnd )
      myResult = wnd->closable();
  }
};

bool SalomePyQt::isViewClosable( const int id )
{
  return ProcessEvent( new TIsViewClosable( id ) );
}

/*!
  \fn bool SalomePyQt::groupAllViews()
  \brief Group all views to the single tab area
  \return \c true if operation is completed successfully and \c false otherwise 
*/

class TGroupAllViews: public SALOME_Event
{
public:
  typedef bool TResult;
  TResult myResult;
  TGroupAllViews()
    : myResult( false ) {}
  virtual void Execute() 
  {
    LightApp_Application* app  = getApplication();
    if ( app )
    {
      STD_TabDesktop* tabDesk = dynamic_cast<STD_TabDesktop*>( app->desktop() );
      if ( tabDesk )
      {
        QtxWorkstack* wStack = tabDesk->workstack();
        if ( wStack )
        {
          wStack->stack();
          myResult = true;
        }
      }
    }
  }
};
bool SalomePyQt::groupAllViews()
{
  return ProcessEvent( new TGroupAllViews() );
}

/*!
  \fn bool SalomePyQt::splitView( const int id, const Orientation ori, const Action action )
  \brief Split tab area to which view with identifier belongs to
  \param id window identifier
  \param ori orientation of split operation
  \param action action to be performed
  \return \c true if operation is completed successfully \c false otherwise 
*/

class TSplitView: public SALOME_Event
{
public:
  typedef bool TResult;
  TResult myResult;
  int myWndId;
  Orientation myOri;
  Action myAction;
  TSplitView( const int id, 
              const Orientation ori, 
              const Action action )
    : myResult( false ),
      myWndId( id ),
      myOri( ori ),
      myAction( action ) {}
  virtual void Execute() 
  {
    SUIT_ViewWindow* wnd = getWnd( myWndId );
    if ( wnd )
    {
      // activate view
      // wnd->setFocus(); ???

      // split workstack
      if ( getApplication() )
      {
        STD_TabDesktop* desk = 
          dynamic_cast<STD_TabDesktop*>( getApplication()->desktop() );
        if ( desk )
        {
          QtxWorkstack* wStack = desk->workstack();
          if ( wStack )
          {
            Qt::Orientation qtOri = 
              ( myOri == Horizontal ) ? Qt::Horizontal : Qt::Vertical;

            QtxWorkstack::SplitType sType;
            if ( myAction == MoveWidget )
              sType = QtxWorkstack::SplitMove;
            else if ( myAction == LeaveWidget )
              sType = QtxWorkstack::SplitStay;
            else 
              sType = QtxWorkstack::SplitAt;

            wStack->Split( wnd, qtOri, sType );
            myResult = true;
          }
        }
      }
    }
  }
};
bool SalomePyQt::splitView( const int id, const Orientation ori, const Action action )
{
  return ProcessEvent( new TSplitView( id, ori, action ) );
}

/*!
  \fn bool SalomePyQt::moveView( const int id, const int id_to, const bool before )
  \brief Move view with the first identifier to the same area which 
         another view with the second identifier belongs to
  \param id source window identifier
  \param id_to destination window identifier  
  param before specifies whether the first viewt has to be moved before or after 
        the second view
  \return \c true if operation is completed successfully and \c false otherwise 
*/

class TMoveView: public SALOME_Event
{
public:
  typedef bool TResult;
  TResult myResult;
  int myWndId;
  int myWndToId;
  bool myIsBefore;
  TMoveView( const int id, const int id_to, const bool before )
    : myResult( false ),
    myWndId( id ),
    myWndToId( id_to ),
    myIsBefore( before ) {}
  virtual void Execute() 
  {
    SUIT_ViewWindow* wnd = getWnd( myWndId );
    SUIT_ViewWindow* wnd_to = getWnd( myWndToId );
    if ( wnd && wnd_to )
    {
      QtxWorkstack* wStack = dynamic_cast<STD_TabDesktop*>( 
        getApplication()->desktop() )->workstack();
      if ( wStack )
        myResult = wStack->move( wnd, wnd_to, myIsBefore );
    }
  }
};
bool SalomePyQt::moveView( const int id, const int id_to, const bool before )
{
  return ProcessEvent( new TMoveView( id, id_to, before ) );
}

/*!
  \fn QList<int> SalomePyQt::neighbourViews( const int id )
  \brief Get list of views identifiers that belongs to the same area as 
         specified view (excluding it)
  \param id window identifier
  \return list of views identifiers
*/

class TNeighbourViews: public SALOME_Event
{
public:
  typedef QList<int> TResult;
  TResult myResult;
  int myWndId;
  TNeighbourViews( const int id )
    : myWndId( id ) {}
  virtual void Execute() 
  {
    myResult.clear();
    SUIT_ViewWindow* wnd = getWnd( myWndId );
    if ( wnd )
    {
      QtxWorkstack* wStack = dynamic_cast<STD_TabDesktop*>( 
        getApplication()->desktop() )->workstack();
      if ( wStack )
      {
        QWidgetList wgList = wStack->windowList( wnd );
        QWidget* wg;
        foreach ( wg, wgList )
        {
          SUIT_ViewWindow* tmpWnd = dynamic_cast<SUIT_ViewWindow*>( wg );
          if ( tmpWnd && tmpWnd != wnd )
            myResult.append( tmpWnd->getId() );
        }
      }
    }
  }
};
QList<int> SalomePyQt::neighbourViews( const int id )
{
  return ProcessEvent( new TNeighbourViews( id ) );
}


/*!
  SalomePyQt::createObject(parent)
  Create empty data object
*/
class TCreateEmptyObjectEvent: public SALOME_Event {
public:
  typedef QString TResult;
  TResult  myResult;
  QString  myParent;
  TCreateEmptyObjectEvent(const QString& parent) : myParent( parent ) {}
  virtual void Execute() {
    SALOME_PYQT_ModuleLight* module = getActiveModule();
    if ( module )
      myResult = (QString)module->createObject(myParent);
  }
};
QString SalomePyQt::createObject(const QString& parent)
{
  return ProcessEvent( new TCreateEmptyObjectEvent(parent) );
}

/*!
  SalomePyQt::createObject( name, icon, tooltip, parent )
  Create data object with name, icon and tooltip
*/
class TCreateObjectEvent: public SALOME_Event {
public:
  typedef QString TResult;
  TResult myResult;
  QString myParent;
  QString myName;
  QString myIconName;
  QString myToolTip;
  TCreateObjectEvent(const QString& name,
                     const QString& iconname,
                     const QString& tooltip,
                     const QString& parent) : myName(name),
                                              myIconName(iconname),
                                              myToolTip(tooltip),
                                              myParent( parent ){}
  virtual void Execute() {
    SALOME_PYQT_ModuleLight* module = getActiveModule();
    if ( module )
      myResult = (QString)module->createObject(myName, myIconName,
                                               myToolTip, myParent);
  }
};
QString SalomePyQt::createObject(const QString& name,
                                 const QString& iconname,
                                 const QString& tooltip,
                                 const QString& parent)
{
  return ProcessEvent( new TCreateObjectEvent(name, iconname, tooltip, parent) );
}


/*!
  SalomePyQt::setName(obj,name)
  Set object name
*/
class TSetNameEvent: public SALOME_Event
{
public:
  QString myObj;
  QString myName;
  TSetNameEvent( const QString& obj,
                 const QString& name) : myObj(obj),
                                         myName(name) {}
  virtual void Execute() {
    SALOME_PYQT_ModuleLight* module = getActiveModule();
    if ( module )
      module->setName(myObj,myName);
  }
};
void SalomePyQt::setName(const QString& obj,const QString& name)
{
  ProcessVoidEvent(new TSetNameEvent(obj,name));
}


/*!
  SalomePyQt::setIcon(obj,icon)
  Set object icon
*/
class TSetIconEvent: public SALOME_Event
{
public:
  QString myObj;
  QString myIconName;
  TSetIconEvent( const QString& obj,
                 const QString& iconname) : myObj(obj),
                                            myIconName(iconname) {}
  virtual void Execute() {
    SALOME_PYQT_ModuleLight* module = getActiveModule();
    if ( module )
      module->setIcon(myObj,myIconName);
  }
};

void SalomePyQt::setIcon(const QString& obj,const QString& iconname)
{
  ProcessVoidEvent(new TSetIconEvent(obj,iconname));
}

/*!
  SalomePyQt::setToolTip(obj,tooltip)
  Set object tool tip
*/
class TSetToolTipEvent: public SALOME_Event
{
public:
  QString myObj;
  QString myToolTip;
  TSetToolTipEvent( const QString& obj,
                    const QString& tooltip) : myObj(obj),
                                              myToolTip(tooltip) {}
  virtual void Execute() {
    SALOME_PYQT_ModuleLight* module = getActiveModule();
    if ( module )
      module->setToolTip(myObj,myToolTip);
  }
};
void SalomePyQt::setToolTip(const QString& obj,const QString& tooltip)
{
  ProcessVoidEvent(new TSetToolTipEvent(obj,tooltip));
}

/*!
  SalomePyQt::setReference(obj,refEntry)
  Set entry to referenced object
*/
class TSetRefEvent: public SALOME_Event
{
public:
  QString myObj;
  QString myRefEntry;
  TSetRefEvent( const QString& obj,
		const QString& refEntry) : myObj(obj),
					   myRefEntry(refEntry) {}
  virtual void Execute() {
    SALOME_PYQT_ModuleLight* module = getActiveModule();
    if ( module )
      module->setReference(myObj,myRefEntry);
  }
};
void SalomePyQt::setReference(const QString& obj,const QString& refEntry)
{
  ProcessVoidEvent(new TSetRefEvent(obj,refEntry));
}

/*!
  SalomePyQt::setColor(obj,color)
  Set object color
*/
class TSetColorEvent: public SALOME_Event
{
public:
  QString myObj;
  QColor  myColor;
  TSetColorEvent( const QString& obj,
		  const QColor& color) : myObj(obj),
					 myColor(color) {}
  virtual void Execute() {
    SALOME_PYQT_ModuleLight* module = getActiveModule();
    if ( module )
      module->setColor(myObj,myColor);
  }
};
void SalomePyQt::setColor(const QString& obj,const QColor& color)
{
  ProcessVoidEvent(new TSetColorEvent(obj,color));
}

/*!
  SalomePyQt::getName(obj)
  Return name of object
*/
class TGetNameEvent: public SALOME_Event
{
public:
  typedef QString TResult;
  TResult myResult;
  QString myObj;
  TGetNameEvent( const QString& obj ) : myObj(obj) {}
  virtual void Execute() {
    SALOME_PYQT_ModuleLight* module = getActiveModule();
    if ( module )
      myResult = (QString) module->getName(myObj);
  }
};

QString SalomePyQt::getName(const QString& obj)
{
  return ProcessEvent(new TGetNameEvent(obj));
}

/*!
  SalomePyQt::getToolTip(obj)
  Return tool tip of object
*/
class TGetToolTipEvent: public SALOME_Event
{
public:
  typedef QString TResult;
  TResult myResult;
  QString myObj;
  TGetToolTipEvent( const QString& obj ) : myObj(obj) {}
  virtual void Execute() {
    SALOME_PYQT_ModuleLight* module = getActiveModule();
    if ( module )
      myResult = (QString)module->getToolTip(myObj);
  }
};
QString SalomePyQt::getToolTip(const QString& obj)
{
  return ProcessEvent(new TGetToolTipEvent(obj));
}

/*!
  SalomePyQt::getReference(obj)
  Return entry of the referenced object (if any)
*/
class TGetRefEvent: public SALOME_Event
{
public:
  typedef QString TResult;
  TResult myResult;
  QString myObj;
  TGetRefEvent( const QString& obj ) : myObj(obj) {}
  virtual void Execute() {
    SALOME_PYQT_ModuleLight* module = getActiveModule();
    if ( module )
      myResult = module->getReference(myObj);
  }
};
QString SalomePyQt::getReference(const QString& obj)
{
  return ProcessEvent(new TGetRefEvent(obj));
}

/*!
  SalomePyQt::getColor(obj)
  Return the color of the object
*/
class TGetColorEvent: public SALOME_Event
{
public:
  typedef QColor TResult;
  TResult myResult;
  QString myObj;
  TGetColorEvent( const QString& obj ) : myObj(obj) {}
  virtual void Execute() {
    SALOME_PYQT_ModuleLight* module = getActiveModule();
    if ( module )
      myResult = module->getColor(myObj);
  }
};
QColor SalomePyQt::getColor(const QString& obj)
{
  return ProcessEvent(new TGetColorEvent(obj));
}

/*!
  SalomePyQt::removeChild(obj)
  Remove childrens from object
*/
class TRemoveChildEvent: public SALOME_Event
{
public:
  QString myObj;
  TRemoveChildEvent(const QString& obj) : myObj(obj) {}
  
  virtual void Execute() {
    SALOME_PYQT_ModuleLight* module = getActiveModule();
    if ( module )
      module->removeChild(myObj);
  }
};
void SalomePyQt::removeChild(const QString& obj)
{
  ProcessVoidEvent(new TRemoveChildEvent(obj));
}


/*!
  SalomePyQt::removeObject(obj)
  Remove object
*/
class TRemoveObjectEvent: public SALOME_Event
{
public:
  QString myObj;
  
  TRemoveObjectEvent( const QString& obj) : myObj(obj) {}
  virtual void Execute() {
    SALOME_PYQT_ModuleLight* module = getActiveModule();
    if ( module )
      module->removeObject(myObj);
  }
};
void SalomePyQt::removeObject(const QString& obj)
{
  ProcessVoidEvent(new TRemoveObjectEvent(obj));
}

/*!
  SalomePyQt::getChildren(obj)
  Return the list of the child objects
  if rec == true then function get all sub children.
*/

class TGetChildrenEvent: public SALOME_Event
{
public:
  typedef QStringList TResult;
  TResult myResult;
  QString myObj;
  bool myRec; 
  TGetChildrenEvent(const QString& obj, const bool rec) : myObj(obj),
                                                          myRec(rec) {}
  virtual void Execute() {
    SALOME_PYQT_ModuleLight* module = getActiveModule();
    if ( module )
      myResult = (QStringList)module->getChildren(myObj,myRec);
  }
};
QStringList SalomePyQt::getChildren(const QString& obj, const bool rec)
{
  return ProcessEvent( new TGetChildrenEvent(obj,rec) ); 
}
