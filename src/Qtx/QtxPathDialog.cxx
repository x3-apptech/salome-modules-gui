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

// File:      QtxPathDialog.cxx
// Author:    Sergey TELKOV
//
#include "QtxPathDialog.h"

#include "QtxGridBox.h"
#include "QtxGroupBox.h"

#include <QDir>
#include <QLabel>
#include <QPixmap>
#include <QLayout>
#include <QLineEdit>
#include <QObjectList>
#include <QStringList>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>

static const char* open_icon[] = {
"16 16 5 1",
"  c none",
". c #ffff00",
"# c #848200",
"a c #ffffff",
"b c #000000",
"                ",
"          bbb   ",
"         b   b b",
"              bb",
"  bbb        bbb",
" ba.abbbbbbb    ",
" b.a.a.a.a.b    ",
" ba.a.a.a.ab    ",
" b.a.abbbbbbbbbb",
" ba.ab#########b",
" b.ab#########b ",
" bab#########b  ",
" bb#########b   ",
" bbbbbbbbbbb    ",
"                ",
"                "
};

/*!
  \class QtxPathDialog
  \brief The QtxPathDialog class provides a simple convenience dialog to
         enter a path to the file or to the directory.

  The QtxPathDialog class adds possibility to browse the file system 
  with help of standard Open/Save dialog boxes or enter the file/directory
  path manually.

  Default implementation provides only one "standard" file entry.
  Sometimes it is necessary to select several different files/directories
  from the same dialog box. In this case it is possible to derive from the
  QtxPathDialog class and use createFileEntry() method to add required
  number of file entries.
*/

/*!
  \brief Constructor.
  \param import if \c true, the dialog box is shown for "open" mode, 
         otherwise, it is shown in the "save" mode
  \param parent parent widget
  \param modal if \c true, the dialog box should be modal
  \param resize if \c true, the dialog box is resizable
  \param buttons required buttons (QtxDialog::ButtonFlags)
  \param f window flags
*/
QtxPathDialog::QtxPathDialog( const bool import, QWidget* parent, const bool modal,
                              const bool resize, const int buttons, Qt::WindowFlags f )
: QtxDialog( parent, modal, resize, buttons, f ),
  myDefault( -1 ),
  myEntriesFrame( 0 ),
  myOptionsFrame( 0 )
{
  initialize();
  
  setWindowTitle( tr( import ? "Open file" : "Save file" ) );
  
  setDefaultEntry( createFileEntry( tr( "File name" ), import ? OpenFile : SaveFile ) );
  QLineEdit* le = fileEntry( defaultEntry() );
  if ( le )
    le->setMinimumWidth( 200 );
  
  validate();

  setFocusProxy( le );

  updateVisibility();
}

/*!
  \brief Constructor.
  \param parent parent widget
  \param modal if \c true, the dialog box should be modal
  \param resize if \c true, the dialog box is resizable
  \param buttons required buttons (QtxDialog::ButtonFlags)
  \param f window flags
*/
QtxPathDialog::QtxPathDialog( QWidget* parent, const bool modal,
                              const bool resize, const int buttons, Qt::WindowFlags f )
: QtxDialog( parent, modal, resize, buttons, f ),
  myDefault( -1 ),
  myEntriesFrame( 0 ),
  myOptionsFrame( 0 )
{
  initialize();

  updateVisibility();
}

/*!
  \brief Destructor.
*/
QtxPathDialog::~QtxPathDialog()
{
}

/*!
  \brief Get selected file name.
  \return file name
*/
QString QtxPathDialog::fileName() const
{
  return fileName( defaultEntry() );
}

/*!
  \brief Set the file name.
  \param txt new file name
  \param autoExtension if \c true an extension is determined automatically by file
*/
void QtxPathDialog::setFileName( const QString& txt, const bool autoExtension )
{
  setFileName( defaultEntry(), txt, autoExtension );
}

/*!
  \brief Get current file filter.
  \return file filter
*/
QString QtxPathDialog::filter() const
{
  return filter( defaultEntry() );
}

