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
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
  #include <QWebView>
#else
  #include <QWebEngineView>
#endif
#include <QProcess>

// RNV:
// Since from Qt 5.6.0 version QtWebKit tool was removed,
// QtxWebBroswer is ported on QtWebEngine. So if it is built with Qt-5.6.0
// and newer, it uses QtWebEngine. But for Qt-5.5.1 and Qt4 QtWebKit tool
// is used, to provide backward compatibility.

namespace
{
  bool isLocalFile( const QUrl& url )
  {
    QFileInfo fi( url.path() );
    return fi.exists();
  }
}

/*!
  \class QtxWebBrowser::Searcher
  \brief A class is used with QtxSearchTool in order to search text within the web page 
  \internal
*/

class QtxWebBrowser::Searcher : public QtxSearchTool::Searcher
{
public:
  Searcher( WebView* );
  ~Searcher();

  bool find( const QString&, QtxSearchTool* );
  bool findNext( const QString&, QtxSearchTool* );
  bool findPrevious( const QString&, QtxSearchTool* );
  bool findFirst( const QString&, QtxSearchTool* );
  bool findLast( const QString&, QtxSearchTool* );

private:
  WebView* myView;
};

/*!
  \brief Constructor
  \param view web view
  \internal
*/
QtxWebBrowser::Searcher::Searcher( WebView* view ) : myView( view )
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
  WebPage::FindFlags fl = 0;
  if ( st->isCaseSensitive() ) fl = fl | WebPage::FindCaseSensitively;
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0) 
  if ( st->isSearchWrapped() ) fl = fl | WebPage::FindWrapsAroundDocument;
  return myView->findText( text, fl );
#else
  myView->findText( text, fl, [this](bool found) { return found; });  
#endif    
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
  WebPage::FindFlags fl = WebPage::FindBackward;
  if ( st->isCaseSensitive() ) fl = fl | WebPage::FindCaseSensitively;
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0) 
  if ( st->isSearchWrapped() ) fl = fl | WebPage::FindWrapsAroundDocument;
  return myView->findText( text, fl );
#else
  myView->findText( text, fl, [this](bool found) { return found; });
#endif 
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
  setWindowTitle( QtxWebBrowser::tr( "Open URL" ) );
  setSizeGripEnabled( true );

  myFileName = new QLabel( this );
  QRadioButton* rbOpen = new QRadioButton( QtxWebBrowser::tr( "Open in" ), this );
  QRadioButton* rbSave = new QRadioButton( QtxWebBrowser::tr( "Save file" ), this );
  myBrowse = new QPushButton( QtxWebBrowser::tr( "&Browse..." ),     this );
  myRepeat = new QCheckBox( QtxWebBrowser::tr( "Use this program for all files of this type" ), this );

  myAction = new QButtonGroup( this );
  myAction->addButton( rbOpen, mOpen );
  myAction->addButton( rbSave, mSave );

  QPushButton* btnOk     = new QPushButton( QtxWebBrowser::tr( "&OK" ),     this );
  QPushButton* btnCancel = new QPushButton( QtxWebBrowser::tr( "&Cancel" ), this );

  QFont f = myFileName->font(); f.setBold( true ); myFileName->setFont( f );

  QHBoxLayout* btnLayout = new QHBoxLayout;
  btnLayout->addWidget( btnOk );
  btnLayout->addStretch();
  btnLayout->addWidget( btnCancel );

  QGridLayout* l = new QGridLayout( this );
  l->addWidget( new QLabel( QtxWebBrowser::tr( "You are opening the file" ), this ), 
		            0, 0, 1, 4 );
  l->addWidget( myFileName, 1, 1, 1, 3 );
  l->addWidget( new QLabel( QtxWebBrowser::tr( "Please choose the action to be done" ), this ), 
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
  QString program = QFileDialog::getOpenFileName( this, QtxWebBrowser::tr( "Choose program" ), myProgram );
  if ( !program.isEmpty() ) myProgram = program;
}


