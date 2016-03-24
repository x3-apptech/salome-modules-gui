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

#include "STD_Application.h"

#include "STD_MDIDesktop.h"

#include <SUIT_Tools.h>
#include <SUIT_Study.h>
#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ResourceMgr.h>

#include <QtxDockAction.h>
#include <QtxMenu.h>
#include <QtxActionMenuMgr.h>
#include <QtxActionToolMgr.h>

#include <QMenu>
#include <QStatusBar>
#include <QCloseEvent>
#include <QFileDialog>
#include <QApplication>

/*!Create and return new instance of STD_Application*/
extern "C" STD_EXPORT SUIT_Application* createApplication()
{
  return new STD_Application();
}

/*!Constructor.*/
STD_Application::STD_Application()
: SUIT_Application(),
  myActiveViewMgr( 0 ),
  myExitConfirm( true ),
  myEditEnabled( true )
{
  setDesktop( new STD_MDIDesktop() );
}

/*!Destructor.*/
STD_Application::~STD_Application()
{
  clearViewManagers();
}

/*! \retval requirement of exit confirmation*/
bool STD_Application::exitConfirmation() const
{
  return myExitConfirm;
}

/*! Set the requirement of exit confirmation*/
void STD_Application::setExitConfirmation( const bool on )
{
  myExitConfirm = on;
}

/*! \retval QString "StdApplication"*/
QString STD_Application::applicationName() const
{
  return QString( "StdApplication" );
}

/*!Start STD_Application*/
void STD_Application::start()
{
  createActions();

  updateDesktopTitle();
  updateCommandsStatus();
  setEditEnabled( myEditEnabled );

  loadPreferences();

  SUIT_Application::start();
}

/*!
  Close the Application
*/
void STD_Application::closeApplication()
{
  if ( desktop() )
    savePreferences();
  SUIT_Study* study = activeStudy();

  if ( study )
  {
    beforeCloseDoc( study );

    study->closeDocument();
    emit appClosed();
    setActiveStudy( 0 );
    delete study;

    afterCloseDoc();
  }

  setDesktop( 0 );

  SUIT_Application::closeApplication();
}

/*!Event on closing desktop*/
void STD_Application::onDesktopClosing( SUIT_Desktop*, QCloseEvent* e )
{
  if ( SUIT_Session::session()->applications().count() < 2 )
  {
    onExit();
    return;
  }

  bool closePermanently;
  if ( !isPossibleToClose( closePermanently ) )
  {
    e->ignore();
    return;
  }

  closeApplication();
}

