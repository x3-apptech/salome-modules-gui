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

// File:      QtxPagePrefMgr.cxx
// Author:    Sergey TELKOV
//
#include "QtxPagePrefMgr.h"

#include "QtxGridBox.h"
#include "QtxFontEdit.h"
#include "QtxGroupBox.h"
#include "QtxComboBox.h"
#include "QtxIntSpinBox.h"
#include "QtxColorButton.h"
#include "QtxBiColorTool.h"
#include "QtxDoubleSpinBox.h"
#include "QtxShortcutEdit.h"
#include "QtxBackgroundTool.h"
#include "QtxResourceMgr.h"

#include <QEvent>
#include <QLayout>
#include <QToolBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QSplitter>
#include <QTabWidget>
#include <QListWidget>
#include <QApplication>
#include <QDateTimeEdit>
#include <QStackedWidget>
#include <QSlider>
#include <QScrollArea>

#include <stdio.h>

namespace
{
  bool toStringList(const QVariant& value, QStringList& result)
  {
    bool ok = false;
    if ( value.type() == QVariant::StringList )
    {
      result = value.toStringList();
      ok = true;
    }
    else if ( value.type() == QVariant::List )
    {
      QList<QVariant> valueList = value.toList();
      for ( QList<QVariant>::const_iterator it = valueList.begin(); it != valueList.end(); ++it )
      {
        if ( (*it).canConvert( QVariant::String ) )
          result.append( (*it).toString() );
      }
      ok = true;
    }
    return ok;
  }
}

/*!
  \class QtxPagePrefMgr
  \brief GUI implementation of the QtxPreferenceMgr class: preferences manager.
*/

/*!
  \brief Constructor.
  \param resMgr resource manager
  \param parent parent widget
*/
QtxPagePrefMgr::QtxPagePrefMgr( QtxResourceMgr* resMgr, QWidget* parent )
: QFrame( parent ),
  QtxPreferenceMgr( resMgr ),
  myInit( false )
{
  myBox = new QtxGridBox( 1, Qt::Horizontal, this, 0 );
  QVBoxLayout* base = new QVBoxLayout( this );
  base->setMargin( 5 );
  base->setSpacing( 0 );
  base->addWidget( myBox );
}

/*!
  \brief Destructor
*/
QtxPagePrefMgr::~QtxPagePrefMgr()
{
}

/*!
  \brief Get recommended size for the widget.
  \return recommended widget size
*/
QSize QtxPagePrefMgr::sizeHint() const
{
  return QFrame::sizeHint();
}

/*!
  \brief Get recommended minimum size for the widget.
  \return recommended minimum widget size
*/
QSize QtxPagePrefMgr::minimumSizeHint() const
{
  return QFrame::minimumSizeHint();
}

/*!
  \brief Customize show/hide widget operation.
  \param on if \c true the widget is being shown, otherswise
  it is being hidden
*/
void QtxPagePrefMgr::setVisible( bool on )
{
  if ( on )
    initialize();

  QApplication::instance()->processEvents();

  QFrame::setVisible( on );
}

/*!
  \brief Update widget contents.
*/
void QtxPagePrefMgr::updateContents()
{
  QtxPreferenceMgr::updateContents();

  QList<QtxPreferenceItem*> lst = childItems();
  for ( QList<QtxPreferenceItem*>::const_iterator it = lst.begin(); it != lst.end(); ++it )
  {
    QtxPagePrefItem* item = dynamic_cast<QtxPagePrefItem*>( *it );
    if ( item && item->widget() && item->widget()->parent() != myBox )
      item->widget()->setParent( myBox );
  }

  setWindowIcon( icon() );
}

/*!
  \brief Callback function which is called when the child
  preference item is added.
  \param item child item being added
  \sa itemRemoved(), itemChanged()
*/
void QtxPagePrefMgr::itemAdded( QtxPreferenceItem* /*item*/ )
{
  triggerUpdate();
}

/*!
  \brief Callback function which is called when the child
  preference item is removed.
  \param item child item being removed
  \sa itemAdded(), itemChanged()
*/
void QtxPagePrefMgr::itemRemoved( QtxPreferenceItem* /*item*/ )
{
  triggerUpdate();
}

/*!
  \brief Callback function which is called when the child
  preference item is modified.
  \param item child item being modified
  \sa itemAdded(), itemRemoved()
*/
void QtxPagePrefMgr::itemChanged( QtxPreferenceItem* /*item*/ )
{
  triggerUpdate();
}

/*!
  \brief Get preference item option value.
  \param name option name
  \return property value or null QVariant if option is not set
  \sa setOptionValue()
*/
QVariant QtxPagePrefMgr::optionValue( const QString& name ) const
{
  if ( name == "orientation" )
    return myBox->orientation() == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal;
  else
    return QtxPreferenceMgr::optionValue( name );
}

/*!
  \brief Set preference item option value.
  \param name option name
  \param val new property value
  \sa optionValue()
*/
void QtxPagePrefMgr::setOptionValue( const QString& name, const QVariant& val )
{
  if ( name == "orientation" )
  {
    if ( val.canConvert( QVariant::Int ) )
      myBox->setOrientation( val.toInt() == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal );
  }
  else
    QtxPreferenceMgr::setOptionValue( name, val );
}

/*!
  \brief Perform internal initialization.
*/
void QtxPagePrefMgr::initialize() const
{
  //  if ( myInit )
  //    return;

  QtxPagePrefMgr* that = (QtxPagePrefMgr*)this;
  that->initialize( that );

  //  that->myInit = true;
}

void QtxPagePrefMgr::initialize( QtxPreferenceItem* item )
{
  if ( !item )
    return;

  QList<QtxPreferenceItem*> lst = item->childItems( false );
  for ( QList<QtxPreferenceItem*>::iterator it = lst.begin(); it != lst.end(); ++it )
    initialize( *it );

  updateContents();
}

/*!
  \class QtxPagePrefItem
  \brief Base class for implementation of all the widget-based
  preference items.
*/

class QtxPagePrefItem::Listener : public QObject
{
public:
  Listener( QtxPagePrefItem* );
  virtual ~Listener();

  virtual bool eventFilter( QObject*, QEvent* );

private:
  QtxPagePrefItem* myItem;
};

QtxPagePrefItem::Listener::Listener( QtxPagePrefItem* item )
: QObject( 0 ),
  myItem( item )
{
}

QtxPagePrefItem::Listener::~Listener()
{
}

bool QtxPagePrefItem::Listener::eventFilter( QObject* o, QEvent* e )
{
  if ( !myItem || myItem->widget() != o )
    return false;

  if ( e->type() == QEvent::Show || e->type() == QEvent::ShowToParent )
    myItem->widgetShown();
  if ( e->type() == QEvent::Hide || e->type() == QEvent::HideToParent )
    myItem->widgetHided();

  return false;
}

/*!
  \brief Constructor.
  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefItem::QtxPagePrefItem( const QString& title, QtxPreferenceItem* parent,
                                  const QString& sect, const QString& param )
: QtxPreferenceItem( title, sect, param, parent ),
  myWidget( 0 ),
  myListener( 0 )
{
}

/*!
  \brief Destructor.
*/
QtxPagePrefItem::~QtxPagePrefItem()
{
  delete myWidget;
  delete myListener;
}

void QtxPagePrefItem::activate()
{
  QtxPreferenceItem::activate();

  if ( widget() )
    widget()->setFocus();
}

/*!
  \brief Get preference item editor widget.
  \return editor widget
  \sa setWidget()
*/
QWidget* QtxPagePrefItem::widget() const
{
  return myWidget;
}

/*!
  \brief Set preference item editor widget.
  \param wid editor widget
  \sa widget()
*/
void QtxPagePrefItem::setWidget( QWidget* wid )
{
  if ( myWidget && myListener )
    myWidget->removeEventFilter( myListener );

  myWidget = wid;

  if ( myWidget )
  {
    if ( !myListener )
      myListener = new Listener( this );
    myWidget->installEventFilter( myListener );
  }

  sendItemChanges();
}

/*!
  \brief Callback function which is called when the child
  preference item is added.
  \param item child item being added
  \sa itemRemoved(), itemChanged()
*/
void QtxPagePrefItem::itemAdded( QtxPreferenceItem* /*item*/ )
{
  contentChanged();
}

/*!
  \brief Callback function which is called when the child
  preference item is removed.
  \param item child item being removed
  \sa itemAdded(), itemChanged()
*/
void QtxPagePrefItem::itemRemoved( QtxPreferenceItem* /*item*/ )
{
  contentChanged();
}

/*!
  \brief Callback function which is called when the child
  preference item is modified.
  \param item child item being modified
  \sa itemAdded(), itemRemoved()
*/
void QtxPagePrefItem::itemChanged( QtxPreferenceItem* /*item*/ )
{
  contentChanged();
}

/*!
  \brief Store preference item to the resource manager.

  This method should be reimplemented in the subclasses.
  Base implementation does nothing.

  \sa retrieve()
*/
void QtxPagePrefItem::store()
{
}

/*!
  \brief Retrieve preference item from the resource manager.

  This method should be reimplemented in the subclasses.
  Base implementation does nothing.

  \sa store()
*/
void QtxPagePrefItem::retrieve()
{
}

/*!
  \brief Invoked when preference item widget is shown.
*/
void QtxPagePrefItem::widgetShown()
{
}

/*!
  \brief Invoked when preference item widget is hided.
*/
void QtxPagePrefItem::widgetHided()
{
}

void QtxPagePrefItem::ensureVisible( QtxPreferenceItem* i )
{
  QtxPreferenceItem::ensureVisible();

  QtxPagePrefItem* item = dynamic_cast<QtxPagePrefItem*>( i );
  if ( item && item->widget() )
    item->widget()->setVisible( true );
}

/*!
  \brief Find all child items of the QtxPagePrefItem type.
  \param list used to return list of child items
  \param rec if \c true, perform recursive search
*/
void QtxPagePrefItem::pageChildItems( QList<QtxPagePrefItem*>& list, const bool rec ) const
{
  QList<QtxPreferenceItem*> lst = childItems( rec );
  for ( QList<QtxPreferenceItem*>::const_iterator it = lst.begin(); it != lst.end(); ++it )
  {
    QtxPagePrefItem* item = dynamic_cast<QtxPagePrefItem*>( *it );
    if ( item )
      list.append( item );
  }
}

/*!
  \brief Called when contents is changed (item is added, removed or modified).

  Triggers the item update.
*/
void QtxPagePrefItem::contentChanged()
{
  triggerUpdate();
}

/*!
  \class QtxPageNamedPrefItem
  \brief Base class for implementation of the named preference items
  (items with text labels).
*/

/*!
  \brief Constructor.
  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPageNamedPrefItem::QtxPageNamedPrefItem( const QString& title, QtxPreferenceItem* parent,
                                            const QString& sect, const QString& param )
: QtxPagePrefItem( title, parent, sect, param ),
  myControl( 0 )
{
  QWidget* main = new QWidget();

  //  QtxPagePrefGroupItem* aGroup = 0;//dynamic_cast<QtxPagePrefGroupItem*>(parent);
  //  if ( !aGroup )
  //  {
    QHBoxLayout* base = new QHBoxLayout( main );
    base->setMargin( 0 );
    base->setSpacing( 5 );

    myLabel = new QLabel( title, main );
    base->addWidget( myLabel );
    //  }
    //  else
    //    myLabel = new QLabel( title, aGroup->gridBox() );

  setWidget( main );

  myLabel->setVisible( !title.isEmpty() );
}

/*!
  \brief Destructor.
*/
QtxPageNamedPrefItem::~QtxPageNamedPrefItem()
{
}

/*!
  \brief Set preference title.
  \param txt new preference title.
*/
void QtxPageNamedPrefItem::setTitle( const QString& txt )
{
  QtxPagePrefItem::setTitle( txt );

  label()->setText( title() );
  if ( !title().isEmpty() )
    label()->setVisible( true );
}

/*!
  \brief Get label widget corresponding to the preference item.
  \return label widget
*/
QLabel* QtxPageNamedPrefItem::label() const
{
  return myLabel;
}

/*!
  \brief Get control widget corresponding to the preference item.
  \return control widget
  \sa setControl()
*/
QWidget* QtxPageNamedPrefItem::control() const
{
  return myControl;
}

/*!
  \brief Set control widget corresponding to the preference item.
  \param wid control widget
  \sa control()
*/
void QtxPageNamedPrefItem::setControl( QWidget* wid )
{
  if ( myControl == wid )
    return;

  delete myControl;
  myControl = wid;

  if ( myControl )
  {
    //    QtxPagePrefGroupItem* aGroup = 0;//dynamic_cast<QtxPagePrefGroupItem*>(parentItem());
    //    if ( !aGroup )
    widget()->layout()->addWidget( myControl );
    widget()->setFocusProxy( myControl );
      //    else myControl->setParent( aGroup->gridBox() );
  }
}

void QtxPageNamedPrefItem::adjustLabels( QtxPagePrefItem* parent )
{
  if ( !parent )
    return;

  QList<QtxPreferenceItem*> childList = parent->childItems();

  QList<QtxPageNamedPrefItem*> namedItems;
  for ( QList<QtxPreferenceItem*>::iterator it = childList.begin(); it != childList.end(); ++it )
  {
    QtxPageNamedPrefItem* item = dynamic_cast<QtxPageNamedPrefItem*>( *it );
    if ( item )
      namedItems.append( item );
  }

  int sz = 0;
  for ( QList<QtxPageNamedPrefItem*>::iterator it1 = namedItems.begin(); it1 != namedItems.end(); ++it1 )
  {
    QtxPageNamedPrefItem* item = *it1;
    if ( item->label() )
      sz = qMax( sz, item->label()->sizeHint().width() );
  }

  for ( QList<QtxPageNamedPrefItem*>::iterator it2 = namedItems.begin(); it2 != namedItems.end(); ++it2 )
  {
    QtxPageNamedPrefItem* item = *it2;
    if ( item->label() )
      item->label()->setMinimumWidth( sz );
  }
}

/*!
  \class QtxPagePrefListItem
  \brief GUI implementation of the list container preference item.
*/

/*!
  \brief Constructor.
  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefListItem::QtxPagePrefListItem( const QString& title, QtxPreferenceItem* parent,
                                          const QString& sect, const QString& param )
: QtxPagePrefItem( title, parent, sect, param ),
  myFix( false )
{
  QSplitter* main = new QSplitter( Qt::Horizontal );
  main->setChildrenCollapsible( false );

  main->addWidget( myList = new QListWidget( main ) );
  main->addWidget( myStack = new QStackedWidget( main ) );

  myList->setSelectionMode( QListWidget::SingleSelection );

  myStack->addWidget( myInfLabel = new QLabel( myStack ) );
  myInfLabel->setAlignment( Qt::AlignCenter );

  connect( myList, SIGNAL( itemSelectionChanged() ), this, SLOT( onItemSelectionChanged() ) );

  setWidget( main );
}

/*!
  \brief Destructor.
*/
QtxPagePrefListItem::~QtxPagePrefListItem()
{
}

/*!
  \brief Get message text which is shown if the container is empty.
  \return message text
  \sa setEmptyInfo()
*/
QString QtxPagePrefListItem::emptyInfo() const
{
  return myInfText;
}

