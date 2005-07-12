#include "STD_Application.h"

#include "STD_MDIDesktop.h"

#include "STD_CloseDlg.h"

#include <SUIT_Tools.h>
#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_ViewModel.h>
#include <SUIT_Operation.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>

#include <QtxDockAction.h>
#include <QtxActionMenuMgr.h>
#include <QtxActionToolMgr.h>
#include <QtxPopupMenu.h>

#include <qmenubar.h>
#include <qtoolbar.h>
#include <qpopupmenu.h>
#include <qstatusbar.h>
#include <qfiledialog.h>
#include <qapplication.h>

#include <iostream.h>

extern "C" STD_EXPORT SUIT_Application* createApplication()
{
  return new STD_Application();
}

STD_Application::STD_Application()
: SUIT_Application(),
myEditEnabled( true ),
myActiveViewMgr( 0 )
{
  STD_MDIDesktop* desk = new STD_MDIDesktop();

  connect( desk, SIGNAL( closing( SUIT_Desktop*, QCloseEvent* ) ),
           this, SLOT( onDesktopClosing( SUIT_Desktop*, QCloseEvent* ) ) );

  setDesktop( desk );
}

STD_Application::~STD_Application()
{
}

QString STD_Application::applicationName() const
{
  return QString( "StdApplication" );
}

void STD_Application::start()
{
  createActions();

  updateDesktopTitle();
  updateCommandsStatus();
  setEditEnabled( myEditEnabled );

  SUIT_Application::start();
}

void STD_Application::onDesktopClosing( SUIT_Desktop*, QCloseEvent* e )
{
  if ( !isPossibleToClose() )
  {
    e->ignore();
    return;
  }

  SUIT_Study* study = activeStudy();

  if ( study )
    study->closeDocument();

  setActiveStudy( 0 );
  delete study;

  setDesktop( 0 );

  closeApplication();
}

