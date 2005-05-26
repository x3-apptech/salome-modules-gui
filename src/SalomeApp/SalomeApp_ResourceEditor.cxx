// File:      SalomeApp_ResourceEditor.cxx
// Created:   
// Author:    Alexander Sladkov
// Copyright: 

#include "SalomeApp_ResourceEditor.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qvalidator.h>
#include <qcolordialog.h>
#include <qlistbox.h>
#include <qwidgetstack.h>
#include <qobjectlist.h>

#include <QtxDblSpinBox.h>
#include <SUIT_OverrideCursor.h>

#include<stdarg.h>

/*
  Class: SalomeApp_ResourceEditor::ListItem
  Descr: GUI implementation of resources list item.
*/

SalomeApp_ResourceEditor::ListItem::ListItem( SalomeApp_ResourceEditor* ed, QWidget* parent )
: Item( ed )
{
  mypName = new QLabel( parent );
  mypList = new QComboBox( false, parent );
}

SalomeApp_ResourceEditor::ListItem::~ListItem()
{
}

void SalomeApp_ResourceEditor::ListItem::Set( const QString& sect, const QString& key,
                                              const int type, const QString& label )
{
  Item::Set( sect, key, type, label );
  mypName->setText( label );
}

void SalomeApp_ResourceEditor::ListItem::SetList( const QValueList<int>& ids,
                                                  const QStringList& names )
{
  myIndex.clear();
  mypList->clear();

  for ( uint i = 0; i < QMIN( ids.count(), names.count() ); i++ )
  {
    mypList->insertItem( *names.at( i ) );
    myIndex.insert( mypList->count() - 1, *ids.at( i ) );
  }
}

void SalomeApp_ResourceEditor::ListItem::Store()
{
  int type;
  QString key, label;
  QString sect = Get( key, type, label );

  if ( !sect.isEmpty() )
  {
    QString sVal;
    int iVal, index = mypList->currentItem();
    if ( GetInteger( sect, key, iVal ) && myIndex.contains( index ) )
      SetInteger( sect, key, myIndex[index] );
    else if ( GetString( sect, key, sVal ) )
      SetString( sect, key, mypList->text( index ) );
  }
}

void SalomeApp_ResourceEditor::ListItem::Retrieve()
{
  int type;
  QString key, label;
  QString sect = Get( key, type, label );

  int index = -1;
  if ( !sect.isEmpty() )
  {
    int iVal;
    QString sVal;
    if ( GetInteger( sect, key, iVal ) )
    {
      for ( QMap<int, int>::ConstIterator it = myIndex.begin(); it != myIndex.end() && index == -1; ++it )
        if ( it.data() == iVal )
          index = it.key();
    }
    else if ( GetString( sect, key, sVal ) )
    {
      for ( int i = 0; i < (int)mypList->count() && index == -1; i++ )
        if ( mypList->text( i ) == sVal )
          index = i;
    }
  }
  mypList->setCurrentItem( index );
}

/*
  Class: SalomeApp_ResourceEditor::ColorItem
  Descr: GUI implementation of resources color item.
*/

SalomeApp_ResourceEditor::ColorItem::ColorItem( SalomeApp_ResourceEditor* ed, QWidget* parent )
: Item( ed )
{
  mypName = new QLabel( parent );
  mypColor = new ColorButton( parent );
  mypColor->setAutoDefault( false );
}

SalomeApp_ResourceEditor::ColorItem::~ColorItem()
{
}

void SalomeApp_ResourceEditor::ColorItem::Set( const QString& sect, const QString& key,
                                               const int type, const QString& label )
{
  Item::Set( sect, key, type, label );
  mypName->setText( label );
}

void SalomeApp_ResourceEditor::ColorItem::Store()
{
  int type;
  QString key, label;
  QString sect = Get( key, type, label );

  if ( !sect.isEmpty() )
    SetColor( sect, key, mypColor->paletteBackgroundColor() );
}