/*!
  \brief Set message text which is shown if the container is empty.
  \param new message text
  \sa emptyInfo()
*/
void QtxPagePrefListItem::setEmptyInfo( const QString& inf )
{
  if ( myInfText == inf )
    return;

  myInfText = inf;

  updateVisible();
}

/*!
  \brief Check if the preference item widget is of fixed size.
  \return \c true if the widget has the fixed size
  \sa setFixedSize()
*/
bool QtxPagePrefListItem::isFixedSize() const
{
  return myFix;
}

/*!
  \brief Set the preference item widget to be of fixed size.
  \param on if \c true, the widget will have the fixed size
  \sa isFixedSize()
*/
void QtxPagePrefListItem::setFixedSize( const bool on )
{
  if ( myFix == on )
    return;

  myFix = on;

  updateGeom();
}

/*!
  \brief Update widget contents.
*/
void QtxPagePrefListItem::updateContents()
{
  QtxPagePrefItem::updateContents();
  updateVisible();
}

/*!
  \brief Get preference item option value.
  \param name option name
  \return property value or null QVariant if option is not set
  \sa setOptionValue()
*/
QVariant QtxPagePrefListItem::optionValue( const QString& name ) const
{
  if ( name == "fixed_size" )
    return isFixedSize();
  else if ( name == "empty_info" || name == "info" )
    return emptyInfo();
  else
    return QtxPagePrefItem::optionValue( name );
}

/*!
  \brief Set preference item option value.
  \param name option name
  \param val new property value
  \sa optionValue()
*/
void QtxPagePrefListItem::setOptionValue( const QString& name, const QVariant& val )
{
  if ( name == "fixed_size" )
  {
    if ( val.canConvert( QVariant::Bool ) )
      setFixedSize( val.toBool() );
  }
  else if ( name == "empty_info" || name == "info" )
  {
    if ( val.canConvert( QVariant::String ) )
      setEmptyInfo( val.toString() );
  }
  else
    QtxPagePrefItem::setOptionValue( name, val );
}

void QtxPagePrefListItem::widgetShown()
{
  updateState();
}

void QtxPagePrefListItem::ensureVisible( QtxPreferenceItem* i )
{
  if ( !i )
    return;

  QtxPreferenceItem::ensureVisible( i );

  setSelected( i->id() );
  updateState();
}

/*!
  \brief Called when the selection in the list box is changed.
*/
void QtxPagePrefListItem::onItemSelectionChanged()
{
  updateState();
}

/*!
  \brief Update information label widget.
*/
void QtxPagePrefListItem::updateInfo()
{
  QString infoText;
  QtxPagePrefItem* item = selectedItem();
  if ( item )
  {
    infoText = emptyInfo();
    QRegExp rx( "%([%|N])" );

    int idx = 0;
    while ( ( idx = rx.indexIn( infoText ) ) != -1 )
    {
      if ( rx.cap() == QString( "%%" ) )
        infoText.replace( idx, rx.matchedLength(), "%" );
      else if ( rx.cap() == QString( "%N" ) )
        infoText.replace( idx, rx.matchedLength(), item->title() );
    }
  }
  myInfLabel->setText( infoText );
}

/*!
  \brief Update widget state.
*/
void QtxPagePrefListItem::updateState()
{
  QtxPagePrefItem* item = selectedItem();
  QWidget* wid = item && !item->isEmpty() ? item->widget() : myInfLabel;
  if ( wid )
    myStack->setCurrentWidget( wid );

  updateInfo();
}

/*!
  \brief Update visibile child widgets.
*/
void QtxPagePrefListItem::updateVisible()
{
  QList<QtxPagePrefItem*> items;
  pageChildItems( items );

  QMap<QWidget*, int> map;
  for ( int i = 0; i < (int)myStack->count(); i++ )
    map.insert( myStack->widget( i ), 0 );

  int selId = selected();
  myList->clear();
  for ( QList<QtxPagePrefItem*>::const_iterator it = items.begin(); it != items.end(); ++it )
  {
    if ( (*it)->isEmpty() && myInfText.isEmpty() )
      continue;

    myList->addItem( (*it)->title() );
    myList->item( myList->count() - 1 )->setIcon( (*it)->icon() );
    myList->item( myList->count() - 1 )->setData( Qt::UserRole, (*it)->id() );

    QWidget* wid = (*it)->widget();
    if ( !map.contains( wid ) )
      myStack->addWidget( wid );

    map.remove( wid );
  }

  map.remove( myInfLabel );

  for ( QMap<QWidget*, int>::const_iterator it = map.begin(); it != map.end(); ++it )
    myStack->removeWidget( it.key() );

  setSelected( selId );
  if ( selected() == -1 && myList->count() )
    setSelected( myList->item( 0 )->data( Qt::UserRole ).toInt() );

  //myList->setVisible( myList->count() > 1 );

  updateState();
  updateGeom();
}

/*!
  \brief Update widget geometry.
*/
void QtxPagePrefListItem::updateGeom()
{
  if ( myFix )
    myList->setFixedWidth( myList->minimumSizeHint().width() + 10 );
  else
  {
    myList->setMinimumWidth( 0 );
    myList->setMaximumWidth( 16777215 );

    QSplitter* s = ::qobject_cast<QSplitter*>( widget() );
    if ( s )
    {
      int w = myList->minimumSizeHint().width() + 30;
      QList<int> szList;
      szList.append( w );
      szList.append( s->width() - w );
      s->setSizes( szList );
    }
  }
}

/*!
  \brief Get identifier of the currently selected preference item.
  \return identifier of the currently selected item or -1 if no item is selected
  \sa setSelected()
*/
int QtxPagePrefListItem::selected() const
{
  QList<QListWidgetItem*> selList = myList->selectedItems();
  if ( selList.isEmpty() )
    return -1;

  QVariant v = selList.first()->data( Qt::UserRole );
  return v.canConvert( QVariant::Int ) ? v.toInt() : -1;
}

/*!
  \brief Get currently selected preference item.
  \return currently selected item or 0 if no item is selected
  \sa setSelected()
*/
QtxPagePrefItem* QtxPagePrefListItem::selectedItem() const
{
  int selId = selected();

  QList<QtxPagePrefItem*> items;
  pageChildItems( items );

  QtxPagePrefItem* item = 0;
  for ( QList<QtxPagePrefItem*>::const_iterator it = items.begin(); it != items.end() && !item; ++it )
  {
    if ( (*it)->id() == selId )
      item = *it;
  }
  return item;
}

/*!
  \brief Set currently selected preference item.
  \param id identifier of the preference item to make selected
*/
void QtxPagePrefListItem::setSelected( const int id )
{
  int idx = -1;
  for ( int i = 0; i < (int)myList->count() && idx < 0; i++ )
  {
    QVariant v = myList->item( i )->data( Qt::UserRole );
    if ( v.canConvert( QVariant::Int ) && v.toInt() == id )
      idx = i;
  }

  QItemSelection sel;
  QItemSelectionModel* selModel = myList->selectionModel();

  if ( idx >= 0 )
    sel.select( myList->model()->index( idx, 0 ), myList->model()->index( idx, 0 ) );

  selModel->select( sel, QItemSelectionModel::ClearAndSelect );
}

/*!
  \class QtxPagePrefToolBoxItem
  \brief GUI implementation of the tool box container preference item.
*/

/*!
  \brief Constructor.
  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefToolBoxItem::QtxPagePrefToolBoxItem( const QString& title, QtxPreferenceItem* parent,
                                                const QString& sect, const QString& param )
: QtxPagePrefItem( title, parent, sect, param )
{
  setWidget( myToolBox = new QToolBox( 0 ) );
}

/*!
  \brief Destructor.
*/
QtxPagePrefToolBoxItem::~QtxPagePrefToolBoxItem()
{
}

/*!
  \brief Update widget contents.
*/
void QtxPagePrefToolBoxItem::updateContents()
{
  QtxPagePrefItem::updateContents();
  updateToolBox();
}

/*!
  \brief Update tool box widget.
*/
void QtxPagePrefToolBoxItem::updateToolBox()
{
  QList<QtxPagePrefItem*> items;
  pageChildItems( items );

  QWidget* cur = myToolBox->currentWidget();

  int i = 0;
  QMap<QWidget*, int> map;
  for ( QList<QtxPagePrefItem*>::const_iterator it = items.begin(); it != items.end(); ++it )
  {
    QWidget* wid = (*it)->widget();
    if ( !wid )
      continue;

    if ( myToolBox->widget( i ) != wid )
    {
      if ( myToolBox->indexOf( wid ) != -1 )
        myToolBox->removeItem( myToolBox->indexOf( wid ) );

      myToolBox->insertItem( i, wid, (*it)->title() );
    }
    else
      myToolBox->setItemText( i, (*it)->title() );

    myToolBox->setItemIcon( i, (*it)->icon() );

    i++;
    map.insert( wid, 0 );
  }

  QList<QWidget*> del;
  for ( int idx = 0; idx < (int)myToolBox->count(); idx++ )
  {
    QWidget* w = myToolBox->widget( idx );
    if ( !map.contains( w ) )
      del.append( w );
  }

  for ( QList<QWidget*>::const_iterator itr = del.begin(); itr != del.end(); ++itr )
    myToolBox->removeItem( myToolBox->indexOf( *itr ) );

  if ( cur )
    myToolBox->setCurrentWidget( cur );
}

void QtxPagePrefToolBoxItem::ensureVisible( QtxPreferenceItem* i )
{
  if ( !i )
    return;

  QtxPreferenceItem::ensureVisible( i );

  QtxPagePrefItem* item = dynamic_cast<QtxPagePrefItem*>( i );
  if ( item && item->widget() )
    myToolBox->setCurrentWidget( item->widget() );
}

/*!
  \class QtxPagePrefTabsItem
  \brief GUI implementation of the tab widget container.
*/

/*!
  \brief Constructor.
  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefTabsItem::QtxPagePrefTabsItem( const QString& title, QtxPreferenceItem* parent,
                                          const QString& sect, const QString& param )
: QtxPagePrefItem( title, parent, sect, param )
{
  setWidget( myTabs = new QTabWidget( 0 ) );
}

/*!
  \brief Destructor.
*/
QtxPagePrefTabsItem::~QtxPagePrefTabsItem()
{
}

/*!
  \brief Update widget contents.
*/
void QtxPagePrefTabsItem::updateContents()
{
  QtxPagePrefItem::updateContents();
  updateTabs();
}

/*!
  \brief Get tabs position.
  \return current tabs position (QTabWidget::TabPosition)
  \sa setTabPosition()
*/
int QtxPagePrefTabsItem::tabPosition() const
{
  return myTabs->tabPosition();
}

/*!
  \brief Set tabs position.
  \param tp new tabs position (QTabWidget::TabPosition)
  \sa tabPosition()
*/
void QtxPagePrefTabsItem::setTabPosition( const int tp )
{
  myTabs->setTabPosition( (QTabWidget::TabPosition)tp );
}

/*!
  \brief Get tabs shape.
  \return current tabs shape (QTabWidget::TabShape)
  \sa setTabShape()
*/
int QtxPagePrefTabsItem::tabShape() const
{
  return myTabs->tabShape();
}

/*!
  \brief Set tabs shape.
  \param ts new tabs shape (QTabWidget::TabShape)
  \sa tabShape()
*/
void QtxPagePrefTabsItem::setTabShape( const int ts )
{
  myTabs->setTabShape( (QTabWidget::TabShape)ts );
}

/*!
  \brief Get tabs icon size.
  \return current tabs icon size
  \sa setTabIconSize()
*/
QSize QtxPagePrefTabsItem::tabIconSize() const
{
  return myTabs->iconSize();
}

/*!
  \brief Set tabs icon size.
  \param sz new tabs icon size
  \sa tabIconSize()
*/
void QtxPagePrefTabsItem::setTabIconSize( const QSize& sz )
{
  myTabs->setIconSize( sz );
}

/*!
  \brief Get preference item option value.
  \param name option name
  \return property value or null QVariant if option is not set
  \sa setOptionValue()
*/
QVariant QtxPagePrefTabsItem::optionValue( const QString& name ) const
{
  if ( name == "position" )
    return tabPosition();
  else if ( name == "shape" )
    return tabShape();
  else if ( name == "icon_size" )
    return tabIconSize();
  else
    return QtxPagePrefItem::optionValue( name );
}

/*!
  \brief Set preference item option value.
  \param name option name
  \param val new property value
  \sa optionValue()
*/
void QtxPagePrefTabsItem::setOptionValue( const QString& name, const QVariant& val )
{
  if ( name == "position" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setTabPosition( val.toInt() );
  }
  else if ( name == "shape" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setTabShape( val.toInt() );
  }
  else if ( name == "icon_size" )
  {
    if ( val.canConvert( QVariant::Size ) )
      setTabIconSize( val.toSize() );
  }
  else
    QtxPagePrefItem::setOptionValue( name, val );
}

void QtxPagePrefTabsItem::ensureVisible( QtxPreferenceItem* i )
{
  if ( !i )
    return;

  QtxPreferenceItem::ensureVisible( i );

  QtxPagePrefItem* item = dynamic_cast<QtxPagePrefItem*>( i );
  if ( item && item->widget() )
    myTabs->setCurrentWidget( item->widget() );
}

/*!
  \brief Update tabs.
*/
void QtxPagePrefTabsItem::updateTabs()
{
  QList<QtxPagePrefItem*> items;
  pageChildItems( items );

  QWidget* cur = myTabs->currentWidget();

  int i = 0;
  QMap<QWidget*, int> map;
  for ( QList<QtxPagePrefItem*>::const_iterator it = items.begin(); it != items.end(); ++it )
  {
    QWidget* wid = (*it)->widget();
    if ( !wid )
      continue;

    if ( myTabs->widget( i ) != wid )
    {
      if ( myTabs->indexOf( wid ) != -1 )
        myTabs->removeTab( myTabs->indexOf( wid ) );

      myTabs->insertTab( i, wid, (*it)->title() );
    }
    else
      myTabs->setTabText( i, (*it)->title() );

    myTabs->setTabIcon( i, (*it)->icon() );

    i++;
    map.insert( wid, 0 );
  }

  QList<QWidget*> del;
  for ( int idx = 0; idx < (int)myTabs->count(); idx++ )
  {
    QWidget* w = myTabs->widget( idx );
    if ( !map.contains( w ) )
      del.append( w );
  }

  for ( QList<QWidget*>::const_iterator itr = del.begin(); itr != del.end(); ++itr )
    myTabs->removeTab( myTabs->indexOf( *itr ) );

  if ( cur )
    myTabs->setCurrentWidget( cur );
}

/*!
  \class QtxPagePrefFrameItem
  \brief GUI implementation of the frame widget container.
*/

/*!
  \brief Constructor.
  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefFrameItem::QtxPagePrefFrameItem( const QString& title, QtxPreferenceItem* parent,
                                            const QString& sect, const QString& param, const bool scrollable )
: QtxPagePrefItem( title, parent, sect, param )
{
  QWidget* main = new QWidget();
  QVBoxLayout* base = new QVBoxLayout( main );
  base->setMargin( 0 );
  base->setSpacing( 0 );

  base->addWidget( myBox = new QtxGridBox( 1, Qt::Horizontal, main, 5, 5 ) );
  base->addStretch();

  if ( scrollable ) {
    QScrollArea* scroll = new QScrollArea();
    scroll->setWidget( main );
    scroll->setWidgetResizable( true );
    base->layout()->setSizeConstraint(QLayout::SetMinAndMaxSize);
    main = scroll;
  }

  setWidget( main );
}

/*!
  \brief Destructor.
*/
QtxPagePrefFrameItem::~QtxPagePrefFrameItem()
{
}