/*!Create actions, menus and tools*/
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
                Qt::CTRL+Qt::Key_N, desk, false, this, SLOT( onNewDoc() ) );

  createAction( FileOpenId, tr( "TOT_DESK_FILE_OPEN" ),
                resMgr->loadPixmap( "STD", tr( "ICON_FILE_OPEN" ) ),
                tr( "MEN_DESK_FILE_OPEN" ), tr( "PRP_DESK_FILE_OPEN" ),
                Qt::CTRL+Qt::Key_O, desk, false, this, SLOT( onOpenDoc() ) );

  createAction( FileReopenId, tr( "TOT_DESK_FILE_REOPEN" ), QIcon(),
                tr( "MEN_DESK_FILE_REOPEN" ), tr( "PRP_DESK_FILE_REOPEN" ),
                0, desk, false, this, SLOT( onReopenDoc() ) );

  createAction( FileCloseId, tr( "TOT_DESK_FILE_CLOSE" ),
                resMgr->loadPixmap( "STD", tr( "ICON_FILE_CLOSE" ) ),
                tr( "MEN_DESK_FILE_CLOSE" ), tr( "PRP_DESK_FILE_CLOSE" ),
                Qt::CTRL+Qt::Key_W, desk, false, this, SLOT( onCloseDoc() ) );

  createAction( FileExitId, tr( "TOT_DESK_FILE_EXIT" ), QIcon(),
                tr( "MEN_DESK_FILE_EXIT" ), tr( "PRP_DESK_FILE_EXIT" ),
                Qt::CTRL+Qt::Key_Q, desk, false, this, SLOT( onExit() ) );

  createAction( FileSaveId, tr( "TOT_DESK_FILE_SAVE" ),
                resMgr->loadPixmap( "STD", tr( "ICON_FILE_SAVE" ) ),
                tr( "MEN_DESK_FILE_SAVE" ), tr( "PRP_DESK_FILE_SAVE" ),
                Qt::CTRL+Qt::Key_S, desk, false, this, SLOT( onSaveDoc() ) );

  createAction( FileSaveAsId, tr( "TOT_DESK_FILE_SAVEAS" ), QIcon(),
                tr( "MEN_DESK_FILE_SAVEAS" ), tr( "PRP_DESK_FILE_SAVEAS" ),
                Qt::CTRL+Qt::SHIFT+Qt::Key_S, desk, false, this, SLOT( onSaveAsDoc() ) );

  createAction( EditCopyId, tr( "TOT_DESK_EDIT_COPY" ),
                resMgr->loadPixmap( "STD", tr( "ICON_EDIT_COPY" ) ),
                tr( "MEN_DESK_EDIT_COPY" ), tr( "PRP_DESK_EDIT_COPY" ),
                Qt::CTRL+Qt::Key_C, desk, false, this, SLOT( onCopy() ) );

  createAction( EditPasteId, tr( "TOT_DESK_EDIT_PASTE" ),
                resMgr->loadPixmap( "STD", tr( "ICON_EDIT_PASTE" ) ),
                tr( "MEN_DESK_EDIT_PASTE" ), tr( "PRP_DESK_EDIT_PASTE" ),
                Qt::CTRL+Qt::Key_V, desk, false, this, SLOT( onPaste() ) );

  QAction* a = createAction( ViewStatusBarId, tr( "TOT_DESK_VIEW_STATUSBAR" ),
                             QIcon(), tr( "MEN_DESK_VIEW_STATUSBAR" ),
                             tr( "PRP_DESK_VIEW_STATUSBAR" ), Qt::ALT+Qt::SHIFT+Qt::Key_S, desk, true );
  a->setChecked( desk->statusBar()->isVisibleTo( desk ) );
  connect( a, SIGNAL( toggled( bool ) ), this, SLOT( onViewStatusBar( bool ) ) );

  createAction( NewWindowId, tr( "TOT_DESK_NEWWINDOW" ), QIcon(),
                tr( "MEN_DESK_NEWWINDOW" ), tr( "PRP_DESK_NEWWINDOW" ), 0, desk  );

  createAction( HelpAboutId, tr( "TOT_DESK_HELP_ABOUT" ), QIcon(),
                tr( "MEN_DESK_HELP_ABOUT" ), tr( "PRP_DESK_HELP_ABOUT" ),
                Qt::ALT+Qt::SHIFT+Qt::Key_A, desk, false, this, SLOT( onHelpAbout() ) );


  QtxDockAction* dwa = new QtxDockAction( tr( "TOT_DOCKWINDOWS" ), tr( "MEN_DESK_VIEW_DOCKWINDOWS" ), desk );
  dwa->setDockType( QtxDockAction::DockWidget );
  registerAction( ViewWindowsId, dwa );

  QtxDockAction* tba = new QtxDockAction( tr( "TOT_TOOLBARS" ), tr( "MEN_DESK_VIEW_TOOLBARS" ), desk );
  tba->setDockType( QtxDockAction::ToolBar );
  registerAction( ViewToolBarsId, tba );

  // Create menus

  int fileMenu = createMenu( tr( "MEN_DESK_FILE" ), -1, MenuFileId, 0 );
  // Let the application developers insert some menus between Edit and View
  int editMenu = createMenu( tr( "MEN_DESK_EDIT" ), -1, MenuEditId, 5 );
  int viewMenu = createMenu( tr( "MEN_DESK_VIEW" ), -1, MenuViewId, 10 );
  int helpMenu = createMenu( tr( "MEN_DESK_HELP" ), -1, MenuHelpId, 1000 );

  // Create menu items

  createMenu( FileNewId,    fileMenu, 0 );
  createMenu( FileOpenId,   fileMenu, 0 );
  createMenu( FileReopenId, fileMenu, 0 ); 
  createMenu( FileSaveId,   fileMenu, 5 );
  createMenu( FileSaveAsId, fileMenu, 5 );
  createMenu( FileCloseId,  fileMenu, 5 );
  createMenu( separator(),  fileMenu, -1, 5 );

  createMenu( separator(),  fileMenu );
  createMenu( FileExitId,   fileMenu );

  createMenu( EditCopyId,  editMenu );
  createMenu( EditPasteId, editMenu );
  createMenu( separator(), editMenu );

  createMenu( ViewToolBarsId,  viewMenu, 0 );
  createMenu( ViewWindowsId,   viewMenu, 0 );
  createMenu( separator(),     viewMenu, -1, 10 );
  createMenu( ViewStatusBarId, viewMenu, 10 );
  createMenu( separator(),     viewMenu );

  createMenu( HelpAboutId, helpMenu );
  createMenu( separator(), helpMenu );

  // Create tool bars

  int stdTBar = createTool( tr( "INF_DESK_TOOLBAR_STANDARD" ),  // title (language-dependant)
			    QString( "SalomeStandard" ) );      // name (language-independant)

  // Create tool items

  createTool( FileNewId, stdTBar );
  createTool( FileOpenId, stdTBar );
  createTool( FileSaveId, stdTBar );
  createTool( FileCloseId, stdTBar );
  createTool( separator(), stdTBar );
  createTool( EditCopyId, stdTBar );
  createTool( EditPasteId, stdTBar );
}

