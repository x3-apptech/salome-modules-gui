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
// File:      QtxListResourceEdit.cxx
// Author:    Sergey TELKOV

#include "QtxListResourceEdit.h"

#include <qhbox.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlistbox.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qtabwidget.h>
#include <qvalidator.h>
#include <qobjectlist.h>
#include <qcolordialog.h>
#include <qwidgetstack.h>
#include <qtoolbutton.h>
#include <qfontdialog.h>
#include <qfontdatabase.h>
#include <qfileinfo.h>
#include <qfiledialog.h>
#include <qapplication.h>

#include "QtxIntSpinBox.h"
#include "QtxDblSpinBox.h"
#include "QtxComboBox.h"
#include "QtxDirListEditor.h"

/*!
  Constructor
*/
QtxListResourceEdit::QtxListResourceEdit( QtxResourceMgr* mgr, QWidget* parent )
: QFrame( parent ),
QtxResourceEdit( mgr )
{
  QVBoxLayout* main = new QVBoxLayout( this, 0, 5 );
  QGroupBox* base = new QGroupBox( 1, Qt::Vertical, "", this );
  base->setFrameStyle( QFrame::NoFrame );
  base->setInsideMargin( 0 );
  main->addWidget( base );

  myList  = new QListBox( base );
  myStack = new QWidgetStack( base );

  myList->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding ) );
  myStack->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );

  myList->setSelectionMode( QListBox::Single );

  connect( myList, SIGNAL( selectionChanged() ), this, SLOT( onSelectionChanged() ) );

  setFocusProxy( myList );

  updateState();
}

/*!
  Destructor
*/
QtxListResourceEdit::~QtxListResourceEdit()
{
}

/*!
  Sets value to widget
  \param id - id of widget
  \param prop - name of resource
  \param val - value of resource
*/
void QtxListResourceEdit::setItemProperty( const int id, const QString& prop, const QVariant& val )
{
  Item* i = item( id );
  if ( !i )
    return;

  bool prev = i->isEmpty();

  QtxResourceEdit::setItemProperty( id, prop, val );

  bool next = i->isEmpty();

  if ( prev != next )
    updateVisible();
}

/*!
  SLOT: called if main list selection changed, raises resource group widgets
*/
void QtxListResourceEdit::onSelectionChanged()
{
  QString title = myList->text( myList->index( myList->selectedItem() ) );
  if ( title.isEmpty() )
    return;

  Item* i = 0;
  QPtrList<Item> lst;
  childItems( lst );
  for ( QPtrListIterator<Item> it( lst ); it.current() && !i; ++it )
  {
    if ( it.current()->title() == title )
      i = it.current();
  }

  if ( i )
    myStack->raiseWidget( i->id() );
}

/*!
  Custom activity after item addition
  \param i - added item
*/
void QtxListResourceEdit::itemAdded( QtxResourceEdit::Item* i )
{
  if ( !i )
    return;

  QPtrList<Item> items;
  childItems( items );

  if ( items.contains( i ) || items.contains( i->parentItem() ) )
    updateVisible();
}

/*!
  Creates and \return category
  \param title - category title
*/
QtxResourceEdit::Item* QtxListResourceEdit::createItem( const QString& title, const int )
{
  Item* i = item( title, -1 );
  if ( i )
    return i;

  Category* category = new Category( this, myStack );
  myStack->addWidget( category, category->id() );

  updateVisible();

  if ( !myList->selectedItem() )
    myList->setSelected( 0, true );

  updateState();

  return category;
}

/*!
  Emits signal about resource changing
  \param map - map of changed resources
*/
void QtxListResourceEdit::changedResources( const QMap<Item*, QString>& map )
{
  QMap<int, QString> idMap;
  for ( QMap<Item*, QString>::ConstIterator it = map.begin(); it != map.end(); ++it )
  {
    idMap.insert( it.key()->id(), it.data() );

    emit resourceChanged( it.key()->id() );

    QString sec, param;
    it.key()->resource( sec, param );
    emit resourceChanged( sec, param );
  }

  emit resourcesChanged( idMap );
}

/*!
  Updates widgets with accordance with main list selection
*/
void QtxListResourceEdit::updateState()
{
  if ( myList->selectedItem() &&  myStack->visibleWidget() )
    myStack->show();
  else
    myStack->hide();

  myList->setShown( myList->count() > 1 );
}

/*!
  Updates visibility state
*/
void QtxListResourceEdit::updateVisible()
{
  QPtrList<Item> items;
  childItems( items );

  QString name = myList->text( myList->index( myList->selectedItem() ) );

  myList->clear();
  for ( QPtrListIterator<Item> it( items ); it.current(); ++it )
  {
    if ( it.current()->isEmpty() )
      continue;

    myList->insertItem( it.current()->title() );
  }

  int idx = -1;
  for ( int i = 0; i < (int)myList->count() && idx == -1; i++ )
  {
    if ( myList->text( i ) == name )
      idx = i;
  }

  myList->setSelected( QMAX( idx, 0 ), true );

  updateState();
}

/*!
  Constructor
*/
QtxListResourceEdit::Category::Category( QtxListResourceEdit* edit, QWidget* parent )
: QFrame( parent ),
Item( edit )
{
  QVBoxLayout* main = new QVBoxLayout( this );
  QGroupBox* base = new QGroupBox( 1, Qt::Horizontal, "", this );
  base->setFrameStyle( QFrame::NoFrame );
  base->setInsideMargin( 0 );
  main->addWidget( base, 1 );

  myTabs = new QTabWidget( base );
  myInfo = new QLabel( base );

  myInfo->setAlignment( Qt::AlignCenter );
  myInfo->setFrameStyle( QFrame::WinPanel | QFrame::Raised );
  myInfo->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );

  updateState();
}