/*!
  \brief Update widget contents.
*/
void QtxPagePrefFrameItem::updateContents()
{
  QtxPagePrefItem::updateContents();

  updateFrame();

  QtxPageNamedPrefItem::adjustLabels( this );
}

/*!
  \brief Get frame margin.
  \return current frame margin
  \sa setMargin()
*/
int QtxPagePrefFrameItem::margin() const
{
  return myBox->insideMargin();
}

/*!
  \brief Get frame margin.
  \param m new frame margin
  \sa margin()
*/
void QtxPagePrefFrameItem::setMargin( const int m )
{
  myBox->setInsideMargin( m );
}

/*!
  \brief Get frame spacing.
  \return current frame spacing
  \sa setSpacing()
*/
int QtxPagePrefFrameItem::spacing() const
{
  return myBox->insideSpacing();
}

/*!
  \brief Set frame spacing.
  \param s new frame spacing
  \sa spacing()
*/
void QtxPagePrefFrameItem::setSpacing( const int s )
{
  myBox->setInsideSpacing( s );
}

/*!
  \brief Get number of frame columns.
  \return current columns number
  \sa setColumns()
*/
int QtxPagePrefFrameItem::columns() const
{
  return myBox->columns();
}

/*!
  \brief Set number of frame columns.
  \param c new columns number
  \sa columns()
*/
void QtxPagePrefFrameItem::setColumns( const int c )
{
  myBox->setColumns( c );
}

/*!
  \brief Get frame box orientation.
  \return current frame orientation
  \sa setOrientation()
*/
Qt::Orientation QtxPagePrefFrameItem::orientation() const
{
  return myBox->orientation();
}

/*!
  \brief Set frame box orientation.
  \param o new frame orientation
  \sa orientation()
*/
void QtxPagePrefFrameItem::setOrientation( const Qt::Orientation o )
{
  myBox->setOrientation( o );
}

/*!
  \brief Check if the frame widget stretching is enabled.
  \return \c true if the widget is stretchable
  \sa setStretch()
*/
bool QtxPagePrefFrameItem::stretch() const
{
  QSpacerItem* s = 0;
  QLayout* l = widget() ? widget()->layout() : 0;
  for ( int i = 0; l && i < l->count() && !s; i++ )
    s = l->itemAt( i )->spacerItem();

  return s ? (bool)( s->expandingDirections() & Qt::Vertical ) : false;
}

/*!
  \brief Enable/disable frame widget stretching.
  \param on new stretchable state
  \sa stretch()
*/
void QtxPagePrefFrameItem::setStretch( const bool on )
{
  QSpacerItem* s = 0;
  QWidget* w = widget();
  if ( qobject_cast<QScrollArea*>( w ) )
    w = qobject_cast<QScrollArea*>( w )->widget();
  QLayout* l = w ? w->layout() : 0;
  for ( int i = 0; l && i < l->count() && !s; i++ )
    s = l->itemAt( i )->spacerItem();

  if ( s )
    s->changeSize( 0, 0, QSizePolicy::Minimum, on ? QSizePolicy::Expanding : QSizePolicy::Minimum );
}

/*!
  \brief Get preference item option value.
  \param name option name
  \return property value or null QVariant if option is not set
  \sa setOptionValue()
*/
QVariant QtxPagePrefFrameItem::optionValue( const QString& name ) const
{
  if ( name == "margin" )
    return margin();
  else if ( name == "spacing" )
    return spacing();
  else if ( name == "columns" )
    return columns();
  else if ( name == "orientation" )
    return orientation();
  else if ( name == "stretch" )
    return stretch();
  else
    return QtxPagePrefItem::optionValue( name );
}

/*!
  \brief Set preference item option value.
  \param name option name
  \param val new property value
  \sa optionValue()
*/
void QtxPagePrefFrameItem::setOptionValue( const QString& name, const QVariant& val )
{
  if ( name == "margin" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setMargin( val.toInt() );
  }
  else if ( name == "spacing" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setSpacing( val.toInt() );
  }
  else if ( name == "columns" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setColumns( val.toInt() );
  }
  else if ( name == "orientation" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setOrientation( (Qt::Orientation)val.toInt() );
  }
  else if ( name == "stretch" )
  {
    if ( val.canConvert( QVariant::Bool ) )
      setStretch( val.toBool() );
  }
  else
    QtxPagePrefItem::setOptionValue( name, val );
}

void QtxPagePrefFrameItem::widgetShown()
{
  QtxPagePrefItem::widgetShown();

  QtxPageNamedPrefItem::adjustLabels( this );
}

/*!
  \brief Update frame widget.
*/
void QtxPagePrefFrameItem::updateFrame()
{
  QList<QtxPagePrefItem*> items;
  pageChildItems( items );

  for ( QList<QtxPagePrefItem*>::const_iterator it = items.begin(); it != items.end(); ++it )
  {
    QWidget* wid = (*it)->widget();
    if ( !wid )
      continue;

    if ( wid->parent() != myBox )
      wid->setParent( myBox );
  }
}

/*!
  \class QtxPagePrefGroupItem
  \brief GUI implementation of the group widget container.
*/

/*!
  \brief Constructor.
  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefGroupItem::QtxPagePrefGroupItem( const QString& title, QtxPreferenceItem* parent,
                                            const QString& sect, const QString& param )
: QtxPagePrefItem( title, parent, sect, param )
{
  myGroup = new QtxGroupBox( title, 0 );
  myBox = new QtxGridBox( 1, Qt::Horizontal, myGroup, 5, 5 );
  myGroup->setWidget( myBox );

  setWidget( myGroup );

  updateState();
}

/*!
  \brief Constructor.
  \param cols columns number
  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefGroupItem::QtxPagePrefGroupItem( const int cols, const QString& title, QtxPreferenceItem* parent,
                                            const QString& sect, const QString& param )
: QtxPagePrefItem( title, parent, sect, param )
{
  myGroup = new QtxGroupBox( title, 0 );
  myBox = new QtxGridBox( cols, Qt::Horizontal, myGroup, 5, 5 );
  myGroup->setWidget( myBox );

  setWidget( myGroup );

  updateState();
}

/*!
  \brief Destructor.
*/
QtxPagePrefGroupItem::~QtxPagePrefGroupItem()
{
}

/*!
  \brief Assign resource file settings to the preference item.
  \param sect resource file section name
  \param param resource file parameter name
  \sa resource()
*/
void QtxPagePrefGroupItem::setResource( const QString& sect, const QString& param )
{
  QtxPagePrefItem::setResource( sect, param );
  updateState();
}

/*!
  \brief Update widget contents.
*/
void QtxPagePrefGroupItem::updateContents()
{
  QtxPagePrefItem::updateContents();

  myGroup->setTitle( title() );

  updateState();
  updateGroup();

  QtxPageNamedPrefItem::adjustLabels( this );
}

/*!
  \brief Get group box margin.
  \return current group box margin
  \sa setMargin()
*/
int QtxPagePrefGroupItem::margin() const
{
  return myBox->insideMargin();
}

/*!
  \brief Get group box margin.
  \param m new group box margin
  \sa margin()
*/
void QtxPagePrefGroupItem::setMargin( const int m )
{
  myBox->setInsideMargin( m );
}

/*!
  \brief Get group box spacing.
  \return current group box spacing
  \sa setSpacing()
*/
int QtxPagePrefGroupItem::spacing() const
{
  return myBox->insideSpacing();
}

/*!
  \brief Set group box spacing.
  \param s new group box spacing
  \sa spacing()
*/
void QtxPagePrefGroupItem::setSpacing( const int s )
{
  myBox->setInsideSpacing( s );
}

/*!
  \brief Get number of group box columns.
  \return current columns number
  \sa setColumns()
*/
int QtxPagePrefGroupItem::columns() const
{
  return myBox->columns();
}

/*!
  \brief Set number of group box columns.
  \param c new columns number
  \sa columns()
*/
void QtxPagePrefGroupItem::setColumns( const int c )
{
  myBox->setColumns( c );
}

/*!
  \brief Get group box orientation.
  \return current group box orientation
  \sa setOrientation()
*/
Qt::Orientation QtxPagePrefGroupItem::orientation() const
{
  return myBox->orientation();
}

/*!
  \brief Set group box orientation.
  \param o new group box orientation
  \sa orientation()
*/
void QtxPagePrefGroupItem::setOrientation( const Qt::Orientation o )
{
  myBox->setOrientation( o );
}

/*!
  \brief Get 'flat' flag of the group box widget.
  \return \c true if the group box is flat
*/
bool QtxPagePrefGroupItem::isFlat() const
{
  return myGroup->isFlat();
}

/*!
  \brief Get 'flat' flag of the group box widget.
  \param on if \c true the group box will be made flat
*/
void QtxPagePrefGroupItem::setFlat( const bool on )
{
  myGroup->setFlat( on );
}

/*!
  \brief Store preference item to the resource manager.
  \sa retrieve()
*/
void QtxPagePrefGroupItem::store()
{
  if ( myGroup->isCheckable() )
    setBoolean( myGroup->isChecked() );
}

/*!
  \brief Return widget contained grid layout of this group.
*/
QtxGridBox* QtxPagePrefGroupItem::gridBox() const
{
  return myBox;
}

/*!
  \brief Retrieve preference item from the resource manager.
  \sa store()
*/
void QtxPagePrefGroupItem::retrieve()
{
  if ( myGroup->isCheckable() )
    myGroup->setChecked( getBoolean() );
}

/*!
  \brief Get preference item option value.
  \param name option name
  \return property value or null QVariant if option is not set
  \sa setOptionValue()
*/
QVariant QtxPagePrefGroupItem::optionValue( const QString& name ) const
{
  if ( name == "margin" )
    return margin();
  else if ( name == "spacing" )
    return spacing();
  else if ( name == "columns" )
    return columns();
  else if ( name == "orientation" )
    return orientation();
  else if ( name == "flat" )
    return isFlat();
  else
    return QtxPagePrefItem::optionValue( name );
}

/*!
  \brief Set preference item option value.
  \param name option name
  \param val new property value
  \sa optionValue()
*/
void QtxPagePrefGroupItem::setOptionValue( const QString& name, const QVariant& val )
{
  if ( name == "margin" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setMargin( val.toInt() );
  }
  else if ( name == "spacing" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setSpacing( val.toInt() );
  }
  else if ( name == "columns" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setColumns( val.toInt() );
  }
  else if ( name == "orientation" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setOrientation( (Qt::Orientation)val.toInt() );
  }
  else if ( name == "flat" )
  {
    if ( val.canConvert( QVariant::Bool ) )
      setFlat( val.toBool() );
  }
  else
    QtxPagePrefItem::setOptionValue( name, val );
}

void QtxPagePrefGroupItem::widgetShown()
{
  QtxPagePrefItem::widgetShown();

  QtxPageNamedPrefItem::adjustLabels( this );
}

/*!
  \brief Update widget state.
*/
void QtxPagePrefGroupItem::updateState()
{
  QString section, param;
  resource( section, param );
  myGroup->setCheckable( !title().isEmpty() && !section.isEmpty() && !param.isEmpty() );
}

/*!
  \brief Update group box widget.
*/
void QtxPagePrefGroupItem::updateGroup()
{
  QList<QtxPagePrefItem*> items;
  pageChildItems( items );

  for ( QList<QtxPagePrefItem*>::const_iterator it = items.begin(); it != items.end(); ++it )
  {
    QWidget* wid = (*it)->widget();
    if ( !wid )
      continue;

    if ( wid->parent() != myBox )
      wid->setParent( myBox );
  }
}

/*!
  \class QtxPagePrefLabelItem
  \brief Label item which can be used in the preferences editor dialog box.
*/

/*!
  \brief Constructor.

  Creates label item with specified title.

  \param text label text
  \param parent parent preference item
*/
QtxPagePrefLabelItem::QtxPagePrefLabelItem( const QString& text, QtxPreferenceItem* parent )
: QtxPagePrefItem( text, parent )
{
  setWidget( myLabel = new QLabel( text ) );
}

QtxPagePrefLabelItem::QtxPagePrefLabelItem( Qt::Alignment align, const QString& text, QtxPreferenceItem* parent )
: QtxPagePrefItem( text, parent )
{
  setWidget( myLabel = new QLabel( text ) );
  myLabel->setAlignment( align );
}

QtxPagePrefLabelItem::~QtxPagePrefLabelItem()
{
}

void QtxPagePrefLabelItem::setTitle( const QString& text )
{
  QtxPagePrefItem::setTitle( text );

  if ( myLabel )
    myLabel->setText( text );
}

Qt::Alignment QtxPagePrefLabelItem::alignment() const
{
  return myLabel->alignment();
}

void QtxPagePrefLabelItem::setAlignment( Qt::Alignment align )
{
  myLabel->setAlignment( align );
}

QVariant QtxPagePrefLabelItem::optionValue( const QString& name ) const
{
  QVariant val;
  if ( name == "alignment" )
    val = (int)alignment();
  return val;
}

void QtxPagePrefLabelItem::setOptionValue( const QString& name, const QVariant& val )
{
  if ( name == "alignment" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setAlignment( (Qt::Alignment)val.toInt() );
  }
}

/*!
  \class QtxPagePrefSpaceItem
  \brief Simple spacer item which can be used in the preferences
  editor dialog box.
*/

/*!
  \brief Constructor.

  Creates spacer item with zero width and height and expanding
  on both directions (by height and width).

  \param parent parent preference item
*/
QtxPagePrefSpaceItem::QtxPagePrefSpaceItem( QtxPreferenceItem* parent )
: QtxPagePrefItem( QString(), parent )
{
  initialize( 0, 0, 1, 1 );
}

/*!
  \brief Constructor.

  Creates spacer item with zero width and height and expanding
  according to the specified orientation.

  \param o spacer orientation
  \param parent parent preference item
*/
QtxPagePrefSpaceItem::QtxPagePrefSpaceItem( Qt::Orientation o, QtxPreferenceItem* parent )
: QtxPagePrefItem( QString(), parent )
{
  if ( o == Qt::Horizontal )
    initialize( 0, 0, 1, 0 );
  else
    initialize( 0, 0, 0, 1 );
}

/*!
  \brief Constructor.

  Creates spacer item with specified width and height. The spacing
  item is expanding horizontally if \a w <= 0 and vertically
  if \a h <= 0.

  \param w spacer width
  \param h spacer height
  \param parent parent preference item
*/
QtxPagePrefSpaceItem::QtxPagePrefSpaceItem( const int w, const int h, QtxPreferenceItem* parent )
: QtxPagePrefItem( QString(), parent )
{
  initialize( w, h, w > 0 ? 0 : 1, h > 0 ? 0 : 1 );
}

/*!
  \brief Destructor.
*/
QtxPagePrefSpaceItem::~QtxPagePrefSpaceItem()
{
}

/*!
  \brief Get spacer item size for the specified direction.
  \param o direction
  \return size for the specified direction
  \sa setSize()
*/
int QtxPagePrefSpaceItem::size( Qt::Orientation o ) const
{
  return o == Qt::Horizontal ? widget()->minimumWidth() : widget()->minimumHeight();
}

