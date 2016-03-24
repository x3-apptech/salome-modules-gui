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

// File   : SUIT_FileDlg.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
/*!
  \class SUIT_FileDlg
  \brief An extension of the Qt Open/Save file dialog box.

  The class SUIT_FileDlg provides a set of static methods which canbe used
  for file or directories selection:
  - getFileName() for single file opening or saving
  - getOpenFileNames() for mulktiple files opening
  - getExistingDirectory() for existing directory selection

  Examples:
  \code
  // select file to dump contents of the view
  QStringList filters;
  filters << "Image files (*.bmp *.gif *.jpg )" << "All files (*)";
  QString fileName = SUIT_FileDlg::getFileName( desktop(), 
                                                QString(), 
                                                filters, 
                                                "Dump view",
                                                false );
  if ( !fileName.isEmpty() ) {
    ... writing image to the file 
  }

  // select list of files to open in the editor windows
  QStringList filters;
  filters << "*.cpp | *.cxx | *.c++" << "*.h | *.hpp | *.hxx";
  QStringList fileNames = SUIT_FileDlg::getOpenFileName( desktop(),
                                                         QString(), 
                                                         filters, 
                                                         QString() );
  if ( !fileNames.isEmpty() ) {
    ... open files
  }
  \endcode

  The class SUIT_FileDlg can be subclassed to implement custom file 
  dialog boxes. The class provides a set of methods which can be used
  in subclasses:
  - setCheckPermissions() - to enable/disable check of files/directories
    permissions
  - setValidator() - to use custom file validator
  - addWidgets() - to add custom widgets to the lower part of the 
    dialog box
  - getLastVisitedDirectory() - to get last visited directory
  - acceptData() - can be used ti customize user selection validation

  \sa SUIT_FileValidator class.
*/

#include "SUIT_FileDlg.h"

#include "SUIT_Tools.h"   
#include "SUIT_Session.h"
#include "SUIT_MessageBox.h"
#include "SUIT_ResourceMgr.h"
#include "SUIT_FileValidator.h"
#include "Qtx.h"

#include <QDir>
#include <QEvent>
#include <QRegExp>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QGridLayout>
#include <QApplication>
#include <QListView>
#include <QLineEdit>
// GDD
#include <QUrl>
#include <QDesktopServices>

/*!
  \brief Defines extension behavior.

  If the selected file name has extension which does not match the selected filter
  and this variable is set to \c true, the file extension is ignored and new one
  (from current file filter will be added.
  \sa addExtension()
*/
const bool IGNORE_NON_MATCHING_EXTENSION = true;

QString SUIT_FileDlg::myLastVisitedPath;

/*!
  \brief Constructor.
  \param parent parent widget
  \param open if \c true dialog box is used for file opening, otherwise - for saving
  \param showQuickDir if \c true the quick directory list widgets will be shown
  \param modal if \c true the dialog box will be modal
*/
SUIT_FileDlg::SUIT_FileDlg( QWidget* parent, bool open, bool showQuickDir, bool modal )
: QFileDialog( parent ),
  myValidator( 0 ),
  myQuickLab( 0 ),
  myQuickCombo( 0 ),
  myQuickButton( 0 ),
  myCheckPermissions( true )
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  setOption(QFileDialog::DontUseNativeDialog, true);
  setModal( modal );
  setSizeGripEnabled( true );
  if ( parent )
    setWindowIcon( parent->windowIcon() );

  // GDD
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  myUrls.insert(0,QUrl::fromLocalFile(QDesktopServices::storageLocation(QDesktopServices::ApplicationsLocation)));
  myUrls.insert(0,QUrl::fromLocalFile(QDesktopServices::storageLocation(QDesktopServices::HomeLocation)));
#else
  myUrls.insert(0,QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation)));
  myUrls.insert(0,QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)));