/*!Opens new application*/
void STD_Application::onNewDoc()
{
  onNewDoc( QString() );
}

/*!Opens new application*/
bool STD_Application::onNewDoc( const QString& name )
{
  QApplication::setOverrideCursor( Qt::WaitCursor );

  bool res = true;
  if ( !activeStudy() )
  {
    createEmptyStudy();
    res = activeStudy()->createDocument( name );
    if ( res )
      studyCreated( activeStudy() );
    else
    {
      SUIT_Study* st = activeStudy();
      setActiveStudy( 0 );
      delete st;
    }
  }
  else
  {
    SUIT_Application* aApp = startApplication( 0, 0 );
    if ( aApp->inherits( "STD_Application" ) )
      res = ((STD_Application*)aApp)->onNewDoc( name );
    else
    {
      aApp->createEmptyStudy();
      res = aApp->activeStudy()->createDocument( name );
    }
    if ( !res )
      aApp->closeApplication();
  }

  QApplication::restoreOverrideCursor();

  return res;
}

/*!Put file name from file dialog to onOpenDoc(const QString&) function*/
void STD_Application::onOpenDoc()
{
  // It is preferrable to use OS-specific file dialog box here !!!
  QString aName = getFileName( true, QString(), getFileFilter(), QString(), 0 );
  if ( aName.isNull() )
    return;

  onOpenDoc( aName );
}

/*! \retval \c true, if document was opened successful, else \c false.*/
bool STD_Application::onOpenDoc( const QString& aName )
{
  if ( !abortAllOperations() )
    return false;

  QApplication::setOverrideCursor( Qt::WaitCursor );

  bool res = openAction( openChoice( aName ), aName );
  
  QApplication::restoreOverrideCursor();

  return res;
}

