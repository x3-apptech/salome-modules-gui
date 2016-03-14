// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File:      QtxPathListEdit.cxx
// Author:    Sergey TELKOV
//
#include "QtxPathListEdit.h"

#include "QtxPathEdit.h"

#include <QLayout>
#include <QPainter>
#include <QListView>
#include <QLineEdit>
#include <QKeyEvent>
#include <QDirModel>
#include <QCompleter>
#include <QToolButton>
#include <QMessageBox>
#include <QFileDialog>
#include <QItemDelegate>
#include <QStringListModel>

static const char* delete_icon[] = {
"16 16 3 1",
"` c #810000",
"  c none",
"# c #ffffff",
"                ",
"                ",
" ``#        ``# ",
" ````#     ``#  ",
"  ````#   ``#   ",
"    ```# `#     ",
"     `````#     ",
"      ```#      ",
"     `````#     ",
"    ```# ``#    ",
"   ```#   ``#   ",
"  ```#     `#   ",
"  ```#      `#  ",
"   `#        `# ",
"                ",
"                "
};

static const char* insert_icon[] = {
"16 16 5 1",
"` c #000000",
". c #ffff00",
"# c #9d9da1",
"  c none",
"b c #ffffff",
"                ",
"                ",
" #  #b #.       ",
"  # #.#.` ` `   ",
"  .#.b####   `  ",
" ### ..         ",
"  . # .#     `  ",
" #` #.          ",
"    #        `  ",
"  `             ",
"             `  ",
"  `             ",
"             `  ",
"  ` ` ` ` ` `   ",
"                ",
"                "
};

static const char* movedown_icon[] = {
"16 16 2 1",
"` c #000000",
"  c none",
"                ",
"                ",
"         ```    ",
"        ```     ",
"       ```      ",
"       ```      ",
"       ```      ",
"       ```      ",
"   ```````````  ",
"    `````````   ",
"     ```````    ",
"      `````     ",
"       ```      ",
"        `       ",
"                ",
"                "
};

static const char* moveup_icon[] = {
"16 16 2 1",
"` c #000000",
"  c none",
"                ",
"                ",
"        `       ",
"       ```      ",
"      `````     ",
"     ```````    ",
"    `````````   ",
"   ```````````  ",
"       ```      ",
"       ```      ",
"       ```      ",
"       ```      ",
"      ```       ",
"     ```        ",
"                ",
"                "
};


/*!
  \class QtxPathListEdit::Editor
  \brief Path editor widget
  \internal
*/

class QtxPathListEdit::Editor : public QtxPathEdit
{
public:
  /*!
    \brief Constructor
    \internal
  */
  Editor( QWidget* parent = 0 ) : QtxPathEdit( parent, true )
  {
    layout()->setSpacing( 0 );
    lineEdit()->setFrame( false );
  }

  /*!
    \brief Destructor
    \internal
  */
  virtual ~Editor() {}
};

/*!
  \class QtxPathListEdit::Delegate
  \brief Custom item delegate for the paths list widget.
  \internal
*/

class QtxPathListEdit::Delegate : public QItemDelegate
{
public:
  Delegate( QtxPathListEdit*, QObject* = 0 );
  virtual ~Delegate();

  virtual QWidget* createEditor( QWidget*, const QStyleOptionViewItem&, const QModelIndex& ) const;
  virtual void     setModelData( QWidget*, QAbstractItemModel*, const QModelIndex& ) const;
  virtual void     setEditorData( QWidget*, const QModelIndex& ) const;
  virtual void     paint( QPainter*, const QStyleOptionViewItem&, const QModelIndex& ) const;

protected:
  virtual void     drawFocus( QPainter*, const QStyleOptionViewItem&, const QRect& ) const;

private:
  QtxPathListEdit* myPathEdit;
};

/*!
  \brief Constructor.
  \internal
  \param pe path list editor
  \param parent parent widget
*/
QtxPathListEdit::Delegate::Delegate( QtxPathListEdit* pe, QObject* parent )
: QItemDelegate( parent ),
  myPathEdit( pe )
{
}

/*!
  \brief Destructor.
  \internal
*/
QtxPathListEdit::Delegate::~Delegate()
{
}

/*!
  \brief Create editor widget.
  \internal
  \param parent parent widget
  \param option style option
  \param index data model index
*/
QWidget* QtxPathListEdit::Delegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option,
                                                  const QModelIndex& index ) const
{
  return myPathEdit->createEditor( parent );
}