/*!
  \brief Change file filter.
  
  Filter is a list of file masks, separated by ';;'. For example, 
  "*.h;;*.cxx"

  \param fltr new file filter
*/
void QtxPathDialog::setFilter( const QString& fltr )
{
  setFilter( defaultEntry(), fltr );
}

/*!
  \brief Show/hide the path dialog box/
  \param on new visibility state
*/
void QtxPathDialog::setVisible( bool on )
{
  if ( on )
    updateVisibility();

  QtxDialog::setVisible( on );
}

/*!
  \brief Called when user clicks a "browse" button 
         to open standard file dialog.
*/
void QtxPathDialog::onBrowse()
{
  const QObject* obj = sender();

  int id = -1;
  
  for ( FileEntryMap::Iterator it = myEntries.begin(); it != myEntries.end() && id == -1; ++it )
  {
    if ( it.value().btn == obj )
      id = it.key();
  }
  
  if ( id == -1 )
    return;
  
  FileEntry& entry = myEntries[id];
  
  bool isDir = entry.mode != OpenFile && entry.mode != SaveFile;
  
  if ( !entry.dlg )
  {
    entry.dlg = new QFileDialog( this, windowTitle(), QDir::current().path() );
    switch ( entry.mode )
    {
    case NewDir:
    case OpenDir:
    case SaveDir:
      isDir = true;
      entry.dlg->setFileMode( QFileDialog::DirectoryOnly );
      break;
    case SaveFile:
      entry.dlg->setFileMode( QFileDialog::AnyFile );
      break;
    case OpenFile:
    default:
      entry.dlg->setFileMode( QFileDialog::ExistingFiles );
      break;
    }
  }
  
  if ( !isDir )
  {
    QStringList fList = prepareFilters( entry.filter );
    if ( !fList.isEmpty() )
      entry.dlg->setNameFilters( fList );
  }
  entry.dlg->selectFile( fileName( id ) );

  if ( entry.dlg->exec() != Accepted )
    return;
  
  QStringList fileList = entry.dlg->selectedFiles();
  QString fName = !fileList.isEmpty() ? fileList.first() : QString();
  
  if ( fName.isEmpty() )
    return;
  
  if ( Qtx::extension( fName ).isEmpty() && !isDir )
    fName = autoExtension( fName, entry.dlg->selectedNameFilter() );

  fName = QDir::toNativeSeparators( fName );
  QString prev = QDir::toNativeSeparators( fileName( id ) );
  if ( isDir )
  {
    while ( prev.length() && prev.at( prev.length() - 1 ) == QDir::separator() )
      prev.remove( prev.length() - 1, 1 );
    while ( fName.length() && fName.at( fName.length() - 1 ) == QDir::separator() )
      fName.remove( fName.length() - 1, 1 );
  }
  
  if ( prev == fName )
    return;
  
  setFileName( id, fName );
  fileNameChanged( id, fName );
  
  if ( id == defaultEntry() )
    emit fileNameChanged( fName );
}

/*!
  \brief Called when user presses \c Return key being in the line edit.
*/
void QtxPathDialog::onReturnPressed()
{
  const QObject* obj = sender();
  
  int id = -1;
  for ( FileEntryMap::Iterator it = myEntries.begin(); it != myEntries.end() && id == -1; ++it )
  {
    if ( it.value().edit == obj )
      id = it.key();
  }

  if ( id == -1 )
    return;
  
  fileNameChanged( id, fileName( id ) );
  
  if ( id == defaultEntry() )
    emit fileNameChanged( fileName() );
}

/*!
 \brief Called when the text in the line edit is changed by the user.
 \param txt current text (not used)
*/
void QtxPathDialog::onTextChanged( const QString& /*txt*/ )
{
  validate();
}

/*!
  \brief Check validity of the entered text and enable/disable standard
  \c OK, \c Yes buttons.
*/
void QtxPathDialog::validate()
{
  setButtonEnabled( isValid(), OK | Yes );
}