void SalomeApp_ResourceEditor::ColorItem::Retrieve()
{
  int type;
  QColor color;
  QString key, label;
  QString sect = Get( key, type, label );

  if ( !sect.isEmpty() && GetColor( sect, key, color ) )
    mypColor->setPaletteBackgroundColor( color );
}

/*
  Class: SalomeApp_ResourceEditor::StateItem
  Descr: GUI implementation of resources bool item.
*/

SalomeApp_ResourceEditor::StateItem::StateItem( SalomeApp_ResourceEditor* ed, QWidget* parent )
: Item( ed )
{
  mypName = new QLabel( parent );
  mypState = new QCheckBox( parent );
}

SalomeApp_ResourceEditor::StateItem::~StateItem()
{
}

void SalomeApp_ResourceEditor::StateItem::Set( const QString& sect, const QString& key,
                                               const int type, const QString& label )
{
  Item::Set( sect, key, type, label );
  mypName->setText( label );
}

void SalomeApp_ResourceEditor::StateItem::Store()
{
  int type;
  QString key, label;
  QString sect = Get( key, type, label );
  if ( !sect.isEmpty() )
    SetBoolean( sect, key, mypState->isChecked() );
}

void SalomeApp_ResourceEditor::StateItem::Retrieve()
{
  int type;
  bool state;
  QString key, label;
  QString sect = Get( key, type, label );
  if ( !sect.isEmpty() && GetBoolean( sect, key, state ) )
    mypState->setChecked( state );
}

/*
  Class: SalomeApp_ResourceEditor::StringItem
  Descr: GUI implementation of resources string item.
*/

SalomeApp_ResourceEditor::StringItem::StringItem( SalomeApp_ResourceEditor* ed, QWidget* parent )
: Item( ed )
{
  mypName = new QLabel( parent );
  mypString = new QLineEdit( parent );
}

SalomeApp_ResourceEditor::StringItem::~StringItem()
{
}

void SalomeApp_ResourceEditor::StringItem::Set( const QString& sect, const QString& key,
                                                const int type, const QString& label )
{
  Item::Set( sect, key, type, label );
  mypName->setText( label );
}

void SalomeApp_ResourceEditor::StringItem::Store()
{
  int type;
  QString key, label;
  QString sect = Get( key, type, label );
  if ( !sect.isEmpty() )
    SetString( sect, key, mypString->text() );
}

void SalomeApp_ResourceEditor::StringItem::Retrieve()
{
  int type;
  QString val, key, label;
  QString sect = Get( key, type, label );
  if ( !sect.isEmpty() && GetString( sect, key, val ) )
    mypString->setText( val );
}

/*
  Class: SalomeApp_ResourceEditor::DoubleSpinItem
  Descr: GUI implementation of resources string item.
*/

SalomeApp_ResourceEditor::DoubleSpinItem::DoubleSpinItem
  ( SalomeApp_ResourceEditor* ed, QWidget* parent )
: Item( ed )
{
  mypName = new QLabel( parent );
  mypDouble = new QtxDblSpinBox( parent, "spinboxdouble" );
  mypDouble->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed ) ); 
}

SalomeApp_ResourceEditor::DoubleSpinItem::~DoubleSpinItem()
{
}

void SalomeApp_ResourceEditor::DoubleSpinItem::Set( const QString& sect, const QString& key,
                                                    const int type, const QString& label )
{
  Item::Set( sect, key, type, label );
  mypName->setText( label );
  mypDouble->setName( label ); // for debug purposes..
}

void SalomeApp_ResourceEditor::DoubleSpinItem::SetRange( const double min, const double max,
                                                         const double prec, const double step )
{
  mypDouble->setMinValue( min );
  mypDouble->setMaxValue( max );
  mypDouble->setPrecision( prec );
  if ( step > 0 )
    mypDouble->setSteps( step, step*5 );
}

void SalomeApp_ResourceEditor::DoubleSpinItem::Store()
{
  int type;
  QString key, label;
  QString sect = Get( key, type, label );
  if ( !sect.isEmpty() )
    SetDouble( sect, key, mypDouble->value() );
}