/*!
  \brief Set modified data back to the data model.
  \internal
  \param editor editor widget
  \param model data model
  \param index data model index
*/
void QtxPathListEdit::Delegate::setModelData( QWidget* editor, QAbstractItemModel* model,
                                              const QModelIndex& index ) const
{
  myPathEdit->setModelData( editor, index );
}

/*!
  \brief Set data from the data model to the editor.
  \internal
  \param editor editor widget
  \param index data model index
*/
void QtxPathListEdit::Delegate::setEditorData( QWidget* editor, const QModelIndex& index ) const
{
  myPathEdit->setEditorData( editor, index );
}

/*!
  \brief Customize paint operation.
  \internal
  \param painter painter
  \param option style option
  \param index data model index
*/
void QtxPathListEdit::Delegate::paint( QPainter* painter, const QStyleOptionViewItem& option,
                                       const QModelIndex& index ) const
{
  QPalette::ColorGroup cg = option.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
  if ( cg == QPalette::Normal && !( option.state & QStyle::State_Active ) )
    cg = QPalette::Inactive;

  if ( option.state & QStyle::State_Selected )
  {
    painter->fillRect( option.rect, option.palette.brush( cg, QPalette::Highlight ) );
    painter->setPen( option.palette.color( cg, QPalette::HighlightedText ) );
  }
  else
    painter->setPen( option.palette.color( cg, QPalette::Text ) );

  QItemDelegate::paint( painter, option, index );
}

/*!
  \brief Customize drawing selection focus operation.
  \internal
  \param painter painter
  \param option style option
  \param rect selection rectangle
*/
void QtxPathListEdit::Delegate::drawFocus( QPainter* painter, const QStyleOptionViewItem& option,
                                           const QRect& rect ) const
{
  QItemDelegate::drawFocus( painter, option, option.rect );
}

/*!
  \class QtxPathListEdit
  \brief The QtxPathListEdit class represents a widget for files or 
  directories paths list preference items editing.

  The path list preference item is represented as the list box widget.
  It provides such operations like adding new file/directory path to the
  list, removing selected paths and modifying of already entered ones.

  The widget can be used in two modes: list of files or list of directories.
  The mode defines the type of the standard browse dialog box which is
  invoked on the browse button clicking.

  Initial path list value can be set with setPathList() method. Chosen path
  list can be retrieved with the pathList() method. The widget mode can be set 
  with setPathType() and retrieved with pathType() method.

  In addition, it is possible to add path items to the list with the insert()
  method, remove items with the remove() methods, clear all the widget contents
  with the clear() method. To get the number of entered paths can be retrieved
  with the count() method. To check if any path already exists in the paths list,
  use contains() method.
*/

/*!
  \brief Constructor.
  \param type widget mode (Qtx::PathType)
  \param parent parent widget
  \sa pathType(), setPathType()
*/
QtxPathListEdit::QtxPathListEdit( const Qtx::PathType type, QWidget* parent )
: QFrame( parent ),
  myCompleter( 0 ),
  myType( type ),
  myDuplicate( false )
{
  initialize();
}

/*!
  \brief Constructor.

  Qtx::PT_OpenFile mode is used by default.

  \param parent parent widget
  \sa pathType(), setPathType()
*/
QtxPathListEdit::QtxPathListEdit( QWidget* parent )
: QFrame( parent ),
  myCompleter( 0 ),
  myType( Qtx::PT_OpenFile ),
  myDuplicate( false )
{
  initialize();
}

/*!
  \brief Destructor.
*/
QtxPathListEdit::~QtxPathListEdit()
{
}

/*!
  \brief Get widget mode.
  \return currently used widget mode (Qtx::PathType)
  \sa setPathType()
*/
Qtx::PathType QtxPathListEdit::pathType() const
{
  return myType;
}

/*!
  \brief Set widget mode.
  \param t new widget mode (Qtx::PathType)
  \sa pathType()
*/
void QtxPathListEdit::setPathType( const Qtx::PathType t )
{
  if ( myType == t )
    return;

  myType = t;

  delete myCompleter;
  myCompleter = 0;
}

/*!
  \brief Get currently selected paths list.
  \return files or directories paths list entered by the user
  \sa setPathList()
*/
QStringList QtxPathListEdit::pathList() const
{
  return myModel->stringList();
}