/*!
  Destructor
*/
QtxListResourceEdit::Category::~Category()
{
}

/*!
  \return true if it is empty
*/
bool QtxListResourceEdit::Category::isEmpty() const
{
  return Item::isEmpty() && myInfo->text().isEmpty();
}

/*!
  \return category type
*/
int QtxListResourceEdit::Category::type() const
{
  return -1;
}

/*!
  Default empty implementation of resources storing
*/
void QtxListResourceEdit::Category::store()
{
}

/*!
  Default empty implementation of resources retrieving
*/
void QtxListResourceEdit::Category::retrieve()
{
}

/*!
  \return value of property
  \param prop - property name
*/
QVariant QtxListResourceEdit::Category::property( const QString& prop ) const
{
  QVariant var;
  if ( prop == QString( "information" ) || prop == QString( "info" ) )
    var = myInfo->text();
  return var;
}

/*!
  Sets property value
  \param name - name of property
  \param var - value of property
*/
void QtxListResourceEdit::Category::setProperty( const QString& name, const QVariant& var )
{
  QVariant prop = var;
  if ( !prop.cast( QVariant::String ) )
    return;

  if ( name == QString( "information" ) || name == QString( "info" ) )
    myInfo->setText( prop.toString() );

  updateState();
}

/*!
  Creates new tab
  \param title - name of tab
*/
QtxResourceEdit::Item* QtxListResourceEdit::Category::createItem( const QString& title, const int )
{
  Item* i = item( title, id() );
  if ( i )
    return i;

  Tab* tab = new Tab( resourceEdit(), this, this );
  myTabs->addTab( tab, title );

  updateState();

  return tab;
}

/*!
  Updates category
*/
void QtxListResourceEdit::Category::updateState()
{
  if ( myTabs->count() )
    myTabs->show();
  else
    myTabs->hide();

  if ( !myTabs->count() && !myInfo->text().isEmpty() )
    myInfo->show();
  else
    myInfo->hide();
}


/*!
  Constructor
*/
QtxListResourceEdit::Tab::Tab( QtxResourceEdit* edit, Item* pItem, QWidget* parent )
: QFrame( parent ),
Item( edit, pItem )
{
  QVBoxLayout* main = new QVBoxLayout( this );
  QVBox* vbox = new QVBox( this );
  vbox->setMargin( 5 );
  myMainFrame = vbox;
  main->addWidget( myMainFrame );
  main->addStretch( 1 );
}

/*!
  Destructor
*/
QtxListResourceEdit::Tab::~Tab()
{
}

/*!
  \return tab type
*/
int QtxListResourceEdit::Tab::type() const
{
  return -1;
}

/*!
  Default empty implementation of resources storing
*/
void QtxListResourceEdit::Tab::store()
{
}

/*!
  Default empty implementation of resources retrieving
*/
void QtxListResourceEdit::Tab::retrieve()
{
}

/*!
  Delayed initialization of a widget
*/
void QtxListResourceEdit::Tab::polish()
{
  QFrame::polish();

  adjustLabels();
}

/*!
  Creates new group
  \param title - name of group
*/
QtxResourceEdit::Item* QtxListResourceEdit::Tab::createItem( const QString& title, const int )
{
  Item* i = item( title, id() );
  if ( i )
    return i;

  Group* group = new Group( title, resourceEdit(), this, myMainFrame );

  return group;
}

/*!
  Adjusts sizes of labels
*/
void QtxListResourceEdit::Tab::adjustLabels()
{
  QObjectList* labels = queryList( "QLabel" );
  if ( labels )
  {
    int w = 0;
    for ( QObjectListIt it1( *labels ); it1.current(); ++it1 )
    {
      if ( it1.current()->isWidgetType() )
      {
        QWidget* wid = (QWidget*)it1.current();
        w = QMAX( w, wid->sizeHint().width() );
      }
    }
    for ( QObjectListIt it2( *labels ); it2.current(); ++it2 )
    {
      if ( it2.current()->isWidgetType() )
      {
        QWidget* wid = (QWidget*)it2.current();
        wid->setMinimumWidth( w );
      }
    }
    delete labels;
  }
}

/*!
  Constructor
*/
QtxListResourceEdit::Group::Group( const QString& title, QtxResourceEdit* edit, Item* pItem, QWidget* parent )
: QGroupBox( 2, Qt::Horizontal, title, parent ),
Item( edit, pItem )
{
}

/*!
  Destructor
*/
QtxListResourceEdit::Group::~Group()
{
}

/*!
  \return group type
*/
int QtxListResourceEdit::Group::type() const
{
  return -1;
}

/*!
  Default empty implementation of resources storing
*/
void QtxListResourceEdit::Group::store()
{
}

/*!
  Default empty implementation of resources retrieving
*/
void QtxListResourceEdit::Group::retrieve()
{
}

/*!
  \return value of property
  \param prop - property name
*/
QVariant QtxListResourceEdit::Group::property( const QString& prop ) const
{
  QVariant var;
  if ( prop == "columns" )
    var = QVariant( columns() );
  else if ( prop == "orientation" )
    var = QVariant( orientation() );
  else if ( prop == "frame" )
    var = QVariant( frameStyle() != QFrame::NoFrame );
  return var;
}

