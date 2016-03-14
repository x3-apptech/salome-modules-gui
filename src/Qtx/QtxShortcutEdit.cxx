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

#include "QtxShortcutEdit.h"

#include <QWidget>
#include <QLayout>
#include <QList>

#include <QToolButton>
#include <QLineEdit>
#include <QTableWidgetItem>
#include <QMessageBox>

#include <QKeyEvent>
#include <QKeySequence>

#define COLUMN_SIZE  500

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

/*!
  \brief Constructor
  \param parent parent widget
*/
QtxShortcutEdit::QtxShortcutEdit( QWidget* parent )
: QFrame( parent )
{
  initialize();
  myShortcut->installEventFilter(this);
}

/*!
  \brief Destructor
*/
QtxShortcutEdit::~QtxShortcutEdit()
{
}

/*!
  \brief Sets custom shortcut
  \param seq a key sequence describes a combination of keys
  \sa shortcut()
*/
void QtxShortcutEdit::setShortcut( const QKeySequence& seq )
{
  QString txt = seq.toString(); 
  myPrevShortcutText = txt;
  myShortcut->setText( txt ); 
}

/*!
  \brief Gets custom shortcut 
  \return a key sequence describes a combination of keys
  \sa setShortcut()
*/
QKeySequence QtxShortcutEdit::shortcut()
{
  return QKeySequence::fromString( myShortcut->text() );
}

/*!
  \brief Gets the key sequence from keys that were pressed 
  \param e a key event
  \return a string representation of the key sequence
*/
QString QtxShortcutEdit::parseEvent( QKeyEvent* e )
{
  bool isShiftPressed = e->modifiers() & Qt::ShiftModifier;
  bool isControlPressed = e->modifiers() & Qt::ControlModifier;
  bool isAltPressed = e->modifiers() & Qt::AltModifier;
  bool isMetaPressed = e->modifiers() & Qt::MetaModifier;
  bool isModifiersPressed = isShiftPressed || isControlPressed || isAltPressed || isMetaPressed;
  int result=0;
  if( isControlPressed )
    result += Qt::CTRL;
  if( isAltPressed )
    result += Qt::ALT;
  if( isShiftPressed )
    result += Qt::SHIFT;
  if( isMetaPressed )
    result += Qt::META;

  int aKey = e->key();
  if ( ( isValidKey( aKey ) && isModifiersPressed ) || ( ( aKey >= Qt::Key_F1 ) && ( aKey <= Qt::Key_F12 ) ) )
    result += aKey;

  return QKeySequence( result ).toString();
}

/*!
  \brief Check if the key event contains a 'valid' key
  \param aKey the code of the key
  \return \c true if the key is 'valid'
*/

bool QtxShortcutEdit::isValidKey( int aKey )
{
  if ( aKey == Qt::Key_Underscore || aKey == Qt::Key_Escape || 
     ( aKey >= Qt::Key_Backspace && aKey <= Qt::Key_Delete ) || 
     ( aKey >= Qt::Key_Home && aKey <= Qt::Key_PageDown ) || 
     ( aKey >= Qt::Key_F1 && aKey <= Qt::Key_F12 )  ||
     ( aKey >= Qt::Key_Space && aKey <= Qt::Key_Asterisk ) ||
     ( aKey >= Qt::Key_Comma && aKey <= Qt::Key_Question ) ||
     ( aKey >= Qt::Key_A && aKey <= Qt::Key_AsciiTilde ) )
    return true;
  return false;
}

/*!
  \brief Called when "Clear" button is clicked.
*/
void QtxShortcutEdit::onCliked() 
{
  myShortcut->setText( "" );
}

/*!
  \brief Called when myShortcut loses focus.
*/
void QtxShortcutEdit::onEditingFinished() 
{
  if ( myShortcut->text().endsWith("+") )
    myShortcut->setText( myPrevShortcutText );
}