/*!
  \brief Set paths list.
  \param lst files or directories paths list
  \sa pathList()
*/
void QtxPathListEdit::setPathList( const QStringList& lst )
{
  myModel->setStringList( lst );
}

/*!
  \brief Check if the duplication of paths is enabled.
  \return \c true if the duplication is enabled
*/
bool QtxPathListEdit::isDuplicateEnabled() const
{
  return myDuplicate;
}

/*!
  \brief Enable/disable paths duplication.
  \param on new flag value
*/
void QtxPathListEdit::setDuplicateEnabled( const bool on )
{
  myDuplicate = on;
}

/*!
  \brief Get number of currently entered paths.
  \return current paths number
*/
int QtxPathListEdit::count() const
{
  return myModel->rowCount();
}

/*!
  \brief Check if the specified \a path already exists in 
  the paths list.
  \param path path to be checked
  \return \c true if the path is already selected by the user 
  or \c false otherwise
*/
bool QtxPathListEdit::contains( const QString& path ) const
{
  return myModel->stringList().contains( path );
}

/*!
  \brief Clear paths list.
*/
void QtxPathListEdit::clear()
{
  myModel->removeRows( 0, myModel->rowCount() );
}

/*!
  \brief Remove path from the paths list.
  \param idx path index in the list
*/
void QtxPathListEdit::remove( const int idx )
{
  if ( 0 <= idx && idx < myModel->rowCount() )
    myModel->removeRow( idx );
}

/*!
  \brief Remove path from the paths list.
  \param path path to be removed
*/
void QtxPathListEdit::remove( const QString& path )
{
  QModelIndexList indexes = myModel->match( myModel->index( 0, 0 ), Qt::DisplayRole, path,
                                            myModel->rowCount(), Qt::MatchExactly | Qt::MatchCaseSensitive );
  while ( !indexes.isEmpty() )
  {
    myModel->removeRow( indexes.last().row() );
    indexes.removeLast();
  }
}

/*!
  \brief Add path to the list of paths.

  If the specified index is out of range, the path is added to 
  the end of the list.

  \param path path to be added
  \param idx index in the list to which the path should be inserted.
*/
void QtxPathListEdit::insert( const QString& path, const int idx )
{
  int index = idx < 0 ? myModel->rowCount() : qMin( idx, myModel->rowCount() );
  if ( myModel->insertRow( index ) )
    myModel->setData( myModel->index( index, 0 ), path, Qt::EditRole );
}

/*
bool QtxPathListEdit::validate( const bool quietMode )
{
  if ( myEdited )
  {
    QString dirPath = QFileInfo( myEdit->text().stripWhiteSpace() ).filePath();
    QDir dir(dirPath);
    QListBoxItem* found = 0;
    for (unsigned i = 0; i < myList->count()-1; i++) {
      QDir aDir(myList->text(i));
      if ( aDir.canonicalPath().isNull() && myList->text(i) == dir.absPath() ||
          !aDir.canonicalPath().isNull() && aDir.exists() && aDir.canonicalPath() == dir.canonicalPath()) {
          found = myList->item(i);
        break;
      }
    }
    if (dirPath.isEmpty()) {
      if (found) {
        // it should be last (empty) item in the list - nothing to do
        return true;
      }
      else {
        // delete directory from the list
        removeDir(myLastSelected);
        return true;
      }
    }
    else {
      if (found) {
        if (found != myLastSelected) {
          // it is forbidden to add directory more then once
          if ( !quietMode )
            QMessageBox::critical(this, 
                                  tr("Error"),
                                  tr("Directory already specified."), 
                                  tr("Ok"));
          myEdit->setFocus();
          return false;
        }
      }
      else {
        if (!dir.exists()) {
          if ( !quietMode && QMessageBox::information(this, 
                                                      tr("Warning"),
                                                      tr("%1\n\nThe directory doesn't exist.\nAdd directory anyway?").arg(dir.absPath()),
                                                      tr("Yes"), tr("No"), QString(), 1, 1) == 1) {
            myEdit->setFocus();
            return false;
          }
        }
        // append
        appendDir(myLastSelected, dir.absPath());
      }
    }
  }
  return true;
}
*/