void STD_Application::createActions()
{
  SUIT_Desktop* desk = desktop();
  SUIT_ResourceMgr* resMgr = resourceMgr();
  if ( !desk || !resMgr )
    return;

  // Create actions

  createAction( FileNewId, tr( "TOT_DESK_FILE_NEW" ),
                resMgr->loadPixmap( "STD", tr( "ICON_FILE_NEW" ) ),
                tr( "MEN_DESK_FILE_NEW" ), tr( "PRP_DESK_FILE_NEW" ),
                CTRL+Key_N, desk, false, this, SLOT( onNewDoc() ) );

  createAction( FileOpenId, tr( "TOT_DESK_FILE_OPEN" ),
                resMgr->loadPixmap( "STD", tr( "ICON_FILE_OPEN" ) ),
                tr( "MEN_DESK_FILE_OPEN" ), tr( "PRP_DESK_FILE_OPEN" ),
                CTRL+Key_O, desk, false, this, SLOT( onOpenDoc() ) );

  createAction( FileCloseId, tr( "TOT_DESK_FILE_CLOSE" ),
                resMgr->loadPixmap( "STD", tr( "ICON_FILE_CLOSE" ) ),
                tr( "MEN_DESK_FILE_CLOSE" ), tr( "PRP_DESK_FILE_CLOSE" ),
                CTRL+Key_W, desk, false, this, SLOT( onCloseDoc() ) );

  createAction( FileExitId, tr( "TOT_DESK_FILE_EXIT" ), QIconSet(),
                tr( "MEN_DESK_FILE_EXIT" ), tr( "PRP_DESK_FILE_EXIT" ),
                CTRL+Key_Q, desk, false, this, SLOT( onExit() ) );

  createAction( FileSaveId, tr( "TOT_DESK_FILE_SAVE" ),
                resMgr->loadPixmap( "STD", tr( "ICON_FILE_SAVE" ) ),
                tr( "MEN_DESK_FILE_SAVE" ), tr( "PRP_DESK_FILE_SAVE" ),
                CTRL+Key_S, desk, false, this, SLOT( onSaveDoc() ) );

  createAction( FileSaveAsId, tr( "TOT_DESK_FILE_SAVEAS" ), QIconSet(),
                tr( "MEN_DESK_FILE_SAVEAS" ), tr( "PRP_DESK_FILE_SAVEAS" ),
                0, desk, false, this, SLOT( onSaveAsDoc() ) );

  createAction( EditCopyId, tr( "TOT_DESK_EDIT_COPY" ),
                resMgr->loadPixmap( "STD", tr( "ICON_EDIT_COPY" ) ),
                tr( "MEN_DESK_EDIT_COPY" ), tr( "PRP_DESK_EDIT_COPY" ),
                CTRL+Key_C, desk, false, this, SLOT( onCopy() ) );

  createAction( EditPasteId, tr( "TOT_DESK_EDIT_PASTE" ),
                resMgr->loadPixmap( "STD", tr( "ICON_EDIT_PASTE" ) ),
                tr( "MEN_DESK_EDIT_PASTE" ), tr( "PRP_DESK_EDIT_PASTE" ),
                CTRL+Key_V, desk, false, this, SLOT( onPaste() ) );

  QAction* a = createAction( ViewStatusBarId, tr( "TOT_DESK_VIEW_STATUSBAR" ),
                             QIconSet(), tr( "MEN_DESK_VIEW_STATUSBAR" ),
                             tr( "PRP_DESK_VIEW_STATUSBAR" ), 0, desk, true );
  a->setOn( desk->statusBar()->isVisibleTo( desk ) );
  connect( a, SIGNAL( toggled( bool ) ), this, SLOT( onViewStatusBar( bool ) ) );

  createAction( NewWindowId, tr( "TOT_DESK_NEWWINDOW" ), QIconSet(),
                tr( "MEN_DESK_NEWWINDOW" ), tr( "PRP_DESK_NEWWINDOW" ), 0, desk  );

  createAction( HelpAboutId, tr( "TOT_DESK_HELP_ABOUT" ), QIconSet(),
                tr( "MEN_DESK_HELP_ABOUT" ), tr( "PRP_DESK_HELP_ABOUT" ),
                0, desk, false, this, SLOT( onHelpAbout() ) );

  //SRN: BugID IPAL9021, add an action "Load"
  createAction( FileLoadId, tr( "TOT_DESK_FILE_LOAD" ),
                resMgr->loadPixmap( "STD", tr( "ICON_FILE_OPEN" ) ),
		tr( "MEN_DESK_FILE_LOAD" ), tr( "PRP_DESK_FILE_LOAD" ),
		CTRL+Key_L, desk, false, this, SLOT( onLoadDoc() ) );      
  //SRN: BugID IPAL9021: End 

  QtxDockAction* da = new QtxDockAction( tr( "TOT_DOCK_WINDOWS" ), tr( "MEN_DOCK_WINDOWS" ), desk );
  registerAction( ViewWindowsId, da );
  da->setAutoPlace( false );

  // Create menus

  int fileMenu = createMenu( tr( "MEN_DESK_FILE" ), -1, -1, 0 );
  int editMenu = createMenu( tr( "MEN_DESK_EDIT" ), -1, -1, 10 );
  int viewMenu = createMenu( tr( "MEN_DESK_VIEW" ), -1, -1, 10 );
  int helpMenu = createMenu( tr( "MEN_DESK_HELP" ), -1, -1, 1000 );

  // Create menu items

  createMenu( FileNewId, fileMenu, 0 );
  createMenu( FileOpenId, fileMenu, 0 );
  createMenu( FileLoadId, fileMenu, 0 );  //SRN: BugID IPAL9021, add a menu item "Load"
  createMenu( FileCloseId, fileMenu, 0 );
  createMenu( separator(), fileMenu, -1, 0 );
  createMenu( FileSaveId, fileMenu, 0 );
  createMenu( FileSaveAsId, fileMenu, 0 );
  createMenu( separator(), fileMenu, -1, 0 );

  createMenu( separator(), fileMenu );
  createMenu( FileExitId, fileMenu );

  createMenu( EditCopyId, editMenu );
  createMenu( EditPasteId, editMenu );
  createMenu( separator(), editMenu );

  createMenu( ViewWindowsId, viewMenu );
  createMenu( ViewStatusBarId, viewMenu );
  createMenu( separator(), viewMenu );

  createMenu( HelpAboutId, helpMenu );
  createMenu( separator(), helpMenu );

  // Create tool bars

  int stdTBar = createTool( tr( "INF_DESK_TOOLBAR_STANDARD" ) );

  // Create tool items

  createTool( FileNewId, stdTBar );
  createTool( FileOpenId, stdTBar );
  createTool( FileSaveId, stdTBar );
  createTool( FileCloseId, stdTBar );
  createTool( separator(), stdTBar );
  createTool( EditCopyId, stdTBar );
  createTool( EditPasteId, stdTBar );
}

