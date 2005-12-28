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
// See http://www.salome-platform.org/
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

#include "QtxIntSpinBox.h"
#include "QtxDblSpinBox.h"
#include "QtxComboBox.h"
#include "QtxDirListEditor.h"

/*
  Class: QtxListResourceEdit
  Descr: GUI implementation of QtxResourceEdit - manager of resources
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

QtxListResourceEdit::~QtxListResourceEdit()
{
}

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

void QtxListResourceEdit::itemAdded( QtxResourceEdit::Item* i )
{
  if ( !i )
    return;

  QPtrList<Item> items;
  childItems( items );

  if ( items.contains( i ) || items.contains( i->parentItem() ) )
    updateVisible();
}

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

void QtxListResourceEdit::updateState()
{
  if ( myList->selectedItem() &&  myStack->visibleWidget() )
    myStack->show();
  else
    myStack->hide();
}

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

/*
  Class: QtxListResourceEdit::Category
  Descr: GUI implementation of preferences category.
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

QtxListResourceEdit::Category::~Category()
{
}

bool QtxListResourceEdit::Category::isEmpty() const
{
  return Item::isEmpty() && myInfo->text().isEmpty();
}

int QtxListResourceEdit::Category::type() const
{
  return -1;
}

void QtxListResourceEdit::Category::store()
{
}

void QtxListResourceEdit::Category::retrieve()
{
}

QVariant QtxListResourceEdit::Category::property( const QString& prop ) const
{
  QVariant var;
  if ( prop == QString( "information" ) || prop == QString( "info" ) )
    var = myInfo->text();
  return var;
}

void QtxListResourceEdit::Category::setProperty( const QString& name, const QVariant& var )
{
  QVariant prop = var;
  if ( !prop.cast( QVariant::String ) )
    return;

  if ( name == QString( "information" ) || name == QString( "info" ) )
    myInfo->setText( prop.toString() );

  updateState();
}

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

/*
  Class: QtxListResourceEdit::Tab
  Descr: GUI implementation of resources tab.
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

QtxListResourceEdit::Tab::~Tab()
{
}

int QtxListResourceEdit::Tab::type() const
{
  return -1;
}

void QtxListResourceEdit::Tab::store()
{
}

void QtxListResourceEdit::Tab::retrieve()
{
}

void QtxListResourceEdit::Tab::polish()
{
  QFrame::polish();

  adjustLabels();
}

QtxResourceEdit::Item* QtxListResourceEdit::Tab::createItem( const QString& title, const int )
{
  Item* i = item( title, id() );
  if ( i )
    return i;

  Group* group = new Group( title, resourceEdit(), this, myMainFrame );

  return group;
}

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

/*
  Class: QtxListResourceEdit::Group
  Descr: GUI implementation of resources group.
*/

QtxListResourceEdit::Group::Group( const QString& title, QtxResourceEdit* edit, Item* pItem, QWidget* parent )
: QGroupBox( 2, Qt::Horizontal, title, parent ),
Item( edit, pItem )
{
}

QtxListResourceEdit::Group::~Group()
{
}

int QtxListResourceEdit::Group::type() const
{
  return -1;
}

void QtxListResourceEdit::Group::store()
{
}

void QtxListResourceEdit::Group::retrieve()
{
}

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

void QtxListResourceEdit::Group::setTitle( const QString& title )
{
  Item::setTitle( title );
  QGroupBox::setTitle( title );
}

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

/*
  Class: QtxListResourceEdit::PrefItem
  Descr: Base class for preferences items.
*/

QtxListResourceEdit::PrefItem::PrefItem( const int type, QtxResourceEdit* edit, Item* pi, QWidget* parent )
: QHBox( parent ),
Item( edit, pi ),
myType( type )
{
  setSpacing( 5 );
}

QtxListResourceEdit::PrefItem::~PrefItem()
{
}

int QtxListResourceEdit::PrefItem::type() const
{
  return myType;
}

QtxResourceEdit::Item* QtxListResourceEdit::PrefItem::createItem( const QString&, const int )
{
  return 0;
}

/*
  Class: QtxListResourceEdit::Spacer
  Descr: GUI implementation of resources spacer.
*/

QtxListResourceEdit::Spacer::Spacer( QtxResourceEdit* edit, Item* pItem, QWidget* parent )
: PrefItem( Space, edit, pItem, parent )
{
  setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
}

QtxListResourceEdit::Spacer::~Spacer()
{
}

void QtxListResourceEdit::Spacer::store()
{
}

void QtxListResourceEdit::Spacer::retrieve()
{
}

/*
  Class: QtxListResourceEdit::SelectItem
  Descr: GUI implementation of resources list item.
*/

QtxListResourceEdit::SelectItem::SelectItem( const QString& title, QtxResourceEdit* edit,
					     Item* pItem, QWidget* parent )
