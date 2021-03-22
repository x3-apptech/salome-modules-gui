// Copyright (C) 2007-2021  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "QtxInfoPanel.h"

#include <QAction>
#include <QFont>
#include <QGroupBox>
#include <QLabel>
#include <QMap>
#include <QPalette>
#include <QSizePolicy>
#include <QToolButton>
#include <QVBoxLayout>
#include <QScrollArea>

/*!
  \internal
  \class QtxInfoPanel::Container
  \brief Container to store widgets within info panel
*/
class QtxInfoPanel::Container: public QWidget
{
public:
  Container( QWidget* = 0 );
  Container( const QString&, QWidget* = 0 );

  void addAction( QAction*, const int );
  void addLabel( const QString&, Qt::Alignment, const int );
  void addGroup( const QString&, const int );

  QWidget* find( const int ) const;

  void remove( const int );
  void clear();

  void put( QWidget* );

private:
  QMap<int, QWidget*> ids;
  QGroupBox* group;
};

QtxInfoPanel::Container::Container( QWidget* parent )
  : QWidget( parent ), group( 0 )
{
  QVBoxLayout* l = new QVBoxLayout( this );
  l->setContentsMargins( 0, 0, 0, 0 );
}

QtxInfoPanel::Container::Container( const QString& title, QWidget* parent )
  : Container( parent )
{
  QVBoxLayout* l = dynamic_cast<QVBoxLayout*>( layout() );
  group = new QGroupBox( title );
  group->setLayout( new QVBoxLayout() );
  l->addWidget( group );
}

void QtxInfoPanel::Container::put( QWidget* widget )
{
  QVBoxLayout* l = group ? dynamic_cast<QVBoxLayout*>( group->layout() ) : dynamic_cast<QVBoxLayout*>( layout() );
  l->addWidget( widget );
}

void QtxInfoPanel::Container::addLabel( const QString& text, Qt::Alignment alignment, const int id )
{
  QLabel* label = new QLabel( text );
  QFont f = label->font();
  f.setItalic( true );
  label->setFont( f );
  label->setAlignment( alignment );
  label->setWordWrap( true );
  put( label );
  ids[ id ] = label;
}

void QtxInfoPanel::Container::addAction( QAction* action, const int id )
{
  QToolButton* button = new QToolButton( this );
  button->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  button->setAutoRaise( true );
  button->setDefaultAction( action );
  put( button );
  ids[ id ] = button;
}

void QtxInfoPanel::Container::addGroup( const QString& text, const int id )
{
  Container* group = new Container( text, this );
  put( group );
  ids[ id ] = group;
}

QWidget* QtxInfoPanel::Container::find( const int id ) const
{
  if ( ids.contains( id ) )
    return ids[id];

  QMap<int, QWidget*>::ConstIterator it;
  QWidget* widget = 0;
  for( it = ids.begin(); it != ids.end() && !widget; ++it )
  {
    Container* group = dynamic_cast<Container*>( it.value() );
    if ( group )
      widget = group->find( id );
  }

  return widget;
}

void QtxInfoPanel::Container::remove( const int id )
{
  if ( ids.contains( id ) )
  {
    QVBoxLayout* l = group ? dynamic_cast<QVBoxLayout*>( group->layout() ) : dynamic_cast<QVBoxLayout*>( layout() );
    l->removeWidget( ids[id] );
    ids[id]->deleteLater();
    l->invalidate();
    ids.remove( id );
  }
}

void QtxInfoPanel::Container::clear()
{
  QVBoxLayout* l = group ? dynamic_cast<QVBoxLayout*>( group->layout() ) : dynamic_cast<QVBoxLayout*>( layout() );

  QList<QWidget*> widgets = ids.values();
  foreach( QWidget* widget, widgets )
  {
    l->removeWidget( widget );
    widget->deleteLater();
  }

  l->invalidate();
  ids.clear();
}


/*!
  \internal
  \class QtxInfoPanel::Title
  \brief Info panel's title widget
*/
class QtxInfoPanel::Title: public QLabel
{
public:
  Title( QWidget* parent = 0 );
};

QtxInfoPanel::Title::Title( QWidget* parent )
  : QLabel( parent )
{
  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  QString bg = palette().color( QPalette::Highlight ).name();
  QString fg = palette().color( QPalette::HighlightedText ).name();
  setStyleSheet( QString( "QLabel { background:%1; color:%2; }" ).arg( bg ).arg( fg ) );
  setTextFormat( Qt::PlainText );
  QFont f = font();
  f.setBold( true );
  setFont( f );
  setContentsMargins( 2, 5, 2, 5 );
}


/*!
  \class QtxInfoPanel
  \brief Info panel which allows presenting welcome, useful hints
  and other information dynamically, e.g. in the dock panel of main
  application's window.

  The *Info panel* normally has a title (aimed to shortly present the
  current application's context) and a set of buttons and text labels
  combined into the groups (which may be nested).

  Buttons normally represent some quick actions which are applicable in
  the current context. Text labels can be used to show additional information
  like hints, proposed actions, etc.

  To set the title to the panel, use method setTitle(). Text label can be
  added to the panel with addLabel() method, action (button) is added via
  addAction() method.

  By default, items are added to the top level, untitled group. Additionally,
  panel allows arranging items into groups; new group can be added with the
  addGroup() method.

  Each of addAction(), addLabel(), addGroup() methods return item's unique
  identifier. This identifier can be used, for instance, to enable/disable
  item with setEnabled() method, hide/show with setVisible() method, remove
  from panel with remove() method. The same action can be added to the panel
  several times, e.g. to the different groups - the corresponding buttons will
  have different unique ids.

  To remove all contents of panel, use clear() method.
*/

