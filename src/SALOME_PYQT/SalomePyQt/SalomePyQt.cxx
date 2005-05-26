//=============================================================================
// File      : SalomePyQt.cxx
// Created   : 25/04/05
// Author    : Vadim SANDLER
// Project   : SALOME
// Copyright : 2003-2005 CEA/DEN, EDF R&D
// $Header   : $
//=============================================================================

#include "SalomePyQt.h"

#include <qapplication.h>
#include <qmenubar.h>

#include "SALOME_Event.hxx"

#include "SUIT_Session.h"
#include "SUIT_Desktop.h"
#include "STD_MDIDesktop.h"
#include "SalomeApp_Application.h"
#include "SalomeApp_Study.h"
#include "OB_Browser.h"
// #include "QAD_FileDlg.h"
// #include "QAD_ViewFrame.h"
// #include "QAD_RightFrame.h"
// #include "QAD_Tools.h"

// #include "QAD_Config.h"
// #include "QAD_Settings.h"

using namespace std;

//====================================================================================
// static functions
//====================================================================================
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

//====================================================================================
// SALOME_Selection class (implemented to keep compatibility with previous SALOME GUI
//====================================================================================
SALOME_Selection::SALOME_Selection()
{
  // VSR: TODO...
}

void SALOME_Selection::Clear()
{
  // VSR: TODO...
}
void SALOME_Selection::ClearIObjects()
{
  // VSR: TODO...
}

//====================================================================================
// SalomePyQt class
//====================================================================================

/*!
  SalomePyQt::getDesktop
  Gets desktop. Returns 0 in error.
*/
class TGetDesktopEvent: public SALOME_Event {
public:
  typedef QWidget* TResult;
  TResult myResult;
  TGetDesktopEvent() : myResult( 0 ) {}
  virtual void Execute() {
    if ( getApplication() )
      myResult = (QWidget*)( getApplication()->desktop() );
  }
};
QWidget* SalomePyQt::getDesktop()
{
  return ProcessEvent( new TGetDesktopEvent() );
}

/*!
  SalomePyQt::getMainFrame
  Gets workspace widget. Returns 0 in error.
*/
class TGetMainFrameEvent: public SALOME_Event {
public:
  typedef QWidget* TResult;
  TResult myResult;
  TGetMainFrameEvent() : myResult( 0 ) {}
  virtual void Execute() {
    if ( getApplication() ) {
      STD_MDIDesktop* aDesktop = dynamic_cast<STD_MDIDesktop*>( getApplication()->desktop() );
      myResult = (QWidget*)( aDesktop->workspace() );
    }
  }
};
QWidget* SalomePyQt::getMainFrame()
{
  return ProcessEvent( new TGetMainFrameEvent() );
}

/*!
  SalomePyQt::getMainMenuBar
  Gets main menu. Returns 0 in error.
*/
class TGetMainMenuBarEvent: public SALOME_Event {
public:
  typedef QMenuBar* TResult;
  TResult myResult;
  TGetMainMenuBarEvent() : myResult( 0 ) {}
  virtual void Execute() {
    if ( getApplication() ) {
      // VSR: TODO
      // myResult = (QMenuBar*)(QAD_Application::getDesktop()->getMainMenuBar());
    }
  }
};
QMenuBar* SalomePyQt::getMainMenuBar() 
{
  return ProcessEvent( new TGetMainMenuBarEvent() );
}

/*!
  SalomePyQt::getPopupMenu
  Gets an main menu's child popup menu by its id
*/
class TGetPopupMenuEvent: public SALOME_Event {
public:
  typedef QPopupMenu* TResult;
  TResult  myResult;
  MenuName myMenuName;
  TGetPopupMenuEvent( const MenuName menu ) : myResult( 0 ), myMenuName( menu ) {}
  virtual void Execute() {
    if ( /*SalomeApp_Study* aStudy =*/ getActiveStudy() ) {
      // VSR: TODO
      //QMenuBar* mainMenu = QAD_Application::getDesktop()->getMainMenuBar();
      //if ( mainMenu->findItem( menu ) ) {
      //  return mainMenu->findItem( menu )->popup();
      //}
      //return 0;
    }
  }
};
QPopupMenu* SalomePyQt::getPopupMenu( const MenuName menu )
{
  return ProcessEvent( new TGetPopupMenuEvent( menu ) );
}