#endif
  setSidebarUrls(myUrls);

  // add quick directories widgets
  if ( showQuickDir ) {
    myQuickLab    = new QLabel( tr( "LAB_QUICK_PATH" ), this );
    myQuickCombo  = new QComboBox( this );
    myQuickButton = new QPushButton( tr( "BUT_ADD_PATH" ), this );
    
    if ( addWidgets( myQuickLab, myQuickCombo, myQuickButton ) ) {
      connect( myQuickCombo,  SIGNAL( activated( const QString& ) ), this, SLOT( quickDir( const QString& ) ) );
      connect( myQuickButton, SIGNAL( clicked() ),                   this, SLOT( addQuickDir() ) );

      // retrieve directories list from the resources
      QStringList dirList;
  
      if ( resMgr )
        dirList = resMgr->stringValue( "FileDlg", "QuickDirList" ).split( ';', QString::SkipEmptyParts );

      if ( dirList.isEmpty() ) 
        dirList << QDir::homePath();

      // GDD
      for ( int i = 0; i < dirList.count(); i++ ) {
        myQuickCombo->addItem( dirList[i] );
        myUrls.append(QUrl::fromLocalFile(dirList[i]));
      }

      // GDD
      setSidebarUrls(myUrls);
      
    }
    else {
      delete myQuickLab;    myQuickLab = 0;
      delete myQuickCombo;  myQuickCombo = 0;
      delete myQuickButton; myQuickButton = 0;
    }
  }

  setAcceptMode( open ? AcceptOpen: AcceptSave );
  setWindowTitle( open ? tr( "INF_DESK_DOC_OPEN" ) : tr( "INF_DESK_DOC_SAVE" ) );

  bool showCurrentDirInitial = resMgr ? resMgr->booleanValue( "FileDlg", "ShowCurDirInitial", false ) : false;

  // If last visited path doesn't exist -> switch to the first preferred path
  if ( !myLastVisitedPath.isEmpty() ) {
    if ( !processPath( myLastVisitedPath ) && showQuickDir )
      processPath( myQuickCombo->itemText( 0 ) );
  }
  else if ( showCurrentDirInitial ) {
    processPath( QDir::currentPath() );
  }
  else if ( showQuickDir ) {
    processPath( myQuickCombo->itemText( 0 ) );
  }

  // set default file validator
  myValidator = new SUIT_FileValidator( this );
}

/*!
  \brief Destructor.
*/
SUIT_FileDlg::~SUIT_FileDlg() 
{
  setValidator( 0 );
}


/*! 
  \brief Check if the dialog box is used for opening or saving the file.
  \return \c true if dialog is used for file opening and \c false otherwise
*/
bool SUIT_FileDlg::isOpenDlg() const
{
  return acceptMode() == AcceptOpen;
}

/*!
  \brief Get 'check file permissions' flag.
  \return flag value
  \sa setCheckPermissions()
*/
bool SUIT_FileDlg::checkPermissions() const
{
  return myCheckPermissions;
}

/*!
  \brief Set 'check file permissions' flag.
 
  If this flag is set and file validator is not null,
  the validator will check the file permissions also.

  \param checkPerm new flag value
  \sa checkPermissions()
*/
void SUIT_FileDlg::setCheckPermissions( const bool checkPerm )
{
  myCheckPermissions = checkPerm;
}

/*!
  \brief Get file validator.
  \return current file validator
  \sa setValidator()
*/
SUIT_FileValidator* SUIT_FileDlg::validator() const
{
  return myValidator;
}

/*!
  \brief Set file validator.
 
  Destroys previous validator if the dialog owns it.

  \param v new file validator
  \sa validator()
*/
void SUIT_FileDlg::setValidator( SUIT_FileValidator* v )
{
  if ( myValidator && myValidator->parent() == this )
    delete myValidator;
  myValidator = v;
}

