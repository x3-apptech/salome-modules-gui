// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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

/*!
  SUIT_FileDlg class is the extension of the Qt's Open/Save file dialog box.
  To get the file/directory name(s) call static methods:

  to invoke "Open file" or "Save file" dialog box
  static QString getFileName(QWidget* parent, const QString& initial, const QStringList& filters, 
                               const QString& caption, const bool open, const bool showQuickDir = true,
                               SUIT_FileValidator* validator = 0);

  to invoke "Open files" dialog box (to get the multiple file selection)
  static QStringList getOpenFileNames(QWidget* parent, const QString& initial, const QStringList& filters, 
                                        const QString& caption, bool showQuickDir = true, 
                                        SUIT_FileValidator* validator = 0);

 to invoke "Select directory" dialog box
    static QString getExistingDirectory(QWidget* parent, const QString& initial,
                                        const QString& caption, const bool showQuickDir = true);

 The parameters:
 - parent        parent widget (if 0, the current desktop is used)
 - initial       starting directory or file name (if null, last visited directory is used)
 - filters       file filters list; patterns inside the filter can be separated by ';','|' or ' ' 
                 symbols
 - caption       dialog box's caption: if null, the default one is used
 - open          open flag - true for "Open File" and false for "Save File" dialog box
 - showQuickDir  this flag enables/disables "Quick directory list" controls
 - validator     you can provide custom file validator with this parameter

 Examples:
   ...
   QStringList flist;
   flist.append( "Image files (*.bmp *.gif *.jpg )" );
   flist.append( "All files (*.*)" );
   QMyFileValidator* v = new QMyFileValidator( 0 );
   QString fileName =  SUIT_FileDlg::getFileName( 0, QString::null, flist, "Dump view", false, true, v );
   if ( !fileName.isEmpty() ) {
      ... writing image to the file 
   }
   ...
   QStringList flist;
   flist.append( "*.cpp | *.cxx | *.c++" );
   flist.append( "*.h | *.hpp | *.hxx" );
   QString fileName =  SUIT_FileDlg::getFileName( desktop(), QString::null, flist, QString::null, true, true );
*/

#include "SUIT_FileDlg.h"

#include "SUIT_Tools.h"   
#include "SUIT_Session.h"
#include "SUIT_Desktop.h"
#include "SUIT_MessageBox.h"
#include "SUIT_ResourceMgr.h"
#include "SUIT_FileValidator.h"

#include <qdir.h>
#include <qlabel.h>
#include <qregexp.h>
#include <qpalette.h>
#include <qobjectlist.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qapplication.h>

#define MIN_COMBO_SIZE 100

/*! If the selected file name has extension which does not match the selected filter
 * this extension is ignored (and new one will be added). See below for details.
 */
const bool IGNORE_NON_MATCHING_EXTENSION = true;

QString SUIT_FileDlg::myLastVisitedPath;

/*! Constructor */
SUIT_FileDlg::SUIT_FileDlg( QWidget* parent, bool open, bool showQuickDir, bool modal ) :
QFileDialog( parent, 0, modal ),
myValidator( 0 ),
myQuickCombo( 0 ), myQuickButton( 0 ), myQuickLab( 0 ),
myOpen( open )//,
//myAccepted( false )
{    
  const QObjectList* child = children();
  QObjectList::const_iterator anIt = child->begin(), aLast = child->end();
  for( ; anIt!=aLast; anIt++ )
    if( (*anIt)->inherits( "QPushButton" ) )
    {
      QPushButton* bt = ( QPushButton* )( *anIt );
      bt->setDefault( false );
      bt->setAutoDefault( false );
    }

  if ( parent->icon() )
    setIcon( *parent->icon() );       
  setSizeGripEnabled( true );
  
  if ( showQuickDir ) {
    // inserting quick dir combo box
    myQuickLab  = new QLabel(tr("LAB_QUICK_PATH"), this);
    myQuickCombo = new QComboBox(false, this);
    myQuickCombo->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    myQuickCombo->setMinimumSize(MIN_COMBO_SIZE, 0);
    
    myQuickButton = new QPushButton(tr("BUT_ADD_PATH"), this);

    connect(myQuickCombo,  SIGNAL(activated(const QString&)), this, SLOT(quickDir(const QString&)));
    connect(myQuickButton, SIGNAL(clicked()),                 this, SLOT(addQuickDir()));
    addWidgets(myQuickLab, myQuickCombo, myQuickButton);

    // getting dir list from settings
    QString dirs;
    SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
    if ( resMgr )
      dirs = resMgr->stringValue( "FileDlg", QString( "QuickDirList" ) );

    QStringList dirList = QStringList::split(';', dirs, false);
    if (dirList.count() > 0) {
      for (unsigned i = 0; i < dirList.count(); i++)
        myQuickCombo->insertItem(dirList[i]);
    }
    else {
      myQuickCombo->insertItem(QDir::homeDirPath());
    }
  }
  setMode( myOpen ? ExistingFile : AnyFile );     
  setCaption( myOpen ? tr( "INF_DESK_DOC_OPEN" ) : tr( "INF_DESK_DOC_SAVE" ) );

  // If last visited path doesn't exist -> switch to the first preferred path
  if ( !myLastVisitedPath.isEmpty() ) {
    if ( !processPath( myLastVisitedPath ) && showQuickDir )
      processPath( myQuickCombo->text( 0 ) );
  }
  else {
    if ( showQuickDir )
      processPath(myQuickCombo->text( 0 ) );
  } 

  // set default file validator
  myValidator = new SUIT_FileValidator(this);
}