/*! Reload document from the file.*/
bool STD_Application::onReopenDoc()
{
  bool res = false;

  SUIT_Study* study = activeStudy();
  if ( study && study->isSaved() ) {
    // ask user for the confirmation
    if ( SUIT_MessageBox::question( desktop(), tr( "REOPEN_STUDY" ), tr( "REOPEN_QUESTION" ),
                                    SUIT_MessageBox::Yes | SUIT_MessageBox::No, SUIT_MessageBox::No
                                    ) == SUIT_MessageBox::No )
      return false;

    // remember study name
    QString studyName = study->studyName();

    // close study
    beforeCloseDoc( study );
    study->closeDocument( true );

    // update views / windows / status bar / title
    clearViewManagers();
    setActiveStudy( 0 );

    // delete study
    delete study;
    study = 0;
    
    // post closing actions
    afterCloseDoc();

    int aNbStudies = 0;
    QList<SUIT_Application*> apps = SUIT_Session::session()->applications();
    for ( int i = 0; i < apps.count(); i++ )
      aNbStudies += apps.at( i )->getNbStudies();

    // reload study from the file
    res = useFile( studyName ) && activeStudy();

    // if reloading is failed, close the desktop
    if ( aNbStudies && !res )
      closeApplication();
    else
    {
      updateDesktopTitle();
      updateCommandsStatus();
    }
  }
  return res;
}

/*!Virtual function. Not implemented here.*/
void STD_Application::beforeCloseDoc( SUIT_Study* )
{
}

/*!Virtual function. Not implemented here.*/
void STD_Application::afterCloseDoc()
{
}

/*!Close document, if it's possible.*/
void STD_Application::onCloseDoc( bool ask )
{
  closeDoc( ask );
}

/*!Close document, if it's possible.*/
bool STD_Application::closeDoc( bool ask )
{
  bool closePermanently = true;

  if ( ask && !isPossibleToClose( closePermanently ) )
    return false;

  return closeActiveDoc( closePermanently );
}

/*!Close document.*/
bool STD_Application::closeActiveDoc( bool permanently )
{
  SUIT_Study* study = activeStudy();
  if ( !study ) // no active study
    return true;

  beforeCloseDoc( study );

  if ( study )
    study->closeDocument( permanently );

  clearViewManagers();

  setActiveStudy( 0 );

  int aNbStudies = 0;
  QList<SUIT_Application*> apps = SUIT_Session::session()->applications();
  for ( int i = 0; i < apps.count(); i++ )
    aNbStudies += apps.at( i )->getNbStudies();

  if ( aNbStudies )
  {
    savePreferences();
    setDesktop( 0 );
  }
  else
  {
    updateDesktopTitle();
    updateCommandsStatus();
  }

  // IPAL19532: deleting study should be performed after calling setDesktop(0)
  delete study;

  afterCloseDoc();

  if ( !desktop() )
    closeApplication();
  return true;
}

/*!Check the application on closing.
 * \retval \c true if possible, else \c false
 */
bool STD_Application::isPossibleToClose( bool& closePermanently )
{
  if ( activeStudy() )
  {
    activeStudy()->abortAllOperations();
    if ( activeStudy()->isModified() )
    {
      QString sName = activeStudy()->studyName().trimmed();
      return closeAction( closeChoice( sName ), closePermanently );
    }
  }
  return true;
}

int STD_Application::closeChoice( const QString& docName )
{
  int answer = SUIT_MessageBox::question( desktop(), tr( "CLOSE_STUDY" ), tr( "CLOSE_QUESTION" ),
                                          SUIT_MessageBox::Save | SUIT_MessageBox::Discard | SUIT_MessageBox::Cancel,
                                          SUIT_MessageBox::Save );

  int res = CloseCancel;
  if ( answer == SUIT_MessageBox::Save )
    res = CloseSave;
  else if ( answer == SUIT_MessageBox::Discard )
    res = CloseDiscard;

  return res;
}

bool STD_Application::closeAction( const int choice, bool& closePermanently )
{
  bool res = true;
  switch( choice )
  {
  case CloseSave:
    if ( activeStudy()->isSaved() )
      onSaveDoc();
    else if ( !onSaveAsDoc() )
      res = false;
    break;
  case CloseDiscard:
    break;
  case CloseCancel:
  default:
    res = false;
  }

  return res;
}

