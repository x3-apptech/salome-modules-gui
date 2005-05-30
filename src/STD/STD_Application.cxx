#include "STD_Application.h"

#include "STD_MDIDesktop.h"

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

#include <qmenubar.h>
#include <qtoolbar.h>
#include <qpopupmenu.h>
#include <qstatusbar.h>
#include <qfiledialog.h>
#include <qapplication.h>

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

  createAction( EditCutId, tr( "TOT_DESK_EDIT_CUT" ),
                resMgr->loadPixmap( "STD", tr( "ICON_EDIT_CUT" ) ),
                tr( "MEN_DESK_EDIT_CUT" ), tr( "PRP_DESK_EDIT_CUT" ),
                CTRL+Key_X, desk, false, this, SLOT( onCut() ) );

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

  QtxDockAction* da = new QtxDockAction( tr( "TOT_DOCK_WINDOWS" ), tr( "MEN_DOCK_WINDOWS" ), desk );
  registerAction( ViewWindowsId, da );
  da->setAutoPlace( false );

  // Create menus

  int fileMenu = createMenu( tr( "MEN_DESK_FILE" ), -1, -1, 0 );
  int editMenu = createMenu( tr( "MEN_DESK_EDIT" ), -1, -1, 10 );
  int viewMenu = createMenu( tr( "MEN_DESK_VIEW" ), -1, -1, 20 );
  int helpMenu = createMenu( tr( "MEN_DESK_HELP" ), -1, -1, 1000 );

  // Create menu items

  createMenu( FileNewId, fileMenu, 0 );
  createMenu( FileOpenId, fileMenu, 0 );
  createMenu( FileCloseId, fileMenu, 0 );
  createMenu( separator(), fileMenu, -1, 0 );
  createMenu( FileSaveId, fileMenu, 0 );
  createMenu( FileSaveAsId, fileMenu, 0 );
  createMenu( separator(), fileMenu, -1, 0 );

  createMenu( separator(), fileMenu );
  createMenu( FileExitId, fileMenu );

  createMenu( EditCutId, editMenu );
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
  createTool( EditCutId, stdTBar );
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
  QString aName = QFileDialog::getOpenFileName( QString::null, getFileFilter(), desktop() );
  if ( aName.isNull() )
    return;

  if ( !activeStudy() )
  {
    // if no study - open in current desktop
    createEmptyStudy();
    activeStudy()->openDocument( aName );
    updateDesktopTitle();
    updateCommandsStatus();
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
        aApp->useFile( aName );
    }
    else
      aApp->desktop()->setActiveWindow();
  }
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

  QPtrListIterator<SUIT_ViewManager> it( myViewMgrs );
  it.toLast();
  SUIT_ViewManager* vm = 0;
  while( ( vm = it.current()) != 0 )
  {
    vm->closeAllViews();
    --it;
  }

  if ( study )
    study->closeDocument();

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
      int aAnswer = SUIT_MessageBox::warn3( desktop(), tr( "TOT_DESK_FILE_CLOSE" ),
                                            tr( "Document \"%1\" has been modified.\nDo you want to save changes?").arg( activeStudy()->studyName() ),
                                            tr( "BUT_YES" ), tr( "BUT_NO" ), tr( "BUT_CANCEL" ), 1, 2, 3, 1 );
      switch ( aAnswer )
      {
      case 1:
        if ( !activeStudy()->isSaved() )
          if ( !onSaveAsDoc() )
            return false;
        else
          onSaveDoc();
        break;
      case 2:
        break;
      case 3:
      default:
        return false;
      }
    }
  }
  return true;
}

void STD_Application::onSaveDoc()
{
  if ( !activeStudy() )
    return;

  if ( activeStudy()->isSaved() )
  {
    activeStudy()->saveDocument();
    updateCommandsStatus();
  }
  else
    onSaveAsDoc();
}

bool STD_Application::onSaveAsDoc()
{
  SUIT_Study* study = activeStudy();
  if ( !study )
    return false;

  QString aName, aUsedFilter, anOldPath = study->studyName();
  bool isSelected = false;
  while( !isSelected )
  {
    // It is preferrable to use OS-specific file dialog box here !!!
    aName = QFileDialog::getSaveFileName( anOldPath, getFileFilter(), desktop(),
                                          0, QString::null, &aUsedFilter);

    if ( aName.isNull() )
      break;

    if ( !getFileFilter().isNull() )
    { // check exstension and add if it is necessary
      int aStart = aUsedFilter.find('*');
      int aEnd = aUsedFilter.find(')', aStart + 1);
      QString aExt = aUsedFilter.mid(aStart + 1, aEnd - aStart - 1);
      if (aExt.contains('*') == 0 ) {  // if it is not *.*
        // Check that there is an extension at the end of the name
        QString aNameTail = aName.right(aExt.length());
          if ( aNameTail != aExt )
            aName += aExt;
      }
    }
    QFile aFile( aName );
    if ( aFile.exists() )
    {
      int aAnswer = SUIT_MessageBox::warn2( desktop(), tr("TIT_FILE_SAVEAS"),
                                                     tr("MSG_FILE_EXISTS").arg(aName),
                                                     tr("BUT_YES"), tr("BUT_NO"), 1, 2, 2);
      if ( aAnswer != 2 )
        isSelected = true;
      else
        anOldPath = aName; // Not to return to the same initial dir at each "while" step
    }
    else
      isSelected = true;
  }

  if ( aName.isNull() ) 
    return false;

  study->saveDocumentAs( aName );
  updateDesktopTitle();
  updateCommandsStatus();
  return true;
}

void STD_Application::onExit()
{
  SUIT_Session::session()->closeSession();
}

void STD_Application::onCut()
{
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

  for ( int i = EditCutId; i <= EditPasteId; i++ )
  {
    mMgr->setShown( i, myEditEnabled );
    tMgr->setShown( i, myEditEnabled );
  }
}

void STD_Application::useFile(const QString& theFileName)
{
  SUIT_Application::useFile(theFileName);
  updateDesktopTitle();
  updateCommandsStatus();
}

void STD_Application::updateDesktopTitle()
{
  QString aTitle = applicationName();
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
  QPopupMenu* popup = new QPopupMenu();
  // fill popup by own items
  contextMenuPopup( client->popupClientType(), popup );

  popup->insertSeparator();
  // add items from popup client
  client->contextMenuPopup( popup );
  
  SUIT_Tools::simplifySeparators( popup );

  if ( popup->count() )
    popup->exec( e->globalPos() );
  delete popup;
}