/*!
  SalomePyQt::getStudyId
  Returns active study's ID or 0 if there is no active study.
*/
class TGetStudyIdEvent: public SALOME_Event {
public:
  typedef int TResult;
  TResult myResult;
  TGetStudyIdEvent() : myResult( 0 ) {}
  virtual void Execute() {
    if ( SalomeApp_Study* aStudy = getActiveStudy() ) {
      myResult = aStudy->studyDS()->StudyId();
    }
  }
};
int SalomePyQt::getStudyId()
{
  return ProcessEvent( new TGetStudyIdEvent() );
}

/*!
  SalomePyQt::getSelection
  Creates a Selection object (to provide a compatibility with previous SALOME GUI).
*/
class TGetSelectionEvent: public SALOME_Event {
public:
  typedef SALOME_Selection* TResult;
  TResult myResult;
  TGetSelectionEvent() : myResult( 0 ) {}
  virtual void Execute() {
    if ( /*SalomeApp_Study* aStudy = */getActiveStudy() ) {
      // VSR: TODO
      // return SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveApp()->getActiveStudy()->getSelection());
      myResult = new SALOME_Selection();
    }
  }
};
SALOME_Selection* SalomePyQt::getSelection()
{
  return ProcessEvent( new TGetSelectionEvent() );
}

/*!
  SalomePyQt::putInfo
  Puts an information message to the desktop's status bar
  (with optional delay parameter given in msec)
*/
class TPutInfoEvent: public SALOME_Event {
  QString myMsg;
  int     myMsecs;
public:
  TPutInfoEvent( const QString& msg, const int ms = 0 ) : myMsg( msg ), myMsecs( ms ) {}
  virtual void Execute() {
    if ( /*SalomeApp_Study* aStudy = */getActiveStudy() ) {
      // VSR: TODO
      // QAD_Application::getDesktop()->putInfo(msg);
      // ... or ...
      // QAD_Application::getDesktop()->putInfo(msg, ms);
    }
  }
};
void SalomePyQt::putInfo( const QString& msg )
{
  ProcessVoidEvent( new TPutInfoEvent( msg ) );
}
void SalomePyQt::putInfo( const QString& msg, const int ms )
{
  ProcessVoidEvent( new TPutInfoEvent( msg, ms ) );
}

/*!
  SalomePyQt::getActiveComponent
  Returns an active component name or empty string if there is no active component
*/
class TGetActiveComponentEvent: public SALOME_Event {
public:
  typedef QString TResult;
  TResult myResult;
  TGetActiveComponentEvent() {}
  virtual void Execute() {
    if ( /*SalomeApp_Study* aStudy = */getActiveStudy() ) {
      // VSR: TODO
      //   return QAD_Application::getDesktop()->getActiveComponent();
    }
  }
};
const QString SalomePyQt::getActiveComponent()
{
  return ProcessEvent( new TGetActiveComponentEvent() );
}