: PrefItem( Selector, edit, pItem, parent )
{
  new QLabel( title, this );
  myList = new QComboBox( false, this );
  myList->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
}

QtxListResourceEdit::SelectItem::~SelectItem()
{
}

void QtxListResourceEdit::SelectItem::store()
{
  int idx = myList->currentItem();
  if ( myIndex.contains( idx ) )
    setInteger( myIndex[idx] );
}

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

void QtxListResourceEdit::SelectItem::setProperty( const QString& name, const QVariant& val )
{
  if ( name == QString( "strings" ) )
    setStrings( val );
  else if ( name == QString( "indexes" ) )
    setIndexes( val );
}

void QtxListResourceEdit::SelectItem::setStrings( const QVariant& var )
{
  if ( var.type() != QVariant::StringList )
    return;

  setStrings( var.toStringList() );
}

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

void QtxListResourceEdit::SelectItem::setStrings( const QStringList& lst )
{
  myList->clear();
  myList->insertStringList( lst );
}

void QtxListResourceEdit::SelectItem::setIndexes( const QValueList<int>& lst )
{
  myIndex.clear();

  int idx = 0;
  for ( QValueList<int>::const_iterator it = lst.begin(); it != lst.end(); ++it, idx++ )
    myIndex.insert( idx, *it );
}

/*
  Class: QtxListResourceEdit::StateItem
  Descr: GUI implementation of resources bool item.
*/

QtxListResourceEdit::StateItem::StateItem( const QString& title, QtxResourceEdit* edit,
                                           Item* pItem, QWidget* parent )
: PrefItem( Bool, edit, pItem, parent )
{
  myState = new QCheckBox( title, this );
  myState->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
}

QtxListResourceEdit::StateItem::~StateItem()
{
}

void QtxListResourceEdit::StateItem::store()
{
  setBoolean( myState->isChecked() );
}

void QtxListResourceEdit::StateItem::retrieve()
{
  myState->setChecked( getBoolean() );
}

/*
  Class: QtxListResourceEdit::StringItem
  Descr: GUI implementation of resources string item.
*/

QtxListResourceEdit::StringItem::StringItem( const QString& title, QtxResourceEdit* edit,
                                             Item* pItem, QWidget* parent )
: PrefItem( String, edit, pItem, parent )
{
  new QLabel( title, this );
  myString = new QLineEdit( this );
  myString->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
}

QtxListResourceEdit::StringItem::~StringItem()
{
}

void QtxListResourceEdit::StringItem::store()
{
  setString( myString->text() );
}

void QtxListResourceEdit::StringItem::retrieve()
{
  myString->setText( getString() );
}

/*
  Class: QtxListResourceEdit::IntegerEditItem
  Descr: GUI implementation of resources integer item.
*/

QtxListResourceEdit::IntegerEditItem::IntegerEditItem( const QString& title, QtxResourceEdit* edit, Item* pItem, QWidget* parent )
: PrefItem( Integer, edit, pItem, parent )
{
  new QLabel( title, this );
  myInteger = new QLineEdit( this );
  myInteger->setValidator( new QIntValidator( myInteger ) );
  myInteger->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
}

QtxListResourceEdit::IntegerEditItem::~IntegerEditItem()
{
}

void QtxListResourceEdit::IntegerEditItem::store()
{
  setString( myInteger->text() );
}

void QtxListResourceEdit::IntegerEditItem::retrieve()
{
  myInteger->setText( getString() );
}

/*
  Class: QtxListResourceEdit::IntegerSpinItem
  Descr: GUI implementation of resources integer item.
*/

QtxListResourceEdit::IntegerSpinItem::IntegerSpinItem( const QString& title, QtxResourceEdit* edit, Item* pItem, QWidget* parent )
: PrefItem( IntSpin, edit, pItem, parent )
{
  new QLabel( title, this );
  myInteger = new QtxIntSpinBox( this );
  myInteger->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
}

QtxListResourceEdit::IntegerSpinItem::~IntegerSpinItem()
{
}

void QtxListResourceEdit::IntegerSpinItem::store()
{
  setInteger( myInteger->value() );
}

void QtxListResourceEdit::IntegerSpinItem::retrieve()
{
  myInteger->setValue( getInteger() );
}

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

/*
  Class: QtxListResourceEdit::DoubleEditItem
  Descr: GUI implementation of resources string item.
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

QtxListResourceEdit::DoubleEditItem::~DoubleEditItem()
{
}

void QtxListResourceEdit::DoubleEditItem::store()
{
  setString( myDouble->text() );
}

void QtxListResourceEdit::DoubleEditItem::retrieve()
{
  myDouble->setText( getString() );
}

/*
  Class: QtxListResourceEdit::DoubleSpinItem
  Descr: GUI implementation of resources double item.
*/

