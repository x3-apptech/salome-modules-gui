// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File:      QtxWebBrowser.cxx
// Author:    Roman NIKOLAEV

#include "QtxWebBrowser.h"
#include "QtxResourceMgr.h"
#include "QtxSearchTool.h"

#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWebView>

/*!
  \class QtxWebBrowser::Searcher
  \brief A class is used with QtxSearchTool in order to search text within the web page 
  \internal
*/

class QtxWebBrowser::Searcher : public QtxSearchTool::Searcher
{
public:
  Searcher( QWebView* );
  ~Searcher();

  bool find( const QString&, QtxSearchTool* );
  bool findNext( const QString&, QtxSearchTool* );
  bool findPrevious( const QString&, QtxSearchTool* );
  bool findFirst( const QString&, QtxSearchTool* );
  bool findLast( const QString&, QtxSearchTool* );

private:
  QWebView* myView;
};

/*!
  \brief Constructor
  \param view web view
  \internal
*/
QtxWebBrowser::Searcher::Searcher( QWebView* view ) : myView( view )
{
}

/*!
  \brief Destructor
  \internal
*/
QtxWebBrowser::Searcher::~Searcher()
{
}

/*!
  \brief Find specified text
  \param text text being searched
  \param st search tool
  \return \c true if text has been found or \c false otherwise
  \internal
*/
bool QtxWebBrowser::Searcher::find( const QString& text, QtxSearchTool* st )
{
  QWebPage::FindFlags fl = 0;
  if ( st->isCaseSensitive() ) fl = fl | QWebPage::FindCaseSensitively;
  if ( st->isSearchWrapped() ) fl = fl | QWebPage::FindWrapsAroundDocument;
  return myView->findText( text, fl );
}

/*!
  \brief Find next entry of specified text starting from the current position
  \param text text being searched
  \param st search tool
  \return \c true if text has been found or \c false otherwise
  \internal
*/
bool QtxWebBrowser::Searcher::findNext( const QString& text, QtxSearchTool* st )
{
  return find( text, st );
}

/*!
  \brief Find previous entry of specified text starting from the current position
  \param text text being searched
  \param st search tool
  \return \c true if text has been found or \c false otherwise
  \internal
*/
bool QtxWebBrowser::Searcher::findPrevious( const QString& text, QtxSearchTool* st )
{
  QWebPage::FindFlags fl = QWebPage::FindBackward;
  if ( st->isCaseSensitive() ) fl = fl | QWebPage::FindCaseSensitively;
  if ( st->isSearchWrapped() ) fl = fl | QWebPage::FindWrapsAroundDocument;
  return myView->findText( text, fl );
}

/*!
  \brief Find first entry of specified text; does nothing in this implementation
  \param text text being searched
  \param st search tool
  \return \c true if text has been found or \c false otherwise
  \internal
*/
bool QtxWebBrowser::Searcher::findFirst( const QString& /*text*/, QtxSearchTool* /*st*/ )
{
  return false;
}

/*!
  \brief Find last entry of specified text; does nothing in this implementation
  \param text text being searched
  \param st search tool
  \return \c true if text has been found or \c false otherwise
  \internal
*/
bool QtxWebBrowser::Searcher::findLast( const QString& /*text*/, QtxSearchTool* /*st*/)
{
  return false;
}


/*!
  \class QtxWebBrowser::Downloader
  \brief A dialog box that is used to process file links
  \internal
*/