/*!
  \brief Create panel.
  \param parent Parent widget.
 */
QtxInfoPanel::QtxInfoPanel( QWidget* parent )
  : QWidget( parent )
{
  title = new Title( this );
  container = new Container( this );

  QVBoxLayout* layout = new QVBoxLayout( this );
  layout->setContentsMargins( 0, 0, 0, 0 );

  QWidget* wg = new QWidget();
  QVBoxLayout* wg_layout = new QVBoxLayout( wg );
  wg_layout->setContentsMargins( 0, 0, 0, 0 );
  wg_layout->addWidget( container );
  wg_layout->addStretch();
  
  QScrollArea* scroll = new QScrollArea();
  scroll->setWidgetResizable( true );
  scroll->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  scroll->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding );
  scroll->setSizeAdjustPolicy( QScrollArea::AdjustToContents );
  scroll->setFrameStyle( QScrollArea::NoFrame );
  scroll->setContentsMargins( 0, 0, 0, 0 );
  scroll->setWidget( wg );
  
  layout->addWidget( title );
  layout->addWidget( scroll );
  setTitle( "" );
}

/*!
  \brief Destructor.
*/
QtxInfoPanel::~QtxInfoPanel()
{
}

/*!
  \brief Add left-aligned text label to the given group.
  \param text Label's text.
  \param groupId Group's identifier. Value -1 (default) is used
  to add label to the top-level (untitled) group.
  \return Label's unique identifier.
*/
int QtxInfoPanel::addLabel( const QString& text, const int groupId )
{
  return addLabel( text, Qt::AlignLeft, groupId );
}

/*!
  \brief Add text label to the given group.
  \param text Label's text.
  \param alignment Label's alignment.
  \param groupId Group's identifier. Value -1 (default) is used
  to add label to the top-level (untitled) group.
  \return Label's unique identifier.
*/
int QtxInfoPanel::addLabel( const QString& text, Qt::Alignment alignment, const int groupId )
{
  int id = 0;
  Container* c = dynamic_cast<Container*>( find( groupId ) );
  if ( c )
  {
    id = generateId();
    c->addLabel( text, alignment, id );
  }
  return id;
}

/*!
  \brief Add action button to the given group.
  \param action Action being added (note: parent is not changed).
  \param groupId Group's identifier. Value -1 (default) is used
  to add button to the top-level (untitled) group.
  \return Button's unique identifier.
*/
int QtxInfoPanel::addAction( QAction* action, const int groupId )
{
  int id = 0;
  Container* c = dynamic_cast<Container*>( find( groupId ) );
  if ( c )
  {
    id = generateId();
    c->addAction( action, id );
  }
  return id;
}

/*!
  \brief Add (sub-)group to the given group.
  \param text Group's title.
  \param groupId Parent group's identifier. Value -1 (default) is used
  to add (sub-)group to the top-level (untitled) group (i.e. panel itself).
  \return Group's unique identifier.
*/
int QtxInfoPanel::addGroup( const QString& text, const int groupId )
{
  int id = 0;
  Container* c = dynamic_cast<Container*>( find( groupId ) );
  if ( c )
  {
    id = generateId();
    c->addGroup( text, id );
  }
  return id;
}

/*!
  \brief Set panel's title.
  \param text %Title text (empty string removes title).
*/
void QtxInfoPanel::setTitle( const QString& text )
{
  title->setText( text );
  title->setVisible( !title->text().isEmpty() );
}

/*!
  \brief Remove given item from panel.
  \note If a group is removed, all its contents (recursively) is removed
  as well.
  \param id Item's (label's, button's, group's) identifier.
*/
void QtxInfoPanel::remove( const int id )
{
  QWidget* widget = find( id );
  if ( widget )
  {
    Container* group = dynamic_cast<Container*>( widget->parentWidget() );
    if ( !group )
      group = dynamic_cast<Container*>( widget->parentWidget()->parentWidget() );
    if ( group )
      group->remove( id );
  }
}

/*!
  \brief Clear contents of panel of group.
  \param groupId Group's identifier. Value -1 (default) is used
  to clear all contents of panel.
*/
void QtxInfoPanel::clear( const int groupId )
{
  Container* c = dynamic_cast<Container*>( find( groupId ) );
  if ( c )
    c->clear();
}

/*!
  \internal
  \brief Find widget that represents given item.
  \param id Item's (label's, button's, group's) identifier.
  \return Item's widget (0 if not found).
*/
QWidget* QtxInfoPanel::find( const int id ) const
{
  if ( id == -1 )
    return container;
  return container->find( id );
}

/*!
  \brief Change item's visibility.
  \param id Item's (label's, button's, group's) identifier.
  \param visible \c true to show item, \c false to hide it.
*/
void QtxInfoPanel::setVisible( const int id, bool visible )
{
  QWidget* widget = find( id );
  if ( widget )
    widget->setVisible( visible );
}

/*!
  \brief Enable / disable item.
  \param id Item's (label's, button's, group's) identifier.
  \param enabled \c true to enable item, \c false to disable it.
*/
void QtxInfoPanel::setEnabled( const int id, bool enabled )
{
  QWidget* widget = find( id );
  if ( widget )
    widget->setEnabled( enabled );
}

/*!
  \internal
  \brief Generate new unique identifier.
  \return Unique identifier.
*/
int QtxInfoPanel::generateId() const
{
  static int id = -100;
  return --id;
}