void SalomeApp_ResourceEditor::DoubleSpinItem::Retrieve()
{
  int type;
  double val;
  QString key, label;
  QString sect = Get( key, type, label );
  if ( sect.isEmpty() && GetDouble( sect, key, val ) )
    mypDouble->setValue( val );
}

/*
  Class: SalomeApp_ResourceEditor::DoubleEditItem
  Descr: GUI implementation of resources string item.
*/

SalomeApp_ResourceEditor::DoubleEditItem::DoubleEditItem
  ( SalomeApp_ResourceEditor* ed, QWidget* parent )
: Item( ed )
{
  mypName = new QLabel( parent );
  mypDouble = new QLineEdit( parent );
  mypDouble->setValidator( new QDoubleValidator( mypDouble ) );
}

SalomeApp_ResourceEditor::DoubleEditItem::~DoubleEditItem()
{
}

void SalomeApp_ResourceEditor::DoubleEditItem::Set( const QString& sect, const QString& key,
                                                    const int type, const QString& label )
{
  Item::Set( sect, key, type, label );
  mypName->setText( label );
}

void SalomeApp_ResourceEditor::DoubleEditItem::Store()
{
  int type;
  QString key, label;
  QString sect = Get( key, type, label );
  if ( !sect.isEmpty() )
    SetDouble( sect, key, mypDouble->text().toDouble() );
}

void SalomeApp_ResourceEditor::DoubleEditItem::Retrieve()
{
  int type;
  double val;
  QString key, label;
  QString sect = Get( key, type, label );
  if ( sect.isEmpty() && GetDouble( sect, key, val ) )
    mypDouble->setText( QString().setNum( val, 'g', 16 ) );
}

/*
  Class: SalomeApp_ResourceEditor::IntegerSpinItem
  Descr: GUI implementation of resources integer item.
*/

SalomeApp_ResourceEditor::IntegerSpinItem::IntegerSpinItem
( SalomeApp_ResourceEditor* ed, QWidget* parent )
: Item( ed )
{
  mypName = new QLabel( parent );
  mypInteger = new QSpinBox( parent );
}

SalomeApp_ResourceEditor::IntegerSpinItem::~IntegerSpinItem()
{
}

void SalomeApp_ResourceEditor::IntegerSpinItem::Set( const QString& sect, const QString& key,
                                                     const int type, const QString& label )
{
  Item::Set( sect, key, type, label );
  mypName->setText( label );
}

void SalomeApp_ResourceEditor::IntegerSpinItem::SetRange( const double min, const double max,
                                                          const double, const double step )
{
  mypInteger->setMinValue( (int)min );
  mypInteger->setMaxValue( (int)max );
  int aStep = (int)step;
  if ( aStep > 0 )
    mypInteger->setLineStep( aStep );
}

void SalomeApp_ResourceEditor::IntegerSpinItem::Store()
{
  int type;
  QString key, label;
  QString sect = Get( key, type, label );
  if ( !sect.isEmpty() )
    SetInteger( sect, key, mypInteger->value() );
}

void SalomeApp_ResourceEditor::IntegerSpinItem::Retrieve()
{
  int val, type;
  QString key, label;
  QString sect = Get( key, type, label );
  if ( !sect.isEmpty() && GetInteger( sect, key, val ) )
    mypInteger->setValue( val );
}

/*
  Class: SalomeApp_ResourceEditor::IntegerEditItem
  Descr: GUI implementation of resources integer item.
*/

SalomeApp_ResourceEditor::IntegerEditItem::IntegerEditItem
  ( SalomeApp_ResourceEditor* ed, QWidget* parent )
: Item( ed )
{
  mypName = new QLabel( parent );
  mypInteger = new QLineEdit( parent );
  mypInteger->setValidator( new QIntValidator( mypInteger ) );
}

SalomeApp_ResourceEditor::IntegerEditItem::~IntegerEditItem()
{
}

void SalomeApp_ResourceEditor::IntegerEditItem::Set( const QString& sect, const QString& key,
                                                     const int type, const QString& label )
{
  Item::Set( sect, key, type, label );
  mypName->setText( label );
}