/*!
  Sets property value
  \param name - name of property
  \param var - value of property
*/
void QtxListResourceEdit::Group::setProperty( const QString& name, const QVariant& var )
{
  QVariant prop = var;
  if ( !prop.cast( QVariant::Int ) )
    return;

  if ( name == QString( "columns" ) && prop.cast( QVariant::Int ) && prop.toInt() > 0 )
    setColumns( prop.toInt() );
  else if ( name == QString( "orientation" ) && prop.cast( QVariant::Int ) )
  {
    int o = prop.toInt();
    if ( o == Qt::Horizontal || o == Qt::Vertical )
      setOrientation( (Orientation)o );
  }
  else if ( name == "frame" && prop.cast( QVariant::Bool ) )
  {
    setInsideMargin( prop.toBool() ? 5 : 0 );
    QGroupBox::setTitle( prop.toBool() ? Item::title() : QString::null );
    setFrameStyle( prop.toBool() ? QFrame::Box | QFrame::Sunken : QFrame::NoFrame );
  }
}

/*!
  Sets title of group
  \param title - new title of group
*/
void QtxListResourceEdit::Group::setTitle( const QString& title )
{
  Item::setTitle( title );
  QGroupBox::setTitle( title );
}

/*!
  Creates new item
  \param title - title of new item
  \type - type of new item
*/
QtxResourceEdit::Item* QtxListResourceEdit::Group::createItem( const QString& title, const int type )
{
  Item* item = 0;

  switch ( type )
  {
  case Color:
    item = new ColorItem( title, resourceEdit(), this, this );
    break;
  case Bool:
    item = new StateItem( title, resourceEdit(), this, this );
    break;
  case String:
    item = new StringItem( title, resourceEdit(), this, this );
    break;
  case Selector:
    item = new SelectItem( title, resourceEdit(), this, this );
    break;
  case DblSpin:
    item = new DoubleSpinItem( title, resourceEdit(), this, this );
    break;
  case IntSpin:
    item = new IntegerSpinItem( title, resourceEdit(), this, this );
    break;
  case Double:
    item = new DoubleEditItem( title, resourceEdit(), this, this );
    break;
  case Integer:
    item = new IntegerEditItem( title, resourceEdit(), this, this );
    break;
  case Space:
    item = new Spacer( resourceEdit(), this, this );
    break;
  case GroupBox:
    item = new Group( title, resourceEdit(), this, this );
    break;
  case Font:
    item = new FontItem( title, resourceEdit(), this, this );
    break;
  case DirList:
    item = new DirListItem( title, resourceEdit(), this, this );
    break;
  case File:
    item = new FileItem( title, resourceEdit(), this, this );
    break;
  }

  return item;
}

/*!
  Constructor
*/
QtxListResourceEdit::PrefItem::PrefItem( const int type, QtxResourceEdit* edit, Item* pi, QWidget* parent )
: QHBox( parent ),
Item( edit, pi ),
myType( type )
{
  setSpacing( 5 );
}

/*!
  Destructor
*/
QtxListResourceEdit::PrefItem::~PrefItem()
{
}

/*!
  \return preference item type
*/
int QtxListResourceEdit::PrefItem::type() const
{
  return myType;
}

/*!
  Doesn't create item, \return 0 by default
*/
QtxResourceEdit::Item* QtxListResourceEdit::PrefItem::createItem( const QString&, const int )
{
  return 0;
}

/*!
  Constructor
*/
QtxListResourceEdit::Spacer::Spacer( QtxResourceEdit* edit, Item* pItem, QWidget* parent )
: PrefItem( Space, edit, pItem, parent )
{
  setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
}

/*!
  Destructor
*/
QtxListResourceEdit::Spacer::~Spacer()
{
}

/*!
  Default empty implementation of resources storing
*/
void QtxListResourceEdit::Spacer::store()
{
}

/*!
  Default empty implementation of resources retrieving
*/
void QtxListResourceEdit::Spacer::retrieve()
{
}

/*!
  Constructor
*/
QtxListResourceEdit::SelectItem::SelectItem( const QString& title, QtxResourceEdit* edit,
					     Item* pItem, QWidget* parent )
: PrefItem( Selector, edit, pItem, parent )
{
  new QLabel( title, this );
  myList = new QComboBox( false, this );
  myList->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
}

/*!
  Destructor
*/
QtxListResourceEdit::SelectItem::~SelectItem()
{
}

/*!
  Stores value to resource manager
*/
void QtxListResourceEdit::SelectItem::store()
{
  int idx = myList->currentItem();
  if ( myIndex.contains( idx ) )
    setInteger( myIndex[idx] );
}

/*!
  Retrieve value to resource manager
*/
void QtxListResourceEdit::SelectItem::retrieve()
{
  int id = getInteger( -1 );

  int idx = -1;
  for ( QMap<int, int>::ConstIterator it = myIndex.begin(); it != myIndex.end() && idx == -1; ++it )
  {
    if ( it.data() == id )
      idx = it.key();
  }

  myList->setCurrentItem( idx );
}

/*!
  \return value of property
  \param prop - property name
*/
QVariant QtxListResourceEdit::SelectItem::property( const QString& name ) const
{
  QVariant val;
  if ( name == QString( "strings" ) )
  {
    QStringList lst;
    for ( int i = 0; i < (int)myList->count(); i++ )
      lst.append( myList->text( i ) );
    val = QVariant( lst );
  }
  else if ( name == QString( "indexes" ) )
  {
    QValueList<QVariant> lst;
    for ( int i = 0; i < (int)myList->count(); i++ )
      lst.append( myIndex.contains( i ) ? myIndex[i] : 0 );
    val = QVariant( lst );
  }
  return val;
}