/*!
  SalomePyQt::updateObjBrowser
  Updates an Object Browser of a given study
  VSR: updateSelection parameter is currently not used. Will be implemented or removed lately.
*/
void SalomePyQt::updateObjBrowser( const int studyId, bool updateSelection )
{  
  class TEvent: public SALOME_Event {
    int  myStudyId;
    bool myUpdateSelection;
  public:
    TEvent( const int studyId, bool updateSelection ) 
      : myStudyId( studyId ), myUpdateSelection( updateSelection ) {}
    virtual void Execute() {
      if ( SalomeApp_Application* anApp = getApplication() ) {
	// VSR: TODO
	// this implementation is temporary and works only for the active study
	OB_Browser* browser = anApp->objectBrowser();
	if ( browser )
	  browser->updateTree();
	///
// 	QList<QAD_Study>& studies = QAD_Application::getDesktop()->getActiveApp()->getStudies();
// 	for ( QAD_Study* study = studies.first(); study; study = studies.next() )  {
// 	  if ( study->getStudyId() == studyId ) {
// 	    study->updateObjBrowser( updateSelection );
// 	    break;
// 	  }
// 	}
	///
      }
    }
  };
  ProcessVoidEvent( new TEvent( studyId, updateSelection ) );
}

/*!
  SalomePyQt::addStringSetting
  Adds a string setting to the application preferences
*/
void SalomePyQt::addStringSetting( const QString& name, const QString& value, bool autoValue )
{
  class TEvent: public SALOME_Event {
    QString myName;
    QString myValue;
    bool    myAutoValue;
  public:
    TEvent( const QString& name, const QString& value, bool autoValue ) 
      : myName( name ), myValue( value ), myAutoValue( autoValue ) {}
    virtual void Execute() {
      if ( /*SalomeApp_Application* anApp =*/ getApplication() ) {
	// VSR: TODO
	// QAD_CONFIG->addSetting(_name, _value, _autoValue);
      }
    }
  };
  ProcessVoidEvent( new TEvent( name, value, autoValue ) );
}

/*!
  SalomePyQt::addIntSetting
  Adds an integer setting to the application preferences
*/
void SalomePyQt::addIntSetting( const QString& name, const int value, bool autoValue)
{
  class TEvent: public SALOME_Event {
    QString myName;
    int     myValue;
    bool    myAutoValue;
  public:
    TEvent( const QString& name, const int value, bool autoValue ) 
      : myName( name ), myValue( value ), myAutoValue( autoValue ) {}
    virtual void Execute() {
      if ( /*SalomeApp_Application* anApp =*/ getApplication() ) {
	// VSR: TODO
	// QAD_CONFIG->addSetting(_name, _value, _autoValue);
      }
    }
  };
  ProcessVoidEvent( new TEvent( name, value, autoValue ) );
}

/*!
  SalomePyQt::addDoubleSetting
  Adds an double setting to the application preferences
*/
void SalomePyQt::addDoubleSetting( const QString& name, const double value, bool autoValue )
{
  class TEvent: public SALOME_Event {
    QString myName;
    double  myValue;
    bool    myAutoValue;
  public:
    TEvent( const QString& name, const double value, bool autoValue ) 
      : myName( name ), myValue( value ), myAutoValue( autoValue ) {}
    virtual void Execute() {
      if ( /*SalomeApp_Application* anApp =*/ getApplication() ) {
	// VSR: TODO
	// QAD_CONFIG->addSetting(_name, _value, _autoValue);
      }
    }
  };
  ProcessVoidEvent( new TEvent( name, value, autoValue ) );
}

/*!
  SalomePyQt::removeSettings
  Removes a setting from the application preferences
*/
void SalomePyQt::removeSettings( const QString& name )
{
  class TEvent: public SALOME_Event {
    QString myName;
  public:
    TEvent( const QString& name ) : myName( name ) {}
    virtual void Execute() {
      if ( /*SalomeApp_Application* anApp =*/ getApplication() ) {
	// VSR: TODO
	// QAD_CONFIG->removeSettings( name );
      }
    }
  };
  ProcessVoidEvent( new TEvent( name ) );
}

