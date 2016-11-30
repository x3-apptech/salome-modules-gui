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

// File   : SalomePyQt.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#ifdef WIN32
// E.A. : On windows with python 2.6, there is a conflict
// E.A. : between pymath.h and Standard_math.h which define
// E.A. : some same symbols : acosh, asinh, ...
#include <Standard_math.hxx>
#include <pymath.h>
#endif

#include "SALOME_PYQT_ModuleLight.h" // this include must be first!!!
#include "SALOME_PYQT_DataModelLight.h"
#include "SALOME_PYQT_PyModule.h"
#include "SalomePyQt.h"

#include "LightApp_SelectionMgr.h"
#include "LogWindow.h"
#ifndef DISABLE_OCCVIEWER
#include "OCCViewer_ViewWindow.h"
#include "OCCViewer_ViewFrame.h"
#endif // DISABLE_OCCVIEWER
#ifndef DISABLE_PLOT2DVIEWER
#include "Plot2d_ViewManager.h"
#include "Plot2d_ViewWindow.h"
#endif // DISABLE_PLOT2DVIEWER
#ifndef DISABLE_PVVIEWER
#include "PVViewer_ViewManager.h"
#include "PVViewer_ViewModel.h"
#endif // DISABLE_PVVIEWER
#include "QtxActionMenuMgr.h"
#include "QtxWorkstack.h"
#include "QtxTreeView.h"
#include "SALOME_Event.h"
#include "STD_TabDesktop.h"
#include "SUIT_DataBrowser.h"
#include "SUIT_ResourceMgr.h"
#include "SUIT_Session.h"
#include "SUIT_Tools.h"
#include "SUIT_ViewManager.h"
#include "SUIT_ViewWindow.h"
#include "PyConsole_Console.h"

#include <QAction>
#include <QApplication>
#include <QPaintEvent>
#include <QCoreApplication>

namespace
{
  /*!
    \brief Get the currently active application.
    \internal
    \return active application object or 0 if there is no any
  */
  LightApp_Application* getApplication()
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
  LightApp_Study* getActiveStudy()
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
  LightApp_Module* getActiveModule()
  {
    LightApp_Module* module = 0;
    if ( LightApp_Application* anApp = getApplication() ) {
      module = PyModuleHelper::getInitModule();
      if ( !module )
        module = dynamic_cast<LightApp_Module*>( anApp->activeModule() );
    }
    return module;
  }
  
  /*!
    \brief Get the currently active Python module's helper.
    \internal
    This function returns correct result only if Python-based
    module is currently active. Otherwize, 0 is returned.
  */
  PyModuleHelper* getPythonHelper()
  {
    LightApp_Module* module = getActiveModule();
    PyModuleHelper* helper = module ? module->findChild<PyModuleHelper*>( "python_module_helper" ) : 0;
    return helper;
  }
  
  /*!
    \brief Get SALOME verbose level
    \internal
    \return \c true if SALOME debug output is allowed or \c false otherwise
  */
  bool verbose()
  {
    bool isVerbose = false;
    if ( getenv( "SALOME_VERBOSE" ) ) {
      QString envVar = getenv( "SALOME_VERBOSE" );
      bool ok;
      int value = envVar.toInt( &ok );
      isVerbose = ok && value != 0;
    }
    return isVerbose;
  }

  /*!
    \brief Get menu item title
    \internal
    \param menuId menu identifier
    \return menu title (localized)
  */
  QString getMenuName( const QString& menuId )
  {
    QStringList contexts;
    contexts << "SalomeApp_Application" << "LightApp_Application" << "STD_TabDesktop" <<
      "STD_MDIDesktop" << "STD_Application" << "SUIT_Application" << "";
    QString menuName = menuId;
    for ( int i = 0; i < contexts.count() && menuName == menuId; i++ )
      menuName = QApplication::translate( contexts[i].toLatin1().data(), menuId.toLatin1().data() );
    return menuName;
  }

  /*!
    \brief Load module icon
    \internal
    \param module module name
    \param fileName path to the icon file
    \return icon
  */
  QIcon loadIconInternal( const QString& module, const QString& fileName )
  {
    QIcon icon;
    
    LightApp_Application* app = getApplication();
    
    if ( app && !fileName.isEmpty() ) {
      QPixmap pixmap = app->resourceMgr()->loadPixmap( module, 
                                                       QApplication::translate( module.toLatin1().data(), 
                                                                                fileName.toLatin1().data() ) );
      if ( !pixmap.isNull() )
        icon = QIcon( pixmap );
    }
    return icon;
  }

  /*!
    \brief Gets window with specified identifier 
    \internal
    \param id window identifier 
    \return pointer on the window
  */
  SUIT_ViewWindow* getWnd( const int id )
  {
    SUIT_ViewWindow* resWnd = 0;
    
    LightApp_Application* app = getApplication();
    if ( app ) {
      ViewManagerList vmlist = app->viewManagers();
      foreach( SUIT_ViewManager* vm, vmlist ) {
        QVector<SUIT_ViewWindow*> vwlist = vm->getViews();
        foreach ( SUIT_ViewWindow* vw, vwlist ) {
          if ( id == vw->getId() ) {
            resWnd = vw;
            break;
          }
        }
      }
    }
    return resWnd;
  }

  /*!
    \brief Map of created selection objects.
    \internal
  */
  QMap<LightApp_Application*, SALOME_Selection*> SelMap;

  /*!
    \brief Default resource file section name.
    \internal
  */
  const char* DEFAULT_SECTION = "SalomePyQt";
}