/*!
  Sets property value
  \param name - name of property
  \param var - value of property
*/
void QtxListResourceEdit::SelectItem::setProperty( const QString& name, const QVariant& val )
{
  if ( name == QString( "strings" ) )
    setStrings( val );
  else if ( name == QString( "indexes" ) )
    setIndexes( val );
}

/*!
  Sets property "strings" - items for selection in combo box
  \param var - must be string list: list of items
*/
void QtxListResourceEdit::SelectItem::setStrings( const QVariant& var )
{
  if ( var.type() != QVariant::StringList )
    return;

  setStrings( var.toStringList() );
}

/*!
  Sets property "indexes" - corresponding indices of items in combo box
  \param var - must be list of integer variants: list of indices
*/
void QtxListResourceEdit::SelectItem::setIndexes( const QVariant& var )
{
  if ( var.type() != QVariant::List )
    return;

  QValueList<QVariant> varList = var.toList();
  QValueList<int> lst;
  for ( QValueList<QVariant>::const_iterator it = varList.begin(); it != varList.end(); ++it )
  {
    if ( (*it).canCast( QVariant::Int ) )
      lst.append( (*it).toInt() );
  }
  setIndexes( lst );
}

/*!
  Sets property "strings" - items for selection in combo box
  \param lst - list of items
*/
void QtxListResourceEdit::SelectItem::setStrings( const QStringList& lst )
{
  myList->clear();
  myList->insertStringList( lst );
}

/*!
  Sets property "indexes" - corresponding indices of items in combo box
  \param var - list of indices
*/
void QtxListResourceEdit::SelectItem::setIndexes( const QValueList<int>& lst )
{
  myIndex.clear();

  int idx = 0;
  for ( QValueList<int>::const_iterator it = lst.begin(); it != lst.end(); ++it, idx++ )
    myIndex.insert( idx, *it );
}

/*!
  Constructor
*/
QtxListResourceEdit::StateItem::StateItem( const QString& title, QtxResourceEdit* edit,
                                           Item* pItem, QWidget* parent )
: PrefItem( Bool, edit, pItem, parent )
{
  myState = new QCheckBox( title, this );
  myState->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
}

/*!
  Destructor
*/
QtxListResourceEdit::StateItem::~StateItem()
{
}

/*!
  Stores value to resource manager
*/
void QtxListResourceEdit::StateItem::store()
{
  setBoolean( myState->isChecked() );
}

/*!
  Retrieve value to resource manager
*/
void QtxListResourceEdit::StateItem::retrieve()
{
  myState->setChecked( getBoolean() );
}

/*!
  Constructor
*/
QtxListResourceEdit::StringItem::StringItem( const QString& title, QtxResourceEdit* edit,
                                             Item* pItem, QWidget* parent )
: PrefItem( String, edit, pItem, parent )
{
  new QLabel( title, this );
  myString = new QLineEdit( this );
  myString->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
}

/*!
  Destructor
*/
QtxListResourceEdit::StringItem::~StringItem()
{
}

/*!
  Stores value to resource manager
*/
void QtxListResourceEdit::StringItem::store()
{
  setString( myString->text() );
}

/*!
  Retrieve value to resource manager
*/
void QtxListResourceEdit::StringItem::retrieve()
{
  myString->setText( getString() );
}

/*!
  Constructor
*/
QtxListResourceEdit::IntegerEditItem::IntegerEditItem( const QString& title, QtxResourceEdit* edit, Item* pItem, QWidget* parent )
: PrefItem( Integer, edit, pItem, parent )
{
  new QLabel( title, this );
  myInteger = new QLineEdit( this );
  myInteger->setValidator( new QIntValidator( myInteger ) );
  myInteger->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
}

/*!
  Destructor
*/
QtxListResourceEdit::IntegerEditItem::~IntegerEditItem()
{
}

/*!
  Stores value to resource manager
*/
void QtxListResourceEdit::IntegerEditItem::store()
{
  setString( myInteger->text() );
}

/*!
  Retrieve value to resource manager
*/
void QtxListResourceEdit::IntegerEditItem::retrieve()
{
  myInteger->setText( getString() );
}

/*!
  Constructor
*/
QtxListResourceEdit::IntegerSpinItem::IntegerSpinItem( const QString& title, QtxResourceEdit* edit, Item* pItem, QWidget* parent )
: PrefItem( IntSpin, edit, pItem, parent )
{
  new QLabel( title, this );
  myInteger = new QtxIntSpinBox( this );
  myInteger->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
}

/*!
  Destructor
*/
QtxListResourceEdit::IntegerSpinItem::~IntegerSpinItem()
{
}

/*!
  Stores value to resource manager
*/
void QtxListResourceEdit::IntegerSpinItem::store()
{
  setInteger( myInteger->value() );
}

/*!
  Retrieve value to resource manager
*/
void QtxListResourceEdit::IntegerSpinItem::retrieve()
{
  myInteger->setValue( getInteger() );
}

/*!
  \return value of property
  \param prop - property name
*/
QVariant QtxListResourceEdit::IntegerSpinItem::property( const QString& name ) const
{
  QVariant var;
  if ( name == QString( "minimum" ) || name == QString( "min" ) )
    var = QVariant( myInteger->minValue() );
  else if ( name == QString( "maximum" ) || name == QString( "max" ) )
    var = QVariant( myInteger->maxValue() );
  else if ( name == QString( "step" ) )
    var = QVariant( myInteger->lineStep() );
  else if ( name == QString( "special" ) )
    var = QVariant( myInteger->specialValueText() );
  else if ( name == QString( "prefix" ) )
    var = QVariant( myInteger->prefix() );
  else if ( name == QString( "suffix" ) )
    var = QVariant( myInteger->suffix() );
  return var;
}