void SalomeApp_ResourceEditor::IntegerEditItem::Store()
{
  int type;
  QString key, label;
  QString sect = Get( key, type, label );
  if ( !sect.isEmpty() )
    SetInteger( sect, key, mypInteger->text().toInt() );
}

void SalomeApp_ResourceEditor::IntegerEditItem::Retrieve()
{
  int val, type;
  QString key, label;
  QString sect = Get( key, type, label );
  if ( !sect.isEmpty() && GetInteger( sect, key, val ) )
    mypInteger->setText( QString().setNum( val ) );
}

/*
  Class: SalomeApp_ResourceEditor::Spacing 
  Descr: GUI implementation of resources spacer.
*/

SalomeApp_ResourceEditor::Spacing::Spacing( SalomeApp_ResourceEditor* ed, QWidget* parent )
: Item( ed )
{
  new QLabel( parent );
  new QLabel( parent );
}

SalomeApp_ResourceEditor::Spacing::~Spacing()
{
}

void SalomeApp_ResourceEditor::Spacing::Store()
{
}

void SalomeApp_ResourceEditor::Spacing::Retrieve()
{
}

/*
  Class: SalomeApp_ResourceEditor::PrefGroup
  Descr: GUI implementation of resources group.
*/

SalomeApp_ResourceEditor::PrefGroup::PrefGroup( const QString& name,
                                                SalomeApp_ResourceEditor* ed,
                                                QWidget* parent )
: QGroupBox( 4, Qt::Horizontal, name, parent ),
  Group( name ),
  myEditor( ed )
{
}

SalomeApp_ResourceEditor::PrefGroup::~PrefGroup()
{
}

void SalomeApp_ResourceEditor::PrefGroup::SetColumns( const int cols )
{
  setColumns( 2 * cols );
}

void SalomeApp_ResourceEditor::PrefGroup::SetTitle( const QString& title )
{
  setTitle( title );
}

SalomeApp_ResourceEditor::Item* SalomeApp_ResourceEditor::PrefGroup::createItem( const int type, const QString& label )
{
  Item* item = 0;

  switch ( type )
  {
  case Color:
    item = new ColorItem( myEditor, this );
    break;
  case Bool:
    item = new StateItem( myEditor, this );
    break;
  case String:
    item = new StringItem( myEditor, this );
    break;
  case List:
    item = new ListItem( myEditor, this );
    break;
  case RealSpin:
    item = new DoubleSpinItem( myEditor, this );
    break;
  case IntegerSpin:
    item = new IntegerSpinItem( myEditor, this );
    break;
  case RealEdit:
    item = new DoubleEditItem( myEditor, this );
    break;
  case IntegerEdit:
    item = new IntegerEditItem( myEditor, this );
    break;
  case Space:
    item = new Spacing( myEditor, this );
    break;
  }

  return item;
}

/*
  Class: SalomeApp_ResourceEditor::PrefTab
  Descr: GUI implementation of resources tab.
*/

SalomeApp_ResourceEditor::PrefTab::PrefTab( const QString& name, 
                                            SalomeApp_ResourceEditor* ed,
                                            QTabWidget* parent )
: QFrame( parent ),
Tab( name ),
mypTabWidget( parent ),
myEditor( ed )
{
  QVBoxLayout* main = new QVBoxLayout( this );
  mypMainFrame = new QGroupBox( 1, Qt::Horizontal, "", this );
  mypMainFrame->setFrameStyle( QFrame::NoFrame );
  mypMainFrame->setInsideMargin( 5 );
  main->addWidget( mypMainFrame );
}

SalomeApp_ResourceEditor::PrefTab::~PrefTab()
{
}

void SalomeApp_ResourceEditor::PrefTab::SetTitle( const QString& title )
{
  if ( !mypTabWidget )
    return;

  mypTabWidget->setTabLabel( this, title );
}

void SalomeApp_ResourceEditor::PrefTab::adjustLabels()
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

SalomeApp_ResourceEditor::Group* SalomeApp_ResourceEditor::PrefTab::createGroup( const QString& name )
{
  return new PrefGroup( name, myEditor, mypMainFrame );
}