/*! Destructor*/
SUIT_FileDlg::~SUIT_FileDlg() 
{
  setValidator( 0 );
}

/*! Redefined from QFileDialog.*/
void SUIT_FileDlg::polish()
{
  QFileDialog::polish();
  if ( myQuickButton && myQuickLab ) {
    // the following is a workaround for proper layouting of custom widgets
    QValueList<QPushButton*> buttonList;
    QValueList<QLabel*> labelList;
    const QObjectList *list = children();
    QObjectListIt it(*list);
    int maxButWidth = myQuickLab->sizeHint().width();
    int maxLabWidth = myQuickButton->sizeHint().width();
    
    for (; it.current() ; ++it) {
      if ( it.current()->isA( "QLabel" ) ) {
	int tempW = ((QLabel*)it.current())->minimumWidth();
	if ( maxLabWidth < tempW ) maxLabWidth = tempW;
	labelList.append( (QLabel*)it.current() );
      }
      else if( it.current()->isA("QPushButton") ) {
	int tempW = ((QPushButton*)it.current())->minimumWidth();
	if ( maxButWidth < tempW ) maxButWidth = tempW;
	buttonList.append( (QPushButton*)it.current() );
      }
    }
    if (maxButWidth > 0) {
      QValueList<QPushButton*>::Iterator bListIt;
      for ( bListIt = buttonList.begin(); bListIt != buttonList.end(); ++bListIt )
	(*bListIt)->setFixedWidth( maxButWidth );
    }
    if (maxLabWidth > 0) {
      QValueList<QLabel*>::Iterator lListIt;
      for ( lListIt = labelList.begin(); lListIt != labelList.end(); ++lListIt )
	(*lListIt)->setFixedWidth( maxLabWidth );
    }
  }
}

/*! Sets validator for file names to open/save
 * Deletes previous validator if the dialog owns it.
 */
void SUIT_FileDlg::setValidator( SUIT_FileValidator* v )
{
  if ( myValidator && myValidator->parent() == this )
    delete myValidator;
  myValidator = v;
}

/*! Returns the selected file */
QString SUIT_FileDlg::selectedFile() const
{
  return mySelectedFile;
}

/*! Returns 'true' if this is 'Open File' dialog 
 *  and 'false' if 'Save File' dialog
 */
bool SUIT_FileDlg::isOpenDlg() const
{
  return myOpen;
}

/*! Closes this dialog and sets the return code to 'Accepted'
 * if the selected name is valid ( see 'acceptData()' )
 */
void SUIT_FileDlg::accept()
{
  /* myAccepted 
   * flag is used to warkaround the Qt 2.2.2 BUG: 
   * accept() method is called twice if user presses 'Enter' key 
   * in file name editor while file name is not acceptable by acceptData()
   * (e.g. permission denied)
   */
//  if ( !myAccepted ) {
    if ( mode() != ExistingFiles ) {
      mySelectedFile = QFileDialog::selectedFile();
      addExtension();
    }

    if ( acceptData() ) {
      myLastVisitedPath = dirPath();
      QFileDialog::accept();        
//      myAccepted = true;
    }
//  }
//  myAccepted = !myAccepted;
}

/*! Closes this dialog and sets the return code to 'Rejected' */
void SUIT_FileDlg::reject()
{
  mySelectedFile = QString::null;
  QFileDialog::reject();        
}

/*! Returns 'true' if selected file is valid.
 * The validity is checked by a file validator, 
 * if there is no validator the file is always
 * considered as valid    
 */
bool SUIT_FileDlg::acceptData()
{    
  if ( myValidator )
  {
    if ( isOpenDlg() )
    {
      if ( mode() == ExistingFiles )
      {
	      QStringList fileNames = selectedFiles();
	      for ( int i = 0; i < (int)fileNames.count(); i++ )
        {
	        if ( !myValidator->canOpen( fileNames[i] ) )
	          return false;
	      }
	      return true;
      }
      else
      {
	      return myValidator->canOpen( selectedFile() );
      }
    }
    else
      return myValidator->canSave( selectedFile() );
  }
  return true;
}