/*!
  Sets property value
  \param name - name of property
  \param var - value of property
*/
void QtxListResourceEdit::IntegerSpinItem::setProperty( const QString& name, const QVariant& var )
{
  QVariant prop = var;

  if ( ( name == QString( "minimum" ) || name == QString( "min" ) ) && prop.cast( QVariant::Int ) )
    myInteger->setMinValue( prop.toInt() );
  else if ( ( name == QString( "maximum" ) || name == QString( "max" ) ) && prop.cast( QVariant::Int ) )
    myInteger->setMaxValue( prop.toInt() );
  else if ( name == QString( "step" ) && prop.cast( QVariant::Int ) && prop.toInt() > 0 )
    myInteger->setLineStep( prop.toInt() );
  else if ( name == QString( "special" ) && prop.cast( QVariant::String ) )
    myInteger->setSpecialValueText( prop.toString() );
  else if ( name == QString( "prefix" ) && prop.cast( QVariant::String ) )
    myInteger->setPrefix( prop.toString() );
  else if ( name == QString( "suffix" ) && prop.cast( QVariant::String ) )
    myInteger->setSuffix( prop.toString() );
}

/*!
  Constructor
*/
QtxListResourceEdit::DoubleEditItem::DoubleEditItem( const QString& title, QtxResourceEdit* edit,
                                                     Item* pItem, QWidget* parent )
: PrefItem( Double, edit, pItem, parent )
{
  new QLabel( title, this );
  myDouble = new QLineEdit( this );
  myDouble->setValidator( new QDoubleValidator( myDouble ) );
  myDouble->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
}

/*!
  Destructor
*/
QtxListResourceEdit::DoubleEditItem::~DoubleEditItem()
{
}

/*!
  Stores value to resource manager
*/
void QtxListResourceEdit::DoubleEditItem::store()
{
  setString( myDouble->text() );
}

/*!
  Retrieve value to resource manager
*/
void QtxListResourceEdit::DoubleEditItem::retrieve()
{
  myDouble->setText( getString() );
}

/*!
  Constructor
*/
QtxListResourceEdit::DoubleSpinItem::DoubleSpinItem( const QString& title, QtxResourceEdit* edit,
                                                     Item* pItem, QWidget* parent )
: PrefItem( DblSpin, edit, pItem, parent )
{
  new QLabel( title, this );
  myDouble = new QtxDblSpinBox( this );
  myDouble->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
}

/*!
  Destructor
*/
QtxListResourceEdit::DoubleSpinItem::~DoubleSpinItem()
{
}

/*!
  Stores value to resource manager
*/
void QtxListResourceEdit::DoubleSpinItem::store()
{
  setDouble( myDouble->value() );
}

/*!
  Retrieve value to resource manager
*/
void QtxListResourceEdit::DoubleSpinItem::retrieve()
{
  myDouble->setValue( getDouble() );
}

/*!
  \return value of property
  \param prop - property name
*/
QVariant QtxListResourceEdit::DoubleSpinItem::property( const QString& name ) const
{
  QVariant var;
  if ( name == QString( "minimum" ) || name == QString( "min" ) )
    var = QVariant( myDouble->minValue() );
  else if ( name == QString( "maximum" ) || name == QString( "max" ) )
    var = QVariant( myDouble->maxValue() );
  else if ( name == QString( "precision" ) )
    var = QVariant( myDouble->precision() );
  else if ( name == QString( "step" ) )
    var = QVariant( myDouble->lineStep() );
  else if ( name == QString( "special" ) )
    var = QVariant( myDouble->specialValueText() );
  else if ( name == QString( "prefix" ) )
    var = QVariant( myDouble->prefix() );
  else if ( name == QString( "suffix" ) )
    var = QVariant( myDouble->suffix() );
  return var;
}

/*!
  Sets property value
  \param name - name of property
  \param var - value of property
*/
void QtxListResourceEdit::DoubleSpinItem::setProperty( const QString& name, const QVariant& var )
{
  QVariant prop = var;

  if ( ( name == QString( "minimum" ) || name == QString( "min" ) ) && prop.cast( QVariant::Double ) )
    myDouble->setMinValue( prop.toDouble() );
  else if ( ( name == QString( "maximum" ) || name == QString( "max" ) ) && prop.cast( QVariant::Double ) )
    myDouble->setMaxValue( prop.toDouble() );
  else if ( name == QString( "step" ) && prop.cast( QVariant::Double ) && prop.toDouble() > 0 )
    myDouble->setLineStep( prop.toDouble() );
  else if ( name == QString( "precision" ) && prop.cast( QVariant::Int ) && prop.toInt() > 0 )
    myDouble->setPrecision( prop.toInt() );
  else if ( name == QString( "special" ) && prop.cast( QVariant::String ) )
    myDouble->setSpecialValueText( prop.toString() );
  else if ( name == QString( "prefix" ) && prop.cast( QVariant::String ) )
    myDouble->setPrefix( prop.toString() );
  else if ( name == QString( "suffix" ) && prop.cast( QVariant::String ) )
    myDouble->setSuffix( prop.toString() );
}

/*!
  Constructor
*/
QtxListResourceEdit::ColorItem::ColorItem( const QString& title, QtxResourceEdit* edit,
                                           Item* pItem, QWidget* parent )