int STD_Application::openChoice( const QString& aName )
{
  SUIT_Session* aSession = SUIT_Session::session();

  bool isAlreadyOpen = false;
  QList<SUIT_Application*> aAppList = aSession->applications();
  for ( QList<SUIT_Application*>::iterator it = aAppList.begin(); it != aAppList.end() && !isAlreadyOpen; ++it )
    isAlreadyOpen = (*it)->activeStudy() && (*it)->activeStudy()->studyName() == aName;
  return isAlreadyOpen ? OpenExist : OpenNew;
}

bool STD_Application::openAction( const int choice, const QString& aName )
{
  bool res = true;
  switch ( choice )
  {
  case OpenExist:
    {
      SUIT_Application* aApp = 0;
      SUIT_Session* aSession = SUIT_Session::session();
      QList<SUIT_Application*> aAppList = aSession->applications();
      for ( QList<SUIT_Application*>::iterator it = aAppList.begin(); it != aAppList.end() && !aApp; ++it )
      {
        if ( (*it)->activeStudy() && (*it)->activeStudy()->studyName() == aName )
          aApp = *it;
      }
      if ( aApp )
        aApp->desktop()->activateWindow();
      else
        res = false;
    }
    break;
  case OpenNew:
    if ( !activeStudy() )
      res = useFile( aName );
    else
    {
      SUIT_Application* aApp = startApplication( 0, 0 );
      if ( aApp )
        res = aApp->useFile( aName );
      if ( !res )
        aApp->closeApplication();
    }
    break;
  case OpenCancel:
  default:
    res = false;
  }

  return res;
}

/*!Save document if all ok, else error message.*/
void STD_Application::onSaveDoc()
{
  if ( !activeStudy() )
    return;

  if ( !abortAllOperations() )
    return;

  bool isOk = false;
  if ( activeStudy()->isSaved() )
  {
    putInfo( tr( "INF_DOC_SAVING" ) + activeStudy()->studyName() );

    QApplication::setOverrideCursor( Qt::WaitCursor );

    isOk = activeStudy()->saveDocument();

    QApplication::restoreOverrideCursor();

    if ( !isOk )
    {
      putInfo( "" );
      // displaying a message box as SUIT_Validator in case file can't be written (the most frequent case)
      SUIT_MessageBox::critical( desktop(), tr( "ERR_ERROR" ),
                                 tr( "INF_DOC_SAVING_FAILS" ).arg( activeStudy()->studyName() ) );
    }
    else
      putInfo( tr( "INF_DOC_SAVED" ).arg( "" ) );
  }

  if ( isOk )
    studySaved( activeStudy() );
  else
    onSaveAsDoc();
}

/*! \retval \c true, if document saved successfully, else \c false.*/
bool STD_Application::onSaveAsDoc()
{
  SUIT_Study* study = activeStudy();
  if ( !study )
    return false;

  if ( !abortAllOperations() )
    return false;

  bool isOk = false;
  while ( !isOk )
  {
    QString aName = getFileName( false, study->studyName(), getFileFilter(), QString(), 0 );
    if ( aName.isNull() )
      return false;

    QApplication::setOverrideCursor( Qt::WaitCursor );

    putInfo( tr( "INF_DOC_SAVING" ) + aName );
    isOk = study->saveDocumentAs( aName );

    putInfo( isOk ? tr( "INF_DOC_SAVED" ).arg( aName ) : "" );

    QApplication::restoreOverrideCursor();

    if ( !isOk )
      SUIT_MessageBox::critical( desktop(), tr( "ERROR" ), tr( "INF_DOC_SAVING_FAILS" ).arg( aName ) );
  }

  studySaved( activeStudy() );

  return isOk;
}