/*!
  \brief Adds the specified widgets to the bottom of the file dialog. 
  
  The first widget (usually label) \a l is placed underneath the "file name" 
  and the "file types" labels. 
  The widget \a w is placed underneath the file types combobox.
  The last widget (usually button) \a b is placed underneath the Cancel push button. 

  In general, the widgets can be arbitrary. This method is added to support 
  the functionality provided by the Qt series 3.x.

  If you don't want to have one of the widgets added, pass 0 in that widget's position. 
  Every time this function is called, a new row of widgets is added to the bottom of the 
  file dialog. 

  \param l first widget (e.g. text label)
  \param w second widget (e.g. combo box)
  \param b third widget (e.g. push button)
  \return \c true if widgets have been added successfully
*/
bool SUIT_FileDlg::addWidgets( QWidget* l, QWidget* w, QWidget* b )
{
  QGridLayout* grid = ::qobject_cast<QGridLayout*>( layout() );
  if ( grid ) {
    int row = grid->rowCount();
    int columns = grid->columnCount();
    if ( l ) 
      grid->addWidget( l, row, 0 );
    if ( w )
      grid->addWidget( w, row, 1, 1, columns-2 );
    if ( b )
      grid->addWidget( b, row, columns-1 );
    return true;
  }
  return false;
}

/*!
  \brief Get list of selected files.
  \return selected file names
*/
QStringList SUIT_FileDlg::selectedFiles() const
{
  QStringList files = QFileDialog::selectedFiles();
  if ( fileMode() != DirectoryOnly && fileMode() != Directory ) {
    QMutableListIterator<QString> it( files );
    while ( it.hasNext() ) {
      QString f = it.next();
      QFileInfo finfo( f );
      if ( !finfo.isDir() )
        it.setValue( addExtension( f ) );
    }
  }
  return files;
}

/*!
  \brief Get selected file.
  \return selected file name or null string if file is not selected
*/
QString SUIT_FileDlg::selectedFile() const
{
  QStringList files = selectedFiles();
  return files.count() > 0 ? files[0] : QString();
}

/*!
  \brief Get last visited directory.

  Note, that last visited path is memorized only if the 
  dialog box is accepted.

  \return last visited directory
*/
QString SUIT_FileDlg::getLastVisitedDirectory()
{
  return myLastVisitedPath;
}

/*!
  \brief Customize events processing.
  \param e event
  \return \c true if the event e was recognized and processed
*/
bool SUIT_FileDlg::event( QEvent* e )
{
  bool res = QFileDialog::event( e );

  if ( e->type() == QEvent::Polish )
    polish();

  return res;
}

/*!
  \brief Get line edit which is used to enter file name.
  \return line edit widget or0 if it could not be found
*/
QLineEdit* SUIT_FileDlg::lineEdit() const
{
  QLineEdit* ebox = 0;
  QList<QLineEdit*> editBoxes = findChildren<QLineEdit*>();
  QGridLayout* grid = ::qobject_cast<QGridLayout*>( layout() );
  if ( grid ) {
    int idx = 10000;
    for ( int i = 0; i < editBoxes.count(); i++ ) {
      int widx = grid->indexOf( editBoxes[ i ] );
      if ( widx >= 0 )
        idx = qMin( idx, widx );
    }
    if ( grid->itemAt( idx )  )
      ebox = qobject_cast<QLineEdit*>( grid->itemAt( idx )->widget() );
  }
  return ebox;
}