: PrefItem( Color, edit, pItem, parent )
{
  /*!
    \class QtxListResourceEdit::ColorItem::ColorSelector
    \brief Label, showing color and allowing to pick color with help of standard color dialog
  */
  class ColorSelector : public QLabel
  {
  public:
    ColorSelector( QWidget* parent = 0 ) : QLabel( parent )
    {
      setFrameStyle( WinPanel | Raised );
    }
    virtual ~ColorSelector() {}
    virtual QSize minimumSizeHint() const
    {
      return QLabel::minimumSizeHint() + QSize( 0, 2 );
    }

  protected:
    virtual void mousePressEvent( QMouseEvent* e )
    {
      if ( e->button() == LeftButton )
      {
        setFrameStyle( WinPanel | Sunken );
        QColor c = QColorDialog::getColor( paletteBackgroundColor(), this );
        if ( c.isValid() )
          setPaletteBackgroundColor( c );

        setFrameStyle( WinPanel | Raised );
      }
    }
  };

  new QLabel( title, this );
  myColor = new ColorSelector( this );
  myColor->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
}

/*!
  Destructor
*/
QtxListResourceEdit::ColorItem::~ColorItem()
{
}

/*!
  Stores value to resource manager
*/
void QtxListResourceEdit::ColorItem::store()
{
  setColor( myColor->paletteBackgroundColor() );
}

/*!
  Retrieve value to resource manager
*/
void QtxListResourceEdit::ColorItem::retrieve()
{
  myColor->setPaletteBackgroundColor( getColor() );
}


/*!
  Constructor
*/
QtxListResourceEdit::FontItem::FontItem( const QString& title, QtxResourceEdit* edit,
                                         Item* pItem, QWidget* parent )
: PrefItem( Font, edit, pItem, parent )
{
  new QLabel( title, this );
  myFamilies = new QtxComboBox( false, this );
  mySizes = new QtxComboBox( true, this );
  mySizes->setInsertionPolicy( QComboBox::NoInsertion );
  myBold = new QCheckBox( tr( "Bold" ), this );
  myItalic = new QCheckBox( tr( "Italic" ), this );
  myUnderline = new QCheckBox( tr( "Underline" ), this );
  myPreview = new QToolButton( this );
  myPreview->setText( "..." );

  myFamilies->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
  mySizes->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );

  connect( myFamilies, SIGNAL( activated( int ) ), this, SLOT( onActivateFamily( int ) ) );
  connect( myPreview, SIGNAL( clicked() ), this, SLOT( onPreview() ) );

  setProperty( "system", ( bool )true );
  setProperty( "widget_flags", ( int )All );
}

/*!
  Destructor
*/
QtxListResourceEdit::FontItem::~FontItem()
{
}

/*!
  Stores value to resource manager
*/
void QtxListResourceEdit::FontItem::store()
{
  QFont f( family(), size() );
  bool bold, italic, underline;
  params( bold, italic, underline );
  f.setBold( bold );
  f.setItalic( italic );
  f.setUnderline( underline );
  Item::setFont( f );
}

/*!
  Retrieve value to resource manager
*/
void QtxListResourceEdit::FontItem::retrieve()
{
  QFont f = getFont();
  setFamily( f.family() );
  setSize( f.pointSize() );
  setParams( f.bold(), f.italic(), f.underline() );
}

/*!
  \return value of property
  \param prop - property name
*/
QVariant QtxListResourceEdit::FontItem::property( const QString& name ) const
{
  if( name=="system" )
    return myIsSystem;

  else if( name=="widget_flags" )
    return ( int )myFlags;
  
  if( myIsSystem )
  {
    if( name=="families" )
    {
      QFontDatabase fdb;
      return fdb.families();
    }

    else if( name=="default_family" )
    {
      QFontDatabase fdb;
      QStringList fam = fdb.families();
      if( fam.count()>0 )
        return fam.first();
      else
        return QString::null;
    }

    else
    {
      QStringList parts = QStringList::split( ":", name );
      if( parts.count()==2 )
      {
        if( parts[1]=="default_bold" || parts[1]=="default_italic" || parts[1]=="default_underline" )
          return false;

        else if( parts[1]=="sizes" )
        {
          QFontDatabase fdb;
          QValueList<int> sizes = fdb.pointSizes( parts[0] );
          QValueList<QVariant> vsizes;
          QValueList<int>::const_iterator anIt = sizes.begin(),
                                          aLast = sizes.end();
          for( ; anIt!=aLast; anIt++ )
            vsizes.append( *anIt );

          return vsizes;
        }

        else if( parts[1]=="default_size" )
        {
          if( parts[0].isEmpty() )
            return 0;
            
          QFontDatabase fdb;
          QValueList<int> sizes = fdb.pointSizes( parts[0] );
          if( sizes.count()>0 )
            return sizes.first();
          else
            return 0;
        }
      }
    }
  }

  else if( myProperties.contains( name ) )
    return myProperties[ name ];

  return QVariant();
}