QtxListResourceEdit::DoubleSpinItem::DoubleSpinItem( const QString& title, QtxResourceEdit* edit,
                                                     Item* pItem, QWidget* parent )
: PrefItem( DblSpin, edit, pItem, parent )
{
  new QLabel( title, this );
  myDouble = new QtxDblSpinBox( this );
  myDouble->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
}

QtxListResourceEdit::DoubleSpinItem::~DoubleSpinItem()
{
}

void QtxListResourceEdit::DoubleSpinItem::store()
{
  setDouble( myDouble->value() );
}

void QtxListResourceEdit::DoubleSpinItem::retrieve()
{
  myDouble->setValue( getDouble() );
}

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

/*
  Class: QtxListResourceEdit::ColorItem
  Descr: GUI implementation of resources color item.
*/

QtxListResourceEdit::ColorItem::ColorItem( const QString& title, QtxResourceEdit* edit,
                                           Item* pItem, QWidget* parent )
: PrefItem( Color, edit, pItem, parent )
{
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

QtxListResourceEdit::ColorItem::~ColorItem()
{
}

void QtxListResourceEdit::ColorItem::store()
{
  setColor( myColor->paletteBackgroundColor() );
}

void QtxListResourceEdit::ColorItem::retrieve()
{
  myColor->setPaletteBackgroundColor( getColor() );
}


/*
  Class: QtxListResourceEdit::FontItem
  Descr: GUI implementation of resources font item.
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

QtxListResourceEdit::FontItem::~FontItem()
{
}

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

void QtxListResourceEdit::FontItem::retrieve()
{
  QFont f = getFont();
  setFamily( f.family() );
  setSize( f.pointSize() );
  setParams( f.bold(), f.italic(), f.underline() );
}

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

QString QtxListResourceEdit::FontItem::family() const
{
  return myFamilies->currentText();
}

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

int QtxListResourceEdit::FontItem::size() const
{
  QString s = mySizes->currentText();
  bool ok;
  int pSize = s.toInt( &ok );
  return ( ok ? pSize : 0 );
}

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

void QtxListResourceEdit::FontItem::params( bool& bold, bool& italic, bool& underline )
{
  bold = myBold->isChecked();
  italic = myItalic->isChecked();
  underline = myUnderline->isChecked();
}

void QtxListResourceEdit::FontItem::internalUpdate()
{
  //update internal selection of font properties
  setFamily( family() );
  setSize( size() );
  bool b1, b2, b3;
  params( b1, b2, b3 );
  setParams( b1, b2, b3 );
}

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





/*
  Class: QtxListResourceEdit::DirListItem
  Descr: 
*/
QtxListResourceEdit::DirListItem::DirListItem( const QString& title, QtxResourceEdit* edit, Item* pItem, QWidget* parent )
: PrefItem( Font, edit, pItem, parent )
{
  myDirListEditor = new QtxDirListEditor( this ); 
}

QtxListResourceEdit::DirListItem::~DirListItem()
{
}

void QtxListResourceEdit::DirListItem::store()
{
  QStringList list;
  myDirListEditor->getPathList(list);
  setString( QString(list.join(";")) );
}

void QtxListResourceEdit::DirListItem::retrieve()
{
  myDirListEditor->setPathList(QStringList::split(";", getString()));
}



/*
  Class: QtxListResourceEdit::FileItem
  Descr: GUI implementation of resources file item.
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

QtxListResourceEdit::FileItem::~FileItem()
{
  if( myFileDlg ) 
    delete myFileDlg;
}

void QtxListResourceEdit::FileItem::store()
{
  setString( myFile->text() );
}

void QtxListResourceEdit::FileItem::retrieve()
{
  myFile->setText( getString() );
}

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

bool QtxListResourceEdit::FileItem::isFileCorrect( const QString& f ) const
{
  bool res = false;
  QFileInfo info( f );
  if( !myIsExisting || info.exists() )
    res = info.isFile() && info.permission( myFlags );

  return res;
}

void QtxListResourceEdit::FileItem::onFileSelected( const QString& f )
{
  if( myFileDlg && !isFileCorrect( f ) )
    myFileDlg->setSelection( "" );
}



QtxListResourceEdit::FileItem::FileValidator::FileValidator( FileItem* item, QObject* parent )
: QValidator( parent ),
  myItem( item )
{
}

QtxListResourceEdit::FileItem::FileValidator::~FileValidator()
{
}

QValidator::State QtxListResourceEdit::FileItem::FileValidator::validate( QString& f, int& ) const
{
  if( myItem && myItem->isFileCorrect( f ) )
    return QValidator::Acceptable;
  else
    return QValidator::Intermediate;
}