/*!
  Opens new application
*/
void STD_Application::onNewDoc() 
{
  if ( !activeStudy() )
  {
    createEmptyStudy();
    activeStudy()->createDocument();
    updateDesktopTitle();
    updateCommandsStatus();
  }
  else
  {
    SUIT_Application* aApp = startApplication( 0, 0 );
    if ( aApp->inherits( "STD_Application" ) )
      ((STD_Application*)aApp)->onNewDoc();
    else {
      aApp->createEmptyStudy();
      aApp->activeStudy()->createDocument();
    }
  }
}

void STD_Application::onOpenDoc()
{
  // It is preferrable to use OS-specific file dialog box here !!!
  QString aName = getFileName( true, QString::null, getFileFilter(), QString::null, 0 );
  if ( aName.isNull() )
    return;

  onOpenDoc( aName );
}

bool STD_Application::onOpenDoc( const QString& aName )
{
  bool res = true;
  if ( !activeStudy() )
  {
    // if no study - open in current desktop
    res = useFile( aName );
  }
  else
  {
    // if study exists - open in new desktop. Check: is the same file is opened?
    SUIT_Session* aSession = SUIT_Session::session();
    QPtrList<SUIT_Application> aAppList = aSession->applications();
    bool isAlreadyOpen = false;
    SUIT_Application* aApp = 0;
    for ( QPtrListIterator<SUIT_Application> it( aAppList ); it.current() && !isAlreadyOpen; ++it )
    {
      aApp = it.current();
      if ( aApp->activeStudy()->studyName() == aName )
        isAlreadyOpen = true;
    }
    if ( !isAlreadyOpen )
    {
      aApp = startApplication( 0, 0 );
      if ( aApp )
        res = aApp->useFile( aName );
    }
    else
      aApp->desktop()->setActiveWindow();
  }
  return res;
}

void STD_Application::onLoadDoc()
{     
}

bool STD_Application::onLoadDoc( const QString& aName )
{
  bool res = true;
  if ( !activeStudy() )
  {
    // if no study - load in current desktop
    res = useStudy( aName );
  }
  else
  {
    // if study exists - load in new desktop. Check: is the same file is loaded?
    SUIT_Session* aSession = SUIT_Session::session();
    QPtrList<SUIT_Application> aAppList = aSession->applications();
    bool isAlreadyOpen = false;
    SUIT_Application* aApp = 0;
    for ( QPtrListIterator<SUIT_Application> it( aAppList ); it.current() && !isAlreadyOpen; ++it )
    {
      aApp = it.current();
      if ( aApp->activeStudy()->studyName() == aName )
        isAlreadyOpen = true;
    }
    if ( !isAlreadyOpen )
    {
      aApp = startApplication( 0, 0 );
      if ( aApp )
        res = aApp->useStudy( aName );
    }
    else
      aApp->desktop()->setActiveWindow();
  }
  return res;
}