/*!
  Sets property value
  \param name - name of property
  \param var - value of property
*/
void QtxListResourceEdit::FontItem::setProperty( const QString& name, const QVariant& value )
{
  if( name=="system" )
  {
    if( !value.canCast( QVariant::Bool ) )
      return;

    bool isSystem = value.toBool();
    if( myIsSystem==isSystem )
      return;

    myIsSystem = isSystem;

    QVariant families = property( "families" );
    QString fam = family();

    myFamilies->clear();
    if( families.canCast( QVariant::StringList ) )
    {
      QStringList list = families.toStringList();
      myFamilies->insertStringList( list );
    }

    setFamily( fam );
    setSize( -1 ); //set default size
  }
  
  else if( name=="widget_flags" )
  {
    if( !value.canCast( QVariant::Int ) )
      return;

    int wf = value.toInt();
    
    myFlags = wf;
    myFamilies ->setShown( wf & Family );
    mySizes    ->setShown( wf & Size );
    mySizes->lineEdit()->setReadOnly( ( wf & UserSize )==0 );
    myBold     ->setShown( wf & Bold );
    myItalic   ->setShown( wf & Italic );
    myUnderline->setShown( wf & Underline );
    bool isSystem = property( "system" ).canCast( QVariant::Bool ) ? property( "system" ).toBool() : false;
    myPreview->setShown( ( wf & Preview ) && isSystem );

    internalUpdate();
  }
  
  else
    myProperties[ name ] = value;
}

/*!
  Sets family of font
  \param f - new family
*/
void QtxListResourceEdit::FontItem::setFamily( const QString& f )
{
  QString curtext;
  if( myFamilies->isShown() )
  {
    if( myFamilies->listBox()->findItem( f, Qt::ExactMatch ) )
      curtext = f;
  }
  else
  {
    QVariant deffam = property( "default_family" );
    if( deffam.canCast( QVariant::String ) )
      curtext = deffam.toString();
  }
  
  if ( curtext.isEmpty() )
    curtext = (QApplication::font()).family();

  int idx = -1;
  for ( int i = 0; i < (int)myFamilies->count() && idx < 0; i++ )
  {
    if ( myFamilies->text( i ) == curtext )
      idx = i;
  }

  if ( idx >= 0 )
    myFamilies->setCurrentItem( idx );

  onActivateFamily( idx );  
}

/*!
  \return family of font
*/
QString QtxListResourceEdit::FontItem::family() const
{
  return myFamilies->currentText();
}

/*!
  Sets size of font
  \param s - new size of font
*/
void QtxListResourceEdit::FontItem::setSize( const int s )
{
  int cursize = -1;
  if( mySizes->isShown() && s>0 )
  {
    if( ( myFlags & UserSize ) || mySizes->listBox()->findItem( QString( "%1" ).arg( s ), Qt::ExactMatch ) )
      cursize = s;
  }
  else
  {
    QVariant defsize = property( QString( "%1:default_size" ).arg( family() ) );
    if( defsize.canCast( QVariant::Int ) )
      cursize = defsize.toInt();
  }

  mySizes->setCurrentText( cursize>0 ? QString( "%1" ).arg( cursize ) : "" );
}

/*!
  \return size of font
*/
int QtxListResourceEdit::FontItem::size() const
{
  QString s = mySizes->currentText();
  bool ok;
  int pSize = s.toInt( &ok );
  return ( ok ? pSize : 0 );
}

/*!
  Sets font parameters
  \param bold - is font bold
  \param italic - is font italic
  \param underline - is font underlined
*/
void QtxListResourceEdit::FontItem::setParams( const bool bold, const bool italic, const bool underline )
{
  bool curbold = false, curitalic = false, curunderline = false;
  if( myBold->isShown() )
    curbold = bold;
  else
  {
    QVariant def = property( QString( "%1:default_bold" ).arg( family() ) );
    if( def.canCast( QVariant::Bool ) )
      curbold = def.toBool();
  }
  if( myItalic->isShown() )
    curitalic = italic;
  else
  {
    QVariant def = property( QString( "%1:default_italic" ).arg( family() ) );
    if( def.canCast( QVariant::Bool ) )
      curitalic = def.toBool();
  }
  if( myUnderline->isShown() )
    curunderline = underline;
  else
  {
    QVariant def = property( QString( "%1:default_underline" ).arg( family() ) );
    if( def.canCast( QVariant::Bool ) )
      curunderline = def.toBool();
  }
  myBold->setChecked( curbold );
  myItalic->setChecked( curitalic );
  myUnderline->setChecked( curunderline );
}

/*!
  \return font parameters
  \param bold - is font bold
  \param italic - is font italic
  \param underline - is font underlined
*/
void QtxListResourceEdit::FontItem::params( bool& bold, bool& italic, bool& underline )
{
  bold = myBold->isChecked();
  italic = myItalic->isChecked();
  underline = myUnderline->isChecked();
}

/*!
  Updates internal selection of font properties
*/
void QtxListResourceEdit::FontItem::internalUpdate()
{
  setFamily( family() );
  setSize( size() );
  bool b1, b2, b3;
  params( b1, b2, b3 );
  setParams( b1, b2, b3 );
}

/*!
  SLOT: called if family is activated, updates list of possible sizes
*/
void QtxListResourceEdit::FontItem::onActivateFamily( int )
{
  QVariant sizes = property( QString( "%1:sizes" ).arg( family() ) );

  int s = size();
  mySizes->clear();
  if( sizes.canCast( QVariant::List ) )
  {
    QValueList<QVariant> list = sizes.toList();
    QStringList sizeItems;
    QValueList<QVariant>::const_iterator anIt = list.begin(),
                                         aLast = list.end();
    for( ; anIt!=aLast; anIt++ )
      if( (*anIt).canCast( QVariant::Int ) && (*anIt).toInt()>0 )
        sizeItems.append( QString( "%1" ).arg( (*anIt).toInt() ) );
    mySizes->insertStringList( sizeItems );
  }
  setSize( s );
}

