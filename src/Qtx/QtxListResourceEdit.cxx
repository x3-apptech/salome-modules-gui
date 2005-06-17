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

#include "QtxIntSpinBox.h"
#include "QtxDblSpinBox.h"

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

void QtxListResourceEdit::onSelectionChanged()
{
  int idx = myList->index( myList->selectedItem() );
  if ( idx < 0 )
    return;

  myStack->raiseWidget( idx );
}

QtxResourceEdit::Item* QtxListResourceEdit::createItem( const QString& title, const int )
{
  Item* i = item( title, -1 );
  if ( i )
    return i;

  Category* category = new Category( this, myStack );
  myList->insertItem( title );
  int id = myList->count() - 1;
  myStack->addWidget( category, id );

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
  if ( myStack->visibleWidget() )
    myStack->show();
  else
    myStack->hide();
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
  return var;
}

void QtxListResourceEdit::Group::setProperty( const QString& name, const QVariant& var )
{
  QVariant prop = var;
  if ( !prop.cast( QVariant::Int ) )
    return;

  if ( name == QString( "columns" ) && prop.toInt() > 0 )
    setColumns( prop.toInt() );
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
    item = new ListItem( title, resourceEdit(), this, this );
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
  Class: QtxListResourceEdit::ListItem
  Descr: GUI implementation of resources list item.
*/

QtxListResourceEdit::ListItem::ListItem( const QString& title, QtxResourceEdit* edit,
                                         Item* pItem, QWidget* parent )
: PrefItem( Selector, edit, pItem, parent )
{
  new QLabel( title, this );
  myList = new QComboBox( false, this );
  myList->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
}

QtxListResourceEdit::ListItem::~ListItem()
{
}

void QtxListResourceEdit::ListItem::store()
{
  int idx = myList->currentItem();
  if ( myIndex.contains( idx ) )
    setInteger( myIndex[idx] );
}

void QtxListResourceEdit::ListItem::retrieve()
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

QVariant QtxListResourceEdit::ListItem::property( const QString& name ) const
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

void QtxListResourceEdit::ListItem::setProperty( const QString& name, const QVariant& val )
{
  if ( name == QString( "strings" ) )
    setStrings( val );
  else if ( name == QString( "indexes" ) )
    setIndexes( val );
}

void QtxListResourceEdit::ListItem::setStrings( const QVariant& var )
{
  if ( var.type() != QVariant::StringList )
    return;

  setStrings( var.toStringList() );
}

void QtxListResourceEdit::ListItem::setIndexes( const QVariant& var )
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

void QtxListResourceEdit::ListItem::setStrings( const QStringList& lst )
{
  myList->clear();
  myList->insertStringList( lst );
}

void QtxListResourceEdit::ListItem::setIndexes( const QValueList<int>& lst )
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
  return var;
}

void QtxListResourceEdit::IntegerSpinItem::setProperty( const QString& name, const QVariant& var )
{
  QVariant prop = var;
  if ( !prop.cast( QVariant::Int ) )
    return;

  int val = prop.toInt();
  if ( name == QString( "minimum" ) || name == QString( "min" ) )
    myInteger->setMinValue( val );
  else if ( name == QString( "maximum" ) || name == QString( "max" ) )
    myInteger->setMaxValue( val );
  else if ( name == QString( "step" ) && val > 0 )
    myInteger->setLineStep( val );
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
  return var;
}

void QtxListResourceEdit::DoubleSpinItem::setProperty( const QString& name, const QVariant& var )
{
  QVariant prop = var;
  if ( prop.cast( QVariant::Double ) )
  {
    double val = prop.toDouble();
    if ( name == QString( "minimum" ) || name == QString( "min" ) )
      myDouble->setMinValue( val );
    else if ( name == QString( "maximum" ) || name == QString( "max" ) )
      myDouble->setMaxValue( val );
    else if ( name == QString( "step" ) && val > 0 )
      myDouble->setLineStep( val );
  }
  else
  {
    QVariant prop = var;
    if ( prop.cast( QVariant::Int ) && name == QString( "precision" ) && prop.toInt() > 0 )
      myDouble->setPrecision( prop.toInt() );
  }
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