/*!
  \brief Customize child widget events processing.
  \param o event receiver object
  \param e event
  \return \c true if the further event processing should be stopped.
*/
bool QtxPathListEdit::eventFilter( QObject* o, QEvent* e )
{
  if ( e->type() == QEvent::KeyPress )
  {
    QKeyEvent* ke = (QKeyEvent*)e;
    if ( ke->key() == Qt::Key_Delete )
      onDelete();
    else if ( ke->key() == Qt::Key_Insert )
      onInsert();
    else if ( ke->key() == Qt::Key_Up && ke->modifiers() == Qt::CTRL )
    {
      onUp();
      return true;
    }
    else if ( ke->key() == Qt::Key_Down && ke->modifiers() == Qt::CTRL )
    {
      onDown();
      return true;
    }
  }

  return QFrame::eventFilter( o, e );
}

/*!
  \brief Called when <Insert> button is clicked.

  Inserts new empty line to the list and sets input focus to it.

  \param on (not used)
*/
void QtxPathListEdit::onInsert( bool /*on*/ )
{
  int empty = -1;
  QStringList lst = myModel->stringList();
  for ( int r = 0; r < lst.count() && empty == -1; r++ )
  {
    if ( lst.at( r ).isEmpty() )
      empty = r;
  }

  if ( empty == -1 )
    myModel->insertRows( empty = myModel->rowCount(), 1 );

  QModelIndex idx = myModel->index( empty, 0 );
  myList->setCurrentIndex( idx );
  myList->edit( idx );
}

/*!
  \brief Called when <Delete> button is clicked.

  Removes currently selected path item.
  
  \param on (not used)
*/
void QtxPathListEdit::onDelete( bool )
{
  QModelIndex idx = myList->currentIndex();
  if ( !idx.isValid() )
    return;

  myModel->removeRow( idx.row() );
}

/*!
  \brief Called when <Up> button is clicked.

  Move currently selected path item up to one row in the paths list.

  \param on (not used)
*/
void QtxPathListEdit::onUp( bool )
{
  QModelIndex idx = myList->currentIndex();
  if ( !idx.isValid() || idx.row() < 1 )
    return;

  QModelIndex toIdx = myModel->index( idx.row() - 1, 0 );

  QVariant val = myModel->data( toIdx, Qt::DisplayRole );
  myModel->setData( toIdx, myModel->data( idx, Qt::DisplayRole ), Qt::DisplayRole );
  myModel->setData( idx, val, Qt::DisplayRole );

  myList->setCurrentIndex( toIdx );
}

/*!
  \brief Called when <Down> button is clicked.

  Move currently selected path item down to one row in the paths list.

  \param on (not used)
*/
void QtxPathListEdit::onDown( bool )
{
  QModelIndex idx = myList->currentIndex();
  if ( !idx.isValid() || idx.row() >= myModel->rowCount() - 1 )
    return;

  QModelIndex toIdx = myModel->index( idx.row() + 1, 0 );

  QVariant val = myModel->data( toIdx, Qt::DisplayRole );
  myModel->setData( toIdx, myModel->data( idx, Qt::DisplayRole ), Qt::DisplayRole );
  myModel->setData( idx, val, Qt::DisplayRole );

  myList->setCurrentIndex( toIdx );
}

/*!
  \brief Perform internal widget initialization.
*/
void QtxPathListEdit::initialize()
{
  QVBoxLayout* base = new QVBoxLayout( this );
  base->setMargin( 0 );
  base->setSpacing( 5 );

  QWidget* cBox = new QWidget( this );
  base->addWidget( cBox );
  
  QHBoxLayout* cLayout = new QHBoxLayout( cBox );
  cLayout->setMargin( 0 );
  cLayout->setSpacing( 0 );

  cLayout->addStretch( 1 );

  QToolButton* insertBtn = new QToolButton( cBox );
  insertBtn->setIcon( QPixmap( insert_icon ) );
  cLayout->addWidget( insertBtn );

  QToolButton* deleteBtn = new QToolButton( cBox );
  deleteBtn->setIcon( QPixmap( delete_icon ) );
  cLayout->addWidget( deleteBtn );

  QToolButton* upBtn = new QToolButton( cBox );
  upBtn->setIcon( QPixmap( moveup_icon ) );
  cLayout->addWidget( upBtn );

  QToolButton* downBtn = new QToolButton( cBox );
  downBtn->setIcon( QPixmap( movedown_icon ) );
  cLayout->addWidget( downBtn );


  myList = new QListView( this );
  myList->setAlternatingRowColors( true );
  myList->setItemDelegate( new Delegate( this, myList ) );
  myList->setModel( myModel = new QStringListModel( myList ) );
  myList->setSelectionMode( QListView::SingleSelection );
  myList->setSelectionBehavior( QListView::SelectRows );
  myList->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  myList->setEditTriggers( QListView::DoubleClicked );
  myList->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
  myList->installEventFilter( this );

  base->addWidget( myList );

  connect( insertBtn, SIGNAL( clicked( bool ) ), this, SLOT( onInsert( bool ) ) );
  connect( deleteBtn, SIGNAL( clicked( bool ) ), this, SLOT( onDelete( bool ) ) );
  connect( upBtn,     SIGNAL( clicked( bool ) ), this, SLOT( onUp( bool ) ) );
  connect( downBtn,   SIGNAL( clicked( bool ) ), this, SLOT( onDown( bool ) ) );
}