void STD_Application::beforeCloseDoc( SUIT_Study* )
{
}

void STD_Application::afterCloseDoc()
{
}

void STD_Application::onCloseDoc()
{
  if ( !isPossibleToClose() )
    return;

  SUIT_Study* study = activeStudy();

  beforeCloseDoc( study );

  if ( study )
    study->closeDocument();

  clearViewManagers();

  setActiveStudy( 0 );
  delete study;

  int aNbStudies = 0;
  QPtrList<SUIT_Application> apps = SUIT_Session::session()->applications();
  for ( unsigned i = 0; i < apps.count(); i++ )
    aNbStudies += apps.at( i )->getNbStudies();

  // STV: aNbStudies - number of currently existing studies (exclude currently closed)
  // STV: aNbStudies should be compared with 0.
  if ( aNbStudies )
    setDesktop( 0 );
  else
  {
    updateDesktopTitle();
    updateCommandsStatus();
  }

  afterCloseDoc();

  if ( !desktop() )
    closeApplication();
}

bool STD_Application::isPossibleToClose()
{
  if ( activeStudy() )
  {
    activeStudy()->abortAllOperations();
    if ( activeStudy()->isModified() )
    {
      QString sName = activeStudy()->studyName().stripWhiteSpace();
      QString msg = sName.isEmpty() ? tr( "INF_DOC_MODIFIED" ) : tr ( "INF_DOCUMENT_MODIFIED" ).arg( sName );

      //SRN: BugID: IPAL9021: Begin
      STD_CloseDlg dlg(desktop());
      switch( dlg.exec() )
      {
      case 1:
        if ( activeStudy()->isSaved() )
          onSaveDoc();
        else if ( !onSaveAsDoc() )
          return false;
        break;
      case 2:
        break;
      case 3:
        break;
      case 4:
      default:
        return false;
      }
     //SRN: BugID: IPAL9021: End 
    }
  }
  return true;
}

void STD_Application::onSaveDoc()
{
  if ( !activeStudy() )
    return;

  bool isOk = false;
  if ( activeStudy()->isSaved() )
  {
    isOk = activeStudy()->saveDocument();
    if ( !isOk )
      SUIT_MessageBox::error1( desktop(), tr( "TIT_FILE_SAVEAS" ),
			       tr( "MSG_CANT_SAVE" ).arg( activeStudy()->studyName() ), tr( "BUT_OK" ) );
  }

  if ( isOk )
    updateCommandsStatus();
  else
    onSaveAsDoc();
}

bool STD_Application::onSaveAsDoc()
{
  SUIT_Study* study = activeStudy();
  if ( !study )
    return false;

  QString aName = getFileName( false, study->studyName(), getFileFilter(), QString::null, 0 );

  if ( aName.isNull() ) 
    return false;
  bool isOk = study->saveDocumentAs( aName );

  updateDesktopTitle();
  updateCommandsStatus();

  return isOk;
}

void STD_Application::onExit()
{
  SUIT_Session::session()->closeSession();
}

void STD_Application::onCopy()
{
}

void STD_Application::onPaste()
{
}

void STD_Application::setEditEnabled( bool theEnable )
{
  myEditEnabled = theEnable;

  QtxActionMenuMgr* mMgr = desktop()->menuMgr();
  QtxActionToolMgr* tMgr = desktop()->toolMgr();

  for ( int i = EditCopyId; i <= EditPasteId; i++ )
  {
    mMgr->setShown( i, myEditEnabled );
    tMgr->setShown( i, myEditEnabled );
  }
}

bool STD_Application::useFile(const QString& theFileName)
{
  bool res = SUIT_Application::useFile(theFileName);
  updateDesktopTitle();
  updateCommandsStatus();
  return res;
}