/*!
  \brief Check if the entered file/directory name is valid.
  \return \c true if selected file name is valid
*/
bool QtxPathDialog::isValid()
{
  bool ok = true;
  for ( FileEntryMap::Iterator it = myEntries.begin(); it != myEntries.end() && ok; ++it )
  {
    if ( it.value().edit->isEnabled() )
      ok = !it.value().edit->text().trimmed().isEmpty();
  }
  
  return ok;
}

/*!
  \brief Check if the entered data is acceptable.
  \return \c true if entered data is acceptable
*/
bool QtxPathDialog::acceptData() const
{
  bool ok = true;
        
  QWidget* parent = (QWidget*)this;

  FileEntryMap::ConstIterator it;
  for ( it = myEntries.begin(); it != myEntries.end() && ok; ++it )
  {
    const FileEntry& entry = it.value();
    QFileInfo fileInfo( entry.edit->text() );
    if ( entry.edit->text().isEmpty() )
    {
      QMessageBox::critical( parent, windowTitle(), tr( "File name not specified" ),
                             QMessageBox::Ok, QMessageBox::NoButton );
      ok = false;
    }
    else switch ( entry.mode )
    {
    case OpenFile:
      if ( !fileInfo.exists() )
      {
        QMessageBox::critical( parent, windowTitle(), tr( "File \"%1\" does not exist" ).arg( fileInfo.filePath() ),
                               QMessageBox::Ok, QMessageBox::NoButton );
        ok = false;
      }
      break;
    case SaveFile:
      if ( fileInfo.exists() )
        ok = QMessageBox::warning( parent, windowTitle(), tr( "File \"%1\" already exist. Do you want to overwrite it?" ).arg( fileInfo.filePath() ),
                                   QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes;
      break;
    case OpenDir:
      if ( !fileInfo.exists() || !fileInfo.isDir() )
      {
        QMessageBox::critical( parent, windowTitle(), tr( "Directory \"%1\" does not exist" ).arg( fileInfo.filePath() ),
                               QMessageBox::Ok, QMessageBox::NoButton );
        ok = false;
      }
      break;
    case SaveDir:
      if ( fileInfo.exists() && !fileInfo.isDir() )
      {
        QMessageBox::critical( parent, windowTitle(), tr( "Directory \"%1\" can't be created because file with the same name exist" ).arg( fileInfo.filePath() ),
                               QMessageBox::Ok, QMessageBox::NoButton );
        ok = false;
      }
      break;
    case NewDir:
      if ( fileInfo.exists() )
      {
        if ( !fileInfo.isDir() )
        {
          QMessageBox::critical( parent, windowTitle(), tr( "Directory \"%1\" can't be created because file with the same name exist" ).arg( fileInfo.filePath() ),
                                 QMessageBox::Ok, QMessageBox::NoButton );
          ok = false;
        }
        else if ( QDir( fileInfo.filePath() ).count() > 2 )
          ok = QMessageBox::warning( parent, windowTitle(), tr( "Directory \"%1\" not empty. Do you want to remove all files in this directory?" ).arg( fileInfo.filePath() ),
                                     QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes;
      }
      break;
    default:
      break;
    }
    
    if ( !ok )
      entry.edit->setFocus();
  }
  
  return ok;
}

/*!
  \brief Perform custom actions when the file name is changed.

  This method can be redefined in the successor classes.
  Default implementation does nothing.

  \param id file entry
  \param fileName file name
*/
void QtxPathDialog::fileNameChanged( int /*id*/, QString /*fileName*/ )
{
}

/*!
  \fn void QtxPathDialog::fileNameChanged( QString fileName );
  \brief Emitted when the file name is changed.
  \param fileName file name
*/

/*!
  \brief Get options grame widget.
  \return options frame widget
*/
QFrame* QtxPathDialog::optionsFrame()
{
  return myOptionsFrame;
}

/*!
  \brief Get file name from specified entry.
  \param id file entry ID
  \return file name or null string if \a id is invalid
*/
QString QtxPathDialog::fileName( const int id ) const
{
  QString res;
  if ( myEntries.contains( id ) )
    res = myEntries[id].edit->text();
  return res;
}

/*!
  \brief Change file name by specified file entry.
  \param id file entry ID
  \param txt new file name
  \param autoExt if \c true, assign extension automatically
*/
void QtxPathDialog::setFileName( const int id, const QString& txt, const bool autoExt )
{
  int mode;
  QLineEdit* le = fileEntry( id, mode );
        
  if ( le )
  {
    if ( autoExt && ( mode == OpenFile || mode == SaveFile ) )
      le->setText( autoExtension( txt, filter( id ) ) );
    else
      le->setText( txt );
  }
}

/*!
  \brief Get file filter from the specified file entry.
  \param id file entry ID
  \return file filter or null string if \a id is invalid
*/
QString QtxPathDialog::filter( const int id ) const
{
  QString res;
  if ( myEntries.contains( id ) )
    res = myEntries[id].filter;
  return res;
}

/*!
  \brief Set file filter to the specified file entry.
  \param id file entry ID
  \param filter file filter or null string if \a id is invalid
*/
void QtxPathDialog::setFilter( const int id, const QString& filter )
{
  if ( myEntries.contains( id ) )
    myEntries[id].filter = filter;
}

/*!
  \brief Get line edit widget for the specified file entry.
  \param id file entry ID
  \return line edit widget or 0 if \a id is invalid
*/
QLineEdit* QtxPathDialog::fileEntry( const int id ) const
{
  QLineEdit* le = 0;
  if ( myEntries.contains( id ) )
    le = myEntries[id].edit;
  
  return le;
}

/*!
  \brief Get line edit widget and file mode for the specified file entry.
  \param id file entry ID
  \param theMode to return file entry mode
  \return line edit widget or 0 if \a id is invalid
*/
QLineEdit* QtxPathDialog::fileEntry( const int theId, int& theMode ) const
{
  QLineEdit* le = 0;
  if ( myEntries.contains( theId ) )
  {
    le = myEntries[theId].edit;
    theMode = myEntries[theId].mode;
  }
  
  return le;
}

/*!
  \brief Create new file entry.

  If required file entry is already in use or if specified \a id is < 0,
  new ID is generated and returned.

  \param lab file entry title
  \param mode file entry mode
  \param id required file entry ID
  \return created file entry ID
*/
int QtxPathDialog::createFileEntry( const QString& lab, const int mode, 
                                                            const QString& filter, const int id )
{
  int num = id;
  if ( num == -1 )
  {
    num--;
    while ( myEntries.contains( num ) )
      num--;
  }
  
  FileEntry entry;
  entry.dlg = 0;
  entry.mode = mode;
  entry.filter = filter;
  
  new QLabel( lab, myEntriesFrame );
  entry.edit = new QLineEdit( myEntriesFrame );

  entry.btn = new QPushButton( myEntriesFrame );
  entry.btn->setAutoDefault( false );
  entry.btn->setIcon( QPixmap( open_icon ) );

  Qtx::PathType type = Qtx::PT_OpenFile;
  switch ( mode )
  {
  case OpenFile:
    type = Qtx::PT_OpenFile;
    break;
  case SaveFile:
    type = Qtx::PT_SaveFile;
    break;
  case OpenDir:
  case SaveDir:
  case NewDir:
    type = Qtx::PT_Directory;
    break;
  }
  entry.edit->setCompleter( Qtx::pathCompleter( type, filter ) );

  connect( entry.btn, SIGNAL( clicked() ), this, SLOT( onBrowse() ) );
  connect( entry.edit, SIGNAL( returnPressed() ), this, SLOT( onReturnPressed() ) );
  connect( entry.edit, SIGNAL( textChanged( const QString& ) ), this, SLOT( onTextChanged( const QString& ) ) );
  
  myEntries.insert( num, entry );
  
  return num;
}

/*!
  \brief Get default file entry ID.
  \return default entry ID
*/
int QtxPathDialog::defaultEntry() const
{
  return myDefault;
}

/*!
  \brief Set default entry.
  \param id new default entry ID
*/
void QtxPathDialog::setDefaultEntry( const int id )
{
  myDefault = id;
}

/*!
  \brief Initialize dialog layout.
*/
void QtxPathDialog::initialize()
{
  setWindowTitle( tr( "File dialog" ) );

  QVBoxLayout* main = new QVBoxLayout( mainFrame() );
  main->setMargin( 0 );

  QtxGroupBox* base = new QtxGroupBox( "", mainFrame() );
  main->addWidget( base );
  
  QtxGridBox*  mainGroup = new QtxGridBox( 1, Qt::Horizontal, base, 0 );
  base->setWidget( mainGroup );
  
  myEntriesFrame = new QtxGridBox( 3, Qt::Horizontal, mainGroup );
  myOptionsFrame = new QFrame( mainGroup );
}

/*!
  \brief Prepare file filters.
  \param list of file masks, separated by ';;', for example, "*.h;;*.cxx"
  \return list of processed file filters
*/
QStringList QtxPathDialog::prepareFilters( const QString& filter ) const
{
  QStringList res;
  bool allFilter = false;
  if ( !filter.isEmpty() )
  {
    res = filter.split( ";;" );
    for ( QStringList::ConstIterator it = res.begin(); it != res.end() && !allFilter; ++it )
    {
      QStringList wildCards = filterWildCards( *it );
      allFilter = wildCards.indexOf( "*.*" ) != -1;
    }
  }
  
  if ( !allFilter )
    res.append( tr( "All files (*.*)" ) );
  
  return res;
}

/*!
  \brief Get wildcards from the specified file filter.
  \param theFilter file filter being processed
  \return list of filters with filtered wild cards
*/
QStringList QtxPathDialog::filterWildCards( const QString& theFilter ) const
{
  QStringList res;

  int b = theFilter.lastIndexOf( "(" );
  int e = theFilter.lastIndexOf( ")" );
  if ( b != -1 && e != -1 )
  {
    QString content = theFilter.mid( b + 1, e - b - 1 ).trimmed();
    QStringList lst = content.split( " " );
    for ( QStringList::ConstIterator it = lst.begin(); it != lst.end(); ++it )
    {
      if ( (*it).indexOf( "." ) != -1 )
        res.append( (*it).trimmed() );
    }
  }
  return res;
}

/*!
  \brief Get file file name with automatically assigned extension.
  \param theFileName file name being processed
  \param theFilter list of file filters
  \return file name with assigned extension
*/
QString QtxPathDialog::autoExtension( const QString& theFileName, const QString& theFilter ) const
{
  QString fName = theFileName;

  if ( fName.isEmpty() )
    return fName;
  
  QString filter;
  QStringList filters = prepareFilters( theFilter );
  if ( !filters.isEmpty() )
    filter = filters.first();

  QStringList wildCards = filterWildCards( filter );
  if ( !wildCards.isEmpty() )
  {
    QString ext = wildCards.first();
    if ( ext.indexOf( "." ) != -1 )
      ext = ext.mid( ext.indexOf( "." ) + 1 );
    
    if ( !ext.isEmpty() && !ext.contains( "*" ) )
      fName = QDir::toNativeSeparators( fName ) + QString( "." ) + ext;
  }
  
  return fName;
}

/*!
  \brief Check if there are visible child widgets.
  \param wid parent widget being checked
  \return \c true if widget \a wid has visible children
*/
bool QtxPathDialog::hasVisibleChildren( QWidget* wid ) const
{
  bool res = false;
  if ( wid )
  {
    const QObjectList& aChildren = wid->children();
    for ( QObjectList::const_iterator it = aChildren.begin(); it != aChildren.end() && !res; ++it )
    {
      if ( (*it)->isWidgetType() )
        res = ((QWidget*)(*it))->isVisibleTo( wid );
    }
  }
  return res;
}

/*!
  \brief Upadte dialof box's child widgets visibility state.
*/
void QtxPathDialog::updateVisibility()
{
  if ( hasVisibleChildren( myEntriesFrame ) )
    myEntriesFrame->show();
  else
    myEntriesFrame->hide();
  
  if ( hasVisibleChildren( myOptionsFrame ) )
    myOptionsFrame->show();
  else
    myOptionsFrame->hide();
}