/*!
  SLOT: called if it is necessary to show font preview
*/
void QtxListResourceEdit::FontItem::onPreview()
{
  QFont f( family(), size() );
  bool bold, italic, underline;
  params( bold, italic, underline );
  f.setBold( bold );
  f.setItalic( italic );
  f.setUnderline( underline );

  bool ok;
  f = QFontDialog::getFont( &ok, f );

  if( ok )
  {
    setFamily( f.family() );
    setSize( f.pointSize() );
    setParams( f.bold(), f.italic(), f.underline() );
  }
}



/*!
  Constructor
*/
QtxListResourceEdit::DirListItem::DirListItem( const QString& title, QtxResourceEdit* edit, Item* pItem, QWidget* parent )
: PrefItem( Font, edit, pItem, parent )
{
  myDirListEditor = new QtxDirListEditor( this ); 
}

/*!
  Destructor
*/
QtxListResourceEdit::DirListItem::~DirListItem()
{
}

/*!
  Stores value to resource manager
*/
void QtxListResourceEdit::DirListItem::store()
{
  QStringList list;
  myDirListEditor->getPathList(list);
  setString( QString(list.join(";")) );
}

/*!
  Retrieve value to resource manager
*/
void QtxListResourceEdit::DirListItem::retrieve()
{
  myDirListEditor->setPathList(QStringList::split(";", getString()));
}



/*!
  Constructor
*/
QtxListResourceEdit::FileItem::FileItem( const QString& title, QtxResourceEdit* edit,
                                         Item* pItem, QWidget* parent )
: PrefItem( Font, edit, pItem, parent ),
  myFlags( QFileInfo::ReadUser ),
  myIsExisting( true ),
  myIsReadOnly ( true ),
  myFileDlg( 0 )
{
  new QLabel( title, this );
  myFile = new QLineEdit( this );
  myFile->setValidator( new FileValidator( this, myFile ) );
  myFile->setReadOnly( myIsReadOnly );
  myOpenFile = new QToolButton( this );
  myOpenFile->setText( "..." );
  connect( myOpenFile, SIGNAL( clicked() ), this, SLOT( onOpenFile() ) );
}

/*!
  Destructor
*/
QtxListResourceEdit::FileItem::~FileItem()
{
  if( myFileDlg ) 
    delete myFileDlg;
}

/*!
  Stores value to resource manager
*/
void QtxListResourceEdit::FileItem::store()
{
  setString( myFile->text() );
}

/*!
  Retrieve value to resource manager
*/
void QtxListResourceEdit::FileItem::retrieve()
{
  myFile->setText( getString() );
}

/*!
  \return value of property
  \param prop - property name
*/
QVariant QtxListResourceEdit::FileItem::property( const QString& name ) const
{
  if( name=="filter" )
    return myFilter;
  else if( name=="existing" )
    return myIsExisting;
  else if( name=="flags" )
    return myFlags;
  else if( name=="readOnly")
    return myIsReadOnly;

  return QVariant();
}

/*!
  Sets property value
  \param name - name of property
  \param var - value of property
*/
void QtxListResourceEdit::FileItem::setProperty( const QString& name, const QVariant& value )
{
  if( name=="filter" )
  {
    if( value.canCast( QVariant::String ) )
    {
      myFilter.clear();
      myFilter.append( value.toString() );
    }
    else if( value.canCast( QVariant::StringList ) )
      myFilter = value.toStringList();
  }
  else if( name=="existing" && value.canCast( QVariant::Bool ) )
    myIsExisting = value.toBool();

  else if( name=="flags" && value.canCast( QVariant::UInt ) )
    myFlags = value.toUInt();

  else if( name=="readOnly" && value.canCast( QVariant::Bool) ) {
    myIsReadOnly = value.toBool();
    myFile->setReadOnly( myIsReadOnly );
  }
}

/*!
  SLOT: called if user click "Open File" button, shows dialog 
*/
void QtxListResourceEdit::FileItem::onOpenFile()
{
  if( !myFileDlg )
  {
    myFileDlg = new QFileDialog( "." );
    connect( myFileDlg, SIGNAL( fileHighlighted( const QString& ) ), this, SLOT( onFileSelected( const QString& ) ) );
  }
  
  myFileDlg->setCaption( title() );
  myFileDlg->setFilters( myFilter );
  myFileDlg->setMode( myIsExisting ? QFileDialog::ExistingFile : QFileDialog::AnyFile );

  if( myFileDlg->exec()==QDialog::Accepted )
  {
    myFile->setText( myFileDlg->selectedFile() ); 
  }
}

/*!
  \return true if file satisfies permissions
  \param f - file name
*/
bool QtxListResourceEdit::FileItem::isFileCorrect( const QString& f ) const
{
  bool res = false;
  QFileInfo info( f );
  if( !myIsExisting || info.exists() )
    res = info.isFile() && info.permission( myFlags );

  return res;
}

/*!
  SLOT: called if user has selected file in file dialog, checks file permissions and passes it's name to widget
  \param f - file name
*/
void QtxListResourceEdit::FileItem::onFileSelected( const QString& f )
{
  if( myFileDlg && !isFileCorrect( f ) )
    myFileDlg->setSelection( "" );
}


/*!
  Constructor
*/
QtxListResourceEdit::FileItem::FileValidator::FileValidator( FileItem* item, QObject* parent )
: QValidator( parent ),
  myItem( item )
{
}

/*!
  Destructor
*/
QtxListResourceEdit::FileItem::FileValidator::~FileValidator()
{
}

/*!
  Check file permissions
  \param f - file name
*/
QValidator::State QtxListResourceEdit::FileItem::FileValidator::validate( QString& f, int& ) const
{
  if( myItem && myItem->isFileCorrect( f ) )
    return QValidator::Acceptable;
  else
    return QValidator::Intermediate;
}