/*!
  SalomePyQt::getSetting
  Gets a setting value (as string)
*/
class TGetSettingEvent: public SALOME_Event {
public:
  typedef QString TResult;
  TResult myResult;
  QString myName;
  TGetSettingEvent( const QString& name ) : myName( name ) {}
  virtual void Execute() {
    if ( /*SalomeApp_Study* aStudy = */getActiveStudy() ) {
      // VSR: TODO
      // myResult = QAD_CONFIG->getSetting(name);
    }
  }
};
QString SalomePyQt::getSetting( const QString& name )
{
  return ProcessEvent( new TGetSettingEvent( name ) );
}

/*!
  SalomePyQt::getFileName
  Displays 'Open/Save file' dialog box and returns a user's choice (file name)
*/
class TGetFileNameEvent: public SALOME_Event {
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
  virtual void Execute() {
    if ( /*SalomeApp_Study* aStudy = */getActiveStudy() ) {
      // VSR: TODO
      // myResult = QAD_FileDlg::getFileName(parent, initial, filters, caption, open);
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
  SalomePyQt::getOpenFileNames
  Displays 'Open files' dialog box and returns a user's choice (a list of file names)
*/
class TGetOpenFileNamesEvent: public SALOME_Event {
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
  virtual void Execute() {
    if ( /*SalomeApp_Study* aStudy = */getActiveStudy() ) {
      // VSR: TODO
      // myResult = QAD_FileDlg::getOpenFileNames(parent, initial, filters, caption);
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
  SalomePyQt::getExistingDirectory
  Displays 'Get Directory' dialog box and returns a user's choice (a directory name)
*/
class TGetExistingDirectoryEvent: public SALOME_Event {
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
  virtual void Execute() {
    if ( /*SalomeApp_Study* aStudy = */getActiveStudy() ) {
      // VSR: TODO
      // myResult = QAD_FileDlg::getExistingDirectory(parent, initial, caption);
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
  SalomePyQt::helpContext
  Opens external browser to display 'context help' information
  VSR: current implementation does nothing.
*/
void SalomePyQt::helpContext( const QString& source, const QString& context ) {
  class TEvent: public SALOME_Event {
    QString mySource;
    QString myContext;
  public:
    TEvent( const QString& source, const QString& context ) 
      : mySource( source ), myContext( context ) {}
    virtual void Execute() {
      if ( /*SalomeApp_Application* anApp =*/ getApplication() ) {
	// VSR: TODO
////QAD_Application::getDesktop()->helpContext(source, context);
      }
    }
  };
  ProcessVoidEvent( new TEvent( source, context ) );
}

/*!
  SalomePyQt::dumpView
  Dumps the contents of the currently active view to the image file 
  in the given format (JPEG, PNG, BMP are supported)
*/
class TDumpViewEvent: public SALOME_Event {
public:
  typedef bool TResult;
  TResult myResult;
  QString myFileName;
  TDumpViewEvent( const QString& filename ) 
    : myResult ( false ), myFileName( filename ) {}
  virtual void Execute() {
    if ( /*SalomeApp_Study* aStudy = */getActiveStudy() ) {
      // VSR: TODO
//   QAD_Study* activeStudy = QAD_Application::getDesktop()->getActiveApp()->getActiveStudy();
//   if ( !activeStudy )
//     return false;
//   QAD_ViewFrame* activeViewFrame = activeStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame();
//   if ( !activeViewFrame )
//     return false;
//   if ( !activeViewFrame->getViewWidget() )
//     return false;

//   qApp->processEvents();
//   QPixmap px = QPixmap::grabWindow( activeViewFrame->getViewWidget()->winId() );
//   if ( !filename.isNull() ) {
//     QString fmt = QAD_Tools::getFileExtensionFromPath( filename ).upper();
//     if ( fmt.isEmpty() )
//       fmt = QString( "PNG" ); // default format
//     if ( fmt == "JPG" )
//       fmt = "JPEG";
//     bool bOk = px.save( filename, fmt.latin1() );
//     return bOk;
//   }
//   return false;
    }
  }
};
bool SalomePyQt::dumpView( const QString& filename )
{
  return ProcessEvent( new TDumpViewEvent( filename ) );
}