/*!
  \brief Set spacer item size for the specified direction.
  \param o direction
  \param sz new size for the specified direction
  \sa size()
*/
void QtxPagePrefSpaceItem::setSize( Qt::Orientation o, const int sz )
{
  if ( o == Qt::Horizontal )
    widget()->setMinimumWidth( sz );
  else
    widget()->setMinimumHeight( sz );
}

/*!
  \brief Get spacer item stretch factor for the specified direction.
  \param o direction
  \return stretch factor for the specified direction
  \sa setStretch()
*/
int QtxPagePrefSpaceItem::stretch( Qt::Orientation o ) const
{
  QSizePolicy sp = widget()->sizePolicy();
  return o == Qt::Horizontal ? sp.horizontalStretch() : sp.verticalStretch();
}

/*!
  \brief Set spacer item stretch factor for the specified direction.
  \param o direction
  \param sf new stretch factor for the specified direction
  \sa stretch()
*/
void QtxPagePrefSpaceItem::setStretch( Qt::Orientation o, const int sf )
{
  QSizePolicy sp = widget()->sizePolicy();
  if ( o == Qt::Horizontal )
  {
    sp.setHorizontalStretch( sf );
    sp.setHorizontalPolicy( sf > 0 ? QSizePolicy::Expanding : QSizePolicy::Fixed );
  }
  else
  {
    sp.setVerticalStretch( sf );
    sp.setVerticalPolicy( sf > 0 ? QSizePolicy::Expanding : QSizePolicy::Fixed );
  }

  widget()->setSizePolicy( sp );
}

/*!
  \brief Get preference item option value.
  \param name option name
  \return property value or null QVariant if option is not set
  \sa setOptionValue()
*/
QVariant QtxPagePrefSpaceItem::optionValue( const QString& name ) const
{
  if ( name == "horizontal_size" || name == "hsize" )
    return size( Qt::Horizontal );
  else if ( name == "vertical_size" || name == "vsize" )
    return size( Qt::Vertical );
  else if ( name == "horizontal_stretch" || name == "hstretch" )
    return stretch( Qt::Horizontal );
  else if ( name == "vertical_stretch" || name == "vstretch" )
    return stretch( Qt::Vertical );
  else
    return QtxPagePrefItem::optionValue( name );
}

/*!
  \brief Set preference item option value.
  \param name option name
  \param val new property value
  \sa optionValue()
*/
void QtxPagePrefSpaceItem::setOptionValue( const QString& name, const QVariant& val )
{
  if ( name == "horizontal_size" || name == "hsize" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setSize( Qt::Horizontal, val.toInt() );
  }
  else if ( name == "vertical_size" || name == "vsize" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setSize( Qt::Vertical, val.toInt() );
  }
  else if ( name == "horizontal_stretch" || name == "hstretch" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setStretch( Qt::Horizontal, val.toInt() );
  }
  else if ( name == "vertical_stretch" || name == "vstretch" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setStretch( Qt::Vertical, val.toInt() );
  }
  else
    QtxPagePrefItem::setOptionValue( name, val );
}

/*!
  \brief Perform internal initialization.
  \param w spacer item width
  \param h spacer item height
  \param ws spacer item horizontal stretch factor
  \param hs spacer item vertical stretch factor
*/
void QtxPagePrefSpaceItem::initialize( const int w, const int h, const int hs, const int vs )
{
  QSizePolicy sp;
  sp.setHorizontalPolicy( hs > 0 ? QSizePolicy::Expanding : QSizePolicy::Fixed );
  sp.setVerticalPolicy( vs > 0 ? QSizePolicy::Expanding : QSizePolicy::Fixed );

  sp.setHorizontalStretch( hs );
  sp.setVerticalStretch( vs );

  QWidget* wid = new QWidget();
  wid->setSizePolicy( sp );

  wid->setMinimumSize( w, h );

  setWidget( wid );
}

/*!
  \class  QtxPagePrefCheckItem
  \brief GUI implementation of the resources check box item (boolean).
*/

/*!
  \brief Constructor.
  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefCheckItem::QtxPagePrefCheckItem( const QString& title, QtxPreferenceItem* parent,
                                            const QString& sect, const QString& param )

: QtxPagePrefItem( title, parent, sect, param )
{
  myCheck = new QCheckBox( title );
  myCheck->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
  setWidget( myCheck );
}

/*!
  \brief Destructor.
*/
QtxPagePrefCheckItem::~QtxPagePrefCheckItem()
{
}

/*!
  \brief Set preference item title.
  \param txt new preference title.
*/
void QtxPagePrefCheckItem::setTitle( const QString& txt )
{
  QtxPagePrefItem::setTitle( txt );

  myCheck->setText( title() );
}

/*!
  \brief Store preference item to the resource manager.
  \sa retrieve()
*/
void QtxPagePrefCheckItem::store()
{
  setBoolean( myCheck->isChecked() );
}

/*!
  \brief Retrieve preference item from the resource manager.
  \sa store()
*/
void QtxPagePrefCheckItem::retrieve()
{
  myCheck->setChecked( getBoolean() );
}

/*!
  \class QtxPagePrefEditItem
  \brief GUI implementation of the resources line edit box item
  for string, integer and double values.
*/

static void fixupAndSet( QLineEdit* le, const QString& txt )
{
  if ( le ) {
    QString val = txt;
    if ( le->validator() ) {
      const QDoubleValidator* de = dynamic_cast<const QDoubleValidator*>( le->validator() );
      if ( de ) {
	int dec = de->decimals();
	int idx = val.lastIndexOf( QRegExp( QString("[.|%1]").arg( le->locale().decimalPoint () ) ) );
	if ( idx >= 0 ) {
	  QString tmp = val.mid( idx+1 );
	  QString exp;
	  val = val.left( idx+1 );
	  idx = tmp.indexOf( QRegExp( QString("[e|E]") ) );
	  if ( idx >= 0 ) {
	    exp = tmp.mid( idx );
	    tmp = tmp.left( idx );
	  }
	  tmp.truncate( dec );
	  val = val + tmp + exp;
	}
      }
      int pos = 0;
      if ( le->validator()->validate( val, pos ) == QValidator::Invalid )
	val.clear();
    }
    le->setText( val );
  }
}

/*!
  \brief Constructor.

  Creates preference item for string value editing.

  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefEditItem::QtxPagePrefEditItem( const QString& title, QtxPreferenceItem* parent,
                                          const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param ),
  myType( String ),
  myDecimals( 1000 ),
  myEchoMode( 0 )
{
  setControl( myEditor = new QLineEdit() );
  updateEditor();
}

/*!
  \brief Constructor.

  Creates preference item for editing of the value which type
  is specified by parameter \a type.

  \param type preference item input type (QtxPagePrefEditItem::InputType)
  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefEditItem::QtxPagePrefEditItem( const int type, const QString& title,
                                          QtxPreferenceItem* parent, const QString& sect,
                                          const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param ),
  myType( type ),
  myDecimals( 1000 ),
  myEchoMode( 0 )
{
  setControl( myEditor = new QLineEdit() );
  updateEditor();
}

/*!
  \brief Destructor.
*/
QtxPagePrefEditItem::~QtxPagePrefEditItem()
{
}

/*!
  \brief Get edit box preference item input type.
  \return preference item input type (QtxPagePrefEditItem::InputType)
  \sa setInputType()
*/
int QtxPagePrefEditItem::inputType() const
{
  return myType;
}

/*!
  \brief Set edit box preference item input type.
  \param type new preference item input type (QtxPagePrefEditItem::InputType)
  \sa inputType()
*/
void QtxPagePrefEditItem::setInputType( const int type )
{
  if ( myType == type )
    return;

  myType = type;
  updateEditor();
}

/*!
  \brief Get number of digits after decimal point (for Double input type)
  \return preference item decimals value
  \sa setDecimals()
*/
int QtxPagePrefEditItem::decimals() const
{
  return myDecimals;
}

/*!
  \brief Set number of digits after decimal point (for Double input type)
  \param dec new preference item decimals value
  \sa decimals()
*/
void QtxPagePrefEditItem::setDecimals( const int dec )
{
  if ( myDecimals == dec )
    return;
  
  myDecimals = dec;
  updateEditor();
}

/*!
  \brief Get the line edit's echo mode
  \return preference item echo mode value
  \sa setEchoMode()
*/
int QtxPagePrefEditItem::echoMode() const
{
  return myEchoMode;
}

/*!
  \brief Set the line edit's echo mode
  \param echo new preference item echo mode value
  \sa echoMode()
*/
void QtxPagePrefEditItem::setEchoMode( const int echo )
{   
  if ( myEchoMode == echo )
    return;
  
  myEchoMode = echo;
  updateEditor();
}

/*!
  \brief Store preference item to the resource manager.
  \sa retrieve()
*/
void QtxPagePrefEditItem::store()
{
  setString( myEditor->text() );
}

/*!
  \brief Retrieve preference item from the resource manager.
  \sa store()
*/
void QtxPagePrefEditItem::retrieve()
{
  QString txt = getString();
  fixupAndSet( myEditor, txt );
}

/*!
  \brief Get preference item option value.
  \param name option name
  \return property value or null QVariant if option is not set
  \sa setOptionValue()
*/
QVariant QtxPagePrefEditItem::optionValue( const QString& name ) const
{
  if ( name == "input_type" || name == "type" )
    return inputType();
  else if ( name == "precision" || name == "prec" || name == "decimals" )
    return decimals();
  else if ( name == "echo" || name == "echo_mode" || name == "echomode")
    return echoMode();
  else
    return QtxPageNamedPrefItem::optionValue( name );
}

/*!
  \brief Set preference item option value.
  \param name option name
  \param val new property value
  \sa optionValue()
*/
void QtxPagePrefEditItem::setOptionValue( const QString& name, const QVariant& val )
{
  if ( name == "input_type" || name == "type" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setInputType( val.toInt() );
  }
  else if ( name == "precision" || name == "prec" || name == "decimals" ) {
    if ( val.canConvert( QVariant::Int ) )
      setDecimals( val.toInt() );
  }
  else if ( name == "echo" || name == "echo_mode" || name == "echomode") {
    if ( val.canConvert( QVariant::Int ) )
      setEchoMode( val.toInt() );
  }
  else
    QtxPageNamedPrefItem::setOptionValue( name, val );
}

/*!
  \brief Update edit box widget.
*/
void QtxPagePrefEditItem::updateEditor()
{
  switch (myEchoMode)
  {
    case 0:
      myEditor->setEchoMode(QLineEdit::Normal);
      break;
    case 1:
      myEditor->setEchoMode(QLineEdit::NoEcho);
      break;
    case 2:
      myEditor->setEchoMode(QLineEdit::Password);
      break;
    case 3:
      myEditor->setEchoMode(QLineEdit::PasswordEchoOnEdit);
      break;
    default:
      myEditor->setEchoMode(QLineEdit::Normal);
  }
  
  QValidator* val = 0;
  switch ( inputType() )
  {
  case Integer:
    val = new QIntValidator( myEditor );
    break;
  case Double:
    val = new QDoubleValidator( myEditor );
    dynamic_cast<QDoubleValidator*>( val )->setDecimals( myDecimals < 0 ? 1000 : myDecimals );
    break;
  default:
    break;
  }

  QString txt = myEditor->text();
  delete myEditor->validator();
  myEditor->setValidator( val );
  fixupAndSet( myEditor, txt );
}

/*!
  \class QtxPagePrefSliderItem
*/

/*!
  \brief Constructor.

  Creates preference item with slider widget

  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefSliderItem::QtxPagePrefSliderItem( const QString& title, QtxPreferenceItem* parent,
                                              const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param )
{
  setControl( mySlider = new QSlider( Qt::Horizontal ) );
  widget()->layout()->addWidget( myLabel = new QLabel( ) );
    
  setMinimum( 0 );
  setMaximum( 0 );
  setSingleStep( 1 );
  setPageStep( 1 );
  
  mySlider->setTickPosition( QSlider::TicksBothSides );
  
  connect (mySlider, SIGNAL(valueChanged(int)), this, SLOT(setIcon(int)));
  updateSlider();
}

/*!
  \brief Destructor.
*/
QtxPagePrefSliderItem::~QtxPagePrefSliderItem()
{
}

/*!
  \brief Get slider preference item step value.
  \return slider single step value
  \sa setSingleStep()
*/
int QtxPagePrefSliderItem::singleStep() const
{
  return mySlider->singleStep();
}

/*!
  \brief Get slider preference item step value.
  \return slider page step value
  \sa setPageStep()
*/
int QtxPagePrefSliderItem::pageStep() const
{
  return mySlider->pageStep();
}

/*!
  \brief Get slider preference item minimum value.
  \return slider minimum value
  \sa setMinimum()
*/
int QtxPagePrefSliderItem::minimum() const
{
    return mySlider->minimum();
}

/*!
  \brief Get slider preference item maximum value.
  \return slider maximum value
  \sa setMaximum()
*/
int QtxPagePrefSliderItem::maximum() const
{
    return mySlider->maximum();
}

/*!
  \brief Get the list of the icons associated with the selection widget items
  \return list of icons
  \sa setIcons()
*/
QList<QIcon> QtxPagePrefSliderItem::icons() const
{
  return myIcons;
}

/*!
  \brief Set slider preference item step value.
  \param step new slider single step value
  \sa step()
*/
void QtxPagePrefSliderItem::setSingleStep( const int& step )
{
  mySlider->setSingleStep( step );
}

/*!
  \brief Set slider preference item step value.
  \param step new slider single step value
  \sa step()
*/
void QtxPagePrefSliderItem::setPageStep( const int& step )
{
  mySlider->setPageStep( step );
}

/*!
  \brief Set slider preference item minimum value.
  \param min new slider minimum value
  \sa minimum()
*/
void QtxPagePrefSliderItem::setMinimum( const int& min )
{
  mySlider->setMinimum( min );
  setIcon( mySlider->value() );
}

/*!
  \brief Set slider preference item maximum value.
  \param max new slider maximum value
  \sa maximum()
*/
void QtxPagePrefSliderItem::setMaximum( const int& max )
{
  mySlider->setMaximum( max );
  setIcon( mySlider->value() );
}

/*!
  \brief Set the list of the icons to the selection widget items
  \param icns new list of icons
  \sa icons()
*/
void QtxPagePrefSliderItem::setIcons( const QList<QIcon>& icns )
{
  if ( icns.isEmpty() ) 
    return;
  myIcons = icns;
  
  QSize maxsize(0, 0); 
  for ( QList<QIcon>::const_iterator it = myIcons.begin(); it != myIcons.end(); ++it ) {
    if ( (*it).isNull() ) continue;
    maxsize = maxsize.expandedTo( (*it).availableSizes().first() );
  }
  myLabel->setFixedSize( maxsize );
  
  updateSlider();
}

/*!
  \brief Store preference item to the resource manager.
  \sa retrieve()
*/
void QtxPagePrefSliderItem::store()
{
  setInteger( mySlider->value() );
}

/*!
  \brief Retrieve preference item from the resource manager.
  \sa store()
*/
void QtxPagePrefSliderItem::retrieve()
{
  mySlider->setValue( getInteger( mySlider->value() ) );
}