/*!
  \class SALOME_Selection
  \brief The class represents selection which can be used in Python.
*/

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
  class TEvent: public SALOME_Event
  {
    LightApp_SelectionMgr* mySelMgr;
  public:
    TEvent( LightApp_SelectionMgr* selMgr ) 
      : mySelMgr( selMgr ) {}
    virtual void Execute() 
    {
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
  class TEvent: public SALOME_Event 
  {
    LightApp_SelectionMgr* mySelMgr;
  public:
    TEvent( LightApp_SelectionMgr* selMgr ) 
      : mySelMgr( selMgr ) {}
    virtual void Execute() 
    {
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
    if ( anApp && anApp->objectBrowser() ) {
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
  \fn SALOME_Selection* SalomePyQt::getSelection();
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
  \fn void SalomePyQt::putInfo( const QString& msg, const int sec );
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
  \fn PyObject* SalomePyQt::getActivePythonModule();
  \brief Access to Python module object currently loaded into SALOME_PYQT_ModuleLight container.
  \return Python module object currently loaded into SALOME_PYQT_ModuleLight container
*/

class TGetActivePyModuleEvent: public SALOME_Event
{
public:
  typedef PyObject* TResult;
  TResult myResult;
  TGetActivePyModuleEvent() : myResult( Py_None ) {}
  virtual void Execute() 
  {
    PyModuleHelper* helper = getPythonHelper();
    if ( helper )
      myResult = (PyObject*)helper->pythonModule();
  }
};
PyObject* SalomePyQt::getActivePythonModule()
{
  return ProcessEvent( new TGetActivePyModuleEvent() );
}

/*!
  \fn bool SalomePyQt::activateModule( const QString& modName );
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
  \note This function is supported for "light" Python-based SALOME modules only.
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
    LightApp_Module* module = getActiveModule();
    if ( !module )
      return;
    
    SALOME_PYQT_DataModelLight* aModel =
      dynamic_cast<SALOME_PYQT_DataModelLight*>( module->dataModel() );
    if ( aModel ) {
      myResult = aModel->isModified();
    }
    else {
      if ( verbose() ) printf( "SalomePyQt.isModified() function is not supported for the current module.\n" );
    }
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

  \note This function is supported for "light" Python-based SALOME modules only.

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
      LightApp_Module* module = getActiveModule();
      if ( !module )
	return;

      SALOME_PYQT_DataModelLight* model =
	dynamic_cast<SALOME_PYQT_DataModelLight*>( module->dataModel() );

      LightApp_Application* app = module->getApp();

      if ( model && app ) {
	model->setModified( myFlag );
	app->updateActions();
      }
      else {
	if ( verbose() ) printf( "SalomePyQt.setModified() function is not supported for the current module.\n" );
      }
    }
  };
  ProcessVoidEvent( new TEvent( flag ) );
}

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
  class TEvent: public SALOME_Event
  {
    QString myName;
  public:
    TEvent( const QString& name ) : myName( name ) {}
    virtual void Execute()
    {
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
  \fn QString SalomePyQt::constant( const QString& name );
  \brief Get constant's value from application's resource manager.

  \param name name of the constant 
  \return value of the constant

  \sa setConstant()
*/

class TGetConstantEvent: public SALOME_Event 
{
public:
  typedef QString TResult;
  TResult myResult;
  QString myName;
  TGetConstantEvent( const QString& name ) : myName( name ) {}
  virtual void Execute() 
  {
    if ( SUIT_Session::session() )
      myResult = SUIT_Session::session()->resourceMgr()->constant( myName );
  }
};
QString SalomePyQt::constant( const QString& name )
{
  return ProcessEvent( new TGetConstantEvent( name ) );
}

/*!
  \brief Add constant to the application's resource manager.

  This function is useful to specify programmatically specific
  variables that are referenced in the resource setting.

  For example, some resource value can be set as "$(myroot)/data/files".
  Then, "mypath" constant can be set programmatically by the application
  depending on run-time requirements.
  
  \param section resources file section name 
  \param name name of the constant 
  \param value value of the constant 

  \sa constant()
*/
void SalomePyQt::setConstant( const QString& name, const QString& value )
{
  class TEvent: public SALOME_Event 
  {
    QString myName, myValue;
  public:
    TEvent( const QString& name, const QString& value ) 
      : myName( name ), myValue( value ) {}
    virtual void Execute() 
    {
      if ( SUIT_Session::session() )
        SUIT_Session::session()->resourceMgr()->setConstant( myName, myValue );
    }
  };
  ProcessVoidEvent( new TEvent( name, value ) );
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
  \brief Add byte array setting to the application preferences.
  \param section resources file section name 
  \param name setting name
  \param value new setting value
*/
void SalomePyQt::addSetting( const QString& section, const QString& name, const QByteArray& value )
{
  class TEvent: public SALOME_Event 
  {
    QString    mySection;
    QString    myName;
    QByteArray myValue;
  public:
    TEvent( const QString& section, const QString& name, const QByteArray& value ) 
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
                                         const QString& def, 
                                         const bool subst );
  \brief Get string setting from the application preferences.
  \param section resources file section name 
  \param name setting name
  \param def default value which is returned if the setting is not found
  \param subst \c true to make substitution, \c false to get "raw" value
  \return setting value
*/

class TGetStrSettingEvent: public SALOME_Event
{
public:
  typedef QString TResult;
  TResult myResult;
  QString mySection;
  QString myName;
  bool mySubst;
  TResult myDefault;
  TGetStrSettingEvent( const QString& section, const QString& name, const QString& def, const bool subst ) 
    : mySection( section ), myName( name ), myDefault( def ), mySubst( subst ) {}
  virtual void Execute() 
  {
    if ( SUIT_Session::session() ) {
      SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
      myResult = ( !mySection.isEmpty() && !myName.isEmpty() ) ? resMgr->stringValue( mySection, myName, myDefault, mySubst ) : myDefault;
    }
  }
};
QString SalomePyQt::stringSetting( const QString& section, const QString& name, const QString& def, const bool subst )
{
  return ProcessEvent( new TGetStrSettingEvent( section, name, def, subst ) );
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
  \fn QByteArray SalomePyQt::byteArraySetting( const QString& section, 
                                               const QString& name, 
                                               const QByteArray def );
  \brief Get byte array setting from the application preferences.
  \param section resources file section name 
  \param name setting name
  \param def default value which is returned if the setting is not found
  \return setting value
*/

class TGetByteArraySettingEvent: public SALOME_Event 
{
public:
  typedef QByteArray TResult;
  TResult myResult;
  QString mySection;
  QString myName;
  TResult myDefault;
  TGetByteArraySettingEvent( const QString& section, const QString& name, const QByteArray& def ) 
    : mySection( section ), myName( name ), myDefault( def ) {}
  virtual void Execute() 
  {
    if ( SUIT_Session::session() ) {
      SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
      myResult = ( !mySection.isEmpty() && !myName.isEmpty() ) ? resMgr->byteArrayValue( mySection, myName, myDefault ) : myDefault;
    }
  }
};
QByteArray SalomePyQt::byteArraySetting ( const QString& section, const QString& name, const QByteArray& def )
{
  return ProcessEvent( new TGetByteArraySettingEvent( section, name, def ) );
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

class THasSettingEvent: public SALOME_Event 
{
public:
  typedef bool TResult;
  TResult myResult;
  QString mySection;
  QString myName;
  THasSettingEvent( const QString& section, const QString& name ) 
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
  return ProcessEvent( new THasSettingEvent( section, name ) );
}

/*!
  \fn QStringList SalomePyQt::parameters( const QString& section );
  \brief Get names of preference items stored within the given section.
  \param section resources file section's name 
  \return \c list of preferences items
*/

/*!
  \fn QStringList SalomePyQt::parameters( const QStringList& section );
  \brief Get names of preference items stored within the given section.
  \param section resources file section's name 
  \return \c list of preferences items
*/

class TParametersEvent: public SALOME_Event 
{
public:
  typedef QStringList TResult;
  TResult myResult;
  QStringList mySection;
  TParametersEvent( const QString& section ) 
  {
    mySection << section;
  }
  TParametersEvent( const QStringList& section ) 
    : mySection( section )
  {}
  virtual void Execute() 
  {
    if ( SUIT_Session::session() ) {
      SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
      myResult = resMgr->parameters( mySection );
    }
  }
};
QStringList SalomePyQt::parameters( const QString& section )
{
  return ProcessEvent( new TParametersEvent( section ) );
}
QStringList SalomePyQt::parameters( const QStringList& section )
{
  return ProcessEvent( new TParametersEvent( section ) );
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
  \param fileName icon file name
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
    myResult = loadIconInternal( myModule, myFileName );
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
    myResult = PyModuleHelper::defaultMenuGroup();
  }
};
int SalomePyQt::defaultMenuGroup()
{
  return ProcessEvent( new TDefMenuGroupEvent() );
}

class CrTool
{
public:
  CrTool( const QString& tBar, const QString& nBar ) 
    : myCase( 0 ), myTbTitle( tBar ), myTbName( nBar)  {}
  CrTool( const int id, const int tBar, const int idx ) 
    : myCase( 1 ), myId( id ), myTbId( tBar ), myIndex( idx ) {}
  CrTool( const int id, const QString& tBar, const int idx )
    : myCase( 2 ), myId( id ), myTbTitle( tBar ), myIndex( idx ) {}
  CrTool( QAction* action, const int tbId, const int id, const int idx )
    : myCase( 3 ), myAction( action ), myTbId( tbId ), myId( id ), myIndex( idx ) {}
  CrTool( QAction* action, const QString& tBar, const int id, const int idx )
    : myCase( 4 ), myAction( action ), myTbTitle( tBar ), myId( id ), myIndex( idx ) {}

  int execute( LightApp_Module* module ) const
  {
    if ( module ) {
      switch ( myCase ) {
      case 0:
        return module->createTool( myTbTitle, myTbName );
      case 1:
        return module->createTool( myId, myTbId, myIndex );
      case 2:
        return module->createTool( myId, myTbTitle, myIndex );
      case 3:
        return module->createTool( myAction, myTbId, myId, myIndex );
      case 4:
        return module->createTool( myAction, myTbTitle, myId, myIndex );
      }
    }
    return -1;
  }
private:
   int        myCase;
   QString    myTbTitle;
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
    LightApp_Module* module = getActiveModule();
    if ( module )
      myResult = myCrTool.execute( module );
  }
};

/*!
  \brief Create toolbar with specified name.
  \param tBar toolbar title (language-dependent)
  \param nBar toolbar name (language-independent) [optional]
  \return toolbar ID or -1 if toolbar creation is failed
*/
int SalomePyQt::createTool( const QString& tBar, const QString& nBar )
{
  return ProcessEvent( new TCreateToolEvent( CrTool( tBar, nBar ) ) );
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
    LightApp_Module* module = getActiveModule();
    if ( module )
      myResult = myCrMenu.execute( module );
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
    LightApp_Module* module = getActiveModule();
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
                                         const bool     toggle );
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
    LightApp_Module* module = getActiveModule();
    if ( module ) {
      QIcon icon = loadIconInternal( module->name(), myIcon );
      myResult = (QAction*)module->action( myId );
      if ( myResult ) {
	if ( myResult->toolTip().isEmpty() && !myTipText.isEmpty() ) 
	  myResult->setToolTip( myTipText );
	if ( myResult->text().isEmpty() && !myMenuText.isEmpty() )
	  myResult->setText( myMenuText );
	if ( myResult->icon().isNull() && !icon.isNull() ) 
	  myResult->setIcon( icon );
	if ( myResult->statusTip().isEmpty() && !myStatusText.isEmpty() )
	  myResult->setStatusTip( myStatusText );
	if ( myResult->shortcut().isEmpty() && myKey )
	  myResult->setShortcut( myKey );
	if ( myResult->isCheckable() != myToggle )
	  myResult->setCheckable( myToggle );
      }
      else {
	myResult = (QAction*)module->createAction( myId, myTipText, icon, myMenuText, myStatusText, myKey, module, myToggle );
      }
      // for Python module, automatically connect action to callback slot
      PyModuleHelper* helper = module->findChild<PyModuleHelper*>( "python_module_helper" );
      if ( helper ) helper->connectAction( myResult );
    }
  }
};
QAction* SalomePyQt::createAction( const int id,           const QString& menuText, 
				   const QString& tipText, const QString& statusText, 
				   const QString& icon,    const int key, const bool toggle )
{
  return ProcessEvent( new TCreateActionEvent( id, menuText, tipText, statusText, icon, key, toggle ) );
}

/*!
  \fn QtxActionGroup* SalomePyQt::createActionGroup( const int id, const bool exclusive );
  \brief Create an action group which can be then used in the menu or toolbar
  \param id         : the unique id action group to be registered to
  \param exclusive  : if \c true the action group does exclusive toggling
*/

struct TCreateActionGroupEvent: public SALOME_Event 
{
  typedef QtxActionGroup* TResult;
  TResult myResult;
  int     myId;
  bool    myExclusive;
  TCreateActionGroupEvent( const int id, const bool exclusive )
    : myId( id ), myExclusive( exclusive ) {}
  virtual void Execute()
  {
    LightApp_Module* module = getActiveModule();
    if ( module )
      myResult = module->createActionGroup( myId, myExclusive );
  }
};
QtxActionGroup* SalomePyQt::createActionGroup( const int id, const bool exclusive )
{
  return ProcessEvent( new TCreateActionGroupEvent( id, exclusive ) );
}

/*!
  \fn QAction* SalomePyQt::action( const int id );
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
    LightApp_Module* module = getActiveModule();
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
    LightApp_Module* module = getActiveModule();
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
    LightApp_Module* module = getActiveModule();
    if ( module ) {
      LightApp_Preferences* pref = module->getApp()->preferences();
      if ( pref )
	myResult = pref->addPreference( myLabel, -1 );
    }
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
    LightApp_Module* module = getActiveModule();
    if ( module ) {
      LightApp_Preferences* pref = module->getApp()->preferences();
      if ( pref ) {
	int cId = pref->addPreference( module->moduleName(), -1 );
	if ( cId != -1 )
	  myResult = pref->addPreference( myLabel, cId );
      }
    }
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
    LightApp_Module* module = getActiveModule();
    if ( module ) {
      LightApp_Preferences* pref = module->getApp()->preferences();
      if ( pref )
	myResult = pref->addPreference( module->moduleName(), myLabel, myPId, myType, mySection, myParam );
    }
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
    LightApp_Module* module = getActiveModule();
    if ( module ) {
      LightApp_Preferences* pref = module->getApp()->preferences();
      if ( pref )
	myResult = pref->itemProperty( myProp, myId );
    }
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
      LightApp_Module* module = getActiveModule();
      if ( module ) {
	LightApp_Preferences* pref = module->getApp()->preferences();
	if ( pref )
	  pref->setItemProperty( myProp, myVar, myId );
      }
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
      LightApp_Module* module = getActiveModule();
      if ( module ) {
	LightApp_Preferences* pref = module->getApp()->preferences();
	if ( pref ) {
	  QVariant var =  pref->itemProperty( myProp, myId );
	  if ( var.isValid() ) {
	    if ( var.type() == QVariant::StringList ) {
	      QStringList sl = var.toStringList();
	      if ( myIdx >= 0 && myIdx < sl.count() ) 
		sl[myIdx] = myVar.toString();
	      else
		sl.append( myVar.toString() );
	      pref->setItemProperty( myProp, sl, myId );
	    }
	    else if ( var.type() == QVariant::List ) {
	      QList<QVariant> vl = var.toList();
	      if ( myIdx >= 0 && myIdx < vl.count() ) 
		vl[myIdx] = myVar;
	      else
		vl.append( myVar );
	      pref->setItemProperty( myProp, vl, myId );
	    }
	  }
	  else {
	    QList<QVariant> vl;
	    vl.append( myVar );
	    pref->setItemProperty( myProp, vl, myId );
	  }
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
  \fn bool SalomePyQt::dumpView( const QString& filename, const int id = 0 );
  \brief Dump the contents of the id view window. If id is 0 then current active view is processed. 
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
  int myWndId;
  TDumpViewEvent( const QString& filename, const int id ) 
    : myResult ( false ), myFileName( filename ), myWndId( id ) {}
  virtual void Execute() 
  {
    SUIT_ViewWindow* wnd = 0;
    if ( !myWndId ) {
      if ( LightApp_Application* anApp = getApplication() ) {
        SUIT_ViewManager* vm = anApp->activeViewManager();
        if ( vm )
          wnd = vm->getActiveView();
      }
      myWndId = wnd->getId();
    }
    else {
      wnd = dynamic_cast<SUIT_ViewWindow*>( getWnd( myWndId ) );
    }
    if ( wnd ) {
      QString fmt = SUIT_Tools::extension( myFileName ).toUpper();
#ifndef DISABLE_PLOT2DVIEWER
      Plot2d_ViewWindow* wnd2D = dynamic_cast<Plot2d_ViewWindow*>( wnd );
      if ( wnd2D ) {
        qApp->postEvent( wnd2D->getViewFrame(), new QPaintEvent( QRect( 0, 0, wnd2D->getViewFrame()->width(), wnd2D->getViewFrame()->height() ) ) );
        qApp->postEvent( wnd2D, new QPaintEvent( QRect( 0, 0, wnd2D->width(), wnd2D->height() ) ) );
        qApp->processEvents();
        if ( fmt == "PS" || fmt == "EPS" || fmt == "PDF" ) {
    	  myResult = wnd2D->getViewFrame()->print( myFileName, fmt );
          return;
        }
      }
#endif // DISABLE_PLOT2DVIEWER
      QImage im = wnd->dumpView();
      if ( !im.isNull() && !myFileName.isEmpty() ) {
        if ( fmt.isEmpty() ) fmt = QString( "BMP" ); // default format
        if ( fmt == "JPG" )  fmt = "JPEG";
        myResult = im.save( myFileName, fmt.toLatin1() );
      }
    }
  }
};
bool SalomePyQt::dumpView( const QString& filename, const int id )
{
  return ProcessEvent( new TDumpViewEvent( filename, id ) );
}

/*!
  \fn QList<int> SalomePyQt::getViews();
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
    if ( app ) {
      STD_TabDesktop* tabDesk = dynamic_cast<STD_TabDesktop*>( app->desktop() );
      if ( tabDesk ) {
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
  \fn int SalomePyQt::getActiveView();
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
    if ( app ) {
      SUIT_ViewManager* viewMgr = app->activeViewManager();
      if ( viewMgr ) {
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
  \fn QString SalomePyQt::getViewType( const int id );
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
    if ( wnd ) {
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
  \fn bool SalomePyQt::setViewTitle( const int id, const QString& title );
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
    if ( wnd ) {
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
  \fn bool SalomePyQt::setViewSize( const int w, const int h, const int id );
  \brief Set view size
  \param w window width
  \param h window height
  \param id window identifier
  \return \c true if operation is completed successfully and \c false otherwise 
*/

class TSetViewSize: public SALOME_Event
{
public:
  typedef bool TResult;
  TResult myResult;
  int myWndWidth;
  int myWndHeight;
  int myWndId;
  TSetViewSize( const int w, const int h, const int id )
    : myResult( false ),
      myWndWidth( w ),
      myWndHeight( h ),
      myWndId( id ) {}
  virtual void Execute() 
  {
    SUIT_ViewWindow* wnd = 0;
    if ( !myWndId ) {
      if ( LightApp_Application* anApp = getApplication() ) {
        SUIT_ViewManager* vm = anApp->activeViewManager();
        if ( vm )
          wnd = vm->getActiveView();
      }
    }
    else {
      wnd = dynamic_cast<SUIT_ViewWindow*>( getWnd( myWndId ) );
    }
    if ( wnd ) {
      SUIT_ViewManager* viewMgr = wnd->getViewManager();
      if ( viewMgr ) {
        QString type = viewMgr->getType();
        if ( type == "OCCViewer") {
#ifndef DISABLE_OCCVIEWER
          // specific processing for OCC viewer:
          // OCC view can embed up to 4 sub-views, split according to the specified layout;
          // - if there is only one sub-view active; it will be resized;
          // - if there are several sub-views, each of them will be resized.
          OCCViewer_ViewWindow* occView = qobject_cast<OCCViewer_ViewWindow*>( wnd );
          for ( int i = OCCViewer_ViewFrame::BOTTOM_RIGHT; i <= OCCViewer_ViewFrame::TOP_RIGHT; i++ ) {
            if ( occView && occView->getView( i ) ) {
              occView->getView( i )->centralWidget()->resize( myWndWidth, myWndHeight );
              myResult = true;
            }
          }
#endif // DISABLE_OCCVIEWER
        }
        else if ( type == "ParaView") {
#ifndef DISABLE_PVVIEWER
          // specific processing for ParaView viewer:
          // hierarchy of ParaView viewer is much complex than for usual view;
          // we look for sub-widget named "Viewport"
          QList<QWidget*> lst = wnd->findChildren<QWidget*>( "Viewport" );
          if ( !lst.isEmpty() ) {
            lst[0]->resize( myWndWidth, myWndHeight );
            myResult = true;
          }
#endif // DISABLE_PVVIEWER
        }
        else {
          if ( wnd->centralWidget() ) {
            wnd->centralWidget()->resize( myWndWidth, myWndHeight );
            myResult = true;
          }
        }
      }
    }
  }
};
bool SalomePyQt::setViewSize( const int w, const int h, const int id )
{
  return ProcessEvent( new TSetViewSize( w, h, id ) );
}

/*!
  \fn QString SalomePyQt::getViewTitle( const int id );
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
  \fn QList<int> SalomePyQt::findViews( const QString& type );
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
    if ( app ) {
      ViewManagerList vmList;
      app->viewManagers( myType, vmList );
      SUIT_ViewManager* viewMgr;
      foreach ( viewMgr, vmList ) {
        QVector<SUIT_ViewWindow*> vec = viewMgr->getViews();
        for ( int i = 0, n = vec.size(); i < n; i++ ) {
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
  \fn bool SalomePyQt::activateView( const int id );
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
    if ( wnd ) {
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
  \fn int SalomePyQt::createView( const QString& type, bool visible = true, const int width = 0, const int height = 0 );
  \brief Create new view and activate it
  \param type viewer type
  \param visible
  \param width
  \param height
  \return integer identifier of created view (or -1 if view could not be created)
*/

class TCreateView: public SALOME_Event
{
public:
  typedef int TResult;
  TResult myResult;
  QString myType;
  bool myVisible;
  int myWidth;
  int myHeight;
  TCreateView( const QString& theType, bool visible, const int width, const int height )
    : myResult( -1 ),
      myType( theType ),
      myVisible(visible),
      myWidth(width),
      myHeight(height) {}
  virtual void Execute() 
  {
    LightApp_Application* app  = getApplication();
    if ( app ) {
      SUIT_ViewManager* viewMgr = app->createViewManager( myType );
      if ( viewMgr ) {
        QWidget* wnd = viewMgr->getActiveView();
        myResult = viewMgr->getActiveView()->getId();
        if ( wnd ) {
          if ( !myVisible )
            wnd->setVisible(false);
          if ( !myVisible && myWidth == 0 && myHeight == 0 ) {
            myWidth = 1024;
            myHeight = 768;
          }
          if (myWidth > 0 && myHeight > 0) {
#ifndef DISABLE_PLOT2DVIEWER
            Plot2d_ViewWindow* wnd2D = dynamic_cast<Plot2d_ViewWindow*>( wnd );
            if ( wnd2D ) wnd = wnd2D->getViewFrame();
#endif // DISABLE_PLOT2DVIEWER
            wnd->setGeometry( 0, 0, myWidth, myHeight );
          }
        }
      }
    }
  }
};
int SalomePyQt::createView( const QString& type, bool visible, const int width, const int height )
{
  int ret = ProcessEvent( new TCreateView( type, visible, width, height ) );
  QCoreApplication::processEvents();
  return ret;
}

/*!
  \fn int SalomePyQt::createView( const QString& type, QWidget* w );
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
    if ( app ) {
      SUIT_ViewManager* viewMgr = app->createViewManager( myType, myWidget );
      if ( viewMgr ) {
        SUIT_ViewWindow* wnd = viewMgr->getActiveView();
        if ( wnd )
          myResult = wnd->getId();
      }
    }
  }
};
int SalomePyQt::createView( const QString& type, QWidget* w )
{
  int ret = ProcessEvent( new TCreateViewWg( type, w ) );
  QCoreApplication::processEvents();
  return ret;
}

/*!
  \fn bool SalomePyQt::closeView( const int id );
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
    if ( wnd ) {
      SUIT_ViewManager* viewMgr = wnd->getViewManager();
      if ( viewMgr ) {
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
  \fn int SalomePyQt::cloneView( const int id );
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
    if ( wnd ) {
      SUIT_ViewManager* viewMgr = wnd->getViewManager();
      if ( viewMgr ) {
#ifndef DISABLE_OCCVIEWER
        if ( wnd->inherits( "OCCViewer_ViewWindow" ) ) {
          OCCViewer_ViewWindow* occView = (OCCViewer_ViewWindow*)( wnd );
          occView->onCloneView();
          wnd = viewMgr->getActiveView();
          if ( wnd )
            myResult = wnd->getId();
        }
#endif // DISABLE_OCCVIEWER
#ifndef DISABLE_PLOT2DVIEWER
        if ( wnd->inherits( "Plot2d_ViewWindow" ) ) {
          Plot2d_ViewManager* viewMgr2d = dynamic_cast<Plot2d_ViewManager*>( viewMgr );
          Plot2d_ViewWindow* srcWnd2d = dynamic_cast<Plot2d_ViewWindow*>( wnd );
          if ( viewMgr2d && srcWnd2d ) {
            Plot2d_ViewWindow* resWnd = viewMgr2d->cloneView( srcWnd2d );
            myResult = resWnd->getId();
          }
        }
#endif // DISABLE_OCCVIEWER
      }
    }
  }
};
int SalomePyQt::cloneView( const int id )
{
  return ProcessEvent( new TCloneView( id ) );
}

/*!
  \fn bool SalomePyQt::setViewVisible( const int id, const bool visible )
  \brief Set view visibility.
  \param id window identifier
  \param visible new visiblity
*/

void SalomePyQt::setViewVisible( const int id, const bool visible )
{
  class TEvent: public SALOME_Event
  {
    int myWndId;
    bool myVisible;
  public:
    TEvent( const int id, const bool visible )
      : myWndId( id ), myVisible( visible ) {}
    virtual void Execute()
    {
      SUIT_ViewWindow* wnd = getWnd( myWndId );
      if ( wnd ) wnd->setVisible( myVisible );
    }
  };
  ProcessVoidEvent( new TEvent( id, visible ) );
}

/*!
  \fn bool SalomePyQt::isViewVisible( const int id );
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
  \fn bool SalomePyQt::setViewClosable( const int id, const bool on );
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
  \fn bool SalomePyQt::isViewClosable( const int id );
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
  \fn bool SalomePyQt::groupAllViews();
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
    if ( app ) {
      STD_TabDesktop* tabDesk = dynamic_cast<STD_TabDesktop*>( app->desktop() );
      if ( tabDesk ) {
        QtxWorkstack* wStack = tabDesk->workstack();
        if ( wStack ) {
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
  \fn bool SalomePyQt::splitView( const int id, const Orientation ori, const Action action );
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
    if ( wnd ) {
      // activate view
      // wnd->setFocus(); ???

      // split workstack
      if ( getApplication() ) {
        STD_TabDesktop* desk = 
          dynamic_cast<STD_TabDesktop*>( getApplication()->desktop() );
        if ( desk ) {
          QtxWorkstack* wStack = desk->workstack();
          if ( wStack ) {
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
  \fn bool SalomePyQt::moveView( const int id, const int id_to, const bool before );
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
    if ( wnd && wnd_to ) {
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
  \fn QList<int> SalomePyQt::neighbourViews( const int id );
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
    if ( wnd ) {
      QtxWorkstack* wStack = dynamic_cast<STD_TabDesktop*>( 
        getApplication()->desktop() )->workstack();
      if ( wStack ) {
        QWidgetList wgList = wStack->windowList( wnd );
        QWidget* wg;
        foreach ( wg, wgList ) {
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
  \fn void SalomePyQt::createRoot();
  \brief Initialize root data object.

  Does nothing if root is already initialized.
*/

void SalomePyQt::createRoot()
{
  class TEvent: public SALOME_Event
  {
  public:
    TEvent() {}
    virtual void Execute() 
    {
      SALOME_PYQT_ModuleLight* module = dynamic_cast<SALOME_PYQT_ModuleLight*>( getActiveModule() );
      if ( module ) {
        SALOME_PYQT_DataModelLight* dm =
          dynamic_cast<SALOME_PYQT_DataModelLight*>( module->dataModel() );
        if ( dm )
          dm->getRoot();
      }
      else {
        if ( verbose() ) printf( "SalomePyQt.createRoot() function is not supported for the current module.\n" );
      }
    }
  };
  ProcessVoidEvent( new TEvent() );
}

/*!
  \fn QString SalomePyQt::createObject( const QString& parent );
  \brief Create empty data object
  \param parent entry of parent data object
  \return entry of created data object
*/

class TCreateEmptyObjectEvent: public SALOME_Event
{
public:
  typedef QString TResult;
  TResult  myResult;
  QString  myParent;
  TCreateEmptyObjectEvent( const QString& parent )
    : myParent( parent ) {}
  virtual void Execute() 
  {
    SALOME_PYQT_ModuleLight* module = dynamic_cast<SALOME_PYQT_ModuleLight*>( getActiveModule() );
    if ( module ) {
       myResult = module->createObject( myParent );
    }
    else {
      if ( verbose() ) printf( "SalomePyQt.createObject() function is not supported for the current module.\n" );
    }
  }
};
QString SalomePyQt::createObject( const QString& parent )
{
  return ProcessEvent( new TCreateEmptyObjectEvent( parent ) );
}

/*!
  \fn QString SalomePyQt::createObject( const QString& name, const QString& icon,
                                        const QString& tooltip,const QString& parent );
  \brief Create new data object with specified name, icon and tooltip
  \param name data object name
  \param icon data object icon
  \param toolTip data object tooltip
  \param parent entry of parent data object
  \return entry of created data object
*/

class TCreateObjectEvent: public SALOME_Event 
{
public:
  typedef QString TResult;
  TResult myResult;
  QString myParent;
  QString myName;
  QString myIcon;
  QString myToolTip;
  TCreateObjectEvent( const QString& name,
		      const QString& icon,
		      const QString& tooltip,
		      const QString& parent )
    : myName( name ),
      myIcon( icon ),
      myToolTip( tooltip ),
      myParent( parent ) {}
  virtual void Execute()
  {
    SALOME_PYQT_ModuleLight* module = dynamic_cast<SALOME_PYQT_ModuleLight*>( getActiveModule() );
    if ( module ) {
      myResult = module->createObject( myName, myIcon, myToolTip, myParent );
    }
    else {
      if ( verbose() ) printf( "SalomePyQt.createObject() function is not supported for the current module.\n" );
    }
  }
};
QString SalomePyQt::createObject( const QString& name,
				  const QString& icon,
				  const QString& toolTip,
				  const QString& parent )
{
  return ProcessEvent( new TCreateObjectEvent( name, icon, toolTip, parent ) );
}


/*!
  \fn void SalomePyQt::setName( const QString& entry, const QString& name );
  \brief Set data object name
  \param entry data object entry
  \param name data object name
*/
class TSetNameEvent: public SALOME_Event
{
public:
  QString myEntry;
  QString myName;
  TSetNameEvent( const QString& entry,
                 const QString& name )
  : myEntry( entry ),
    myName( name ) {}
  virtual void Execute()
  {
    SALOME_PYQT_ModuleLight* module = dynamic_cast<SALOME_PYQT_ModuleLight*>( getActiveModule() );
    if ( module ) {
      module->setName( myEntry, myName );
    }
    else {
      if ( verbose() ) printf( "SalomePyQt.setName() function is not supported for the current module.\n" );
    }
  }
};
void SalomePyQt::setName( const QString& entry, const QString& name )
{
  ProcessVoidEvent( new TSetNameEvent( entry, name ) );
}

/*!
  \fn void SalomePyQt::setIcon( const QString& entry, const QString& icon );
  \brief Set data object icon
  \param entry data object entry
  \param icon data object icon file name (icon is loaded from module resources)
*/

class TSetIconEvent: public SALOME_Event
{
public:
  QString myEntry;
  QString myIcon;
  TSetIconEvent( const QString& entry,
                 const QString& icon )
  : myEntry( entry ),
    myIcon( icon ) {}
  virtual void Execute()
  {
    SALOME_PYQT_ModuleLight* module = dynamic_cast<SALOME_PYQT_ModuleLight*>( getActiveModule() );
    if ( module ) {
      module->setIcon( myEntry, myIcon );
    }
    else {
      if ( verbose() ) printf( "SalomePyQt.setIcon() function is not supported for the current module.\n" );
    }
  }
};

void SalomePyQt::setIcon( const QString& entry, const QString& icon )
{
  ProcessVoidEvent( new TSetIconEvent( entry, icon ) );
}

/*!
  \fn void SalomePyQt::setToolTip( const QString& entry, const QString& toolTip );
  \brief Set data object tooltip
  \param entry data object entry
  \param toolTip data object tooltip
*/

class TSetToolTipEvent: public SALOME_Event
{
public:
  QString myEntry;
  QString myToolTip;
  TSetToolTipEvent( const QString& entry,
                    const QString& toolTip )
    : myEntry( entry ),
      myToolTip( toolTip ) {}
  virtual void Execute()
  {
    SALOME_PYQT_ModuleLight* module = dynamic_cast<SALOME_PYQT_ModuleLight*>( getActiveModule() );
    if ( module ) {
      module->setToolTip( myEntry, myToolTip );
    }
    else {
      if ( verbose() ) printf( "SalomePyQt.setToolTip() function is not supported for the current module.\n" );
    }
  }
};
void SalomePyQt::setToolTip( const QString& entry, const QString& toolTip )
{
  ProcessVoidEvent( new TSetToolTipEvent( entry, toolTip ) );
}

/*!
  \fn void SalomePyQt::setReference( const QString& entry, const QString& refEntry );
  \brief Set reference to another data object
  \param entry data object entry
  \param refEntry referenced data object entry
*/

class TSetRefEvent: public SALOME_Event
{
public:
  QString myEntry;
  QString myRefEntry;
  TSetRefEvent( const QString& entry,
		const QString& refEntry )
    : myEntry( entry ),
      myRefEntry( refEntry ) {}
  virtual void Execute()
  {
    SALOME_PYQT_ModuleLight* module = dynamic_cast<SALOME_PYQT_ModuleLight*>( getActiveModule() );
    if ( module ) {
      module->setReference( myEntry, myRefEntry );
    }
    else {
      if ( verbose() ) printf( "SalomePyQt.setReference() function is not supported for the current module.\n" );
    }
  }
};
void SalomePyQt::setReference( const QString& entry, const QString& refEntry )
{
  ProcessVoidEvent( new TSetRefEvent( entry, refEntry ) );
}

/*!
  \fn void SalomePyQt::setColor( const QString& entry, const QColor& color );
  \brief Set data object color
  \param entry data object entry
  \param color data object color
 */

class TSetColorEvent: public SALOME_Event
{
public:
  QString myEntry;
  QColor  myColor;
  TSetColorEvent( const QString& entry,
		  const QColor& color )
    : myEntry( entry ),
      myColor( color ) {}
  virtual void Execute()
  {
    SALOME_PYQT_ModuleLight* module = dynamic_cast<SALOME_PYQT_ModuleLight*>( getActiveModule() );
    if ( module ) {
      module->setColor( myEntry, myColor );
    }
    else {
      if ( verbose() ) printf( "SalomePyQt.setColor() function is not supported for the current module.\n" );
    }
  }
};
void SalomePyQt::setColor( const QString& entry, const QColor& color )
{
  ProcessVoidEvent( new TSetColorEvent( entry, color ) );
}

/*!
  \fn QString SalomePyQt::getName( const QString& entry );
  \brief Get data object name
  \param entry data object entry
  \return data object name
*/

class TGetNameEvent: public SALOME_Event
{
public:
  typedef QString TResult;
  TResult myResult;
  QString myEntry;
  TGetNameEvent( const QString& entry )
    : myEntry( entry ) {}
  virtual void Execute()
  {
    SALOME_PYQT_ModuleLight* module = dynamic_cast<SALOME_PYQT_ModuleLight*>( getActiveModule() );
    if ( module ) {
      myResult = module->getName( myEntry );
    }
    else {
      if ( verbose() ) printf( "SalomePyQt.getName() function is not supported for the current module.\n" );
    }
  }
};
QString SalomePyQt::getName( const QString& entry )
{
  return ProcessEvent( new TGetNameEvent( entry ) );
}

/*!
  \fn QString SalomePyQt::getToolTip( const QString& entry );
  \brief Get data object tooltip
  \param entry data object entry
  \return data object tooltip
*/

class TGetToolTipEvent: public SALOME_Event
{
public:
  typedef QString TResult;
  TResult myResult;
  QString myEntry;
  TGetToolTipEvent( const QString& entry )
  : myEntry( entry ) {}
  virtual void Execute()
  {
    SALOME_PYQT_ModuleLight* module = dynamic_cast<SALOME_PYQT_ModuleLight*>( getActiveModule() );
    if ( module ) {
      myResult = module->getToolTip( myEntry );
    }
    else {
      if ( verbose() ) printf( "SalomePyQt.getToolTip() function is not supported for the current module.\n" );
    }
  }
};
QString SalomePyQt::getToolTip( const QString& entry )
{
  return ProcessEvent( new TGetToolTipEvent( entry ) );
}

/*
  \fn QString SalomePyQt::getReference( const QString& entry );
  \brief Get entry of the referenced object (if there's any)
  \param entry data object entry
  \return referenced data object entry
*/

class TGetRefEvent: public SALOME_Event
{
public:
  typedef QString TResult;
  TResult myResult;
  QString myEntry;
  TGetRefEvent( const QString& entry )
  : myEntry( entry ) {}
  virtual void Execute()
  {
    SALOME_PYQT_ModuleLight* module = dynamic_cast<SALOME_PYQT_ModuleLight*>( getActiveModule() );
    if ( module ) {
      myResult = module->getReference( myEntry );
    }
    else {
      if ( verbose() ) printf( "SalomePyQt.getReference() function is not supported for the current module.\n" );
    }
  }
};
QString SalomePyQt::getReference( const QString& entry )
{
  return ProcessEvent( new TGetRefEvent( entry ) );
}

/*!
  \fn QColor SalomePyQt::getColor( const QString& entry );
  \brief Get data object color
  \param entry data object entry
  \return data object color
*/

class TGetColorEvent: public SALOME_Event
{
public:
  typedef QColor TResult;
  TResult myResult;
  QString myEntry;
  TGetColorEvent( const QString& entry )
  : myEntry( entry ) {}
  virtual void Execute()
  {
    SALOME_PYQT_ModuleLight* module = dynamic_cast<SALOME_PYQT_ModuleLight*>( getActiveModule() );
    if ( module ) {
      myResult = module->getColor( myEntry );
    }
    else {
      if ( verbose() ) printf( "SalomePyQt.getColor() function is not supported for the current module.\n" );
    }
  }
};
QColor SalomePyQt::getColor( const QString& entry )
{
  return ProcessEvent( new TGetColorEvent( entry ) );
}

/*!
  \fn void SalomePyQt::removeChildren( const QString& entry );
  \brief Remove all child data objects from specified data object
  \param entry data object entry
*/

class TRemoveChildEvent: public SALOME_Event
{
public:
  QString myEntry;
  TRemoveChildEvent( const QString& entry )
  : myEntry( entry ) {}
  virtual void Execute()
  {
    SALOME_PYQT_ModuleLight* module = dynamic_cast<SALOME_PYQT_ModuleLight*>( getActiveModule() );
    if ( module ) {
      module->removeChildren( myEntry );
    }
    else {
      if ( verbose() ) printf( "SalomePyQt.removeChildren() function is not supported for the current module.\n" );
    }
  }
};
void SalomePyQt::removeChildren( const QString& entry )
{
  ProcessVoidEvent( new TRemoveChildEvent( entry ) );
}
void SalomePyQt::removeChild( const QString& entry )
{
  if ( verbose() ) printf( "SalomePyQt.removeChild() function is obsolete. Use SalomePyQt.removeChildren() instead." );
  removeChildren( entry );
}

/*!
  \fn void SalomePyQt::removeObject( const QString& entry );
  \brief Remove object by entry
  \param entry data object entry
*/

class TRemoveObjectEvent: public SALOME_Event
{
public:
  QString myEntry;
  
  TRemoveObjectEvent( const QString& entry )
  : myEntry( entry ) {}
  virtual void Execute()
  {
    SALOME_PYQT_ModuleLight* module = dynamic_cast<SALOME_PYQT_ModuleLight*>( getActiveModule() );
    if ( module ) {
      module->removeObject( myEntry );
    }
    else {
      if ( verbose() ) printf( "SalomePyQt.removeObject() function is not supported for the current module.\n" );
    }
  }
};
void SalomePyQt::removeObject( const QString& entry )
{
  ProcessVoidEvent( new TRemoveObjectEvent( entry ) );
}

/*!
  \fn QStringList SalomePyQt::getChildren( const QString& entry, const bool recursive );
  \brief Get entries of all child data objects of specified data object
  \param entry data object entry
  \param recursive \c true for recursive processing
*/

class TGetChildrenEvent: public SALOME_Event
{
public:
  typedef QStringList TResult;
  TResult myResult;
  QString myEntry;
  bool    myRecursive; 
  TGetChildrenEvent( const QString& entry, const bool recursive )
    : myEntry( entry ),
      myRecursive( recursive ) {}
  virtual void Execute()
  {
    SALOME_PYQT_ModuleLight* module = dynamic_cast<SALOME_PYQT_ModuleLight*>( getActiveModule() );
    if ( module ) {
      myResult = module->getChildren( myEntry, myRecursive );
    }
    else {
      if ( verbose() ) printf( "SalomePyQt.getChildren() function is not supported for the current module.\n" );
    }
  }
};
QStringList SalomePyQt::getChildren( const QString& entry, const bool recursive )
{
  return ProcessEvent( new TGetChildrenEvent( entry, recursive ) ); 
}

#ifndef DISABLE_PLOT2DVIEWER
// Next set of methods relates to the Plot2d viewer functionality

/*!
  \fn void SalomePyQt::displayCurve( const int id, Plot2d_Curve* theCurve )
  \brief Display theCurve in view
  \param id window identifier
  \param theCurve curve to display
*/

class TDisplayCurve: public SALOME_Event
{
public:
  int myWndId;
  Plot2d_Curve* myCurve;
  TDisplayCurve( const int id, Plot2d_Curve* theCurve ) : myWndId( id ), myCurve( theCurve ) {}
  virtual void Execute() {
    Plot2d_ViewWindow* wnd = dynamic_cast<Plot2d_ViewWindow*>( getWnd( myWndId ) );
    if ( wnd )
      wnd->getViewFrame()->displayCurve( myCurve );
  }
};
void SalomePyQt::displayCurve( const int id, Plot2d_Curve* theCurve )
{
  ProcessVoidEvent( new TDisplayCurve( id, theCurve ) ); 
}

/*!
  \fn void SalomePyQt::eraseCurve( const int id, Plot2d_Curve* theCurve )
  \brief Erase theCurve in view
  \param id window identifier
  \param theCurve curve to erase
*/

class TEraseCurve: public SALOME_Event
{
public:
  int myWndId;
  Plot2d_Curve* myCurve;
  TEraseCurve( const int id, Plot2d_Curve* theCurve ) : myWndId( id ), myCurve( theCurve ) {}
  virtual void Execute() {
    Plot2d_ViewWindow* wnd = dynamic_cast<Plot2d_ViewWindow*>( getWnd( myWndId ) );
    wnd->getViewFrame()->eraseCurve( myCurve );
  }
};
void SalomePyQt::eraseCurve( const int id, Plot2d_Curve* theCurve )
{
  ProcessVoidEvent( new TEraseCurve( id, theCurve ) ); 
}

/*!
  \fn void SalomePyQt::deleteCurve( Plot2d_Curve* theCurve )
  \brief Delete theCurve from all views
  \param theCurve curve to delete
*/

class TDeleteCurve: public SALOME_Event
{
public:
  Plot2d_Curve* myCurve;
  TDeleteCurve( Plot2d_Curve* theCurve ) : myCurve( theCurve ) {}
  virtual void Execute() {
    LightApp_Application* app  = getApplication();
    if ( app ) {
      STD_TabDesktop* tabDesk = dynamic_cast<STD_TabDesktop*>( app->desktop() );
      if ( tabDesk ) {
        QList<SUIT_ViewWindow*> wndlist = tabDesk->windows();
        SUIT_ViewWindow* wnd;
        foreach ( wnd, wndlist ) {
          Plot2d_ViewWindow* aP2d = dynamic_cast<Plot2d_ViewWindow*>( wnd );
          if ( aP2d )
            aP2d->getViewFrame()->eraseObject( myCurve );
        }
      }
    }
  }
};
void SalomePyQt::eraseCurve( Plot2d_Curve* theCurve )
{
  ProcessVoidEvent( new TDeleteCurve( theCurve ) );
}

/*!
  \brief updateCurves (repaint) curves in view window.
*/
void SalomePyQt::updateCurves( const int id )
{
  class TEvent: public SALOME_Event
  {
  public:
    int myWndId;
    TEvent( const int id ) : myWndId( id ) {}
    virtual void Execute()
    {
      Plot2d_ViewWindow* wnd = dynamic_cast<Plot2d_ViewWindow*>( getWnd( myWndId ) );
      if ( wnd )
    	wnd->getViewFrame()->DisplayAll();
    }
  };
  ProcessVoidEvent( new TEvent( id ) );
}

/*!
  \fn QString SalomePyQt::getPlot2dTitle( const int id, ObjectType type = MainTitle )
  \brief Get title of corresponding type
  \param id window identifier
  \param type is type of title
  \return title of corresponding type
*/

class TGetPlot2dTitle: public SALOME_Event
{
public:
  typedef QString TResult;
  TResult myResult;
  int myWndId;
  ObjectType myType;
  TGetPlot2dTitle(const int id, ObjectType type) :
    myWndId( id ),
    myType( type ) {}
  virtual void Execute() {
    Plot2d_ViewWindow* wnd = dynamic_cast<Plot2d_ViewWindow*>( getWnd( myWndId ) );
    if ( wnd )
      myResult = wnd->getViewFrame()->getTitle( (Plot2d_ViewFrame::ObjectType)myType );
  }
};
QString SalomePyQt::getPlot2dTitle( const int id, ObjectType type )
{
  return ProcessEvent( new TGetPlot2dTitle( id, type ) ); 
}


/*!
  \fn void SalomePyQt::setPlot2dTitle( const int id, const QString& title, ObjectType type = MainTitle, bool show = true )
  \brief Set title of corresponding type
  \param id window identifier
  \param title
  \param type is type of title
  \param show
*/

class TSetPlot2dTitle: public SALOME_Event
{
public:
  int myWndId;
  Plot2d_Curve* myCurve;
  QString myTitle;
  ObjectType myType;
  bool myShow;
  TSetPlot2dTitle( const int id, const QString& title, ObjectType type, bool show ) :
    myWndId( id ),
    myTitle( title ),
    myType( type ),
    myShow( show ) {}
  virtual void Execute() {
    Plot2d_ViewWindow* wnd = dynamic_cast<Plot2d_ViewWindow*>( getWnd( myWndId ) );
    wnd->getViewFrame()->setTitle( myShow, myTitle, (Plot2d_ViewFrame::ObjectType)myType, false );
  }
};
void SalomePyQt::setPlot2dTitle( const int id, const QString& title, ObjectType type, bool show )
{
  ProcessVoidEvent( new TSetPlot2dTitle( id, title, type, show ) ); 
}

/*!
  \fn QList<int> SalomePyQt::getPlot2dFitRangeByCurves( const int id )
  \brief Get list of Plot2d view ranges
  \param id window identifier
  \return list of view ranges (XMin, XMax, YMin, YMax)
*/

class TFitRangeByCurves: public SALOME_Event
{
public:
  typedef QList<double> TResult;
  TResult myResult;
  int myWndId;
  TFitRangeByCurves( const int id )
    : myWndId( id ) {}
  virtual void Execute() 
  {
    myResult.clear();
    Plot2d_ViewWindow* wnd = dynamic_cast<Plot2d_ViewWindow*>( getWnd( myWndId ) );
    if ( wnd ) {
      double XMin, XMax, YMin, YMax, Y2Min, Y2Max;
      wnd->getViewFrame()->getFitRangeByCurves( XMin, XMax, YMin, YMax, Y2Min, Y2Max );
      myResult.append( XMin );
      myResult.append( XMax );
      myResult.append( YMin );
      myResult.append( YMax );
    }
  }
};
QList<double> SalomePyQt::getPlot2dFitRangeByCurves( const int id )
{
  return ProcessEvent( new TFitRangeByCurves( id ) );
}

/*!
  \fn QList<int> SalomePyQt::getPlot2dFitRangeCurrent( const int id )
  \brief Get list of current Plot2d view ranges
  \param id window identifier
  \return list of view ranges (XMin, XMax, YMin, YMax)
*/

class TFitRangeCurrent: public SALOME_Event
{
public:
  typedef QList<double> TResult;
  TResult myResult;
  int myWndId;
  TFitRangeCurrent( const int id )
    : myWndId( id ) {}
  virtual void Execute() 
  {
    myResult.clear();
    Plot2d_ViewWindow* wnd = dynamic_cast<Plot2d_ViewWindow*>( getWnd( myWndId ) );
    if ( wnd ) {
      double XMin, XMax, YMin, YMax, Y2Min, Y2Max;
      wnd->getViewFrame()->getFitRanges( XMin, XMax, YMin, YMax, Y2Min, Y2Max );
      myResult.append( XMin );
      myResult.append( XMax );
      myResult.append( YMin );
      myResult.append( YMax );
    }
  }
};
QList<double> SalomePyQt::getPlot2dFitRangeCurrent( const int id )
{
  return ProcessEvent( new TFitRangeCurrent( id ) );
}

/*!
  \fn void SalomePyQt::setPlot2dFitRange( const int id, const double XMin, const double XMax, const double YMin, const double YMax )
  \brief Set range of Plot2d view
  \param id window identifier
  \param XMin
  \param XMax
  \param YMin
  \param YMax
*/

class TPlot2dFitRange: public SALOME_Event
{
public:
  int myWndId;
  double myXMin;
  double myXMax;
  double myYMin;
  double myYMax;
  TPlot2dFitRange( const int id, const double XMin, const double XMax, const double YMin, const double YMax ) :
    myWndId( id ),
    myXMin( XMin ),
    myXMax( XMax ),
    myYMin( YMin ),
    myYMax( YMax ) {}
  virtual void Execute() {
    Plot2d_ViewWindow* wnd = dynamic_cast<Plot2d_ViewWindow*>( getWnd( myWndId ) );
    if ( wnd )
      wnd->getViewFrame()->fitData( 0, myXMin, myXMax, myYMin, myYMax );
  }
};
void SalomePyQt::setPlot2dFitRange( const int id, const double XMin, const double XMax, const double YMin, const double YMax )
{
  ProcessVoidEvent( new TPlot2dFitRange( id, XMin, XMax, YMin, YMax ) ); 
}

// End of methods related to the Plot2d viewer functionality
#endif // DISABLE_PLOT2DVIEWER

/*!
  \brief Process Qt event loop
*/
void SalomePyQt::processEvents()
{
  QCoreApplication::processEvents();
}

/*!
  \brief Set visibility state for given object
  \param theEntry study ID of the object
  \param theState visibility state
*/
void SalomePyQt::setVisibilityState( const QString& theEntry, VisibilityState theState )
{
  class TEvent: public SALOME_Event
  {
    QString myEntry;
    int myState;
  public:
    TEvent( const QString& theEntry, int theState ):
      myEntry( theEntry ), myState( theState ) {}
    virtual void Execute() 
    {
      LightApp_Study* aStudy = getActiveStudy();
      if ( !aStudy )
        return;
      aStudy->setVisibilityState( myEntry, (Qtx::VisibilityState)myState );
    }
  };
  ProcessVoidEvent( new TEvent( theEntry, theState ) );
}

/*!
  \fn VisibilityState SalomePyQt::getVisibilityState( const QString& theEntry )
  \brief Get visibility state for given object
  \param theEntry study ID of the object
  \return visibility state
*/

class TGetVisibilityStateEvent: public SALOME_Event 
{
public:
  typedef int TResult;
  TResult myResult;
  QString myEntry;
  TGetVisibilityStateEvent( const QString& theEntry ) : myResult( 0 ), myEntry( theEntry ) {}
  virtual void Execute()
  {
    LightApp_Study* aStudy = getActiveStudy();
    if ( aStudy )
      myResult = aStudy->visibilityState( myEntry );
  }
};
VisibilityState SalomePyQt::getVisibilityState( const QString& theEntry )
{
  return (VisibilityState) ProcessEvent( new TGetVisibilityStateEvent( theEntry ) );
}

/*!
  \brief Set position of given object in the tree
  \param theEntry study ID of the object
  \param thePos position
*/
void SalomePyQt::setObjectPosition( const QString& theEntry, int thePos )
{
  class TEvent: public SALOME_Event
  {
    QString myEntry;
    int myPos;
  public:
    TEvent( const QString& theEntry, int thePos ):
      myEntry( theEntry ), myPos( thePos ) {}
    virtual void Execute() 
    {
      SALOME_PYQT_ModuleLight* module = dynamic_cast<SALOME_PYQT_ModuleLight*>( getActiveModule() );
      if ( module )
        module->setObjectPosition( myEntry, myPos );
    }
  };
  ProcessVoidEvent( new TEvent( theEntry, thePos ) );
}

/*!
  \fn int SalomePyQt::getObjectPosition( const QString& theEntry )
  \brief Get position of given object in the tree
  \param theEntry study ID of the object
  \return position
*/

class TGetObjectPositionEvent: public SALOME_Event 
{
public:
  typedef int TResult;
  TResult myResult;
  QString myEntry;
  TGetObjectPositionEvent( const QString& theEntry ) : myResult( 0 ), myEntry( theEntry ) {}
  virtual void Execute()
  {
    SALOME_PYQT_ModuleLight* module = dynamic_cast<SALOME_PYQT_ModuleLight*>( getActiveModule() );
    if ( module )
      myResult = module->getObjectPosition( myEntry );
  }
};
int SalomePyQt::getObjectPosition( const QString& theEntry )
{
  return ProcessEvent( new TGetObjectPositionEvent( theEntry ) );
}

/*!
  \brief Start recordind a log of Python commands from embedded console
  \param theFileName output lof file name
*/
void SalomePyQt::startPyLog( const QString& theFileName )
{
  class TEvent: public SALOME_Event
  {
    QString myFileName;
  public:
    TEvent( const QString& theFileName ):
      myFileName( theFileName ) {}
    virtual void Execute() 
    {
      if ( getApplication() ) {
	PyConsole_Console* pyConsole = getApplication()->pythonConsole( false );
	if ( pyConsole ) pyConsole->startLog( myFileName );
      }
    }
  };
  ProcessVoidEvent( new TEvent( theFileName ) );
}

/*!
  \brief Stop recordind a log of Python commands from embedded console
*/
void SalomePyQt::stopPyLog()
{
  class TEvent: public SALOME_Event
  {
  public:
    TEvent() {}
    virtual void Execute() 
    {
      if ( getApplication() ) {
	PyConsole_Console* pyConsole = getApplication()->pythonConsole( false );
	if ( pyConsole ) pyConsole->stopLog();
      }
    }
  };
  ProcessVoidEvent( new TEvent() );
}