/*! 
  \brief Validate user selection.

  The validation is done by calling the corresponding methods
  of the validator. If the validator is not set, this method
  always returns \c true.

  This method can be re-implemented in the subclasses to customize
  the file dialog behavior.
  Another solution could be implementing own file validator class.

  \return \c true if user selection (file(s) or directory) is valid
  \sa SUIT_FileValidator class, validator(), setValidator()
*/
bool SUIT_FileDlg::acceptData()
{    
  QStringList files = selectedFiles();
  if ( files.isEmpty() )
    return false;

  // special case for ".."
  if ( lineEdit() ) {
    QString txt = lineEdit()->text();
    if ( txt == ".." ) {
      QDir dir = directory();
      if ( dir.cdUp() ) {
        setDirectory( dir );
        bool block = lineEdit()->blockSignals( true );
        lineEdit()->setText( ".." );
        lineEdit()->selectAll();
        lineEdit()->setFocus( Qt::OtherFocusReason );
        lineEdit()->blockSignals( block );
        return false;
      }
    }
    else if ( fileMode() != DirectoryOnly ) {
      QStringList fs = txt.split( " ", QString::SkipEmptyParts );
      for ( int i = 0; i < fs.count(); i++ ) {
        QString wc = fs.at( i );
        if ( wc.startsWith( "\"" ) && wc.endsWith( "\"" ) )
          wc = wc.mid( 1, wc.length()-2 );
        if ( hasWildCards( wc ) ) {
          addFilter( wc );
          lineEdit()->clear();
          return false;
        }
      }
    }
  }

  // special case for wildcards
  for ( int i = 0; i < files.count(); ++i ) {
  }

  bool bOk = true;

  switch ( fileMode() ) {
  case DirectoryOnly:
  case Directory: 
    {
      QString fn = files.first();
      if ( validator() ) {
        bOk = isOpenDlg() ? validator()->canReadDir( fn, checkPermissions() ) : 
                            validator()->canWriteDir( fn, checkPermissions() );
      }
      break;
    }
  case AnyFile: 
    {
      QString fn = files.first();
      QFileInfo info( fn );
      if ( info.isDir() ) {
        setDirectory( info.absoluteFilePath() );
        if ( lineEdit() ) {
          lineEdit()->selectAll();
          lineEdit()->setFocus( Qt::OtherFocusReason );
        }
        return false;
      }
      // validation is not required
      if ( validator() ) {
        bOk = isOpenDlg() ? validator()->canOpen( fn, checkPermissions() ) : 
                            validator()->canSave( fn, checkPermissions() );
      }
      break;
    }
  case ExistingFile:
  case ExistingFiles: 
    {
      for ( int i = 0; i < files.count(); ++i ) {
        QFileInfo info( files.at( i ) );
        if ( info.isDir() ) {
          setDirectory( info.absoluteFilePath() );
          if ( lineEdit() ) {
            lineEdit()->selectAll();
            lineEdit()->setFocus( Qt::OtherFocusReason );
          }
          return false;
        }
        if ( validator() ) {
          bOk = isOpenDlg() ? validator()->canOpen( files.at( i ), checkPermissions() ) : 
                              validator()->canSave( files.at( i ), checkPermissions() );
        if ( !bOk )
          return false;
        }
      }
      break;
    }
  }

  if ( bOk )
    emit filesSelected( files );

  return bOk;
}