/*!
  \brief Get preference item option value.
  \param name option name
  \return property value or null integer if option is not set
  \sa setOptionValue()
*/
QVariant QtxPagePrefSliderItem::optionValue( const QString& name ) const
{
  if ( name == "minimum" || name == "min" )
    return minimum();
  else if ( name == "maximum" || name == "max" )
    return maximum();
  else if ( name == "single_step" )
    return singleStep();
  else if ( name == "page_step" )
    return pageStep();
  else if ( name == "icons" || name == "pixmaps" )
  {
    QList<QVariant> lst;
    QList<QIcon> ics = icons();
    for ( QList<QIcon>::const_iterator it = ics.begin(); it != ics.end(); ++it )
      lst.append( *it );
    return lst;
  }
  else
    return QtxPageNamedPrefItem::optionValue( name );
}

/*!
  \brief Set preference item option value.
  \param name option name
  \param val new property value
  \sa optionValue()
*/
void QtxPagePrefSliderItem::setOptionValue( const QString& name, const QVariant& val )
{
  if ( val.canConvert( QVariant::Int ) )
  {
    if ( name == "minimum" || name == "min" )
      setMinimum( val.toInt() );
    else if ( name == "maximum" || name == "max" )
      setMaximum( val.toInt() );
    else if ( name == "single_step" )
      setSingleStep( val.toInt() );
    else if ( name == "page_step" )
      setPageStep( val.toInt() );
    else
      QtxPageNamedPrefItem::setOptionValue( name, val );
  }
  else if ( name == "icons" || name == "pixmaps" )
    setIcons( val );
  else
    QtxPageNamedPrefItem::setOptionValue( name, val );
}

void QtxPagePrefSliderItem::setIcon( int pos )
{
  int index = pos - mySlider->minimum();
  if ( !myIcons.isEmpty() && index >= 0 && index < myIcons.size() && !myIcons[index].isNull() )
    myLabel->setPixmap( myIcons[index].pixmap( myIcons[index].availableSizes().first() ) );
  else
    myLabel->clear();
}

/*!
  \brief Update slider widget.
*/
void QtxPagePrefSliderItem::updateSlider()
{
  int val = mySlider->value();
  int stp = singleStep();
  int ptp = pageStep();
  int min = minimum();
  int max = maximum();

  control()->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  mySlider->setFocusPolicy(Qt::StrongFocus);
  
  mySlider->setValue( val );
  setSingleStep( stp );
  setPageStep( ptp );
  setMinimum( min );
  setMaximum( max );
  
  myLabel->setVisible( !myIcons.empty() );
  widget()->layout()->setSpacing( !myIcons.empty() ? 6 : 0 );
}

/*!
  \brief Set the list of the icons from the resource manager.
  \param var new icons list
  \internal
*/
void  QtxPagePrefSliderItem::setIcons( const QVariant& var )
{
  if ( var.type() != QVariant::List )
    return;

  QList<QIcon> lst;
  QList<QVariant> varList = var.toList();
  for ( QList<QVariant>::const_iterator it = varList.begin(); it != varList.end(); ++it )
  {
    if ( (*it).canConvert<QIcon>() )
      lst.append( (*it).value<QIcon>() );
    else if ( (*it).canConvert<QPixmap>() )
      lst.append( (*it).value<QPixmap>() );
    else
      lst.append( QIcon() );
  }
  setIcons( lst );
}

/*!
  \class QtxPagePrefSelectItem
  \brief GUI implementation of the resources selector item
  (string, integer or double values list).

  All items in the list (represented as combo box) should be specified
  by the unique identifier which is stored to the resource file instead
  of the value itself.
*/

/*!
  \brief Constructor.

  Creates preference item with combo box widget which is not editable
  (direct value entering is disabled).

  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefSelectItem::QtxPagePrefSelectItem( const QString& title, QtxPreferenceItem* parent,
                                              const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param ),
  myType( NoInput )
{
  setControl( mySelector = new QtxComboBox() );
  mySelector->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  mySelector->setDuplicatesEnabled( false );
  updateSelector();
}

/*!
  \brief Constructor.

  Creates preference item with combo box widget which is editable
  according to the specified input type (integer, double or string values).

  \param type input type (QtxPagePrefSelectItem::InputType)
  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefSelectItem::QtxPagePrefSelectItem( const int type, const QString& title, QtxPreferenceItem* parent,
                                              const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param ),
  myType( type )
{
  setControl( mySelector = new QtxComboBox() );
  mySelector->setDuplicatesEnabled( false );
  updateSelector();
}

/*!
  \brief Destructor.
*/
QtxPagePrefSelectItem::~QtxPagePrefSelectItem()
{
}

/*!
  \brief Get edit box preference item input type.
  \return preference item input type (QtxPagePrefSelectItem::InputType)
  \sa setInputType()
*/
int QtxPagePrefSelectItem::inputType() const
{
  return myType;
}

/*!
  \brief Set edit box preference item input type.
  \param type new preference item input type (QtxPagePrefSelectItem::InputType)
  \sa inputType()
*/
void QtxPagePrefSelectItem::setInputType( const int type )
{
  if ( myType == type )
    return;

  myType = type;
  updateSelector();
}

/*!
  \brief Get the list of the values from the selection widget.
  \return list of values
  \sa numbers(), icons(), setStrings()
*/
QStringList QtxPagePrefSelectItem::strings() const
{
  QStringList res;
  for ( int i = 0; i < mySelector->count(); i++ )
    res.append( mySelector->itemText( i ) );
  return res;
}

/*!
  \brief Get the list of the values identifiers from the selection widget.
  \return list of values IDs
  \sa strings(), icons(), setNumbers()
*/
QList<QVariant> QtxPagePrefSelectItem::numbers() const
{
  QList<QVariant> res;
  for ( int i = 0; i < mySelector->count(); i++ )
  {
    if ( mySelector->hasId( i ) )
      res.append( mySelector->id( i ) );
  }
  return res;
}

/*!
  \brief Get the list of the icons associated with the selection widget.items
  \return list of icons
  \sa strings(), numbers(), setIcons()
*/
QList<QIcon> QtxPagePrefSelectItem::icons() const
{
  QList<QIcon> res;
  for ( int i = 0; i < mySelector->count(); i++ )
    res.append( mySelector->itemIcon( i ) );
  return res;
}

/*!
  \brief Set the list of the values to the selection widget.
  \param lst new list of values
  \sa strings(), setNumbers(), setIcons()
*/
void QtxPagePrefSelectItem::setStrings( const QStringList& lst )
{
  mySelector->clear();
  mySelector->addItems( lst );
}

/*!
  \brief Set the list of the values identifiers to the selection widget
  \param ids new list of values IDs
  \sa numbers(), setStrings(), setIcons()
*/
void QtxPagePrefSelectItem::setNumbers( const QList<QVariant>& ids )
{
  int i = 0;
  for ( QList<QVariant>::const_iterator it = ids.begin(); it != ids.end(); ++it, i++ ) {
    if ( i >= mySelector->count() )
      mySelector->addItem(QString("") );
    
    mySelector->setId( i, *it );
  }
}

/*!
  \brief Set the list of the icons to the selection widget items

  Important: call this method after setStrings() or setNumbers()

  \param icns new list of icons
  \sa icons(), setStrings(), setNumbers()
*/
void QtxPagePrefSelectItem::setIcons( const QList<QIcon>& icns )
{
  int i = 0;
  for ( QList<QIcon>::const_iterator it = icns.begin(); it != icns.end() && i < mySelector->count(); ++it, i++ )
    mySelector->setItemIcon( i, *it );
}

/*!
  \brief Store preference item to the resource manager.
  \sa retrieve()
*/
void QtxPagePrefSelectItem::store()
{
  if ( mySelector->isCleared() )
    return;

  int idx = mySelector->currentIndex();

  if ( mySelector->hasId( idx ) ) {
    QVariant id = mySelector->id( idx );
    if ( id.type() == QVariant::Int )
      setInteger( id.toInt() );
    else if ( id.type() == QVariant::String )
      setString( id.toString() );
  }
  else if ( idx >= 0 ) {
    setString( mySelector->itemText( idx ) );
  }
}

/*!
  \brief Retrieve preference item from the resource manager.
  \sa store()
*/
void QtxPagePrefSelectItem::retrieve()
{
  QString txt = getString();
  
  // try to find via the id
  int idx = mySelector->index( txt );
  if ( idx < 0 )
  {
    for ( int i = 0; i < mySelector->count() && idx == -1; i++ )
    {
      if ( mySelector->itemText( i ) == txt )
        idx = i;
    }
  }

  if ( idx != -1 )
    mySelector->setCurrentIndex( idx );
  else if ( mySelector->isEditable() )
  {
    int pos = 0;
    if ( mySelector->validator() &&
         mySelector->validator()->validate( txt, pos ) == QValidator::Invalid )
      mySelector->setCleared( true );
    else
    {
      mySelector->setCleared( false );
      mySelector->addItem( txt );
      mySelector->setCurrentIndex( mySelector->count() - 1 );
    }
  }
}

/*!
  \brief Get preference item option value.
  \param name option name
  \return property value or null QVariant if option is not set
  \sa setOptionValue()
*/
QVariant QtxPagePrefSelectItem::optionValue( const QString& name ) const
{
  if ( name == "input_type" || name == "type" )
    return inputType();
  else if ( name == "strings" || name == "labels" )
    return strings();
  else if ( name == "numbers" || name == "ids" || name == "indexes" )
  {
    return numbers();
  }
  else if ( name == "icons" || name == "pixmaps" )
  {
    QList<QVariant> lst;
    QList<QIcon> ics = icons();
    for ( QList<QIcon>::const_iterator it = ics.begin(); it != ics.end(); ++it )
      lst.append( *it );
    return lst;
  }
  else
    return QtxPageNamedPrefItem::optionValue( name );
}

/*!
  \brief Set preference item option value.
  \param name option name
  \param val new property value
  \sa optionValue()
*/
void QtxPagePrefSelectItem::setOptionValue( const QString& name, const QVariant& val )
{
  if ( name == "input_type" || name == "type" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setInputType( val.toInt() );
  }
  else if ( name == "strings" || name == "labels" )
    setStrings( val );
  else if ( name == "numbers" || name == "ids" || name == "indexes" )
    setNumbers( val );
  else if ( name == "icons" || name == "pixmaps" )
    setIcons( val );
  else
    QtxPageNamedPrefItem::setOptionValue( name, val );
}

/*!
  \brief Set the list of the values from the resource manager.
  \param var new values list
  \internal
*/
void QtxPagePrefSelectItem::setStrings( const QVariant& var )
{
  QStringList values;
  if ( toStringList( var, values ) )
    setStrings( values );
}

/*!
  \brief Set the list of the values identifiers from the resource manager.
  \param var new values IDs list
  \internal
*/
void QtxPagePrefSelectItem::setNumbers( const QVariant& var )
{
  if ( var.type() != QVariant::List )
    return;

  setNumbers( var.toList() );
}

/*!
  \brief Set the list of the icons from the resource manager.
  \param var new icons list
  \internal
*/
void QtxPagePrefSelectItem::setIcons( const QVariant& var )
{
  if ( var.type() != QVariant::List )
    return;

  QList<QIcon> lst;
  QList<QVariant> varList = var.toList();
  for ( QList<QVariant>::const_iterator it = varList.begin(); it != varList.end(); ++it )
  {
    if ( (*it).canConvert<QIcon>() )
      lst.append( (*it).value<QIcon>() );
    else if ( (*it).canConvert<QPixmap>() )
      lst.append( (*it).value<QPixmap>() );
    else
      lst.append( QIcon() );
  }
  setIcons( lst );
}

/*!
  \brief Update selector widget.
*/
void QtxPagePrefSelectItem::updateSelector()
{
  QValidator* val = 0;
  switch ( inputType() )
  {
  case Integer:
    val = new QIntValidator( mySelector );
    break;
  case Double:
    val = new QDoubleValidator( mySelector );
    break;
  default:
    break;
  }

  mySelector->setEditable( inputType() != NoInput );

  if ( mySelector->isEditable() && !mySelector->currentText().isEmpty() && val )
  {
    int pos = 0;
    QString str = mySelector->currentText();
    if ( val->validate( str, pos ) == QValidator::Invalid )
      mySelector->clearEditText();
  }

  delete mySelector->validator();
  mySelector->setValidator( val );
}

/*!
  \class QtxPagePrefSpinItem
  \brief GUI implementation of the resources spin box item
  (for integer or double value).
*/

/*!
  \brief Constructor.

  Creates spin box preference item for the entering integer values.

  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefSpinItem::QtxPagePrefSpinItem( const QString& title, QtxPreferenceItem* parent,
                                          const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param ),
  myType( Integer )
{
  updateSpinBox();
}

/*!
  \brief Constructor.

  Creates spin box preference item for the entering values which type
  is specified by the parameter \a type.

  \param type input type (QtxPagePrefSpinItem::InputType).
  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefSpinItem::QtxPagePrefSpinItem( const int type, const QString& title,
                                          QtxPreferenceItem* parent, const QString& sect,
                                          const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param ),
  myType( type )
{
  updateSpinBox();
}

/*!
  \brief Destructor.
*/
QtxPagePrefSpinItem::~QtxPagePrefSpinItem()
{
}

/*!
  \brief Get spin box preference item input type.
  \return preference item input type (QtxPagePrefSpinItem::InputType)
  \sa setInputType()
*/
int QtxPagePrefSpinItem::inputType() const
{
  return myType;
}

/*!
  \brief Set spin box preference item input type.
  \param type new preference item input type (QtxPagePrefSpinItem::InputType)
  \sa inputType()
*/
void QtxPagePrefSpinItem::setInputType( const int type )
{
  if ( myType == type )
    return;

  myType = type;
  updateSpinBox();
}

/*!
  \brief Get spin box preference item step value.
  \return spin box single step value
  \sa setStep()
*/
QVariant QtxPagePrefSpinItem::step() const
{
  if ( QtxIntSpinBox* isb = ::qobject_cast<QtxIntSpinBox*>( control() ) )
    return isb->singleStep();
  else if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
    return dsb->singleStep();
  else
    return QVariant();
}

/*!
  \brief Get double spin box preference item precision value.
  \return double spin box precision
  \sa setPrecision()
*/
QVariant QtxPagePrefSpinItem::precision() const
{
  if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
    return dsb->decimals();
  else
    return QVariant();
}

/*!
  \brief Get spin box preference item minimum value.
  \return spin box minimum value
  \sa setMinimum()
*/
QVariant QtxPagePrefSpinItem::minimum() const
{
  if ( QtxIntSpinBox* isb = ::qobject_cast<QtxIntSpinBox*>( control() ) )
    return isb->minimum();
  else if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
    return dsb->minimum();
  else
    return QVariant();
}

/*!
  \brief Get spin box preference item maximum value.
  \return spin box maximum value
  \sa setMaximum()
*/
QVariant QtxPagePrefSpinItem::maximum() const
{
  if ( QtxIntSpinBox* isb = ::qobject_cast<QtxIntSpinBox*>( control() ) )
    return isb->maximum();
  else if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
    return dsb->maximum();
  else
    return QVariant();
}

/*!
  \brief Get spin box preference item prefix string.
  \return spin box prefix string
  \sa setPrefix()
*/
QString QtxPagePrefSpinItem::prefix() const
{
  if ( QtxIntSpinBox* isb = ::qobject_cast<QtxIntSpinBox*>( control() ) )
    return isb->prefix();
  else if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
    return dsb->prefix();
  else
    return QString();
}