/*!
  \brief Custom event filter.
  \param obj event receiver object
  \param event event
  \return \c true if further event processing should be stopped
*/
bool QtxShortcutEdit::eventFilter(QObject* obj, QEvent* event) 
{ 
  if ( obj == myShortcut ) { 
    if (event->type() == QEvent::KeyPress ) {
      QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
      QString text = parseEvent( keyEvent );
      if ( keyEvent->key() == Qt::Key_Delete || keyEvent->key() == Qt::Key_Backspace )
        onCliked();
      if ( text != "" )
        myShortcut->setText( text );
      return true;
    }
    if ( event->type() == QEvent::KeyRelease ) {
      if ( myShortcut->text().endsWith("+") )
        myShortcut->setText( myPrevShortcutText );
      else myPrevShortcutText = myShortcut->text();

      return true;
    }
  } 
  return false;
}

/*
  \brief Perform internal intialization.
*/
void QtxShortcutEdit::initialize()
{
  myPrevShortcutText = QString();

  QHBoxLayout* base = new QHBoxLayout( this );
  base->setMargin( 0 );
  base->setSpacing( 5 );

  base->addWidget( myShortcut = new QLineEdit( this ) );

  QToolButton* deleteBtn = new QToolButton();
  deleteBtn->setIcon( QPixmap( delete_icon ) );
  base->addWidget( deleteBtn );
 
  myShortcut->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
  deleteBtn->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

  connect( deleteBtn, SIGNAL( clicked() ), this, SLOT( onCliked() ) );
  connect( myShortcut, SIGNAL( editingFinished() ), this, SLOT( onEditingFinished() ) );
}

/*!
  \brief Constructor
  \param parent parent widget
*/
QtxShortcutTree::QtxShortcutTree( QWidget * parent ) : QTreeWidget( parent )
{
  setColumnCount( 2 );
  setSelectionMode( QAbstractItemView::SingleSelection );
  setColumnWidth ( 0, COLUMN_SIZE);
  setSortingEnabled(false);
  headerItem()->setHidden ( true ); 

  this->installEventFilter(this);
  connect( this, SIGNAL( currentItemChanged ( QTreeWidgetItem*, QTreeWidgetItem* ) ), this, SLOT( onCurrentItemChanged ( QTreeWidgetItem*, QTreeWidgetItem* ) ) );

}

/*!
  \brief Destructor
*/
QtxShortcutTree::~QtxShortcutTree(){}

/*!
  \brief Custom event filter. 
  \param obj event receiver object
  \param event event
  \return \c true if further event processing should be stopped
*/
bool QtxShortcutTree::eventFilter(QObject* obj, QEvent* event) 
{ 
  if ( currentItem() && currentItem()->isSelected() ) {
    
    if (event->type() == QEvent::KeyPress ) {
      QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
      QString text = QtxShortcutEdit::parseEvent( keyEvent );
      if ( keyEvent->key() == Qt::Key_Delete || keyEvent->key() == Qt::Key_Backspace )
        currentItem()->setText( 1, "" );
      if ( text != "" ) {
	if ( text.endsWith( "+" ) || checkUniqueness( currentItem(), text ) )
	   currentItem()->setText( 1, text );
      }
      return true;
    }
    if ( event->type() == QEvent::KeyRelease ) {
      if ( currentItem()->text( 1 ).endsWith( "+" ) )
	currentItem()->setText( 1, myPrevBindings[ currentItem()->parent()->text( 0 ) ][ currentItem()->text( 0 ) ] );
      else myPrevBindings[ currentItem()->parent()->text( 0 ) ][ currentItem()->text( 0 ) ] = currentItem()->text( 1 );

      return true;
    }    
  } 
  return false;
}

/*!
  \brief Called when the current item changes.
  \param cur the current item
  \param prev the previous current item
*/
void QtxShortcutTree::onCurrentItemChanged( QTreeWidgetItem* cur, QTreeWidgetItem* prev )
{
  if ( prev && prev->text( 1 ).endsWith( "+" ) )
      prev->setText( 1, myPrevBindings[ prev->parent()->text( 0 ) ][ prev->text( 0 ) ] );
}