/*!
  \brief Add an extension to the specified file name.
 
  The extension is extracted from the active filter.

  \param fileName file name to be processed
  \return fileName with the extension added
*/
QString SUIT_FileDlg::addExtension( const QString& fileName ) const
{
  QString fname = fileName.trimmed();

  // check if file name entered is empty
  if ( fname.isEmpty() )
    return fileName;

  // current file extension
  QString anExt = "." + SUIT_Tools::extension( fname ).trimmed();

  // If the file already has extension and it does not match the filter there are two choices:
  // - to leave it 'as is'
  // - to ignore it
  // The behavior is defined by IGNORE_NON_MATCHING_EXTENSION constant
  if ( anExt != "." && !IGNORE_NON_MATCHING_EXTENSION )
    return fileName;

  QRegExp r( QString::fromLatin1("\\(?[a-zA-Z0-9.*? +;#|]*\\)?$") );
  int index = r.indexIn( selectedNameFilter().trimmed() );

  if ( QFileInfo( fileName ).exists() )
    return fileName; // if file exists return as is

  if ( index >= 0 ) {            
    // Create wildcard regular expression basing on selected filter 
    // in order to validate a file extension.
    // Due to transformations from the filter list (*.txt *.*xx *.c++ SUIT*.* ) we 
    // will have the pattern (\.txt|\..*xx|\.c\+\+|\..*) (as we validate extension only, 
    // we remove everything except extension mask from the pattern
    QString wildcard = selectedNameFilter().mid( index, r.matchedLength() ).trimmed();
    // replace '|' and ';' separators by space symbol and also brackets if there are some
    wildcard.replace( QRegExp( "[\\|;|(|)]" )," " ); 

    QString aPattern = wildcard.replace( QRegExp( "(^| )(\\s*)[0-9a-zA-Z*_?]*\\."), " \\." ).trimmed().
                                         replace( QRegExp( "\\s+" ), "|" ).replace( QRegExp( "[?]" ),".?" ).
                                         replace( QRegExp( "[*]" ),".*" ).replace( QRegExp( "[+]" ),"\\+" );

    // now we get the list of all extension masks and remove all which does not contain wildcard symbols
    QStringList extList = aPattern.split( "|", QString::SkipEmptyParts );
    for ( int i = extList.count() - 1; i >= 0; i-- ) {
      if ( !extList[i].contains( "." ) )
        extList.removeAt( i );
    }
    aPattern = extList.join( "|" );

    // finalize pattern
    QRegExp anExtRExp( "^("+ aPattern + ")$" );

    // Check if the current file extension matches the pattern
    if ( !anExtRExp.exactMatch( anExt ) ) {
      // find first appropriate extension in the selected filter 
      // (it should be without wildcard symbols)
      for ( int i = 0; i < extList.count(); i++ ) {
        QString newExt = extList[i].replace( QRegExp( "[\\\\][+]" ),"+" );
        int res = newExt.lastIndexOf( '.' );
        if ( res >= 0 )
          newExt = newExt.mid( res + 1 );
        if ( newExt.indexOf( QRegExp("[*|?]" ) ) < 0 ) {
          fname += fname.endsWith( "." ) ? newExt : QString( "." ) + newExt;
          return fname;
        }
      }
    }
  }
  return fileName;
}

/*!
  \brief Processes selection : tries to set specified sirectory or filename
  as current file dialog selection.
  \param path file or directory path
  \return \c true if \a path is processed correctly and \c false otherwise
*/
bool SUIT_FileDlg::processPath( const QString& path )
{
  if ( !path.isNull() ) {
    QFileInfo fi( path );
    if ( fi.exists() ) {
      if ( fi.isFile() )
        selectFile( path );
      else if ( fi.isDir() )
        setDirectory( path );
      return true;
    }
    QString dirPath = SUIT_Tools::dir( path, false );
    if ( !dirPath.isEmpty() && QFileInfo( dirPath ).exists() )
      setDirectory( dirPath );
    selectFile( SUIT_Tools::file( path ) );
    return true;
  }
  return false;
}

/*!
  \brief Add file filter and activates it.
  \param filter new file filter
*/
void SUIT_FileDlg::addFilter( const QString& filter )
{
  QStringList flist = nameFilters();
  if ( !flist.contains( filter ) ) {
    flist << filter;
    setNameFilters( flist );
  }
  selectNameFilter( filter );
}

/*!
  \brief Check if the string contains wildcard symbols.
  \param s string to be checked (for example, file name)
  \return \c true if string contains "*" or "?" symbols
*/
bool SUIT_FileDlg::hasWildCards( const QString& s )
{
  return s.contains( QRegExp("[*|?]") );
}

/*!
  \brief Called when the user presses "Open"or "Save" button.

  Verifies the user choice and closes dialog box, setting the return code to QDialog::Accepted

  \sa acceptData()
*/
void SUIT_FileDlg::accept()
{
  if ( acceptData() ) {
    myLastVisitedPath = directory().path();
    QDialog::accept();        
  }
}

/*!
  \brief Called when user selects directory from the "Quick Dir" combo box.

  Browses the file dialog to the specified directory (if it is valid).

  \param dirPath selected directory
*/
void SUIT_FileDlg::quickDir( const QString& dirPath )
{
  if ( !QDir( dirPath ).exists() )
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "ERR_DIR_NOT_EXIST" ).arg( dirPath ) );
  else
    processPath( dirPath );
}