/*!
  \brief Get spin box preference item suffix string.
  \return spin box suffix string
  \sa setSuffix()
*/
QString QtxPagePrefSpinItem::suffix() const
{
  if ( QtxIntSpinBox* isb = ::qobject_cast<QtxIntSpinBox*>( control() ) )
    return isb->suffix();
  else if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
    return dsb->suffix();
  else
    return QString();
}

/*!
  \brief Get spin box preference item special value text (which is shown
  when the spin box reaches minimum value).
  \return spin box special value text
  \sa setSpecialValueText()
*/
QString QtxPagePrefSpinItem::specialValueText() const
{
  QAbstractSpinBox* sb = ::qobject_cast<QAbstractSpinBox*>( control() );
  if ( sb )
    return sb->specialValueText();
  else
    return QString();
}

/*!
  \brief Set spin box preference item step value.
  \param step new spin box single step value
  \sa step()
*/
void QtxPagePrefSpinItem::setStep( const QVariant& step )
{
  if ( QtxIntSpinBox* isb = ::qobject_cast<QtxIntSpinBox*>( control() ) )
  {
    if ( step.canConvert( QVariant::Int ) )
      isb->setSingleStep( step.toInt() );
  }
  else if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
  {
    if ( step.canConvert( QVariant::Double ) )
      dsb->setSingleStep( step.toDouble() );
  }
}

/*!
  \brief Set double spin box preference item precision value.
  \param step new double spin box precision value
  \sa precision()
*/
void QtxPagePrefSpinItem::setPrecision( const QVariant& prec )
{
  if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
  {
    if ( prec.canConvert( QVariant::Int ) ) {
      dsb->setDecimals( qAbs( prec.toInt() ) );
      dsb->setPrecision( prec.toInt() );
    }
  }
}

/*!
  \brief Set spin box preference item minimum value.
  \param min new spin box minimum value
  \sa minimum()
*/
void QtxPagePrefSpinItem::setMinimum( const QVariant& min )
{
  if ( QtxIntSpinBox* isb = ::qobject_cast<QtxIntSpinBox*>( control() ) )
  {
    if ( min.canConvert( QVariant::Int ) )
      isb->setMinimum( min.toInt() );
  }
  else if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
  {
    if ( min.canConvert( QVariant::Double ) )
      dsb->setMinimum( min.toDouble() );
  }
}

/*!
  \brief Set spin box preference item maximum value.
  \param min new spin box maximum value
  \sa maximum()
*/
void QtxPagePrefSpinItem::setMaximum( const QVariant& max )
{
  if ( QtxIntSpinBox* isb = ::qobject_cast<QtxIntSpinBox*>( control() ) )
  {
    if ( max.canConvert( QVariant::Int ) )
      isb->setMaximum( max.toInt() );
  }
  else if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
  {
    if ( max.canConvert( QVariant::Double ) )
      dsb->setMaximum( max.toDouble() );
  }
}

/*!
  \brief Set spin box preference item prefix string.
  \param txt new spin box prefix string
  \sa prefix()
*/
void QtxPagePrefSpinItem::setPrefix( const QString& txt )
{
  if ( QtxIntSpinBox* isb = ::qobject_cast<QtxIntSpinBox*>( control() ) )
    isb->setPrefix( txt );
  else if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
    dsb->setPrefix( txt );
}

/*!
  \brief Set spin box preference item suffix string.
  \param txt new spin box suffix string
  \sa suffix()
*/
void QtxPagePrefSpinItem::setSuffix( const QString& txt )
{
  if ( QtxIntSpinBox* isb = ::qobject_cast<QtxIntSpinBox*>( control() ) )
    isb->setSuffix( txt );
  else if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
    dsb->setSuffix( txt );
}

/*!
  \brief Set spin box preference item special value text (which is shown
  when the spin box reaches minimum value).
  \param txt new spin box special value text
  \sa specialValueText()
*/
void QtxPagePrefSpinItem::setSpecialValueText( const QString& txt )
{
  QAbstractSpinBox* sb = ::qobject_cast<QAbstractSpinBox*>( control() );
  if ( sb )
    sb->setSpecialValueText( txt );
}

/*!
  \brief Store preference item to the resource manager.
  \sa retrieve()
*/
void QtxPagePrefSpinItem::store()
{
  if ( QtxIntSpinBox* isb = ::qobject_cast<QtxIntSpinBox*>( control() ) )
    setInteger( isb->value() );
  else if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
    setDouble( dsb->value() );
}

/*!
  \brief Retrieve preference item from the resource manager.
  \sa store()
*/
void QtxPagePrefSpinItem::retrieve()
{
  if ( QtxIntSpinBox* isb = ::qobject_cast<QtxIntSpinBox*>( control() ) )
    isb->setValue( getInteger( isb->value() ) );
  else if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
    dsb->setValue( getDouble( dsb->value() ) );
}

/*!
  \brief Get preference item option value.
  \param name option name
  \return property value or null QVariant if option is not set
  \sa setOptionValue()
*/
QVariant QtxPagePrefSpinItem::optionValue( const QString& name ) const
{
  if ( name == "input_type" || name == "type" )
    return inputType();
  else if ( name == "minimum" || name == "min" )
    return minimum();
  else if ( name == "maximum" || name == "max" )
    return maximum();
  else if ( name == "step" )
    return step();
  else if ( name == "precision" )
    return precision();
  else if ( name == "prefix" )
    return prefix();
  else if ( name == "suffix" )
    return suffix();
  else if ( name == "special" )
    return specialValueText();
  else
    return QtxPageNamedPrefItem::optionValue( name );
}

/*!
  \brief Set preference item option value.
  \param name option name
  \param val new property value
  \sa optionValue()
*/
void QtxPagePrefSpinItem::setOptionValue( const QString& name, const QVariant& val )
{
  if ( name == "input_type" || name == "type" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setInputType( val.toInt() );
  }
  else if ( name == "minimum" || name == "min" )
    setMinimum( val );
  else if ( name == "maximum" || name == "max" )
    setMaximum( val );
  else if ( name == "step" )
    setStep( val );
  else if ( name == "precision" )
    setPrecision( val );
  else if ( name == "prefix" )
  {
    if ( val.canConvert( QVariant::String ) )
      setPrefix( val.toString() );
  }
  else if ( name == "suffix" )
  {
    if ( val.canConvert( QVariant::String ) )
      setSuffix( val.toString() );
  }
  else if ( name == "special" )
  {
    if ( val.canConvert( QVariant::String ) )
      setSpecialValueText( val.toString() );
  }
  else
    QtxPageNamedPrefItem::setOptionValue( name, val );
}

/*!
  \brief Update spin box widget.
*/
void QtxPagePrefSpinItem::updateSpinBox()
{
  QVariant val;
  QVariant stp = step();
  QVariant prec = precision();
  QVariant min = minimum();
  QVariant max = maximum();

  if ( QtxIntSpinBox* isb = ::qobject_cast<QtxIntSpinBox*>( control() ) )
    val = isb->value();
  else if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
    val = dsb->value();

  switch ( inputType() )
  {
  case Integer:
    setControl( new QtxIntSpinBox() );
    break;
  case Double:
    setControl( new QtxDoubleSpinBox() );
    break;
  default:
    break;
  }

  control()->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

  setStep( stp );
  setPrecision( prec );
  setMinimum( min );
  setMaximum( max );

  if ( QtxIntSpinBox* isb = ::qobject_cast<QtxIntSpinBox*>( control() ) )
  {
    if ( val.canConvert( QVariant::Int ) )
      isb->setValue( val.toInt() );
  }
  else if ( QtxDoubleSpinBox* dsb = ::qobject_cast<QtxDoubleSpinBox*>( control() ) )
  {
    if ( val.canConvert( QVariant::Double ) )
      dsb->setValue( val.toDouble() );
  }
}

/*!
  \class  QtxPagePrefTextItem
  \brief GUI implementation of the resources text box edit item
  (for large text data).
*/

/*!
  \brief Constructor.
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefTextItem::QtxPagePrefTextItem( QtxPreferenceItem* parent, const QString& sect,
                                          const QString& param )
: QtxPageNamedPrefItem( QString(), parent, sect, param )
{
  myEditor = new QTextEdit();
  myEditor->setAcceptRichText( false );

  setControl( myEditor );
}

/*!
  \brief Constructor.
  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefTextItem::QtxPagePrefTextItem( const QString& title, QtxPreferenceItem* parent,
                                          const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param )
{
  myEditor = new QTextEdit();
  myEditor->setAcceptRichText( false );

  setControl( myEditor );
}

/*!
  \brief Destructor.
*/
QtxPagePrefTextItem::~QtxPagePrefTextItem()
{
}

/*!
  \brief Store preference item to the resource manager.
  \sa retrieve()
*/
void QtxPagePrefTextItem::store()
{
  setString( myEditor->toPlainText() );
}

/*!
  \brief Retrieve preference item from the resource manager.
  \sa store()
*/
void QtxPagePrefTextItem::retrieve()
{
  myEditor->setPlainText( getString() );
}

/*!
  \class QtxPagePrefColorItem
  \brief GUI implementation of the resources color item.
*/

/*!
  \brief Constructor.
  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefColorItem::QtxPagePrefColorItem( const QString& title, QtxPreferenceItem* parent,
                                            const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param )
{
  //  QtxPagePrefGroupItem* aGroup 0; //= dynamic_cast<QtxPagePrefGroupItem*>( parent );

  //  setControl( myColor = new QtxColorButton( aGroup ? aGroup->gridBox() : 0 ) );
  setControl( myColor = new QtxColorButton( 0 ) );
  myColor->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
}

/*!
  \brief Destructor.
*/
QtxPagePrefColorItem::~QtxPagePrefColorItem()
{
}

/*!
  \brief Store preference item to the resource manager.
  \sa retrieve()
*/
void QtxPagePrefColorItem::store()
{
  setColor( myColor->color() );
}

/*!
  \brief Retrieve preference item from the resource manager.
  \sa store()
*/
void QtxPagePrefColorItem::retrieve()
{
  myColor->setColor( getColor() );
}

/*!
  \class QtxPagePrefBiColorItem
  \brief GUI implementation of the resources item to store a bi-color value.

  The main color is specified explicitly. The secondary color is calculated
  by changing "value" and "saturation" parameters of the main color in the 
  HSV notation using specified delta.
*/

/*!
  \brief Constructor.
  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefBiColorItem::QtxPagePrefBiColorItem( const QString& title, QtxPreferenceItem* parent,
						const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param )
{
  setControl( myColors = new QtxBiColorTool( 0 ) );
}

/*!
  \brief Destructor.
*/
QtxPagePrefBiColorItem::~QtxPagePrefBiColorItem()
{
}

/*!
  \bried Get auxiliary text
  \return text assigned to the item
  \sa setText()
*/
QString QtxPagePrefBiColorItem::text() const
{
  return myColors->text();
}

/*!
  \bried Set auxiliary text
  \param t text being assigned to the item
  \sa text()
*/
void QtxPagePrefBiColorItem::setText( const QString& t )
{
  myColors->setText( t );
}

/*!
  \brief Store preference item to the resource manager.
  \sa retrieve()
*/
void QtxPagePrefBiColorItem::store()
{
  setString( Qtx::biColorToString( myColors->mainColor(), myColors->delta() ) );
}

/*!
  \brief Retrieve preference item from the resource manager.
  \sa store()
*/
void QtxPagePrefBiColorItem::retrieve()
{
  QColor c;
  int d;
  Qtx::stringToBiColor( getString(), c, d );
  myColors->setMainColor( c );
  myColors->setDelta( d );
}

/*!
  \brief Get preference item option value.
  \param name option name
  \return property value or null QVariant if option is not set
  \sa setOptionValue()
*/
QVariant QtxPagePrefBiColorItem::optionValue( const QString& name ) const
{
  if ( name == "text" )
    return text();
  else
    return QtxPageNamedPrefItem::optionValue( name );
}

/*!
  \brief Set preference item option value.
  \param name option name
  \param val new property value
  \sa optionValue()
*/
void QtxPagePrefBiColorItem::setOptionValue( const QString& name, const QVariant& val )
{
  if ( name == "text" )
  {
    if ( val.canConvert( QVariant::String ) )
      setText( val.toString() );
  }
  else
    QtxPageNamedPrefItem::setOptionValue( name, val );
}

/*!
  \class QtxPagePrefFontItem
  \brief GUI implementation of the resources font item.
*/

/*!
  \brief Constructor.
  \param feat font editor widget features (QtxFontEdit::Features)
  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefFontItem::QtxPagePrefFontItem( const int feat, const QString& title,
                                          QtxPreferenceItem* parent, const QString& sect,
                                          const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param )
{
  setControl( myFont = new QtxFontEdit( feat ) );
}

/*!
  \brief Constructor.
  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefFontItem::QtxPagePrefFontItem( const QString& title, QtxPreferenceItem* parent,
                                          const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param )
{
  setControl( myFont = new QtxFontEdit() );
}

/*!
  \brief Destructor.
*/
QtxPagePrefFontItem::~QtxPagePrefFontItem()
{
}

/*!
  \brief Get font widget features.
  \return font widget features (ORed QtxFontEdit::Features flags)
  \sa setFeatures()
*/
int QtxPagePrefFontItem::features() const
{
  return myFont->features();
}

/*!
  \brief Set font widget features.
  \param f new font widget features (ORed QtxFontEdit::Features flags)
  \sa features()
*/
void QtxPagePrefFontItem::setFeatures( const int f )
{
  myFont->setFeatures( f );
}

/*!
  \brief Specifies whether widget works in Native or Custom mode. Native mode 
  is intended for working with system fonts. Custom mode is intended for 
  working with manually defined set of fonts. Set of custom fonts can be 
  specified with setFonts() method 
  \param mode mode from QtxFontEdit::Mode enumeration
  \sa mode()
*/
void QtxPagePrefFontItem::setMode( const int mode )
{
  myFont->setMode( mode );
}

/*!
  \brief Verifies whether widget works in Native or Custom mode
  \return Native or Custom mode
  \sa setMode()
*/
int QtxPagePrefFontItem::mode() const
{
  return myFont->mode();
}

/*!
  \brief Sets list of custom fonts. 
  <b>This method is intended for working in Custom mode only.</b>
  \param fams list of families
  \sa fonts(), setMode()
*/
void QtxPagePrefFontItem::setFonts( const QStringList& fams )
{
  myFont->setFonts( fams );
}

/*!
  \brief Gets list of custom fonts 
  \return list of families
  \sa setFonts(), setMode()
*/
QStringList QtxPagePrefFontItem::fonts() const
{
  return myFont->fonts();
}

/*!
  \brief Sets list of available font sizes. 
  <b>This method is intended for working in Custom mode only.</b> The list of sizes can 
  be empty. In this case system generate listof size automatically from 8 till 72.
  \param sizes list of sizes
  \sa sizes(), setMode()
*/
void QtxPagePrefFontItem::setSizes( const QList<int>& sizes )
{
  myFont->setSizes( sizes );
}

/*!
  \brief Gets list of custom fonts 
  \return list of families
  \sa setFonts(), setMode()
*/
QList<int> QtxPagePrefFontItem::sizes() const
{
  return myFont->sizes();
}