void STD_Application::updateDesktopTitle()
{
  QString aTitle = applicationName();
  QString aVer = applicationVersion();
  if ( !aVer.isEmpty() )
    aTitle += QString( " " ) + aVer;

  if ( activeStudy() )
  {
    QString sName = SUIT_Tools::file( activeStudy()->studyName().stripWhiteSpace(), false );
    if ( !sName.isEmpty() )
      aTitle += QString( " - [%1]" ).arg( sName );
  }

  desktop()->setCaption( aTitle );
}

void STD_Application::updateCommandsStatus()
{
  bool aHasStudy = activeStudy() != 0;
  bool aIsNeedToSave = false;
  if ( aHasStudy ) 
    aIsNeedToSave = !activeStudy()->isSaved() || activeStudy()->isModified();

  if ( action( FileSaveId ) )
    action( FileSaveId )->setEnabled( aIsNeedToSave );
  if ( action( FileSaveAsId ) )
    action( FileSaveAsId )->setEnabled( aHasStudy );
  if ( action( FileCloseId ) )
    action( FileCloseId )->setEnabled( aHasStudy );
  if ( action( NewWindowId ) )
    action( NewWindowId )->setEnabled( aHasStudy );
}

SUIT_ViewManager* STD_Application::viewManager( const QString& vmType ) const
{
  SUIT_ViewManager* vm = 0;
  for ( QPtrListIterator<SUIT_ViewManager> it( myViewMgrs ); it.current() && !vm; ++it )
  {
    if ( it.current()->getType() == vmType )
      vm = it.current();
  }
  return vm;
}

void STD_Application::viewManagers( const QString& vmType, ViewManagerList& lst ) const
{
  for ( QPtrListIterator<SUIT_ViewManager> it( myViewMgrs ); it.current(); ++it )
    if ( it.current()->getType() == vmType )
      lst.append( it.current() );
}

void STD_Application::viewManagers( ViewManagerList& lst ) const
{
  for ( QPtrListIterator<SUIT_ViewManager> it( myViewMgrs ); it.current(); ++it )
    lst.append( it.current() );
}

ViewManagerList STD_Application::viewManagers() const
{
  ViewManagerList lst;
  viewManagers( lst );
  return lst;
}

SUIT_ViewManager* STD_Application::activeViewManager() const
{
  return myActiveViewMgr;
}

void STD_Application::addViewManager( SUIT_ViewManager* vm )
{
  if ( !vm )
    return;

  if ( !containsViewManager( vm ) )
  {
    myViewMgrs.append( vm );
    connect( vm, SIGNAL( activated( SUIT_ViewManager* ) ),
             this, SLOT( onViewManagerActivated( SUIT_ViewManager* ) ) );
    vm->connectPopupRequest( this, SLOT( onConnectPopupRequest( SUIT_PopupClient*, QContextMenuEvent* ) ) );

    emit viewManagerAdded( vm );
  }
/*
  if ( !activeViewManager() && myViewMgrs.count() == 1 )
    setActiveViewManager( vm );
*/
}

void STD_Application::removeViewManager( SUIT_ViewManager* vm )
{
  if ( !vm )
    return;

  vm->closeAllViews();

  emit viewManagerRemoved( vm );

  vm->disconnectPopupRequest( this, SLOT( onConnectPopupRequest( SUIT_PopupClient*, QContextMenuEvent* ) ) );
  vm->disconnect();
  myViewMgrs.removeRef( vm );

  if ( myActiveViewMgr == vm )
    myActiveViewMgr = 0;
}

void STD_Application::clearViewManagers()
{
  ViewManagerList lst;
  viewManagers( lst );

  for ( QPtrListIterator<SUIT_ViewManager> it( lst ); it.current(); ++it )
    removeViewManager( it.current() );
}

bool STD_Application::containsViewManager( SUIT_ViewManager* vm ) const
{
  return myViewMgrs.contains( vm ) > 0;
}