/*!Closing session.*/
void STD_Application::onExit()
{
  int aAnswer = SUIT_MessageBox::Ok;
  if ( exitConfirmation() )
    aAnswer = SUIT_MessageBox::question( desktop(), tr( "INF_DESK_EXIT" ), tr( "QUE_DESK_EXIT" ),
                                         SUIT_MessageBox::Ok | SUIT_MessageBox::Cancel, SUIT_MessageBox::Cancel );
  if ( aAnswer == SUIT_MessageBox::Ok )
    SUIT_Session::session()->closeSession();
}

/*!Virtual slot. Not implemented here.*/
void STD_Application::onCopy()
{
}

/*!Virtual slot. Not implemented here.*/
void STD_Application::onPaste()
{
}

/*!Sets \a theEnable for menu manager and for tool manager.*/
void STD_Application::setEditEnabled( bool theEnable )
{
  myEditEnabled = theEnable;

  QtxActionMenuMgr* mMgr = desktop()->menuMgr();
  QtxActionToolMgr* tMgr = desktop()->toolMgr();

  for ( int i = EditCopyId; i <= EditPasteId; i++ )
  {
    mMgr->setShown( mMgr->actionId(action(i)), myEditEnabled );
    tMgr->setShown( tMgr->actionId(action(i)), myEditEnabled );
  }
}

/*!\retval \c true, if document opened successfully, else \c false.*/
bool STD_Application::useFile(const QString& theFileName)
{
  bool res = SUIT_Application::useFile( theFileName );

  if ( res )
    studyOpened( activeStudy() );

  return res;
}

/*!Update desktop title.*/
void STD_Application::updateDesktopTitle()
{
  QString aTitle = applicationName();
  QString aVer = applicationVersion();
  if ( !aVer.isEmpty() )
    aTitle += QString( " " ) + aVer;

  if ( activeStudy() )
  {
    QString sName = SUIT_Tools::file( activeStudy()->studyName().trimmed(), false );
    if ( !sName.isEmpty() )
      aTitle += QString( " - [%1]" ).arg( sName );
  }

  desktop()->setWindowTitle( aTitle );
}

/*!Update commands status.*/
void STD_Application::updateCommandsStatus()
{
  SUIT_Application::updateCommandsStatus();

  bool aHasStudy     = activeStudy() != 0;
  bool aSaved        = aHasStudy && activeStudy()->isSaved();
  bool aModified     = aHasStudy && activeStudy()->isModified();
  bool aIsNeedToSave = aHasStudy && ( !aSaved || aModified );

 if ( action( FileReopenId ) )
    action( FileReopenId )->setEnabled( aSaved );
 if ( action( FileSaveId ) )
    action( FileSaveId )->setEnabled( aIsNeedToSave );
  if ( action( FileSaveAsId ) )
    action( FileSaveAsId )->setEnabled( aHasStudy );
  if ( action( FileCloseId ) )
    action( FileCloseId )->setEnabled( aHasStudy );
  if ( action( NewWindowId ) )
    action( NewWindowId )->setEnabled( aHasStudy );
}

/*!\retval SUIT_ViewManager by viewer manager type name.*/
SUIT_ViewManager* STD_Application::viewManager( const QString& vmType ) const
{
  SUIT_ViewManager* vm = 0;
  for ( QList<SUIT_ViewManager*>::const_iterator it = myViewMgrs.begin(); it != myViewMgrs.end() && !vm; ++it )
  {
    if ( (*it)->getType() == vmType )
      vm = *it;
  }
  return vm;
}

/*! \param vmType - input view manager type name
 * \param lst - output list of view managers with types \a vmType.
 */
void STD_Application::viewManagers( const QString& vmType, ViewManagerList& lst ) const
{
  for ( QList<SUIT_ViewManager*>::const_iterator it = myViewMgrs.begin(); it != myViewMgrs.end(); ++it )
  {
    if ( (*it)->getType() == vmType )
      lst.append( *it );
  }
}