/*!
  \brief Constructor
  \param fileName name of the file being opened
  \param action default action to be used for the file
  \param program default program to be used to open the file
  \param parent parent widget
  \internal
*/
QtxWebBrowser::Downloader::Downloader( const QString& fileName, int action, const QString& program, QWidget* parent )
  : QDialog( parent ), myProgram( program )
{
  setModal( true );
  setWindowTitle( tr( "Open URL" ) );
  setSizeGripEnabled( true );

  myFileName = new QLabel( this );
  QRadioButton* rbOpen = new QRadioButton( tr( "Open in" ), this );
  QRadioButton* rbSave = new QRadioButton( tr( "Save file" ), this );
  myBrowse = new QPushButton( tr( "&Browse..." ),     this );
  myRepeat = new QCheckBox( tr( "Use this program for all files of this type" ), this );

  myAction = new QButtonGroup( this );
  myAction->addButton( rbOpen, mOpen );
  myAction->addButton( rbSave, mSave );

  QPushButton* btnOk     = new QPushButton( tr( "&OK" ),     this );
  QPushButton* btnCancel = new QPushButton( tr( "&Cancel" ), this );

  QFont f = myFileName->font(); f.setBold( true ); myFileName->setFont( f );

  QHBoxLayout* btnLayout = new QHBoxLayout;
  btnLayout->addWidget( btnOk );
  btnLayout->addStretch();
  btnLayout->addWidget( btnCancel );

  QGridLayout* l = new QGridLayout( this );
  l->addWidget( new QLabel( tr( "You are opening the file" ), this ), 
		            0, 0, 1, 4 );
  l->addWidget( myFileName, 1, 1, 1, 3 );
  l->addWidget( new QLabel( tr( "Please choose the action to be done" ), this ), 
		            3, 0, 1, 4 );
  l->addWidget( rbOpen,     4, 1, 1, 1 );
  l->addWidget( myBrowse,   4, 2, 1, 1 );
  l->addWidget( rbSave,     5, 1, 1, 3 );
  l->addWidget( myRepeat,   6, 1, 1, 3 );
  l->addLayout( btnLayout,  7, 0, 1, 4 );
  l->setRowMinimumHeight( 2, 10 );

  connect( myAction,  SIGNAL( buttonClicked( int ) ), this, SLOT( setAction( int ) ) );
  connect( myBrowse,  SIGNAL( clicked() ), this, SLOT( browse() ) );
  connect( btnOk,     SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( btnCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

  myFileName->setText( QFileInfo( fileName ).fileName() );
  myAction->button( action )->click();
}

/*!
  \brief Destructor
*/
QtxWebBrowser::Downloader::~Downloader()
{
}

/*!
  \brief Get action selected by the user
  \return action being selected:
  - 0: open file
  - 1: save file
*/
int QtxWebBrowser::Downloader::action() const
{
  return myAction->checkedId();
}

/*!
  \brief Get "repeat action for all such files" flag status
  \return \c true if chosen action should be automatically done for all files of given type
  or \c false otherwise
*/
bool QtxWebBrowser::Downloader::isRepeatAction() const
{
  return myRepeat->isChecked();
}

/*!
  \brief Get program to be used to open chosen file
  \return path to the program
*/
QString QtxWebBrowser::Downloader::program() const
{
  return myProgram;
}

/*!
  \brief Set current action
  \param action action to be done for the file:
  - 0: open file
  - 1: save file
*/
void QtxWebBrowser::Downloader::setAction( int action )
{
  myBrowse->setEnabled( action == mOpen );
}

/*!
  \brief Browse program to be used to open the file
*/
void QtxWebBrowser::Downloader::browse()
{
  QString program = QFileDialog::getOpenFileName( this, tr( "Choose program" ), myProgram );
  if ( !program.isEmpty() ) myProgram = program;
}


/*!
  \class QtxWebBrowser

  \brief The QtxWebBrowser provides a window that can display html pages.
  
  Only one instance of the QtxWebBrowser class can be created. To access the browser 
  window, use static method QtxWebBrowser::webBrowser(), which creates an
  instance of the QtxWebBrowser widget (if it is not yet created) and returns a
  pointer to it.

  You should not destroy this instance - it is done automatically after
  closing of the browser window. To close window programmatically use 
  method close().

  To set visual properties of the browser use static method setData().

  Optionally resource manager can be specified to automatically store
  action (open/save) and program to be used to download files to the
  user preferences.

  The following sample demonstrates how to use web browser.
  In this code the browser window is created, /data/index.html file is opened
  and scrolled to the "anchor1" anchor on this page.

  \code
  int main(int argc, char *argv[])
  {
    QApplication app(argc, argv);    

    // set resource manager
    QtxWebBrowser::setResourceManager(myResourceMgr);
    // set icon, title and menu items
    QtxWebBrowser::setData("browser:title",      tr("Web Browser"));
    QtxWebBrowser::setData("browser:icon",       QPixmap(":/icon.png"));
    QtxWebBrowser::setData("menu:file:title",    tr("&File"));
    QtxWebBrowser::setData("action:close:title", tr("&Close"));

    // show HTML page
    QtxWebBrowser::loadUrl("file:///data/index.html", "anchor1");
    
    return app.exec();
  }
  \endcode

*/

//! The only one instance of web browser
QtxWebBrowser* QtxWebBrowser::myBrowser = 0;

//! Resources manager
QtxResourceMgr* QtxWebBrowser::myResourceMgr = 0;

//! Internal data map to store resources of the browser.
QMap<QString, QVariant> QtxWebBrowser::myData;

/*!
  \brief Constructor.
  Construct the web browser.
*/
QtxWebBrowser::QtxWebBrowser( ) : QMainWindow( 0 )
{
  setAttribute( Qt::WA_DeleteOnClose );
  statusBar();

  QWidget* frame = new QWidget( this );

  myWebView = new QWebView( frame );

  QAction *copyAction = myWebView->pageAction(QWebPage::Copy);
  copyAction->setShortcut(QKeySequence::Copy);
  myWebView->addAction(copyAction);


  myWebView->page()->setLinkDelegationPolicy( QWebPage::DelegateAllLinks );
  myFindPanel = new QtxSearchTool( frame, myWebView,
				   QtxSearchTool::Basic | QtxSearchTool::Case | QtxSearchTool::Wrap, 
				   Qt::Horizontal );
  myFindPanel->setFrameStyle( QFrame::NoFrame | QFrame::Plain );
  myFindPanel->setActivators( QtxSearchTool::SlashKey );
  myFindPanel->setSearcher( new Searcher( myWebView ) );
  myFindPanel->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

  myToolbar = addToolBar( tr( "Navigation" ) );
  myToolbar->addAction( myWebView->pageAction( QWebPage::Back ) );
  myToolbar->addAction( myWebView->pageAction( QWebPage::Forward ) );

  myMenus[ File ]        = menuBar()->addMenu( tr( "&File" ) );
  myActions[ Find ]      = myMenus[ File ]->addAction( tr( "&Find in text..." ), myFindPanel, SLOT( find() ),         QKeySequence( QKeySequence::Find ) );
  myActions[ FindNext ]  = myMenus[ File ]->addAction( tr( "&Find next" ),       myFindPanel, SLOT( findNext() ),     QKeySequence( QKeySequence::FindNext ) );
  myActions[ FindPrev ]  = myMenus[ File ]->addAction( tr( "&Find previous" ),   myFindPanel, SLOT( findPrevious() ), QKeySequence( QKeySequence::FindPrevious ) );
  myMenus[ File ]->addSeparator();
  myActions[ Close ]     = myMenus[ File ]->addAction( tr( "&Close" ),           this, SLOT( close() ) );

  QVBoxLayout* main = new QVBoxLayout( frame );
  main->addWidget( myWebView );
  main->addWidget( myFindPanel );
  main->setMargin( 0 );
  main->setSpacing( 3 );

  connect( myWebView, SIGNAL( titleChanged( QString ) ), SLOT( adjustTitle() ) ); 
  connect( myWebView, SIGNAL( loadFinished( bool ) ),    SLOT( finished( bool ) ) ); 
  connect( myWebView, SIGNAL( linkClicked( QUrl ) ),     SLOT( linkClicked( QUrl ) ) ); 
  connect( myWebView->page(), SIGNAL( linkHovered( QString, QString, QString ) ), 
	   SLOT( linkHovered( QString, QString, QString ) ) ); 
  connect( myWebView->pageAction( QWebPage::DownloadLinkToDisk ), SIGNAL( activated() ),
	   SLOT( linkAction() ) );
  disconnect( myWebView->pageAction( QWebPage::OpenLink ), 0, 0, 0 );
  connect( myWebView->pageAction( QWebPage::OpenLink ), SIGNAL( activated() ),
	   SLOT( linkAction() ) );
  

  myWebView->pageAction( QWebPage::OpenLinkInNewWindow )->setVisible( false );

  setCentralWidget( frame );
  setFocusProxy( myWebView );
  updateData();
  qAddPostRoutine( QtxWebBrowser::clearData );
}

/*!
  \brief Destructor.
*/
QtxWebBrowser::~QtxWebBrowser()
{
  myBrowser = 0;
}

/*!
  \brief Return the only instance of the QtxWebBrowser
  \return instance of the QtxWebBrowser
*/
QtxWebBrowser* QtxWebBrowser::webBrowser()
{
  if ( !myBrowser )
    myBrowser = new QtxWebBrowser();
  return myBrowser;
}

/*!
  \brief Load given url address and optional scroll to the specified anchor
  \param url an url address to load
  \param anchor an anchor to scroll page to
*/
void QtxWebBrowser::loadUrl( const QString& url, const QString& anchor )
{
  QString anUrl = url;
  if( !anchor.isEmpty() ) anUrl += "#" + anchor;
  anUrl.replace('\\', '/');

  Qtx::alignWidget( webBrowser(), (QWidget*)QApplication::desktop(), Qtx::AlignCenter );

  QtxWebBrowser* browser = webBrowser();
  browser->show();
  browser->myWebView->load( QUrl( anUrl ) );
  browser->setFocus();
  browser->activateWindow();
  browser->raise();
}

/*!
  \brief  Set browser settings from.

  This method can be used to setup the browser properties.
  - \c "browser:title"         : title of the browser window
  - \c "browser:icon"          : icon of the browser window
  - \c "toolbar:title"         : title of the toolbar
  - \c "menu:file:title"       : File menu of the browser
  - \c "action:close:title"    : File/Close menu item title
  - \c "action:close:icon"     : File/Close menu item icon
  - \c "action:back:title"     : Navigation/Back menu item title
  - \c "action:back:icon"      : Navigation/Back menu item icon
  - \c "action:forward:title"  : Navigation/Forward menu item title
  - \c "action:forward:icon"   : Navigation/Forward menu item icon
  - \c "action:find:title"     : File/Find menu item title
  - \c "action:find:icon"      : File/Find menu item icon
  - \c "action:findnext:title" : File/Find Next menu item title
  - \c "action:findnext:icon"  : File/Find Next menu item icon
  - \c "action:findprev:title" : File/Find Previous menu item title
  - \c "action:findprev:icon"  : File/Find Previous menu item icon
  - \c "preferences:section"   : Preferences file section (base, used as prefix to the file extension)
  - \c "preferences:action"    : Preferences file parameter name for action
  - \c "preferences:program"   : Preferences file parameter name for program
  - \c "preferences:repeat"    : Preferences file parameter name for repeat action flag
  
  \param key name of the property
  \param val value of the property
  
*/
void QtxWebBrowser::setData( const QString& key, const QVariant& val )
{
  myData.insert( key, val );
  if ( myBrowser ) myBrowser->updateData();
}

/*!
  \brief Shutdown help browser
*/
void QtxWebBrowser::shutdown()
{
  if ( myBrowser )
    myBrowser->close();
}

/*!
  \brief Get string value by key from the internal data map
  \param key data key identifier
  \param def default value
  \return string value assigned to the key (null string if data is not assigned to the key)
  \internal
*/
QString QtxWebBrowser::getStringValue( const QString& key, const QString& def )
{
  QString val = def;
  if ( myData.contains( key ) && myData[key].canConvert( QVariant::String ) )
    val = myData[key].toString();
  return val;
}

/*!
  \brief Get icon value by key from the internal data map
  \param key data key identifier
  \param def default value
  \return icon assigned to the key (null icon if data is not assigned to the key)
  \internal
*/
QIcon QtxWebBrowser::getIconValue( const QString& key, const QIcon& def )
{
  QIcon val = def;
  if ( myData.contains( key ) ) {
    if ( myData[key].canConvert( QVariant::Pixmap ) )
      val = myData[key].value<QPixmap>();
    else if ( myData[key].canConvert( QVariant::Icon ) )
      val = myData[key].value<QIcon>();
  }
  return val;
}

/*!
  \brief Update web browser properties from internal data map
*/
void QtxWebBrowser::updateData()
{
  // main title
  adjustTitle();

  // window icon
  QIcon icon = getIconValue( "browser:icon" );
  if ( !icon.isNull() )
    setWindowIcon( icon );

  // toolbar title
  QString tbTitle = getStringValue( "toolbar:title" );
  if ( myToolbar && !tbTitle.isEmpty() )
    myToolbar->setWindowTitle( tbTitle );

  // File menu
  QString fmenu = getStringValue( "menu:file:title" );
  if ( myMenus.contains( File ) && !fmenu.isEmpty() )
    myMenus[ File ]->setTitle( fmenu );

  // File/Close menu
  QString closeTlt = getStringValue( "action:close:title" );
  QIcon closeIco = getIconValue( "action:close:icon" );
  if ( myActions.contains( Close ) ) {
    if ( !closeTlt.isEmpty() )
      myActions[ Close ]->setText( closeTlt );
    if ( !closeIco.isNull() )
      myActions[ Close ]->setIcon( closeIco );
  }

  // Navigation/Go Back menu
  QString backTlt = getStringValue( "action:back:title" );
  QIcon backIco = getIconValue( "action:back:icon" );
  if ( !backTlt.isEmpty() )
    myWebView->pageAction( QWebPage::Back )->setText( backTlt );
  if ( !backIco.isNull() )
    myWebView->pageAction( QWebPage::Back )->setIcon( backIco );

  // Navigation/Go Forward menu
  QString fwdTlt = getStringValue( "action:forward:title" );
  QIcon fwdIco = getIconValue( "action:forward:icon" );
  if ( !fwdTlt.isEmpty() )
    myWebView->pageAction( QWebPage::Forward )->setText( fwdTlt );
  if ( !fwdIco.isNull() )
    myWebView->pageAction( QWebPage::Forward )->setIcon( fwdIco );

  // File/Find menu
  QString findTlt = getStringValue( "action:find:title" );
  QIcon findIco = getIconValue( "action:find:icon" );
  if ( myActions.contains( Find ) ) {
    if ( !findTlt.isEmpty() )
      myActions[ Find ]->setText( findTlt );
    if ( !findIco.isNull() )
      myActions[ Find ]->setIcon( findIco );
  }

  // File/Find Next menu
  QString findNextTlt = getStringValue( "action:findnext:title" );
  QIcon findNextIco = getIconValue( "action:findnext:icon" );
  if ( myActions.contains( FindNext ) ) {
    if ( !findNextTlt.isEmpty() )
      myActions[ FindNext ]->setText( findNextTlt );
    if ( !findNextIco.isNull() )
      myActions[ FindNext ]->setIcon( findNextIco );
  }

  // File/Find Previous menu
  QString findPrevTlt = getStringValue( "action:findprev:title" );
  QIcon findPrevIco = getIconValue( "action:findprev:icon" );
  if ( myActions.contains( FindPrev ) ) {
    if ( !findPrevTlt.isEmpty() )
      myActions[ FindPrev ]->setText( findPrevTlt );
    if ( !findPrevIco.isNull() )
      myActions[ FindPrev ]->setIcon( findPrevIco );
  }
}

/*!
  \brief Clear internal data map
  \internal
*/
void QtxWebBrowser::clearData()
{
  myData.clear();
}

/*!
  \brief Set resource manager
*/
void QtxWebBrowser::setResourceManager( QtxResourceMgr* resMgr )
{
  myResourceMgr = resMgr;
}

/*!
  \brief Called when users activated any link at the page
  \param url URL being clicked
  \internal
*/
void QtxWebBrowser::linkClicked( const QUrl& url )
{
  myWebView->page()->setLinkDelegationPolicy( QWebPage::DontDelegateLinks );
  myWebView->load( url );
  myWebView->page()->setLinkDelegationPolicy( QWebPage::DelegateAllLinks );
}

/*!
  \brief Called when link is hovered
  \param link link being hovered
  \param title link title (if it is specified in the markup)
  \param content provides text within the link element, e.g., text inside an HTML anchor tag
  \internal
*/
void QtxWebBrowser::linkHovered( const QString& link, const QString& /*title*/, const QString& /*context*/ )
{
  QUrl url = link;
  if ( !link.isEmpty() && url.scheme() == "file" ) myLastUrl = url;
  statusBar()->showMessage( link );
}

/*!
  \brief Update title of the window
  \internal
*/
void QtxWebBrowser::adjustTitle()
{
  QString title = getStringValue( "browser:title" );
  setWindowTitle( title.isEmpty() ? myWebView->title() : title + QString( " [%1]" ).arg( myWebView->title() ) );
}

/*
  \brief Called when link is processed by browser
  \param ok operation status: \c true is URL is correctly processed, \c false otherwise
*/
void QtxWebBrowser::finished( bool ok )
{
  if ( !ok && !myLastUrl.isEmpty() ) {
    if ( myLastUrl.scheme() == "file" ) {
      QString filename = myLastUrl.toLocalFile();
      QString extension = QFileInfo( filename ).suffix();
      if ( extension == "html" || extension == "htm" ) return;
      openLink( filename );
    }
  }
}

/*
  \brief Called when link is processed from browser via popup menu actions
*/
void QtxWebBrowser::linkAction()
{
  QObject* s = sender();
  if ( s == myWebView->pageAction( QWebPage::DownloadLinkToDisk ) ) {
    saveLink( myLastUrl.toLocalFile() );
  }
  else if ( s == myWebView->pageAction( QWebPage::OpenLink ) ) {
    QString fileName  = myLastUrl.toLocalFile();
    QString extension = QFileInfo( fileName ).suffix();
    if ( extension != "html" && extension != "htm" ) {
      openLink( fileName, true );
    }
    else {
      linkClicked( myLastUrl );
    }
  }
}

/*!
  \brief Open file
  \param fileName link to the file being opened
  Opens dialog box to allow the user to choose the program to be used to open the file.
*/
void QtxWebBrowser::openLink( const QString& fileName, bool force )
{
  QString extension = QFileInfo( fileName ).suffix();
  int defAction = Downloader::mOpen;
  bool defRepeat = false;
  QString defProgram;
  QString resSection   = QString( "%1:%2" ).arg( getStringValue( "preferences:section", "web_browser" ) ).arg( extension );
  QString actionParam  = getStringValue( "preferences:action",  "action" );
  QString programParam = getStringValue( "preferences:program", "program" );
  QString repeatParam  = getStringValue( "preferences:repeat",  "repeat" );
  
  if ( !extension.isEmpty() && myResourceMgr ) {
    defAction  = myResourceMgr->integerValue( resSection, actionParam, defAction );
    defRepeat  = myResourceMgr->booleanValue( resSection, repeatParam, defRepeat );
    defProgram = myResourceMgr->stringValue( resSection, programParam, defProgram );
  }
  
  if ( force || !defRepeat || ( defAction == Downloader::mOpen && defProgram.isEmpty() ) ) {
    Downloader downloader( fileName, defAction, defProgram, this );
    if ( !downloader.exec() ) return;
    defAction  = downloader.action();
    defRepeat  = downloader.isRepeatAction();
    defProgram = downloader.program();
    if ( myResourceMgr ) {
      myResourceMgr->setValue( resSection, actionParam, defAction );
      myResourceMgr->setValue( resSection, repeatParam, defRepeat );
      if ( defAction == Downloader::mOpen )
	myResourceMgr->setValue( resSection, programParam, defProgram );
    }
  }
  switch( defAction ) {
  case Downloader::mOpen:
    if ( !defProgram.isEmpty() ) {
#ifdef WIN32
      QString cmd = "";
#else
      // If Salome Qt version is lower than the system one, on KDE an unresolved symbol is raised
      // In this case, we can try to launch the pdf viewer after unsetting the LD_LIBRARY_PATH environnement variable
      QString cmd = "env LD_LIBRARY_PATH=/usr/lib:/usr/lib64";
#endif
      int r = ::system( QString( "%1 %2 %3 &" ).arg( cmd ).arg( defProgram ).arg( myLastUrl.toLocalFile() ).toLatin1().constData() );
    }
    break;
  case Downloader::mSave:
    {
      saveLink( fileName );
    }
    break;
  default:
    break;
  }
}

/*!
  \brief Save file
  \param fileName link to the file being saved
  Shows "Save file" standard dialog box to allow user to choose where to save the file.
*/
void QtxWebBrowser::saveLink( const QString& fileName )
{
  QString newFileName = QFileDialog::getSaveFileName( this, tr( "Save File" ), fileName, 
						      QString( "*.%1" ).arg( QFileInfo( fileName ).suffix() ) );
  if ( !newFileName.isEmpty() && 
       QFileInfo( newFileName ).canonicalFilePath() != QFileInfo( fileName ).canonicalFilePath() ) {
    QFile toFile( newFileName );
    QFile fromFile( fileName );
    if ( toFile.exists() && !toFile.remove() || !fromFile.copy( newFileName ) )
      QMessageBox::warning( this, tr( "Error"), tr( "Can't save file:\n%1" ).arg( newFileName ) );
  }
}