/*!
  \brief Called when user presses "Quick Dir Add" button.
  
  Adds current directory to the quick directories list and to the preferences.
*/
void SUIT_FileDlg::addQuickDir()
{
  QString dp = directory().path();
  if ( !dp.isEmpty() ) {
    QDir dir( dp );

    QStringList dirList;

    SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
    if ( resMgr )
      dirList = resMgr->stringValue( "FileDlg", "QuickDirList" ).split( ';', QString::SkipEmptyParts );

    bool found = false;
    bool emptyAndHome = false;
    if ( dirList.count() > 0 ) {
      for ( int i = 0; i < dirList.count() && !found; i++ )  {
        QDir aDir( dirList[i] );
        if ( ( aDir.canonicalPath().isNull() && dirList[i] == dir.absolutePath() ) ||
             ( !aDir.canonicalPath().isNull() && aDir.exists() &&  
             aDir.canonicalPath() == dir.canonicalPath() ) ) {
          found = true;
        }
      }
    }
    else {
      emptyAndHome = dir.canonicalPath() == QDir( QDir::homePath() ).canonicalPath();
    }

    if ( !found ) {
      dirList.append( dp );
      resMgr->setValue( "FileDlg", "QuickDirList", dirList.join( ";" ) );
      // GDD
      if ( !emptyAndHome ) {
        myQuickCombo->addItem( dp );
        myUrls.append(QUrl::fromLocalFile( dp ));
        setSidebarUrls(myUrls);
      }
    }
  }
}

/*!
  \brief Polish the dialog box.
*/
void SUIT_FileDlg::polish()
{
  QList<QPushButton*> buttons = findChildren<QPushButton*>();

  int maxBtnWidth = 0;

  for ( QList<QPushButton*>::const_iterator it = buttons.begin(); 
        it != buttons.end(); ++it )
    maxBtnWidth = qMax( maxBtnWidth, (*it)->sizeHint().width() );

  for ( QList<QPushButton*>::const_iterator it = buttons.begin(); 
        it != buttons.end(); ++it ) {
    (*it)->setDefault( false );
    (*it)->setAutoDefault( false );
    (*it)->setFixedWidth( maxBtnWidth );
  }

  QList<QListView*> views = findChildren<QListView*>();
  for ( QList<QListView*>::const_iterator it = views.begin(); 
        it != views.end(); ++it ) {
    (*it)->setViewMode( QListView::ListMode );
  }
}

/*!
  \brief Show dialog box for the file opening/saving.

  This method can be used to select the file for opening
  or saving. The behavior is defined by the \a open parameter.
  Note, that selection validation depends on the dialog mode used.

  If \a initial parameter is not null string it is used as starting directory
  or file at which dialog box is opened.
  
  The parameter \a filters defines file filters (wildcards) to be used.
  If filters list is empty, "All files (*)" is used by default.
  
  The parameter \a caption is used as dialog box title. If it is
  is empty, the default title is used.
  
  The parameter \a showQuickDir specifies if it is necessary to 
  show additional quick directories list controls in the bottom part
  of the dialog box.

  The validation of the user selection is done with help of the file 
  validator (SUIT_FileValidator class). The last parameter \a validator
  can be used to pass the custom file validator to the dialog box.
  
  \param parent parent widget
  \param initial initial file (or directory) dialog box to be opened on
  \param filters file filters list
  \param caption dialog box title
  \param open if \c true dialog box is used for file opening, otherwise - for saving
  \param showQuickDir if \c true the quick directory list widgets will be shown
  \param validator custom file validator
  \return selected file name or null string if dialog box is cancelled
  \sa getOpenFileNames(), getExistingDirectory()
*/
QString SUIT_FileDlg::getFileName( QWidget* parent, const QString& initial, 
                                   const QStringList& filters, const QString& caption, 
                                   const bool open, const bool showQuickDir,
                                   SUIT_FileValidator* validator )
{            
  SUIT_FileDlg fd( parent, open, showQuickDir, true );    

  fd.setFileMode( open ? ExistingFile : AnyFile );

  QString tmpfilename = initial;
  tmpfilename = tmpfilename.simplified();
  tmpfilename = tmpfilename.replace(QRegExp("\\*"), "" ).replace(QRegExp("\\?"), "" );

  if ( filters.isEmpty() )
    fd.setNameFilter( tr( "ALL_FILES_FILTER" ) ); // All files (*)
  else
    fd.setNameFilters( filters );

  if ( !caption.isEmpty() )
    fd.setWindowTitle( caption );

  if ( !tmpfilename.isEmpty() )
    fd.processPath( tmpfilename );

  if ( validator )
    fd.setValidator( validator );

  QString filename;

  if ( fd.exec() == QDialog::Accepted )
    filename = fd.selectedFile();

  QApplication::processEvents();

  return filename;
}