/*
  Class: SalomeApp_ResourceEditor::PrefCategory
  Descr: GUI implementation of preferences category.
*/

SalomeApp_ResourceEditor::PrefCategory::PrefCategory( const QString& name,
                                                      SalomeApp_ResourceEditor* ed,
                                                      QListBox* listBox,
                                                      QWidgetStack* parent )
: QTabWidget( parent ),
Category( name ),
mypListBox( listBox ),
mypWidgetStack( parent ),
myEditor( ed )
{
}

SalomeApp_ResourceEditor::PrefCategory::~PrefCategory()
{
}

void SalomeApp_ResourceEditor::PrefCategory::SetTitle( const QString& title )
{
  if ( !mypWidgetStack || !mypListBox )
    return;

  int id = mypWidgetStack->id( this );
  if ( id < 0 )
    return;

  //int idx = mypListBox->index( mypListBox->selectedItem() );
  int idx = mypListBox->currentItem();

  mypListBox->changeItem( title, id );
  mypListBox->setSelected( idx, true );
}

void SalomeApp_ResourceEditor::PrefCategory::adjustLabels()
{
  for ( int i = 0; i < count(); i++ )
  {
    QWidget* wid = page( i );
    if ( wid )
    {
      PrefTab* tab = (PrefTab*)wid;
      tab->adjustLabels();
    }
  }
}

SalomeApp_ResourceEditor::Tab* SalomeApp_ResourceEditor::PrefCategory::createTab( const QString& name )
{
  PrefTab* aTab = new PrefTab( name, myEditor, this );
  addTab( aTab, name );
  return aTab;
}

//=======================================================================
//function : SalomeApp_ResourceEditor
//purpose  : 
//=======================================================================

SalomeApp_ResourceEditor::SalomeApp_ResourceEditor( QtxResourceMgr* theMgr,
                                                    QWidget* theParent )
: QtxDialog( theParent, 0, true, false, Standard | Apply ),
  QtxResourceEditor( theMgr )
{
  setCaption( tr( "CAPTION" ) );

  QHBoxLayout* main = new QHBoxLayout( mainFrame(), 5, 5 );

  mypCatList = new QListBox( mainFrame() );
  mypCatStack = new QWidgetStack( mainFrame() );

  main->addWidget( mypCatList );
  main->addWidget( mypCatStack );

  mypCatList->setSelectionMode( QListBox::Single );

  //int defId = addUserButton( tr( "DEFAULT" ) );
  //QButton* defButton = userButton( defId );

  //if ( defId != -1 )
  //  setUserButtonPosition( Right, defId );

  setTabOrder( button( OK ), button( Cancel ),
               /*defButton,*/ button( Help ), 0 );

  connect( this, SIGNAL( dlgHelp() ),  this, SLOT( onHelp() ) );
  connect( this, SIGNAL( dlgApply() ), this, SLOT( onApply() ) );
  connect( mypCatList, SIGNAL( selectionChanged() ), this, SLOT( onSelectionChanged() ) );

/*  if ( defButton )
    connect( defButton, SIGNAL( clicked() ), this, SLOT( onDefault() ) );*/

  setFocusProxy( mypCatList );
}

//=======================================================================
//function : ~SalomeApp_ResourceEditor
//purpose  : 
//=======================================================================

SalomeApp_ResourceEditor::~SalomeApp_ResourceEditor()
{
}

//=======================================================================
//function : show
//purpose  : 
//=======================================================================

void SalomeApp_ResourceEditor::show()
{
  SUIT_OverrideCursor c;

  setAlignment( 0 );
/*
  for ( QPtrListIterator<Category> it( categories() ); it.current(); ++it )
  {
    PrefCategory* aCat = (PrefCategory*)it.current();
    aCat->adjustLabels();
  }
*/
  Update(); // update all tabs, groups, items.

//  Backup();
  Retrieve();

  QtxDialog::show();
}

//=======================================================================
//function : accept
//purpose  : 
//=======================================================================