/*!
  \brief Set key bindings to the tree
  \param title the name of top-level item
  \param theShortcutMap map of key bindings
*/
void QtxShortcutTree::setBindings( const QString& title, const ShortcutMap& theShortcutMap )
{
  QTreeWidgetItem* item= new QTreeWidgetItem();
  QFont font = item->font(0);
  font.setBold(true);
  
  if ( findItems( title, Qt::MatchFixedString ).isEmpty()  ) {
    item->setText( 0, title );
    item->setFont( 0, font );
    addTopLevelItem( item );
    item->setFlags( Qt::ItemIsEnabled );
  } else {
    item = findItems( title, Qt::MatchFixedString ).first();
    item->takeChildren();
  }
  for( ShortcutMap::const_iterator it = theShortcutMap.constBegin(); it != theShortcutMap.constEnd(); ++it )
      item->addChild( new QTreeWidgetItem( QStringList() << it.key() << it.value() ) );
  myPrevBindings.insert( title, theShortcutMap);
}

/*!
  \brief Get all sections names.
  \return list of section names
*/
QStringList QtxShortcutTree::sections() const
{
  QStringList lst;
  for( int i = 0; i < topLevelItemCount(); i++ )
    lst << topLevelItem( i )->text( 0 ); 
  return lst;
}

ShortcutMap* QtxShortcutTree::bindings( const QString& sec ) const
{
  ShortcutMap* aMap = new ShortcutMap();
  QTreeWidgetItem* item = findItems( sec, Qt::MatchFixedString ).first();
  int nbChildren = item->childCount();

  for( int i = 0; i < nbChildren; i++ ) {
    QTreeWidgetItem* child =  item->child(i);
    aMap->insert( child->text( 0 ), child->text( 1 ) );
  }

  return aMap;
}

void QtxShortcutTree::focusOutEvent ( QFocusEvent* event )
{
  QWidget::focusOutEvent( event );
  if ( currentItem() && currentItem()->isSelected() )
    currentItem()->setText( 1, myPrevBindings[ currentItem()->parent()->text( 0 ) ][ currentItem()->text( 0 ) ] );
}

/*!
  \brief Set the list of shortcuts general sections.
  
  Key combinations in general sections should not intersect
  with any other key combinations.

  \param sectionsList list of common section names
*/
void QtxShortcutTree::setGeneralSections( const QStringList& sectionsList )
{
  myGeneralSections = sectionsList;
}

/*!
  \brief Check uniqueness of the shortcut.
  \param item current item of the shortcut tree
  \param shortcut shortcut appointed for the current item
  \return \c true if the given shortcut is allowed
*/
bool QtxShortcutTree::checkUniqueness( QTreeWidgetItem* item, const QString& shortcut )
{
  // List of sections to check shortcut intersections
  QStringList sectionsList;

  // Current section
  QString currentSection = currentItem()->parent()->text( 0 );

  // If the current section is general 
  if ( myGeneralSections.contains(currentSection) ) {
    sectionsList = sections();
    int currentSectionIndex = sectionsList.indexOf(currentSection);
    sectionsList.move( currentSectionIndex, 0);
  } 
  else {
    sectionsList = myGeneralSections;
    sectionsList.prepend(currentSection);
  }

  // Iterate on sections
  QStringList::const_iterator it;
  for( it = sectionsList.constBegin(); it != sectionsList.constEnd(); ++it ) {
    QString section = *it;

    // Iterate on actual section
    QTreeWidgetItem* sectionRoot = findItems( section, Qt::MatchFixedString ).first();
    int nbChildren = sectionRoot->childCount();

    for( int i = 0; i < nbChildren; i++ ) {
      QTreeWidgetItem* child =  sectionRoot->child(i);
      
      if ( (child != item) && (shortcut == child->text( 1 )) ) {
	bool res = QMessageBox::warning( parentWidget(), tr("Warning"), 
					 tr("The \"%1\" shortcut has already used by the \"%2\" action.\n")
					 .arg(shortcut, section + ":" + child->text( 0 ) ) +
					 tr("Do you want to reassign it from that action to the current one?"),
					 QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes;
	if (res) 
	  child->setText( 1, "" );
	return res;	
      }
    }
  }

  return true;
}