/*!
  \brief Show dialog box for the file opening/saving.
  \overload

  This method can be used to select the file for opening
  or saving. The behavior is defined by the \a open parameter.
  Note, that selection validation depends on the dialog mode used.

  If \a initial parameter is not null string it is used as starting directory
  or file at which dialog box is opened.
  
  The parameter \a filters defines file filters (wildcards) to be used.
  This is the list of wildcards, separated by the ";;" symbols.
  If filters list is empty, "All files (*)" is used by default.
  
  The parameter \a caption is used as dialog box title. If it is
  is empty, the default title is used.
  
  The parameter \a showQuickDir specifies if it is necessary to 
  show additional quick directories list controls in the bottom part
  of the dialog box.

  The validation of the user selection is done with help of the file 
  validator (SUIT_FileValidator class). The last parameter \a validator
  can be used to pass the custom file validator to the dialog box.
  
  \param parent parent widget
  \param initial initial file (or directory) dialog box to be opened on
  \param filters file filters separated by ";;"
  \param caption dialog box title
  \param open if \c true dialog box is used for file opening, otherwise - for saving
  \param showQuickDir if \c true the quick directory list widgets will be shown
  \param validator custom file validator
  \return selected file name or null string if dialog box is cancelled
  \sa getOpenFileNames(), getExistingDirectory()
*/
QString SUIT_FileDlg::getFileName( QWidget* parent, const QString& initial, 
                                   const QString& filters, const QString& caption, 
                                   const bool open, const bool showQuickDir,
                                   SUIT_FileValidator* validator )
{
  return getFileName( parent, initial, filters.split( ";;", QString::SkipEmptyParts ), 
                      caption, open, showQuickDir, validator );
}

/*!
  \brief Show dialog box for the multiple files selection.

  If \a initial parameter is not null string it is used as starting directory
  or file at which dialog box is opened.
  
  The parameter \a filters defines file filters (wildcards) to be used.
  If filters list is empty, "All files (*)" is used by default.
  
  The parameter \a caption is used as dialog box title. If it is
  is empty, the default title is used.
  
  The parameter \a showQuickDir specifies if it is necessary to 
  show additional quick directories list controls in the bottom part
  of the dialog box.

  The validation of the user selection is done with help of the file 
  validator (SUIT_FileValidator class). The last parameter \a validator
  can be used to pass the custom file validator to the dialog box.
  
  \param parent parent widget
  \param initial initial file (or directory) dialog box to be opened on
  \param filters file filters list
  \param caption dialog box title
  \param showQuickDir if \c true the quick directory list widgets will be shown
  \param validator custom file validator
  \return selected file names or empty list if dialog box is cancelled
  \sa getFileName(), getExistingDirectory()
*/
QStringList SUIT_FileDlg::getOpenFileNames( QWidget* parent, const QString& initial,
                                            const QStringList& filters, const QString& caption,
                                            const bool showQuickDir, 
                                            SUIT_FileValidator* validator )
{            
  SUIT_FileDlg fd( parent, true, showQuickDir, true );

  fd.setFileMode( ExistingFiles );

  if ( filters.isEmpty() )
    fd.setNameFilter( tr( "ALL_FILES_FILTER" ) ); // All files (*)
  else
    fd.setNameFilters( filters );

  if ( !caption.isEmpty() )
    fd.setWindowTitle( caption );

  if ( !initial.isEmpty() )
    fd.processPath( initial );

  if ( validator )
    fd.setValidator( validator );

  QStringList filenames;

  if ( fd.exec() == QDialog::Accepted )
    filenames = fd.selectedFiles();

  QApplication::processEvents();

  return filenames;
}