/*!
  \brief Store preference item to the resource manager.
  \sa retrieve()
*/
void QtxPagePrefFontItem::store()
{
  setFont( myFont->currentFont() );
}

/*!
  \brief Retrieve preference item from the resource manager.
  \sa store()
*/
void QtxPagePrefFontItem::retrieve()
{
  myFont->setCurrentFont( getFont() );
}

/*!
  \brief Get preference item option value.
  \param name option name
  \return property value or null QVariant if option is not set
  \sa setOptionValue()
*/
QVariant QtxPagePrefFontItem::optionValue( const QString& name ) const
{
  if ( name == "features" )
    return features();
  else if ( name == "mode" )
    return mode();
  else if ( name == "fonts" || name == "families" )
    return fonts();
  else if ( name == "sizes" )
  {
    QList<QVariant> lst;
    QList<int> nums = sizes();
    for ( QList<int>::const_iterator it = nums.begin(); it != nums.end(); ++it )
      lst.append( *it );
    return lst;
  }
  else
    return QtxPageNamedPrefItem::optionValue( name );
}

/*!
  \brief Set preference item option value.
  \param name option name
  \param val new property value
  \sa optionValue()
*/
void QtxPagePrefFontItem::setOptionValue( const QString& name, const QVariant& val )
{
  if ( name == "features" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setFeatures( val.toInt() );
  }
  else if ( name == "mode" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setMode( val.toInt() );
  }
  else if ( name == "fonts" || name == "families" )
  {
    QStringList values;
    if ( toStringList( val, values ) )
      setFonts( values );
  }
  else if ( name == "sizes" )
  {
    if ( val.type() == QVariant::List )
    {
      QList<int> lst;
      QList<QVariant> varList = val.toList();
      for ( QList<QVariant>::const_iterator it = varList.begin(); it != varList.end(); ++it )
      {
        if ( (*it).canConvert( QVariant::Int ) )
          lst.append( (*it).toInt() );
      }
      setSizes( lst );
    }
  }
  else
    QtxPageNamedPrefItem::setOptionValue( name, val );
}

/*!
  \class  QtxPagePrefPathItem
  \brief GUI implementation of the resources file/directory path item.
*/

/*!
  \brief Constructor.
  \param type path widget mode (Qtx::PathType )
  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefPathItem::QtxPagePrefPathItem( const Qtx::PathType type, const QString& title,
                                          QtxPreferenceItem* parent, const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param )
{
  setControl( myPath = new QtxPathEdit( type ) );
}

/*!
  \brief Constructor.
  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefPathItem::QtxPagePrefPathItem( const QString& title, QtxPreferenceItem* parent,
                                          const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param )
{
  setControl( myPath = new QtxPathEdit() );
}

/*!
  \brief Destructor.
*/
QtxPagePrefPathItem::~QtxPagePrefPathItem()
{
}

/*!
  \brief Get path widget mode.
  \return current path widget mode (Qtx::PathType)
  \sa setPathType()
*/
Qtx::PathType QtxPagePrefPathItem::pathType() const
{
  return myPath->pathType();
}

/*!
  \brief Set path widget mode.
  \param type new path widget mode (Qtx::PathType)
  \sa pathType()
*/
void QtxPagePrefPathItem::setPathType( const Qtx::PathType type )
{
  myPath->setPathType( type );
}

/*!
  \brief Get currently used path widget filters.
  \return file or directory path filters
  \sa setPathFilter()
*/
QString QtxPagePrefPathItem::pathFilter() const
{
  return myPath->pathFilter();
}

/*!
  \brief Set path widget filters.
  \param f new file or directory path filters
  \sa pathFilter()
*/
void QtxPagePrefPathItem::setPathFilter( const QString& f )
{
  myPath->setPathFilter( f );
}

/*!
  \brief Store preference item to the resource manager.
  \sa retrieve()
*/
void QtxPagePrefPathItem::store()
{
  setString( myPath->path() );
}

/*!
  \brief Retrieve preference item from the resource manager.
  \sa store()
*/
void QtxPagePrefPathItem::retrieve()
{
  myPath->setPath( getString() );
}

/*!
  \brief Get preference item option value.
  \param name option name
  \return property value or null QVariant if option is not set
  \sa setOptionValue()
*/
QVariant QtxPagePrefPathItem::optionValue( const QString& name ) const
{
  if ( name == "path_type" )
    return pathType();
  else if ( name == "path_filter" )
    return pathFilter();
  else
    return QtxPageNamedPrefItem::optionValue( name );
}

/*!
  \brief Set preference item option value.
  \param name option name
  \param val new property value
  \sa optionValue()
*/
void QtxPagePrefPathItem::setOptionValue( const QString& name, const QVariant& val )
{
  if ( name == "path_type" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setPathType( (Qtx::PathType)val.toInt() );
  }
  else if ( name == "path_filter" )
  {
    if ( val.canConvert( QVariant::String ) )
      setPathFilter( val.toString() );
  }
  else
    QtxPageNamedPrefItem::setOptionValue( name, val );
}

/*!
  \class QtxPagePrefPathListItem
  \brief GUI implementation of the resources files/directories list item.
*/

/*!
  \brief Constructor.
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefPathListItem::QtxPagePrefPathListItem( QtxPreferenceItem* parent,
                                                  const QString& sect, const QString& param )
: QtxPageNamedPrefItem( QString(), parent, sect, param )
{
  setControl( myPaths = new QtxPathListEdit() );
}

/*!
  \brief Constructor.
  \param type path list widget mode (Qtx::PathType)
  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefPathListItem::QtxPagePrefPathListItem( const Qtx::PathType type, const QString& title,
                                                  QtxPreferenceItem* parent, const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param )
{
  setControl( myPaths = new QtxPathListEdit( type ) );
}

/*!
  \brief Constructor.
  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefPathListItem::QtxPagePrefPathListItem( const QString& title, QtxPreferenceItem* parent,
                                                  const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param )
{
  setControl( myPaths = new QtxPathListEdit() );
}

/*!
  \brief Destructor.
*/
QtxPagePrefPathListItem::~QtxPagePrefPathListItem()
{
}

/*!
  \brief Get path list widget mode.
  \return currently used path list widget mode (Qtx::PathType)
  \sa setPathType()
*/
Qtx::PathType QtxPagePrefPathListItem::pathType() const
{
  return myPaths->pathType();
}

/*!
  \brief Set path list widget mode.
  \param type new path list widget mode (Qtx::PathType)
  \sa pathType()
*/
void QtxPagePrefPathListItem::setPathType( const Qtx::PathType type )
{
  myPaths->setPathType( type );
}

/*!
  \brief Store preference item to the resource manager.
  \sa retrieve()
*/
void QtxPagePrefPathListItem::store()
{
  setString( myPaths->pathList().join( ";" ) );
}

/*!
  \brief Retrieve preference item from the resource manager.
  \sa store()
*/
void QtxPagePrefPathListItem::retrieve()
{
  myPaths->setPathList( getString().split( ";" ) );
}

/*!
  \brief Get preference item option value.
  \param name option name
  \return property value or null QVariant if option is not set
  \sa setOptionValue()
*/
QVariant QtxPagePrefPathListItem::optionValue( const QString& name ) const
{
  if ( name == "path_type" )
    return pathType();
  else
    return QtxPageNamedPrefItem::optionValue( name );
}

/*!
  \brief Set preference item option value.
  \param name option name
  \param val new property value
  \sa optionValue()
*/
void QtxPagePrefPathListItem::setOptionValue( const QString& name, const QVariant& val )
{
  if ( name == "path_type" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setPathType( (Qtx::PathType)val.toInt() );
  }
  else
    QtxPageNamedPrefItem::setOptionValue( name, val );
}

/*!
  \class  QtxPagePrefDateTimeItem
  \brief GUI implementation of resources date/time item.
*/

/*!
  \brief Constructor.

  Creates an item to enter date and time.

  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefDateTimeItem::QtxPagePrefDateTimeItem( const QString& title, QtxPreferenceItem* parent,
                                                  const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param ),
  myType( DateTime )
{
  setControl( myDateTime = new QDateTimeEdit() );
  myDateTime->setCalendarPopup( true );
  myDateTime->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  updateDateTime();
}

/*!
  \brief Constructor.

  Creates preference item for editing of the date and/or time value:
  the type is specified by parameter \a type.

  \param type preference item input type (QtxPagePrefDateTimeItem::InputType)
  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefDateTimeItem::QtxPagePrefDateTimeItem( const int type, const QString& title, QtxPreferenceItem* parent,
                                                  const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param ),
  myType( type )
{
  setControl( myDateTime = new QDateTimeEdit() );
  myDateTime->setCalendarPopup( true );
  myDateTime->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  updateDateTime();
}

/*!
  \brief Destructor.
*/
QtxPagePrefDateTimeItem::~QtxPagePrefDateTimeItem()
{
}

/*!
  \brief Get date/time box preference item input type.
  \return preference item input type (QtxPagePrefDateTimeItem::InputType)
  \sa setInputType()
*/
int QtxPagePrefDateTimeItem::inputType() const
{
  return myType;
}

/*!
  \brief Set date/time box preference item input type.
  \param type new preference item input type (QtxPagePrefDateTimeItem::InputType)
  \sa inputType()
*/
void QtxPagePrefDateTimeItem::setInputType( const int type )
{
  if ( myType == type )
    return;

  myType = type;
  updateDateTime();
}

/*!
  \brief Check if the popup calendar menu is enabled.
  \return \c true if calendar popup menu is enabled
*/
bool QtxPagePrefDateTimeItem::calendar() const
{
  return myDateTime->calendarPopup();
}

/*!
  \brief Enable/disable popup calendar menu.
  \param on new flag state
*/
void QtxPagePrefDateTimeItem::setCalendar( const bool on )
{
  myDateTime->setCalendarPopup( on );
}

/*!
  \brief Get maximum date value.
  \return maximum date value
  \sa setMaximumDate(), minimumDate(), maximumTime(), minimumTime()
*/
QDate QtxPagePrefDateTimeItem::maximumDate() const
{
  return myDateTime->maximumDate();
}

/*!
  \brief Get maximum time value.
  \return maximum time value
  \sa setMaximumTime(), minimumTime(), maximumDate(), minimumDate()
*/
QTime QtxPagePrefDateTimeItem::maximumTime() const
{
  return myDateTime->maximumTime();
}

/*!
  \brief Get minimum date value.
  \return minimum date value
  \sa setMinimumDate(), maximumDate(), maximumTime(), minimumTime()
*/
QDate QtxPagePrefDateTimeItem::minimumDate() const
{
  return myDateTime->minimumDate();
}

/*!
  \brief Get minimum time value.
  \return maximum time value
  \sa setMinimumTime(), maximumTime(), maximumDate(), minimumDate()
*/
QTime QtxPagePrefDateTimeItem::minimumTime() const
{
  return myDateTime->minimumTime();
}

/*!
  \brief Set maximum date value.
  \param d new maximum date value
  \sa maximumDate(), minimumDate(), maximumTime(), minimumTime()
*/
void QtxPagePrefDateTimeItem::setMaximumDate( const QDate& d )
{
  if ( d.isValid() )
    myDateTime->setMaximumDate( d );
  else
    myDateTime->clearMaximumDate();
}

/*!
  \brief Set maximum time value.
  \param t new maximum time value
  \sa maximumTime(), minimumTime(), maximumDate(), minimumDate()
*/
void QtxPagePrefDateTimeItem::setMaximumTime( const QTime& t )
{
  if ( t.isValid() )
    myDateTime->setMaximumTime( t );
  else
    myDateTime->clearMaximumTime();
}

/*!
  \brief Set minimum date value.
  \param d new minimum date value
  \sa minimumDate(), maximumDate(), maximumTime(), minimumTime()
*/
void QtxPagePrefDateTimeItem::setMinimumDate( const QDate& d )
{
  if ( d.isValid() )
    myDateTime->setMinimumDate( d );
  else
    myDateTime->clearMinimumDate();
}

/*!
  \brief Set minimum time value.
  \param t new minimum time value
  \sa minimumTime(), maximumTime(), maximumDate(), minimumDate()
*/
void QtxPagePrefDateTimeItem::setMinimumTime( const QTime& t )
{
  if ( t.isValid() )
    myDateTime->setMinimumTime( t );
  else
    myDateTime->clearMinimumTime();
}

/*!
  \brief Store preference item to the resource manager.
  \sa retrieve()
*/
void QtxPagePrefDateTimeItem::store()
{
  QString str;
  switch ( inputType() )
  {
  case Date:
    str = myDateTime->date().toString( Qt::ISODate );
    break;
  case Time:
    str = myDateTime->time().toString( Qt::ISODate );
    break;
  case DateTime:
    str = myDateTime->dateTime().toString( Qt::ISODate );
    break;
  }

  setString( str );
}

/*!
  \brief Retrieve preference item from the resource manager.
  \sa store()
*/
void QtxPagePrefDateTimeItem::retrieve()
{
  QString str = getString();
  switch ( inputType() )
  {
  case Date:
    myDateTime->setDate( QDate::fromString( str, Qt::ISODate ) );
    break;
  case Time:
    myDateTime->setTime( QTime::fromString( str, Qt::ISODate ) );
    break;
  case DateTime:
    myDateTime->setDateTime( QDateTime::fromString( str, Qt::ISODate ) );
    break;
  }
}

/*!
  \brief Get preference item option value.
  \param name option name
  \return property value or null QVariant if option is not set
  \sa setOptionValue()
*/
QVariant QtxPagePrefDateTimeItem::optionValue( const QString& name ) const
{
  if ( name == "input_type" || name == "type" )
    return inputType();
  else if ( name == "minimum_date" || name == "min_date" )
    return minimumDate();
  else if ( name == "maximum_date" || name == "max_date" )
    return maximumDate();
  else if ( name == "minimum_time" || name == "min_time" )
    return minimumTime();
  else if ( name == "maximum_time" || name == "max_time" )
    return maximumTime();
  else
    return QtxPageNamedPrefItem::optionValue( name );
}

/*!
  \brief Set preference item option value.
  \param name option name
  \param val new property value
  \sa optionValue()
*/
void QtxPagePrefDateTimeItem::setOptionValue( const QString& name, const QVariant& val )
{
  if ( name == "input_type" || name == "type" )
  {
    if ( val.canConvert( QVariant::Int ) )
      setInputType( val.toInt() );
  }
  else if ( name == "minimum_date" || name == "min_date" )
  {
    if ( val.canConvert( QVariant::Date ) )
      setMinimumDate( val.toDate() );
  }
  else if ( name == "maximum_date" || name == "max_date" )
  {
    if ( val.canConvert( QVariant::Date ) )
      setMaximumDate( val.toDate() );
  }
  else if ( name == "minimum_time" || name == "min_time" )
  {
    if ( val.canConvert( QVariant::Time ) )
      setMinimumTime( val.toTime() );
  }
  else if ( name == "maximum_time" || name == "max_time" )
  {
    if ( val.canConvert( QVariant::Time ) )
      setMaximumTime( val.toTime() );
  }
  else
    QtxPageNamedPrefItem::setOptionValue( name, val );
}