/*!\param lst - output list of all view managers.*/
void STD_Application::viewManagers( ViewManagerList& lst ) const
{
  for ( QList<SUIT_ViewManager*>::const_iterator it = myViewMgrs.begin(); it != myViewMgrs.end(); ++it )
    lst.append( *it );
}

/*!\retval ViewManagerList - const list of all view managers.*/
ViewManagerList STD_Application::viewManagers() const
{
  ViewManagerList lst;
  viewManagers( lst );
  return lst;
}

/*!\retval SUIT_ViewManager - return pointer to active view manager.*/
SUIT_ViewManager* STD_Application::activeViewManager() const
{
  return myActiveViewMgr;
}

/*!Add view manager to view managers list, if it not already there.*/
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

/*!Remove view manager from view managers list.*/
void STD_Application::removeViewManager( SUIT_ViewManager* vm )
{
  if ( !vm )
    return;

  vm->closeAllViews();

  emit viewManagerRemoved( vm );

  vm->disconnectPopupRequest( this, SLOT( onConnectPopupRequest( SUIT_PopupClient*, QContextMenuEvent* ) ) );
  disconnect( vm, SIGNAL( activated( SUIT_ViewManager* ) ),
             this, SLOT( onViewManagerActivated( SUIT_ViewManager* ) ) );
  myViewMgrs.removeAll( vm );

  if ( myActiveViewMgr == vm )
    myActiveViewMgr = 0;
}

/*!Remove all view managers from view managers list.*/
void STD_Application::clearViewManagers()
{
  ViewManagerList lst;
  viewManagers( lst );

  for ( QList<SUIT_ViewManager*>::iterator it = lst.begin(); it != lst.end(); ++it )
  {
    QPointer<SUIT_ViewManager> vm = *it;
    removeViewManager( vm );
    delete vm;
  }
}

/*!\retval \c true, if view manager \a vm, already in view manager list (\a myViewMgrs).*/
bool STD_Application::containsViewManager( SUIT_ViewManager* vm ) const
{
  return myViewMgrs.contains( vm );
}

/*!Private slot, sets active manager to \vm, if \vm in view managers list.*/
void STD_Application::onViewManagerActivated( SUIT_ViewManager* vm )
{
  setActiveViewManager( vm );
}

/*!Shows status bar, if on is \c true, else hides status bar.*/
void STD_Application::onViewStatusBar( bool on )
{
  if ( on )
    desktop()->statusBar()->show();
  else
    desktop()->statusBar()->hide();
}

/*!Call SUIT_MessageBox::info1(...) with about information.*/
void STD_Application::onHelpAbout()
{
  SUIT_MessageBox::information( desktop(), tr( "About" ), tr( "ABOUT_INFO" ) );
}

/*!Create empty study. \n
 * Create new view manager and adding it to view managers list.
 */
void STD_Application::createEmptyStudy()
{
  SUIT_Application::createEmptyStudy();
}

/*!Sets active manager to \vm, if \vm in view managers list.*/
void STD_Application::setActiveViewManager( SUIT_ViewManager* vm )
{
  if ( !containsViewManager( vm ) )
    return;

  myActiveViewMgr = vm;
  emit viewManagerActivated( vm );
}

/*!Public slot. */
void STD_Application::onConnectPopupRequest( SUIT_PopupClient* client, QContextMenuEvent* e )
{
  QtxMenu* popup = new QtxMenu();
  // fill popup by own items
  QString title;
  contextMenuPopup( client->popupClientType(), popup, title );
  popup->setTitleText( title );

  popup->addSeparator();
  // add items from popup client
  client->contextMenuPopup( popup );

  SUIT_Tools::simplifySeparators( popup );

  if ( !popup->actions().isEmpty() )
    popup->exec( e->globalPos() );
  delete popup;
}