/*! Adds an extension to the selected file name
 * if the file has not it.
 * The extension is extracted from the active filter.
 */
void SUIT_FileDlg::addExtension()
{
  // check if file name entered is empty
  if ( mySelectedFile.stripWhiteSpace().isEmpty() )
    return;

  // current file extension
  QString anExt = "." + SUIT_Tools::extension( mySelectedFile.stripWhiteSpace() ).stripWhiteSpace();

  // If the file already has extension and it does not match the filter there are two choices:
  // - to leave it 'as is'
  // - to ignore it
  // The behavior is defined by IGNORE_NON_MATCHING_EXTENSION constant
  if ( anExt != "." && !IGNORE_NON_MATCHING_EXTENSION )
    return;

  // get selected file filter
#if QT_VERSION < 0x030000
  QRegExp r( QString::fromLatin1("(?[a-zA-Z0-9.*? +;#|]*)?$") );
  int len, index = r.match( selectedFilter().stripWhiteSpace(), 0, &len );
#else
  QRegExp r( QString::fromLatin1("\\(?[a-zA-Z0-9.*? +;#|]*\\)?$") );
  int index = r.search( selectedFilter().stripWhiteSpace() );
#endif

  if ( index >= 0 ) {            
    // Create wildcard regular expression basing on selected filter 
    // in order to validate a file extension.
    // Due to transformations from the filter list (*.txt *.*xx *.c++ SUIT*.* ) we 
    // will have the pattern (\.txt|\..*xx|\.c\+\+|\..*) (as we validate extension only, 
    // we remove everything except extension mask from the pattern
#if QT_VERSION < 0x030000
    QString wildcard = selectedFilter().mid( index, len ).stripWhiteSpace();
#else
    QString wildcard = selectedFilter().mid( index, r.matchedLength() ).stripWhiteSpace();
#endif
    // replace '|' and ';' separators by space symbol and also brackets if there are some
    wildcard.replace( QRegExp( "[\\|;|(|)]" )," " ); 

    QString aPattern = wildcard.replace( QRegExp( "(^| )(\\s*)[0-9a-zA-Z*_?]*\\."), " \\." ).stripWhiteSpace().
                                         replace( QRegExp( "\\s+" ), "|" ).replace( QRegExp( "[?]" ),".?" ).
                                         replace( QRegExp( "[*]" ),".*" ).replace( QRegExp( "[+]" ),"\\+" );

    // now we get the list of all extension masks and remove all which does not contain wildcard symbols
    QStringList extList = QStringList::split( "|",aPattern );
    for( int i = extList.count() - 1; i >= 0; i-- ) {
      if ( !extList[i].contains( "." ) )
        extList.remove( extList.at( i ) );
    }
    aPattern = extList.join( "|" );

    // finalize pattern
    QRegExp anExtRExp( "^("+ aPattern + ")$" );

    // Check if the current file extension matches the pattern
    if ( anExtRExp.match( anExt ) < 0 )
    {
      // find first appropriate extension in the selected filter 
      // (it should be without wildcard symbols)
      for ( int i = 0; i < (int)extList.count(); i++ )
      {
        QString newExt = extList[i].replace( QRegExp( "[\\\\][+]" ),"+" );
        int res = newExt.findRev( '.' );
        if ( res >= 0 )
          newExt = newExt.mid( res + 1 );
        if ( newExt.find( QRegExp("[*|?]" ) ) < 0 )
        {
          mySelectedFile.stripWhiteSpace();
          mySelectedFile += mySelectedFile.endsWith(".") ? newExt : QString(".") + newExt;
          break;
        }
      }
    }
  }
}