/*!
  \brief Update date/time widget.
*/
void QtxPagePrefDateTimeItem::updateDateTime()
{
  QString dispFmt;
  switch ( inputType() )
  {
  case Date:
    dispFmt = QDateEdit().displayFormat();
    break;
  case Time:
    dispFmt = QTimeEdit().displayFormat();
    break;
  case DateTime:
    dispFmt = QDateTimeEdit().displayFormat();
    break;
  }

  myDateTime->setDisplayFormat( dispFmt );
}

/*!
  \brief Constructor.
  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefShortcutBtnsItem::QtxPagePrefShortcutBtnsItem( const QString& title, QtxPreferenceItem* parent, const QString& sect,
                                                          const QString& param ): QtxPageNamedPrefItem( title, parent, sect, param )
{
  setControl( myShortcut = new QtxShortcutEdit() );
}

/*!
  \brief Destructor.
*/  
QtxPagePrefShortcutBtnsItem::~QtxPagePrefShortcutBtnsItem()
{
}

/*!
  \brief Store preference item to the resource manager.
  \sa retrieve()
*/
void QtxPagePrefShortcutBtnsItem::store()
{
  setString( myShortcut->shortcut().toString() );
}

/*!
  \brief Retrieve preference item from the resource manager.
  \sa store()
*/
void QtxPagePrefShortcutBtnsItem::retrieve()
{
  myShortcut->setShortcut( QKeySequence::fromString( getString() ) );
}

/*!
  \brief Constructor.

  Creates preference item for editing of key bindings

  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefShortcutTreeItem::QtxPagePrefShortcutTreeItem( const QString& title, QtxPreferenceItem* parent, const QString& sect, 
                                                          const QString& param ): QtxPageNamedPrefItem( title, parent, sect, "" )
{
  mySection = sect;

  myShortcutTree = new QtxShortcutTree();

  // Retrieve shortcuts common sections from resources
  QtxResourceMgr* resMgr = resourceMgr();
  if ( resMgr ){
    QString generalSections = resourceMgr()->stringValue( "shortcuts_settings", "general_sections", QString() );
    QStringList sectionsList = generalSections.split( ";", QString::SkipEmptyParts );
    myShortcutTree->setGeneralSections( sectionsList );
  }
 
  setControl( myShortcutTree );
}

/*!
  \brief Destructor.
*/
QtxPagePrefShortcutTreeItem::~QtxPagePrefShortcutTreeItem()
{
}
						    
/*!
  \brief Retrieve preference item from the resource manager.
  \sa store()
*/
void QtxPagePrefShortcutTreeItem::retrieve()
{
  QtxResourceMgr* resMgr = resourceMgr();
  if ( resMgr ){
    QStringList secLst = resMgr->subSections( mySection, false );
    ShortcutMap aMap; QStringList paramLst;
    for( int i = 0; i < secLst.size(); i++ ) {
      paramLst = resMgr->parameters( QStringList() << mySection << secLst.at( i ) );
      for( int j = 0; j < paramLst.size(); j++ )
        resMgr->value( mySection + resMgr->sectionsToken() + secLst.at( i ), paramLst.at( j ),aMap[ paramLst.at( j ) ], false );
      myShortcutTree->setBindings( secLst.at( i ), aMap );
      aMap.clear();
    }
  }
}
	      
/*!
  \brief Store preference item to the resource manager.
  \sa retrieve()
*/
void QtxPagePrefShortcutTreeItem::store()
{
  QStringList lst = myShortcutTree->sections();
  QString aSection;
  QtxResourceMgr* resMgr = resourceMgr();
  
  if ( resMgr ) {
    for( int i = 0; i < lst.size(); i++ ) {
      ShortcutMap* aMap( myShortcutTree->bindings( lst.at( i ) ) );
      aSection = mySection + resMgr->sectionsToken() + lst.at( i );
      for( ShortcutMap::const_iterator it = aMap->constBegin(); it != aMap->constEnd(); ++it )
	resMgr->setValue( aSection, it.key(), it.value() );
    }
  }
}

/*!
  \class QtxPagePrefBackgroundItem
  \brief GUI implementation of the resources item to store background data.

  Preference item allows specifying background data in different ways:
  - solid color
  - texture image file
  - simple two-color gradient
  - complex custom gradient (NOT IMPLEMENTED YET)
  
  Allowed background modes can be specified using setModeAllowed() method.
  Texture modes can be enabled/disabled using setTextureModeAllowed() method.
  Also, showing texture controls can be enabled/disabled by means of
  setTextureAllowed() method.
  Verical or horizontal orientation of the widget can be chosen via setOrientation()
  method (default orientation is horizontal).

  Simple gradient types can be specified using setGradients() method.

  \sa Qtx::BackgroundData, QtxBackgroundTool
*/

/*!
  \brief Constructor.
  \param title preference item title
  \param parent parent preference item
  \param sect resource file section associated with the preference item
  \param param resource file parameter associated with the preference item
*/
QtxPagePrefBackgroundItem::QtxPagePrefBackgroundItem( const QString& title, QtxPreferenceItem* parent,
						      const QString& sect, const QString& param )
: QtxPageNamedPrefItem( title, parent, sect, param )
{
  setControl( myBgTool = new QtxBackgroundTool( 0 ) );
}

/*!
  \brief Destructor.
*/
QtxPagePrefBackgroundItem::~QtxPagePrefBackgroundItem()
{
}

/*!
  \brief Get allowed two-color gradients to the widget
  \param gradients gradients names are returned via this parameter
  \param ids gradients identifiers are returned via this parameter (empty list can be returned)
*/
void QtxPagePrefBackgroundItem::gradients( QStringList& gradList, QIntList& idList ) const
{
  myBgTool->gradients( gradList, idList );
}

/*!
  \brief Set allowed two-color gradients to the widget
  \param gradients gradients names
  \param ids optional gradients identifiers; if not specified, gradients are automatically numbered starting from 0
*/
void QtxPagePrefBackgroundItem::setGradients( const QStringList& gradients, const QIntList& ids )
{
  myBgTool->setGradients( gradients, ids );
}

/*!
  \brief Check if specific background mode is allowed
  \param mode background mode
  \return \c true if specified background mode is enabled or \c false otherwise
  \sa setModeAllowed()
*/
bool QtxPagePrefBackgroundItem::isModeAllowed( Qtx::BackgroundMode mode ) const
{
  return myBgTool->isModeAllowed( mode );
}

/*!
  \brief Enable / disable specific background mode
  \param mode background mode
  \param on enable / disable flag
  \sa isModeAllowed()
*/
void QtxPagePrefBackgroundItem::setModeAllowed( Qtx::BackgroundMode mode, bool on )
{
  myBgTool->setModeAllowed( mode, on );
}

/*!
  \brief Check if specific texture mode is allowed
  \param mode texture mode
  \return \c true if specified texture mode is enabled or \c false otherwise
  \sa setTextureModeAllowed(), setTextureAllowed()
*/
bool QtxPagePrefBackgroundItem::isTextureModeAllowed( Qtx::TextureMode mode ) const
{
  return myBgTool->isTextureModeAllowed( mode );
}

/*!
  \brief Enable / disable specific texture mode
  \param mode texture mode
  \param on enable / disable flag (\c true by default)
  \sa isTextureModeAllowed(), setTextureAllowed()
*/
void QtxPagePrefBackgroundItem::setTextureModeAllowed( Qtx::TextureMode mode, bool on )
{
  myBgTool->setTextureModeAllowed( mode, on );
}

/*!
  \brief Check if texture controls are allowed (shown)
  \return \c true if texture controls are enabled or \c false otherwise
  \sa setTextureAllowed(), setTextureModeAllowed()
*/
bool QtxPagePrefBackgroundItem::isTextureAllowed() const
{
  return myBgTool->isTextureAllowed();
}

/*!
  \brief Enable / disable texture controls
  \param on enable / disable flag (\c true by default)
  \sa isTextureAllowed(), setTextureModeAllowed()
*/
void QtxPagePrefBackgroundItem::setTextureAllowed( bool on )
{
  myBgTool->setTextureAllowed( on );
}

/*!
  \brief Get allowed image formats
  \return image formats
*/
QString QtxPagePrefBackgroundItem::imageFormats() const
{
  return myBgTool->imageFormats();
}

/*!
  \brief Set allowed image formats
  \param formats image formats
*/
void QtxPagePrefBackgroundItem::setImageFormats( const QString& formats )
{
  myBgTool->setImageFormats( formats );
}

/*!
  \brief Get widget editor orientation
  \return orientation
*/
Qt::Orientation QtxPagePrefBackgroundItem::orientation() const
{
  return myBgTool->orientation();
}

/*!
  \brief Set widget editor orientation
  \param o orientation
*/
void QtxPagePrefBackgroundItem::setOrientation( Qt::Orientation o )
{
  myBgTool->setOrientation( o );
}

/*!
  \brief Store preference item to the resource manager.
  \sa retrieve()
*/
void QtxPagePrefBackgroundItem::store()
{
  setString( Qtx::backgroundToString( myBgTool->data() ) );
}

/*!
  \brief Retrieve preference item from the resource manager.
  \sa store()
*/
void QtxPagePrefBackgroundItem::retrieve()
{
  myBgTool->setData( Qtx::stringToBackground( getString() ) );
}

/*!
  \brief Get preference item option value.
  \param name option name
  \return property value or null QVariant if option is not set
  \sa setOptionValue()
*/
QVariant QtxPagePrefBackgroundItem::optionValue( const QString& name ) const
{
  if ( name == "texture_enabled" )
    return isTextureAllowed();
  else if ( name == "color_enabled" )
    return isModeAllowed( Qtx::ColorBackground );
  else if ( name == "gradient_enabled" )
    return isModeAllowed( Qtx::SimpleGradientBackground );
  else if ( name == "custom_enabled" )
    return isModeAllowed( Qtx::CustomGradientBackground );
  else if ( name == "texture_center_enabled" )
    return isTextureModeAllowed( Qtx::CenterTexture );
  else if ( name == "texture_tile_enabled" )
    return isTextureModeAllowed( Qtx::TileTexture );
  else if ( name == "texture_stretch_enabled" )
    return isTextureModeAllowed( Qtx::StretchTexture );
  else if ( name == "orientation" )
    return orientation();
  else if ( name == "image_formats" )
    return imageFormats();
  else if ( name == "gradient_names" ) {
    QStringList grList;
    QIntList    idList;
    gradients( grList, idList );
    return grList;
  }
  else if ( name == "gradient_ids" ) {
    QStringList grList;
    QIntList    idList;
    gradients( grList, idList );
    QList<QVariant> lst;
    for ( QIntList::const_iterator it = idList.begin(); it != idList.end(); ++it )
      lst.append( *it );
    return lst;
  }
  else
    return QtxPageNamedPrefItem::optionValue( name );
}

/*!
  \brief Set preference item option value.
  \param name option name
  \param val new property value
  \sa optionValue()
*/
void QtxPagePrefBackgroundItem::setOptionValue( const QString& name, const QVariant& val )
{
  if ( name == "texture_enabled" ) {
    if ( val.canConvert( QVariant::Bool ) )
      setTextureAllowed( val.toBool() );
  }
  else if ( name == "color_enabled" ) {
    if ( val.canConvert( QVariant::Bool ) )
      setModeAllowed( Qtx::ColorBackground, val.toBool() );
  }
  else if ( name == "gradient_enabled" ) {
    if ( val.canConvert( QVariant::Bool ) )
      setModeAllowed( Qtx::SimpleGradientBackground, val.toBool() );
  }
  else if ( name == "custom_enabled" ) {
    if ( val.canConvert( QVariant::Bool ) )
      setModeAllowed( Qtx::CustomGradientBackground, val.toBool() );
  }
  else if ( name == "texture_center_enabled" ) {
    if ( val.canConvert( QVariant::Bool ) )
      setTextureModeAllowed( Qtx::CenterTexture, val.toBool() );
  }
  else if ( name == "texture_tile_enabled" ) {
    if ( val.canConvert( QVariant::Bool ) )
      setTextureModeAllowed( Qtx::TileTexture, val.toBool() );
  }
  else if ( name == "texture_stretch_enabled" ) {
    if ( val.canConvert( QVariant::Bool ) )
      setTextureModeAllowed( Qtx::StretchTexture, val.toBool() );
  }
  else if ( name == "orientation" ) {
    if ( val.canConvert( QVariant::Int ) )
      setOrientation( (Qt::Orientation)val.toInt() );
  }
  else if ( name == "image_formats" ) {
    if ( val.canConvert( QVariant::String ) )
      setImageFormats( val.toString() );
  }
  else if ( name == "gradient_names" ) {
    QStringList values;
    if ( toStringList( val, values ) )
      setGradients( values );
  }
  else if ( name == "gradient_ids" ) {
    if ( val.canConvert( QVariant::List ) ) {
      QStringList grList;
      QIntList    idList;
      gradients( grList, idList );
      idList.clear();
      QList<QVariant> varList = val.toList();
      for ( QList<QVariant>::const_iterator it = varList.begin(); it != varList.end(); ++it ) {
	if ( (*it).canConvert( QVariant::Int ) )
	  idList.append( (*it).toInt() );
      }
      setGradients( grList, idList );
    }
  }
  else
    QtxPageNamedPrefItem::setOptionValue( name, val );
}

/*!
  \brief Constructor.

  Creates preference item of user defined widget. The item widget has to be inherited from 
  QtxUserDefinedContent class. An instance of this class has to be installed into the item
  with help of setContent method. Methods optionValue and setOptionValue use pointer on the 
  content widget an qint64 value.

  \param parent parent preference item
*/
QtxUserDefinedItem::QtxUserDefinedItem( QtxPreferenceItem* parent )
  : QtxPageNamedPrefItem(QString(), parent), myContent(0)
{
}
  
/*!
  \brief Lets to Store preferences for content instance
  \sa retrieve()
*/
void QtxUserDefinedItem::store()
{
  if (myContent) {
    myContent->store( resourceMgr(), preferenceMgr());
  }
}

/*!
  \brief Lets to Retrieve preferences for content instance
  \sa store()
*/
void QtxUserDefinedItem::retrieve()
{
  if (myContent) {
    myContent->retrieve( resourceMgr(), preferenceMgr());
  }
}

/*!
 * \brief Returns option value
 * \param theName is a string "content"
 * \return pointer on QtxUserDefinedContent class instance as a qint64 value
 */
QVariant QtxUserDefinedItem::optionValue( const QString& theName ) const
{
  if ( theName == "content" )
    return (qint64) myContent;
  else
    return QtxPreferenceItem::optionValue( theName );
}

/*!
 * \brief Sets option value
 * \param theName is a string "content" 
 * \param theVal is a pointer on QtxUserDefinedContent class instance represented as qint64 value
 */
void QtxUserDefinedItem::setOptionValue( const QString& theName, const QVariant& theVal)
{
  if ( theName == "content" ) {
    if ( theVal.canConvert( QVariant::ULongLong ) ) {
      setContent( (QtxUserDefinedContent*)theVal.toULongLong() );
    }
  } else
    QtxPreferenceItem::setOptionValue( theName, theVal );
}
  
/*!
 * \brief Defines content of the property item as a Widget which has to be inherited from 
 * QtxUserDefinedContent class.
 * \param theContent is an QtxUserDefinedContent class instance.
 */
void QtxUserDefinedItem::setContent( QtxUserDefinedContent* theContent ) 
{ 
  myContent = theContent; 
  setControl(myContent);
}