void SalomeApp_ResourceEditor::accept()
{
  SUIT_OverrideCursor c;

  QtxDialog::accept();

  Store();
  updateViewer();
}

//=======================================================================
//function : reject
//purpose  : 
//=======================================================================

void SalomeApp_ResourceEditor::reject()
{
  SUIT_OverrideCursor c;

  QtxDialog::reject();

//  Restore();
  updateViewer();
}

//=======================================================================
//function : onHelp
//purpose  : 
//=======================================================================

void SalomeApp_ResourceEditor::onHelp()
{
  /*LH3DGui_Desktop* desk = LH3DGui_Desktop::getDesktop();
  if ( desk )
  {
    LH3DGui_Helper* helper = desk->getHelper();
    if ( helper )
      helper->ShowPage( "preferences" );
  }*/
}

//=======================================================================
//function : onApply
//purpose  : 
//=======================================================================

void SalomeApp_ResourceEditor::onApply()
{
  SUIT_OverrideCursor c;

  Store();
  updateViewer();
}

//=======================================================================
//function : onSelectionChanged
//purpose  : 
//=======================================================================

void SalomeApp_ResourceEditor::onSelectionChanged()
{
  //int index = mypCatList->index( mypCatList->selectedItem() );
  int index = mypCatList->currentItem();
  if ( index < 0 )
    return;

  mypCatStack->raiseWidget( index );
}

//=======================================================================
//function : updateViewer
//purpose  : 
//=======================================================================

void SalomeApp_ResourceEditor::updateViewer()
{
/*  LH3DGui_Desktop* desktop = LH3DGui_Desktop::getDesktop();
  if ( !desktop )
    return;

  LH3DGui_Modeler* active = desktop->getActiveModeler();
  if ( !active )
    return;

  active->Update( LH3DGui_Desktop::Viewer3d );
  if ( desktop->getViewer3d() )
    desktop->getViewer3d()->update();*/
}

//=======================================================================
//function : createCategory
//purpose  : 
//=======================================================================

SalomeApp_ResourceEditor::Category* SalomeApp_ResourceEditor::createCategory( const QString& name )
{
  PrefCategory* aCat = new PrefCategory( name, this, mypCatList, mypCatStack );
  mypCatList->insertItem( name );
  int id = mypCatList->count() - 1;
  mypCatStack->addWidget( aCat, id );

  if ( !mypCatList->currentItem() )
    mypCatList->setSelected( 0, true );

  return aCat;
}

//=======================================================================
//function : setTabOrder
//purpose  : 
//=======================================================================

void SalomeApp_ResourceEditor::setTabOrder( QObject* first, ... )
{
  va_list objs;
  va_start( objs, first );

  QObjectList objList;

  QObject* cur = first;
  while ( cur )
  {
    objList.append( cur );
    cur = va_arg( objs, QObject* );
  }

  setTabOrder( objList );
}

//=======================================================================
//function : setTabOrder
//purpose  : 
//=======================================================================

void SalomeApp_ResourceEditor::setTabOrder( const QObjectList& objs )
{
  QPtrList<QWidget> widgets;

  for ( QObjectListIt it( objs ); it.current(); ++it )
  {
    QWidget* wid = 0;
    QObject* cur = *it;
    if ( cur->isWidgetType() )
      wid = (QWidget*)cur;
    if ( wid )
      widgets.append( wid );
  }

  if ( widgets.count() < 2 )
    return;

  QWidget* prev = widgets.first();
  for ( QWidget* next = widgets.at( 1 ); next; next = widgets.next() )
  {
    QWidget::setTabOrder( prev, next );
    prev = next;
  }
}




ColorButton::ColorButton( QWidget* parent )
: QPushButton( parent )
{
  connect( this, SIGNAL( clicked() ), this, SLOT( onColor() ) );
}

ColorButton::~ColorButton()
{
}

void ColorButton::onColor()
{
  setDown( true );

  QColor c = QColorDialog::getColor( paletteBackgroundColor(), this );
  if ( c.isValid() )
    setPaletteBackgroundColor( c );

  setDown( false );
}