/*!
  \brief Show dialog box for the multiple file opening.
  \overload

  If \a initial parameter is not null string it is used as starting directory
  or file at which dialog box is opened.
  
  The parameter \a filters defines file filters (wildcards) to be used.
  This is the list of wildcards, separated by the ";;" symbols.
  If filters list is empty, "All files (*)" is used by default.
  
  The parameter \a caption is used as dialog box title. If it is
  is empty, the default title is used.
  
  The parameter \a showQuickDir specifies if it is necessary to 
  show additional quick directories list controls in the bottom part
  of the dialog box.

  The validation of the user selection is done with help of the file 
  validator (SUIT_FileValidator class). The last parameter \a validator
  can be used to pass the custom file validator to the dialog box.
  
  \param parent parent widget
  \param initial initial file (or directory) dialog box to be opened on
  \param filters file filters separated by ";;"
  \param caption dialog box title
  \param showQuickDir if \c true the quick directory list widgets will be shown
  \param validator custom file validator
  \return selected file names or empty list if dialog box is cancelled
  \sa getFileName(), getExistingDirectory()
*/
QStringList SUIT_FileDlg::getOpenFileNames( QWidget* parent, const QString& initial,
                                            const QString& filters, const QString& caption,
                                            const bool showQuickDir,
                                            SUIT_FileValidator* validator )
{
  return getOpenFileNames( parent, initial, filters.split( ";;", QString::SkipEmptyParts ), 
                           caption, showQuickDir, validator );
}

/*!
  \brief Show dialog box for the existing directory selection.

  If \a initial parameter is not null string it is used as starting directory
  at which dialog box is opened.
  
  The parameter \a caption is used as dialog box title. If it is
  is empty, the default title is used.
  
  The parameter \a showQuickDir specifies if it is necessary to 
  show additional quick directories list controls in the bottom part
  of the dialog box.

  The validation of the user selection is done with help of the file 
  validator (SUIT_FileValidator class). The last parameter \a validator
  can be used to pass the custom file validator to the dialog box.
  
  \param parent parent widget
  \param initial initial directory dialog box to be opened on
  \param caption dialog box title
  \param showQuickDir if \c true the quick directory list widgets will be shown
  \param validator custom file validator
  \return selected directory name or null string if dialog box is cancelled
  \sa getFileName(), getOpenFileNames()
*/
QString SUIT_FileDlg::getExistingDirectory( QWidget* parent, const QString& initial,
                                            const QString& caption, const bool showQuickDir,
                                            SUIT_FileValidator* validator )
{
  SUIT_FileDlg fd( parent, true, showQuickDir, true );

  fd.setFileMode( DirectoryOnly );

  if ( !caption.isEmpty() )
    fd.setWindowTitle( caption );

  if ( !initial.isEmpty() )
    fd.processPath( initial );
  
  if ( validator )
    fd.setValidator( validator );

  QString dirname;

  if ( fd.exec() == QDialog::Accepted )
    dirname = fd.selectedFile();

  QApplication::processEvents();

  return dirname;
}

/*!
  \brief Get last visited path
  \return last visited path
*/
QString SUIT_FileDlg::getLastVisitedPath()
{
  return myLastVisitedPath;
}

/*!
  \brief Selects current file

  This version of selectFile() methods works similar to Qt version 3.x:
  it selects the given file as current and it changes the current file dialog's directory
  to the directory of the file
  
  \param f - new current file name 
*/
void SUIT_FileDlg::selectFile( const QString& f )
{
  QFileDialog::selectFile( QFileInfo( f ).baseName() );
  if ( !Qtx::dir( f, false ).isEmpty() )
    setDirectory( QFileInfo( f ).absolutePath() );
}