/*! Processes selection : tries to set given path or filename as selection */
bool SUIT_FileDlg::processPath( const QString& path )
{
  if ( !path.isNull() ) {
    QFileInfo fi( path );
    if ( fi.exists() ) {
      if ( fi.isFile() )
	setSelection( path );
      else if ( fi.isDir() )
	setDir( path );
      return true;
    }
    else {
      if ( QFileInfo( fi.dirPath() ).exists() ) {
	setDir( fi.dirPath() );
	setSelection( path );
	return true;
      }
    }
  }
  return false;
}
/*! Called when user selects item from "Quick Dir" combo box */
void SUIT_FileDlg::quickDir(const QString& dirPath)
{
  QString aPath = dirPath;
  if ( !QDir(aPath).exists() ) {
    aPath = QDir::homeDirPath();
    SUIT_MessageBox::error1(this, 
		   tr("ERR_ERROR"),
		   tr("ERR_DIR_NOT_EXIST").arg(dirPath), 
		   tr("BUT_OK"));    
  }
  else
  processPath(aPath);
}
/*!
  Called when user presses "Add" button - adds current directory to quick directory
  list and to the preferences
*/
void SUIT_FileDlg::addQuickDir()
{
  QString dp = dirPath();
  if ( !dp.isEmpty() ) {
    QDir dir( dp );
    // getting dir list from settings
    QString dirs;
    SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
    if ( resMgr )
      dirs = resMgr->stringValue( "FileDlg", QString( "QuickDirList" ) );
    QStringList dirList = QStringList::split(';', dirs, false);
    bool found = false;
    bool emptyAndHome = false;
    if ( dirList.count() > 0 ) {
      for ( unsigned i = 0; i < dirList.count(); i++ ) {
	QDir aDir( dirList[i] );
	if ( aDir.canonicalPath().isNull() && dirList[i] == dir.absPath() ||
	    !aDir.canonicalPath().isNull() && aDir.exists() && aDir.canonicalPath() == dir.canonicalPath() ) {
	  found = true;
	  break;
	}
      }
    }
    else {
      emptyAndHome = dir.canonicalPath() == QDir(QDir::homeDirPath()).canonicalPath();
    }
    if ( !found ) {
      dirList.append( dp );
      resMgr->setValue( "FileDlg", QString( "QuickDirList" ), dirList.join(";") );
      if ( !emptyAndHome )
	myQuickCombo->insertItem( dp );
    }
  }
}
/*!
  Returns the file name for Open/Save [ static ]
*/
QString SUIT_FileDlg::getFileName( QWidget*            parent, 
				   const QString&      initial, 
                                   const QStringList&  filters, 
                                   const QString&      caption,
                                   bool                open,
				   bool                showQuickDir, 
				   SUIT_FileValidator* validator )
{            
  SUIT_FileDlg* fd = new SUIT_FileDlg( parent, open, showQuickDir, true );    
  if ( !caption.isEmpty() )
    fd->setCaption( caption );
  if ( !initial.isEmpty() ) { 
    fd->processPath( initial ); // VSR 24/03/03 check for existing of directory has been added to avoid QFileDialog's bug
  }
  fd->setFilters( filters );        
  if ( validator )
    fd->setValidator( validator );
  fd->exec();
  QString filename = fd->selectedFile();
  delete fd;
  qApp->processEvents();
  return filename;
}


/*!
  Returns the list of files to be opened [ static ]
*/
QStringList SUIT_FileDlg::getOpenFileNames( QWidget*            parent, 
					    const QString&      initial, 
					    const QStringList&  filters, 
					    const QString&      caption,
					    bool                showQuickDir, 
					    SUIT_FileValidator* validator )
{            
  SUIT_FileDlg* fd = new SUIT_FileDlg( parent, true, showQuickDir, true );    
  fd->setMode( ExistingFiles );     
  if ( !caption.isEmpty() )
    fd->setCaption( caption );
  if ( !initial.isEmpty() ) { 
    fd->processPath( initial ); // VSR 24/03/03 check for existing of directory has been added to avoid QFileDialog's bug
  }
  fd->setFilters( filters );        
  if ( validator )
    fd->setValidator( validator );
  fd->exec();
  QStringList filenames = fd->selectedFiles();
  delete fd;
  qApp->processEvents();
  return filenames;
}

/*!
  Existing directory selection dialog [ static ]
*/
QString SUIT_FileDlg::getExistingDirectory( QWidget*       parent,
					    const QString& initial,
					    const QString& caption, 
					    bool           showQuickDir )
{
  SUIT_FileDlg* fd = new SUIT_FileDlg( parent, true, showQuickDir, true);
  if ( !caption.isEmpty() )
    fd->setCaption( caption );
  if ( !initial.isEmpty() ) {
    fd->processPath( initial ); // VSR 24/03/03 check for existing of directory has been added to avoid QFileDialog's bug
  }
  fd->setMode( DirectoryOnly );
  fd->setFilters(tr("INF_DIRECTORIES_FILTER"));
  fd->exec();
  QString dirname = fd->selectedFile();
  delete fd;
  qApp->processEvents();
  return dirname;
  
}

/*!
  QFileDialog::dirPath() has a bug on Linux Debian (1 level up from correct
  directory is returned).  This function fixes the bug. 
*/
QString SUIT_FileDlg::dirPath() const
{
  if ( !mySelectedFile.isNull() )
    return QFileInfo( mySelectedFile ).dirPath();

  const QDir* aDir = dir();
  if ( aDir->exists() )
    return aDir->absPath();
  
  return QFileDialog::dirPath();
}