/*!
  \brief Create editor widget.
  \param parent parent widget for the editor
  \return created editor widget
*/
QWidget* QtxPathListEdit::createEditor( QWidget* parent )
{
  QtxPathEdit* edit = new Editor( parent );
  edit->setPathType( pathType() );
  return edit;
}

/*!
  \brief Set modified data from the editor to the list widget.
  \param editor editor widget
  \param index data model index
*/
void QtxPathListEdit::setModelData( QWidget* editor, const QModelIndex& index )
{
  QtxPathEdit* edit = ::qobject_cast<QtxPathEdit*>( editor );
  if ( !edit )
    return;

  QString path = edit->path().trimmed();

  if ( !isDuplicateEnabled() && !checkDuplicate( path, index.row() ) )
    return;

  if ( !checkExistance( path ) )
    return;

  myModel->setData( index, path, Qt::EditRole );
}

/*!
  \brief Set data to the editor from the list widget when 
  user starts path edition.
  \param editor editor widget
  \param index data model index
*/
void QtxPathListEdit::setEditorData( QWidget* editor, const QModelIndex& index )
{
  QtxPathEdit* edit = ::qobject_cast<QtxPathEdit*>( editor );
  if ( !edit )
    return;

  QVariant v = myModel->data( index, Qt::EditRole );
  edit->setPath( v.toString() );
}

/*!
  \brief Check if path is correct (exists) and optionally 
  show the question message box.
  \param str path to be checked
  \param msg if \c true and path does not exist, question message box is shown
  \return \c true if the user confirms the path adding
*/
bool QtxPathListEdit::checkExistance( const QString& str, const bool msg )
{
  if ( pathType() == Qtx::PT_SaveFile )
    return true;

  QFileInfo aFI = QFileInfo( Qtx::makeEnvVarSubst( str ) );
  bool ok = aFI.exists();
  if ( !ok && msg )
    ok = QMessageBox::question( this, tr( "Warning" ), tr( "Path \"%1\" doesn't exist. Add it to list anyway?" ).arg( str ),
                                QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes;

  if ( ok && aFI.exists() )
  {
    switch ( pathType() )
    {
    case Qtx::PT_OpenFile:
      ok = aFI.isFile();
      if ( !ok && msg )
        QMessageBox::warning( this, tr( "Error" ), tr( "Location \"%1\" doesn't point to file" ).arg( str ) );
      break;
    case Qtx::PT_Directory:
      ok = aFI.isDir();
      if ( !ok && msg )
        QMessageBox::warning( this, tr( "Error" ), tr( "Location \"%1\" doesn't point to directory" ).arg( str ) );
      break;
    }
  }

  return ok;
}

/*!
  \brief Check if path already exists in the list and optionally
  show the warning message box.
  \param str path to be checked
  \param row row corresponding to the path checked
  \param msg if \c true and path does not exist, warning message box is shown
  \return \c true if the user confirms the path adding
*/
bool QtxPathListEdit::checkDuplicate( const QString& str, const int row, const bool msg )
{
  int cur = -1;
  QStringList lst = myModel->stringList();
  for ( int r = 0; r < lst.count() && cur == -1; r++ )
  {
    if ( r != row && lst.at( r ) == str )
      cur = r;
  }

  if ( cur != -1 && msg )
    QMessageBox::warning( this, tr( "Error" ), tr( "Path \"%1\" already exist in the list" ).arg( str ) );
   
  return cur == -1;
}