/*!\retval QString - return file name from dialog.*/
QString STD_Application::getFileName( bool open, const QString& initial, const QString& filters,
                                      const QString& caption, QWidget* parent )
{
  if ( !parent )
    parent = desktop();
  if ( open )
    return QFileDialog::getOpenFileName( parent, caption, initial, filters );
  else
  {
    QString aName;
    QString aUsedFilter;
    QString anOldPath = initial;

    bool isOk = false;
    while ( !isOk )
    {
      // It is preferrable to use OS-specific file dialog box here !!!
      aName = QFileDialog::getSaveFileName( parent, caption, anOldPath, filters, &aUsedFilter );

      if ( aName.isNull() )
        isOk = true;
      else
      {
        int aEnd = aUsedFilter.lastIndexOf( ')' );
        int aStart = aUsedFilter.lastIndexOf( '(', aEnd );
        QString wcStr = aUsedFilter.mid( aStart + 1, aEnd - aStart - 1 );

        int idx = 0;
        QStringList extList;
        QRegExp rx( "[\b\\*]*\\.([\\w]+)" );
        while ( ( idx = rx.indexIn( wcStr, idx ) ) != -1 )
        {
          extList.append( rx.cap( 1 ) );
          idx += rx.matchedLength();
        }

        if ( !extList.isEmpty() && !extList.contains( SUIT_Tools::extension( aName ) ) )
          aName += QString( ".%1" ).arg( extList.first() );

        if ( QFileInfo( aName ).exists() )
        {
          int aAnswer = SUIT_MessageBox::question( desktop(), tr( "TIT_FILE_SAVEAS" ),
                                                   tr( "MSG_FILE_EXISTS" ).arg( aName ),
                                                   SUIT_MessageBox::Yes | SUIT_MessageBox::No | SUIT_MessageBox::Cancel, SUIT_MessageBox::Yes );
          if ( aAnswer == SUIT_MessageBox::Cancel )
          {     // cancelled
            aName = QString();
            isOk = true;
          }
          else if ( aAnswer == SUIT_MessageBox::No ) // not save to this file
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

/*!\retval QString - return directory name from dialog.*/
QString STD_Application::getDirectory( const QString& initial, const QString& caption, QWidget* parent )
{
  if ( !parent )
    parent = desktop();

  return QFileDialog::getExistingDirectory( parent, caption, initial );
}

/*!
  Changes desktop
  \param desk - new desktop
*/
void STD_Application::setDesktop( SUIT_Desktop* desk )
{
  SUIT_Application::setDesktop( desk );

  if ( desk ) {
    connect( desk, SIGNAL( closing( SUIT_Desktop*, QCloseEvent* ) ),
             this, SLOT( onDesktopClosing( SUIT_Desktop*, QCloseEvent* ) ), Qt::UniqueConnection );
  }
}

/*!
  Allow to load preferences before the desktop will be shown.
*/
void STD_Application::loadPreferences()
{
}

/*!
  Allow to save preferences before the application will be closed.
*/
void STD_Application::savePreferences()
{
}

/*!
  Custom activity after study is created
  Updates desktop and actions
*/
void STD_Application::studyCreated( SUIT_Study* )
{
  updateDesktopTitle();
  updateCommandsStatus();
}

/*!
  Custom activity after study is opened
  Updates desktop and actions
*/
void STD_Application::studyOpened( SUIT_Study* )
{
  updateDesktopTitle();
  updateCommandsStatus();
}

/*!
  Custom activity after study is opened
  Updates desktop and actions
*/
void STD_Application::studySaved( SUIT_Study* )
{
  updateDesktopTitle();
  updateCommandsStatus();
}

/*!
  Return index of the view ma
*/
int STD_Application::viewManagerId( const SUIT_ViewManager* theManager) const
{
  return myViewMgrs.indexOf(const_cast<SUIT_ViewManager*>(theManager));
}

/*!
  \brief Abort active operations if there are any
  \return \c false if some operation cannot be aborted
*/
bool STD_Application::abortAllOperations()
{
  return true;
}