/*!
  \class QtxWebBrowser

  \brief The QtxWebBrowser provides a window that can display html pages from local file system.
  
  Only one instance of the QtxWebBrowser class can be created. To access the browser 
  window, use static method QtxWebBrowser::webBrowser(), which creates an
  instance of the QtxWebBrowser widget (if it is not yet created) and returns a
  pointer to it.

  You should not destroy this instance - it is done automatically after
  closing of the browser window. To close window programmatically use 
  method close().

  Optionally resource manager can be specified to automatically store
  action (open/save) and program to be used to download files in the
  application preferences.

  The following sample demonstrates how to use web browser.
  In this code the browser window is created, /data/index.html file is opened
  and scrolled to the "anchor1" anchor on this page.

  \code
  // initialize application
  // - set resource manager
  QtxWebBrowser::setResourceMgr(myResourceMgr);
  // ...
  // show HTML page
  QtxWebBrowser::loadUrl("file:///data/index.html", "anchor1");
  \endcode
*/

//! The only one instance of web browser
QtxWebBrowser* QtxWebBrowser::myBrowser = 0;

//! Resources manager
QtxResourceMgr* QtxWebBrowser::myResourceMgr = 0;

/*!
  \brief Constructor.
  Construct the web browser.
*/
QtxWebBrowser::QtxWebBrowser( ) : QMainWindow( 0 )
{
  Q_INIT_RESOURCE( Qtx );

  setAttribute( Qt::WA_DeleteOnClose );
  statusBar();

  QWidget* frame = new QWidget( this );

  myWebView = new WebView( frame );

  myWebView->pageAction( WebPage::Copy )->setShortcut( QKeySequence::Copy );
  myWebView->addAction( myWebView->pageAction( WebPage::Copy ) );
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
  myWebView->pageAction( WebPage::OpenLinkInNewWindow )->setVisible( false );
#endif  
  myWebView->pageAction( WebPage::Back )->setText( tr( "Go Back" ) );
  myWebView->pageAction( WebPage::Forward )->setText( tr( "Go Forward" ) );
  myWebView->pageAction( WebPage::Reload )->setText( tr( "Refresh" ) );
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
  myWebView->page()->setLinkDelegationPolicy( WebPage::DelegateAllLinks );
#endif
  
  myFindPanel = new QtxSearchTool( frame, myWebView,
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
				   QtxSearchTool::Basic | QtxSearchTool::Case | QtxSearchTool::Wrap,
#else
				   QtxSearchTool::Basic | QtxSearchTool::Case,
#endif				   
				   Qt::Horizontal );
  myFindPanel->setFrameStyle( QFrame::NoFrame | QFrame::Plain );
  myFindPanel->setActivators( QtxSearchTool::SlashKey );
  myFindPanel->setSearcher( new Searcher( myWebView ) );
  myFindPanel->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

  QToolBar* toolbar = addToolBar( tr( "Navigation" ) );
  toolbar->addAction( myWebView->pageAction( WebPage::Back ) );
  toolbar->addAction( myWebView->pageAction( WebPage::Forward ) );
  toolbar->addAction( myWebView->pageAction( WebPage::Reload ) );

  QMenu* fileMenu = menuBar()->addMenu( tr( "&File" ) );
  fileMenu->addAction( QPixmap( ":/images/open.png" ), tr( "&Open..." ), 
		       this, SLOT( open() ),
		       QKeySequence( QKeySequence::Open ) );
  fileMenu->addSeparator();
  fileMenu->addAction( myWebView->pageAction( WebPage::Back ) );
  fileMenu->addAction( myWebView->pageAction( WebPage::Forward ) );
  fileMenu->addAction( myWebView->pageAction( WebPage::Reload ) );
  fileMenu->addSeparator();
  fileMenu->addAction( tr( "&Find in text..." ),
		       myFindPanel, SLOT( find() ),
		       QKeySequence( QKeySequence::Find ) );
  fileMenu->addAction( tr( "&Find next" ),
		       myFindPanel, SLOT( findNext() ),
		       QKeySequence( QKeySequence::FindNext ) );
  fileMenu->addAction( tr( "&Find previous" ),
		       myFindPanel, SLOT( findPrevious() ),
		       QKeySequence( QKeySequence::FindPrevious ) );
  fileMenu->addSeparator();
  fileMenu->addAction( QPixmap( ":/images/close.png" ), tr( "&Close" ),
		       this, SLOT( close() ) );

  QMenu* helpMenu = menuBar()->addMenu( tr( "&Help" ) );
  helpMenu->addAction( tr( "&About..." ),
		       this, SLOT( about() ) );
  
  QVBoxLayout* main = new QVBoxLayout( frame );
  main->addWidget( myWebView );
  main->addWidget( myFindPanel );
  main->setMargin( 0 );
  main->setSpacing( 3 );

  connect( myWebView, SIGNAL( titleChanged( QString ) ), SLOT( adjustTitle() ) ); 
  connect( myWebView, SIGNAL( loadFinished( bool ) ),    SLOT( finished( bool ) ) );
  
  connect( myWebView->pageAction( WebPage::DownloadLinkToDisk ), SIGNAL( triggered() ),
	   SLOT( linkAction() ) );
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
  //QtWebKit case:
  connect( myWebView, SIGNAL( linkClicked( QUrl ) ),     SLOT( linkClicked( QUrl ) ) );
  connect( myWebView->page(), SIGNAL( linkHovered( QString, QString, QString ) ), 
	   SLOT( linkHovered( QString, QString, QString ) ) ); 
  disconnect( myWebView->pageAction( WebPage::OpenLink ), 0, 0, 0 );
  connect( myWebView->pageAction( WebPage::OpenLink ), SIGNAL( triggered() ),
	   SLOT( linkAction() ) );
#else
  //QtWebEngine (Qt-5.6.0) case:
  connect( myWebView->page(), SIGNAL( linkHovered( QString ) ), 
	   SLOT( linkHovered( QString ) ) );
  disconnect( myWebView->pageAction( WebPage::OpenLinkInThisWindow ), 0, 0, 0 );
  connect( myWebView->pageAction( WebPage::OpenLinkInThisWindow ), SIGNAL( triggered() ),
	   SLOT( linkAction() ) );
#endif  
  setCentralWidget( frame );
  setFocusProxy( myWebView );
  setWindowIcon( QPixmap( ":/images/appicon.png" ) );
  adjustTitle();
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

  Qtx::alignWidget( webBrowser(), (QWidget*)QApplication::desktop(), Qtx::AlignCenter );

  QtxWebBrowser* browser = webBrowser();
  browser->show();
  browser->load( anUrl );
  browser->setFocus();
  browser->activateWindow();
  browser->raise();
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
  \brief Set resource manager
  \param resMgr resource manager
*/
void QtxWebBrowser::setResourceMgr( QtxResourceMgr* resMgr )
{
  myResourceMgr = resMgr;
}

/*!
  \brief Get resource manager
  \return resource manager
*/
QtxResourceMgr* QtxWebBrowser::resourceMgr() const
{
  return myResourceMgr;
}

/*!
  Shows About dialog box
*/
void QtxWebBrowser::about()
{
  QMessageBox::about( this, tr( "About %1" ).arg( tr( "Help Browser" ) ),
		      QString( "SALOME %1" ).arg( tr( "Help Browser" ) ) );
}

/*!
  \brief Called when users activated any link at the page
  \param url URL being clicked
  \internal
*/
void QtxWebBrowser::linkClicked( const QUrl& url )
{
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
  myWebView->page()->setLinkDelegationPolicy( WebPage::DontDelegateLinks );
#endif  
  myWebView->load( url );
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)  
  myWebView->page()->setLinkDelegationPolicy( WebPage::DelegateAllLinks );
#endif  
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
  linkHovered(link);
}