void STD_Application::onViewManagerActivated( SUIT_ViewManager* vm )
{
  setActiveViewManager( vm );
}

void STD_Application::onViewStatusBar( bool on )
{
  if ( on )
    desktop()->statusBar()->show();
  else
    desktop()->statusBar()->hide();
}

void STD_Application::onHelpAbout()
{
  SUIT_MessageBox::info1( desktop(), tr( "About" ), tr( "ABOUT_INFO" ), "&OK" );
}

void STD_Application::createEmptyStudy()
{
  SUIT_Application::createEmptyStudy();

  SUIT_ViewManager* vm = new SUIT_ViewManager( activeStudy(), desktop(), new SUIT_ViewModel() );

  addViewManager( vm );
}

void STD_Application::setActiveViewManager( SUIT_ViewManager* vm )
{
  if ( !containsViewManager( vm ) )
    return;

  myActiveViewMgr = vm;
  emit viewManagerActivated( vm );
}

void STD_Application::onConnectPopupRequest( SUIT_PopupClient* client, QContextMenuEvent* e )
{
  QtxPopupMenu* popup = new QtxPopupMenu();
  // fill popup by own items
  QString title;
  contextMenuPopup( client->popupClientType(), popup, title );
  popup->setTitleText( title );

  popup->insertSeparator();
  // add items from popup client
  client->contextMenuPopup( popup );
  
  SUIT_Tools::simplifySeparators( popup );

  if ( popup->count() )
    popup->exec( e->globalPos() );
  delete popup;
}

QString STD_Application::getFileName( bool open, const QString& initial, const QString& filters, 
				      const QString& caption, QWidget* parent )
{
  if ( !parent )
    parent = desktop();
  if ( open ) 
  {
    return QFileDialog::getOpenFileName( initial, filters, parent, 0, caption );
  }
  else
  {
    QString aName;
    QString aUsedFilter;
    QString anOldPath = initial;

    bool isOk = false;
    while ( !isOk )
    {
      // It is preferrable to use OS-specific file dialog box here !!!
      aName = QFileDialog::getSaveFileName( anOldPath, filters, parent,
					    0, caption, &aUsedFilter);

      if ( aName.isNull() )
        isOk = true;
      else
      {
        if ( !getFileFilter().isNull() ) // check extension and add if it is necessary
        {
	  int aStart = aUsedFilter.find( '*' );
	  int aEnd = aUsedFilter.find( ')', aStart + 1 );
	  QString aExt = aUsedFilter.mid( aStart + 1, aEnd - aStart - 1 );
	  if ( aExt.contains( '*' ) == 0 ) // if it is not *.*
	  {
	    // Check that there is an extension at the end of the name
  	    QString aNameTail = aName.right( aExt.length() );
	    if ( aNameTail != aExt )
              aName += aExt;
	  }
        }
	if ( QFileInfo( aName ).exists() )
        {
	  int aAnswer = SUIT_MessageBox::warn3( desktop(), tr( "TIT_FILE_SAVEAS" ),
					        tr( "MSG_FILE_EXISTS" ).arg( aName ),
					        tr( "BUT_YES" ), tr( "BUT_NO" ), tr( "BUT_CANCEL" ), 1, 2, 3, 1 );
	  if ( aAnswer == 3 ) {     // cancelled
            aName = QString::null;
	    isOk = true;
          }
	  else if ( aAnswer == 2 ) // not save to this file
	    anOldPath = aName;             // not to return to the same initial dir at each "while" step
	  else                     // overwrite the existing file
	    isOk = true;
        }
	else
	  isOk = true;
      }
    }
    return aName;
  }
}

QString STD_Application::getDirectory( const QString& initial, const QString& caption, QWidget* parent )
{
  if ( !parent )
    parent = desktop();
  return QFileDialog::getExistingDirectory( initial, parent, 0, caption, true );
}