void QtxWebBrowser::linkHovered( const QString& link)
{
  QUrl url = link;
  if ( !link.isEmpty() && isLocalFile( url ) ) myLastUrl = url;
  statusBar()->showMessage( link );  
}

/*!
  \brief Update title of the window
  \internal
*/
void QtxWebBrowser::adjustTitle()
{
  QString title = tr( "Help Browser" );
  if ( !myWebView->title().isEmpty() ) title += QString( " [%1]" ).arg( myWebView->title() );
  setWindowTitle( title );
}

/*
  \brief Called when link is processed by browser
  \param ok operation status: \c true is URL is correctly processed, \c false otherwise
*/
void QtxWebBrowser::finished( bool ok )
{
  if ( !ok && !myLastUrl.isEmpty() ) {
    if ( isLocalFile( myLastUrl ) ) {
      QString filename = myLastUrl.path();
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
  if ( s == myWebView->pageAction( WebPage::DownloadLinkToDisk ) ) {
    saveLink( myLastUrl.path() );
  }
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
  if ( s == myWebView->pageAction( WebPage::OpenLink ) ) {
#else
  if ( s == myWebView->pageAction( WebPage::OpenLinkInThisWindow ) ) {  
#endif    
    QString fileName  = myLastUrl.path();
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
  QString resSection   = QString( "%1:%2" ).arg( "web_browser" ).arg( extension );
  QString actionParam  = "action";
  QString programParam = "program";
  QString repeatParam  = "repeat";
  
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
      QStringList parameters;
#ifdef WIN32
      QString cmd = defProgram;
#else
      // If Salome Qt version is lower than the system one, on KDE an unresolved symbol is raised
      // In this case, we can try to launch the pdf viewer after unsetting the LD_LIBRARY_PATH environnement variable
      QString cmd = "env";
      parameters << "LD_LIBRARY_PATH=/usr/lib:/usr/lib64";
      parameters << defProgram;
#endif
      parameters << QFileInfo( myLastUrl.path() ).absoluteFilePath();
      QProcess::startDetached( cmd, parameters );
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
  \brief Load URL
  \param url path to the file to be opened in the browser
*/
void QtxWebBrowser::load( const QString& link )
{
  QString linkPath = link;
  linkPath.replace('\\', '/');
  QUrl url = linkPath;
  if ( !url.isEmpty() ) {
    if ( url.scheme().isEmpty() ) url.setScheme( "file" );
    myWebView->load( url );
  }
}

/*!
  \brief Save file
  \param fileName link to the file being saved
  Shows "Save file" standard dialog box to allow user to choose where to save the file.
*/
void QtxWebBrowser::saveLink( const QString& fileName )
{
  QString newFileName = QFileDialog::getSaveFileName( this, tr( "Save file" ), fileName, 
						      QString( "*.%1" ).arg( QFileInfo( fileName ).suffix() ) );
  if ( !newFileName.isEmpty() && 
       QFileInfo( newFileName ).canonicalFilePath() != QFileInfo( fileName ).canonicalFilePath() ) {
    QFile toFile( newFileName );
    QFile fromFile( fileName );
    if ( toFile.exists() && !toFile.remove() || !fromFile.copy( newFileName ) )
      QMessageBox::warning( this, tr( "Error"), tr( "Can't save file:\n%1" ).arg( newFileName ) );
  }
}

/*!
  \brief Open file
  Shows "Open file" standard dialog box to allow user to choose file to open.
*/
void QtxWebBrowser::open()
{
  QString url;
  if ( isLocalFile( myWebView->url() ) ) url = myWebView->url().path();
  url = QFileDialog::getOpenFileName( this, tr( "Open file" ), url, "HTML files (*.html *.htm);; All files (*)" );
  if ( !url.isEmpty() ) load( url );
}
