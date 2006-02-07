#include "QDS_Datum.h"

#include "QDS_Validator.h"

#include <DDS_Dictionary.h>

#include <qtimer.h>
#include <qlabel.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qvalidator.h>
#include <qmessagebox.h>

/*!
  Class: QDS_Datum::Wrapper
  Descr: Wrapper widget for sub widgets. [internal]
*/

class QDS_Datum::Wrapper : public QWidget
{
public:
  Wrapper( QWidget* = 0 );
  virtual ~Wrapper();

  QWidget*      widget() const;
  void          setWidget( QWidget* );

  virtual bool  eventFilter( QObject*, QEvent* );

protected:
  virtual void  resizeEvent( QResizeEvent* );

private:
  QWidget*      myWid;
};


QDS_Datum::Wrapper::Wrapper( QWidget* parent )
: QWidget( parent ),
myWid( 0 )
{
  QHBoxLayout* base = new QHBoxLayout( this );
  base->setAutoAdd( true );
  setFocusPolicy( StrongFocus );
}

QDS_Datum::Wrapper::~Wrapper()
{
}

QWidget* QDS_Datum::Wrapper::widget() const
{
  return myWid;
}

void QDS_Datum::Wrapper::setWidget( QWidget* wid )
{
  if ( myWid == wid )
    return;

  if ( myWid )
    myWid->removeEventFilter( this );

  myWid = wid;

  if ( !myWid )
    return;

  if ( myWid->parent() != this )
    myWid->reparent( this, QPoint( 0, 0 ) );

  setTabOrder( this, myWid );
  setFocusProxy( myWid );

  myWid->updateGeometry();
  updateGeometry();

  myWid->installEventFilter( this );
}

bool QDS_Datum::Wrapper::eventFilter( QObject* o, QEvent* e )
{
  if ( e->type() == QEvent::Resize && o == widget() )
  {
    QResizeEvent* re = (QResizeEvent*)e;
    if ( re->size() != size() )
      resize( re->size() );
  }
  return QWidget::eventFilter( o, e );
}

void QDS_Datum::Wrapper::resizeEvent( QResizeEvent* e )
{
  QWidget::resizeEvent( e );

  if ( widget() && widget()->size() != size() )
    widget()->resize( size() );
}

/*!
  Class: QDS_Datum
  Descr: Base class for control used data dictionary. [public]
*/

QDS_Datum::QDS_Datum( const QString& id, QWidget* parent, const int flags, const QString& comp )
: QObject( parent ),
myId( id ),
myLabel( 0 ),
myUnits( 0 ),
myControl( 0 ),
myFlags( flags ),
myInitialised( false )
{
  if ( myFlags & Label )
    myWrapper.insert( Label, new Wrapper( parent ) );
  if ( myFlags & Control )
    myWrapper.insert( Control, new Wrapper( parent ) );
  if ( myFlags & Units )
    myWrapper.insert( Units, new Wrapper( parent ) );

  for ( QMap<int, Wrapper*>::Iterator it = myWrapper.begin(); it != myWrapper.end(); ++it )
    connect( it.data(), SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );

  Handle(DDS_Dictionary) aDict = DDS_Dictionary::Get();
  if ( aDict.IsNull() )
    return;

  TCollection_AsciiString anId = toAsciiString( id );
  TCollection_AsciiString aComp = toAsciiString( comp );

  if ( aComp.IsEmpty() )
    setDicItem( aDict->GetDicItem( anId ) );
  else
    setDicItem( aDict->GetDicItem( anId, aComp ) );

  QTimer::singleShot( 0, this, SLOT( onInitDatum() ) );

  if ( parent )
    parent->installEventFilter( this );

  insertDatum( this );
}

QDS_Datum::~QDS_Datum()
{
  removeDatum( this );

  delete myLabel;
  delete myUnits;
  delete myControl;
/*
  for ( QMap<int, Wrapper*>::Iterator it = myWrapper.begin(); it != myWrapper.end(); ++it )
    delete it.data();
*/
}

QDS_Datum::operator QWidget*() const
{
  return widget( Control );
}

QString QDS_Datum::id() const
{
  initDatum();

  return myId;
}

int QDS_Datum::type() const
{
  initDatum();

  int res = DDS_DicItem::Unknown;
  if ( !myDicItem.IsNull() )
    res = myDicItem->GetType();
  return res;
}

QString QDS_Datum::label() const
{
  initDatum();

  QString labStr;
  if ( !myDicItem.IsNull() )
    labStr = toQString( myDicItem->GetLabel() );

  if ( flags() & NotAccel )
    labStr = removeAccel( labStr );

  return labStr;
}

QString QDS_Datum::units() const
{
  initDatum();

  QString unitStr;
  if ( !myDicItem.IsNull() )
    unitStr = toQString( myDicItem->GetUnits() );
  return unitStr;
}

QString QDS_Datum::filter() const
{
  initDatum();

  QString fltr;
  if ( !myDicItem.IsNull() )
    fltr = toQString( myDicItem->GetFilter() );
  return fltr;
}

QString QDS_Datum::format() const
{
  initDatum();

  QString fmtStr;
  if ( !myDicItem.IsNull() )
    fmtStr = toQString( myDicItem->GetFormat( false ) );
  return fmtStr;
}

QString QDS_Datum::defaultValue() const
{
  initDatum();

  QString pref = prefix();
  QString suff = suffix();

  QString def;
  if ( !myDicItem.IsNull() )
    def = toQString( myDicItem->GetDefaultValue() );

  QString aDef = def.stripWhiteSpace();
  if ( !pref.isEmpty() && aDef.left( pref.length() ) == pref )
    aDef = aDef.mid( pref.length() );

  if ( !suff.isEmpty() && aDef.right( suff.length() ) == suff )
    aDef = aDef.mid( 0, aDef.length() - suff.length() );

  return aDef;
}

QString QDS_Datum::minimumValue() const
{
  initDatum();

  QString min;
  if ( !myDicItem.IsNull() && myDicItem->HasData( DDS_DicItem::MinValue ) )
      min = format( format(), type(), myDicItem->GetMinValue() );
  return min;
}

QString QDS_Datum::maximumValue() const
{
  initDatum();

  QString max;
  if ( !myDicItem.IsNull() && myDicItem->HasData( DDS_DicItem::MaxValue ) )
    max = format( format(), type(), myDicItem->GetMaxValue() );
  return max;
}

QString QDS_Datum::longDescription() const
{
  initDatum();

  QString ldStr;
  if ( !myDicItem.IsNull() )
    ldStr = toQString( myDicItem->GetLongDescription() );
  return ldStr;
}

QString QDS_Datum::shortDescription() const
{
  initDatum();

  QString sdStr;
  if ( !myDicItem.IsNull() )
    sdStr = toQString( myDicItem->GetLongDescription() );
  return sdStr;
}

QVariant QDS_Datum::value() const
{
  QVariant val;
  if ( !isEmpty() )
    val = stringValue();
  return val;
}

QString QDS_Datum::stringValue() const
{
  initDatum();

  if ( getString() == myTargetValue )
    return mySourceValue;
  else
    return getString();
}

double QDS_Datum::doubleValue() const
{
  initDatum();

  double res = 0;
  if ( !myTargetValue.isNull() && myTargetValue == getString() )
    res = mySourceValue.toDouble();
  else
  {
    res = getString().toDouble();
    if ( !myDicItem.IsNull() )
      res = myDicItem->ToSI( res );
  }

  return res;
}

int QDS_Datum::integerValue() const
{
  initDatum();

  int res = 0;
  if ( !myTargetValue.isNull() && myTargetValue == getString() )
    res = mySourceValue.toInt();
  else
  {
    double val = getString().toDouble();
    if ( !myDicItem.IsNull() )
      res = (int)myDicItem->ToSI( val );
  }

  return res;
}

QString QDS_Datum::text() const
{
  initDatum();

  QString aLabel = label();
  QString aData  = stringValue();
  QString aUnits = units();

  QString res = aLabel;
  if ( !res.isEmpty() && !aData.isEmpty() )
    res += QString( ": " );

  res += aData;
  if ( !aUnits.isEmpty() )
    res += QString( " " ) + aUnits;

  return res;
}

bool QDS_Datum::isEmpty() const
{
  return stringValue().isEmpty();
}

void QDS_Datum::reset()
{
  initDatum();

  mySourceValue = defaultValue();
  setString( format( ( myFlags & NotFormat ) ? (QString) "" : format(), type(), mySourceValue ) );
  invalidateCache();

  onParamChanged();
  QString str = getString();
  emit paramChanged();
  emit paramChanged( str );
}

void QDS_Datum::clear()
{
  initDatum();

  if ( !getString().isEmpty() )
  {
    mySourceValue = "";
    setString( mySourceValue );
    invalidateCache();

    onParamChanged();
    QString str = getString();
    emit paramChanged();
    emit paramChanged( str );
  }
}

void QDS_Datum::setValue( const QVariant& val )
{
  if ( val.isValid() && val.canCast( QVariant::String ) )
    setStringValue( val.toString() );
  else
    clear();
}

void QDS_Datum::setStringValue( const QString& txt )
{
  initDatum();

  mySourceValue = txt;
  QString aStr = format( ( flags() & NotFormat ) ? (QString) "" : format(), type(), txt );
  setString( aStr );
  myTargetValue = aStr;

  onParamChanged();
  QString str = getString();
  emit paramChanged();
  emit paramChanged( str );
}

void QDS_Datum::setDoubleValue( const double num )
{
  initDatum();

  mySourceValue = QString().setNum( num, 'g', 16 );
  double val = num;
  if ( !myDicItem.IsNull() )
    val = myDicItem->FromSI( val );

  QString aStr = format( ( flags() & NotFormat ) ? (QString) "" : format(), type(), val );
  setString( aStr );
  myTargetValue = aStr;

  onParamChanged();
  QString str = getString();
  emit paramChanged();
  emit paramChanged( str );
}

void QDS_Datum::setIntegerValue( const int num )
{
  initDatum();

  mySourceValue = QString().setNum( num );
  double val = num;
  if ( !myDicItem.IsNull() )
    val = myDicItem->FromSI( val );

  QString aStr = format( ( flags() & NotFormat ) ? (QString) "" : format(), type(), val );
  setString( aStr );
  myTargetValue = aStr;

  onParamChanged();
  QString str = getString();
  emit paramChanged();
  emit paramChanged( str );
}

/*!
  Returns true if all subwidgets specified by 'element' enabled.
*/
bool QDS_Datum::isEnabled( const int element ) const
{
  initDatum();

  bool res = true;
  if ( element & Label )
    res = res && labelWidget() && labelWidget()->isEnabled();
  if ( element & Units )
    res = res && unitsWidget() && unitsWidget()->isEnabled();
  if ( element & Control )
    res = res && controlWidget() && controlWidget()->isEnabled();
  return res;
}

/*!
  Enable/Disable subwidgets specified by 'element'.
  Values: Label, Control, Units or their combinations.
*/
void QDS_Datum::setEnabled( const bool on, const int element )
{
  initDatum();

  if ( element & Label && labelWidget() )
    labelWidget()->setEnabled( on );
  if ( element & Units && unitsWidget() )
    unitsWidget()->setEnabled( on );
  if ( element & Control && controlWidget() )
    controlWidget()->setEnabled( on );
}

/*!
  Enable/Disable control.
*/
void QDS_Datum::setEnabled( bool on )
{
  setEnabled( on, Control );
}

void QDS_Datum::setShown( const bool visible, const int flags )
{
  if ( visible )
    show( flags );
  else
    hide( flags );
}

/*!
  Show subwidgets specified by 'element'.
  Values: Label, Control, Units or their combinations.
*/
void QDS_Datum::show( const int element )
{
  initDatum();

  if ( ( element & Label ) && labelWidget() )
    labelWidget()->show();
  if ( ( element & Units ) && unitsWidget() )
    unitsWidget()->show();
  if ( ( element & Control ) && controlWidget() )
    controlWidget()->show();
}

/*!
  Hide subwidgets specified by 'element'.
  Values: Label, Control, Units or their combinations.
*/
void QDS_Datum::hide( const int element )
{
  initDatum();

  if ( ( element & Label ) && labelWidget() )
    labelWidget()->hide();
  if ( ( element & Units ) && unitsWidget() )
    unitsWidget()->hide();
  if ( ( element & Control ) && controlWidget() )
    controlWidget()->hide();
}

/*!
  Returns subwidget specified by 'element'.
  Possible values: Label, Control, Units.
*/
QWidget* QDS_Datum::widget( const int element ) const
{
  initDatum();
  return wrapper( element );
}

/*!
  Set the input focus on the control widget.
*/
void QDS_Datum::setFocus()
{
  initDatum();

  if ( controlWidget() )
    controlWidget()->setFocus();
}

/*!
  Returns true if control contains valid value otherwise returns false
  and display warning message box if parameter msgBox is set.
*/
bool QDS_Datum::isValid( const bool msgBox, const QString& extMsg, const QString& extLabel ) const
{
  initDatum();

  if ( type() == DDS_DicItem::String && isDoubleFormat( format() ) )
    return true;

  QString req;
  if ( !dicItem().IsNull() )
    req = toQString( dicItem()->GetRequired() );

  bool aState = true;
  QString aStr = getString();

  if ( aStr.isEmpty() )
    aState = !( req == QString( "yes" ) || req == QString( "true" ) || req.toInt() );
  else
    aState = validate( aStr );

  if ( msgBox && !aState )
  {
    QString info;
    if ( !label().isEmpty() )
      info += tr( "DATA_INCORRECT_VALUE" ).arg( label() );
    else if ( !extLabel.isEmpty() )
      info += tr( "DATA_INCORRECT_VALUE" ).arg( extLabel );

    QString typeStr;
    switch ( type() )
    {
    case DDS_DicItem::String:
      typeStr = tr( "DATA_STRING" );
      break;
    case DDS_DicItem::Integer:
      typeStr = tr( "DATA_INTEGER" );
      break;
    case DDS_DicItem::Float:
      typeStr = tr( "DATA_FLOAT" );
      break;
    default:
      typeStr = tr( "DATA_NON_EMPTY" );
      break;
    }
    info += ( info.isEmpty() ? (QString) "" : QString( "\n" ) ) + 
            tr( "DATA_SHOULD_BE_VALUE" ).arg( typeStr );
    QString limit;
    if ( type() == DDS_DicItem::Float || type() == DDS_DicItem::Integer )
    {
      QString aMinValue = minValue();
      QString aMaxValue = maxValue();
      if ( !aMinValue.isEmpty() && !aMaxValue.isEmpty() )
        limit = tr( "DATA_RANGE" ).arg( aMinValue ).arg( aMaxValue );
      else if ( !aMinValue.isEmpty() )
        limit = tr( "DATA_MIN_LIMIT" ).arg( aMinValue );
      else if ( !aMaxValue.isEmpty() )
        limit = tr( "DATA_MAX_LIMIT" ).arg( aMaxValue );
    }
    if ( !limit.isEmpty() )
      info += limit;

    info += QString( ".\n" ) + tr( "DATA_INPUT_VALUE" );

    if ( !extMsg.isEmpty() )
      info += QString( "\n" ) + extMsg;

    QString msg;
    for ( uint i = 0; i < info.length(); i++ )
      if ( info.at( i ) == '\n' )
        msg += QString( "<br>" );
      else
        msg += info.at( i );

    info = QString( "<p><nobr>%1</nobr></p>" ).arg( msg );

    QMessageBox::critical( controlWidget() ? controlWidget()->topLevelWidget() : 0,
                           tr( "DATA_ERR_TITLE" ), info, tr( "OK" ) );
    if ( controlWidget() )
      controlWidget()->setFocus();
  }
  return aState;
}

/*!
  Add widgets to the vertical layout.
*/
void QDS_Datum::addTo( QVBoxLayout* l )
{
  initDatum();

  if ( !l )
    return;

  if ( wrapper( Label ) )
    l->addWidget( wrapper( Label ) );
  if ( wrapper( Control ) )
    l->addWidget( wrapper( Control ) );
  if ( wrapper( Units ) )
    l->addWidget( wrapper( Units ) );
}

/*!
  Add widgets to the horizaontal layout.
*/
void QDS_Datum::addTo( QHBoxLayout* l )
{
  initDatum();

  if ( !l )
    return;

  if ( wrapper( Label ) )
    l->addWidget( wrapper( Label ) );
  if ( wrapper( Control ) )
    l->addWidget( wrapper( Control ) );
  if ( wrapper( Units ) )
    l->addWidget( unitsWidget() );
}

/*!
  Add widgets to the grid layout.
*/
void QDS_Datum::addTo( QGridLayout* theLay, const int theRow, const int theCol, bool vertical )
{
  initDatum();

  if ( !theLay )
    return;

  int row = theRow;
  int col = theCol;
  if ( wrapper( Label ) )
  {
    theLay->addWidget( wrapper( Label ), row, col );
    vertical ? row++ : col++;
  }
  if ( wrapper( Control ) )
  {
    theLay->addWidget( wrapper( Control ), row, col );
    vertical ? row++ : col++;
  }
  if ( wrapper( Units ) )
    theLay->addWidget( wrapper( Units ), row, col );
}

/*!
  Set the aligment of Label or Units. For Control nothing happens.
*/
void QDS_Datum::setAlignment( const int align, const int type )
{
  initDatum();

  if ( ( type & Label ) && labelWidget() )
    labelWidget()->setAlignment( align );
  if ( ( type & Units ) && unitsWidget() )
    unitsWidget()->setAlignment( align );
}

bool QDS_Datum::eventFilter( QObject* o, QEvent* e )
{
  if ( o == parent() )
  {
    if ( e->type() == QEvent::Show || e->type() == QEvent::ShowToParent ||
         ( e->type() == QEvent::ChildInserted && ((QChildEvent*)e)->child() == this ) )
      initDatum();
  }
  return QObject::eventFilter( o, e );
}

/*!
  Notify about parameter changing.
*/
void QDS_Datum::onParamChanged()
{
}

/*!
  Delayed initialization.
*/
void QDS_Datum::onInitDatum()
{
  initDatum();
}

/*!
  Notify about subwidgets destroying. Allow to avoid repeated deleting in destructor.
*/
void QDS_Datum::onDestroyed( QObject* obj )
{
  myWrapper.remove( wrapperType( (Wrapper*)obj ) );
}

/*!
  Returns QLabel instance which contains data dictionary label.
*/
QLabel* QDS_Datum::labelWidget() const
{
  initDatum();
  return myLabel;
}

/*!
  Returns QLabel instance which contains data dictionary units.
*/
QLabel* QDS_Datum::unitsWidget() const
{
  initDatum();
  return myUnits;
}

/*!
  Returns QWidget which contains user input data.
*/
QWidget* QDS_Datum::controlWidget() const
{
  initDatum();
  return myControl;
}

/*!
  Returns the dictionary item from the datum.
*/
Handle(DDS_DicItem) QDS_Datum::dicItem() const
{
  return myDicItem;
}

/*!
  Set the dictionary item in to the datum.
*/
void QDS_Datum::setDicItem( const Handle(DDS_DicItem)& item )
{
  myDicItem = item;
}

/*!
  Creates QLabel widget for data label.
*/
QLabel* QDS_Datum::createLabel( QWidget* parent )
{
  return new QLabel( parent );
}

/*!
  Creates QLabel widget for data units.
*/
QLabel* QDS_Datum::createUnits( QWidget* parent )
{
  return new QLabel( parent );
}

/*!
  Returns validator accordance to data type.
*/
QValidator* QDS_Datum::validator( const bool limits ) const
{
  QValidator* aValidator = 0;

  QString fltr = filter();

  if ( type() == DDS_DicItem::String )
  {
    QString aFlags;
    QString aFormat = canonicalFormat( format(), aFlags );

    int len = -1;
    int pos = aFormat.find( "." );
    if ( pos != -1 )
    {
      QString numStr = aFormat.mid( pos + 1, aFormat.length() - pos - 2 );
      bool ok;
      int numVal = numStr.toInt( &ok );
      if ( ok )
        len = numVal;
    }

    QDS_StringValidator* aStrVal = new QDS_StringValidator( fltr, aFlags, (QObject*)this );
    aStrVal->setLength( len );

    aValidator = aStrVal;
  }
  else if ( type() == DDS_DicItem::Integer )
  {
    QDS_IntegerValidator* aIntVal = new QDS_IntegerValidator( fltr, (QObject*)this );

    bool ok;
    int limit;
    limit = minValue().toInt( &ok );
    if ( ok && limits )
      aIntVal->setBottom( limit );
    limit = maxValue().toInt( &ok );
    if ( ok && limits )
      aIntVal->setTop( limit );

    aValidator = aIntVal;
  }
  else if ( type() == DDS_DicItem::Float )
  {
    QDS_DoubleValidator* aFloatVal = new QDS_DoubleValidator( fltr, (QObject*)this );

    bool ok;
    double limit;
    limit = minValue().toDouble( &ok );
    if ( ok && limits )
      aFloatVal->setBottom( limit );
    limit = maxValue().toDouble( &ok );
    if ( ok && limits )
      aFloatVal->setTop( limit );

    aValidator = aFloatVal;
  }

  return aValidator;
}

/*!
  Checks the given string are valid or not.
*/
bool QDS_Datum::validate( const QString& txt ) const
{
  if ( type() == DDS_DicItem::Unknown ||
       type() == DDS_DicItem::String && isDoubleFormat( format() ) )
    return true;

  QValidator* aValidator = validator( true );

  if ( !aValidator )
    return true;

  int pos = 0;
  QString str( txt );
  bool res = aValidator->validate( str, pos ) == QValidator::Acceptable;

  delete aValidator;

  return res;
}

/*!
  Retrieves information from data dictionary and create subwidgets using virtual mechanism.
  Virtual mechanism doesn't work in constructor and destructor, therefore this method should
  be called outside the constructor.
*/
void QDS_Datum::initialize()
{
  if ( wrapper( Label ) )
    wrapper( Label )->setWidget( myLabel = createLabel( wrapper( Label ) ) );
  if ( wrapper( Control ) )
    wrapper( Control )->setWidget( myControl = createControl( wrapper( Control ) ) );
  if ( wrapper( Units ) )
    wrapper( Units )->setWidget( myUnits = createUnits( wrapper( Units ) ) );

  TCollection_AsciiString comp;
  Handle(DDS_DicItem) item = dicItem();
  if ( !item.IsNull() )
    comp = item->GetComponent();

  QString unitSystem;
  Handle(DDS_Dictionary) dic = DDS_Dictionary::Get();
  if ( !dic.IsNull() )
    unitSystem = toQString( comp.IsEmpty() ? dic->GetActiveUnitSystem() :
                                             dic->GetActiveUnitSystem( comp ) );

  unitSystemChanged( unitSystem );

  QWidget* ctrl = controlWidget();
  if ( ctrl )
  {
    QString lDescr = longDescription();
    QString sDescr = shortDescription();
    if ( !sDescr.isEmpty() )
      QToolTip::add( ctrl, sDescr );
    if ( !lDescr.isEmpty() )
      QWhatsThis::add( ctrl, lDescr );
  }

  if ( labelWidget() && ctrl && !( flags() & NotAccel ) )
    labelWidget()->setBuddy( ctrl );
}

void QDS_Datum::unitSystemChanged( const QString& unitSystem )
{
  QString labText = label();
  QString unitText = unitsToText( units() );

  if ( flags() & UnitsWithLabel )
  {
    if ( labText.isEmpty() )
      labText = unitText;
    else if ( !unitText.isEmpty() )
      labText = QString( "%1 (%2)" ).arg( labText ).arg( unitText );
    unitText = QString::null;
  }

  if ( labelWidget() )
    labelWidget()->setText( labText );

  if ( unitsWidget() )
    unitsWidget()->setText( unitText );

  reset();
}

/*!
  Covert units into text presentation.
*/
QString QDS_Datum::unitsToText( const QString& uni )
{
  int pos = -1;
  QString aUnits = uni;
  while ( ( pos = aUnits.find( "**" ) ) != -1 )
  {
    aUnits = aUnits.mid( 0, pos ) + QString( "<tt><font size=+2><sup>" ) +
             aUnits.mid( pos + 2, 1 ) + QString( "</sup></font></tt>" ) +
             aUnits.mid( pos + 3 );
  }
  return aUnits;
}

/*!
  Covert text presentation into internal units format.
*/
QString QDS_Datum::textToUnits( const QString& txt )
{
  int pos = -1;
  QString aUnits = txt;
  while ( ( pos = aUnits.find( "<sup>" ) ) != -1 )
  {
    aUnits.remove( pos, 5 );
    aUnits.insert( pos, "**" );
  }
  while ( ( pos = aUnits.find( "</sup>" ) ) != -1 )
    aUnits.remove( pos, 6 );
  return aUnits;
}

/*!
  Format the specified integer as data dictionary value.
*/
QString QDS_Datum::format( const int num, const QString& id, const bool convert )
{
  Handle(DDS_DicItem) anItem;
  int aNum = num;
  QString anUnit;
  
  QString aFormat;
  int aType = DDS_DicItem::Unknown;
  Handle(DDS_Dictionary) aDict = DDS_Dictionary::Get();
  if ( !aDict.IsNull() )
  {
    anItem = aDict->GetDicItem( toAsciiString( id ) );
    if ( !anItem.IsNull() )
    {
      aType = anItem->GetType();
      aFormat = toQString( anItem->GetFormat( false ) );
      if ( convert )
        aNum = ( int )anItem->FromSI( aNum );
    }
  }

  return format( aFormat, aType, aNum );
}

/*!
  Format the specified double as data dictionary value.
*/
QString QDS_Datum::format( const double num, const QString& id, const bool convert )
{
  Handle(DDS_DicItem) anItem;
  double aNum = num;
  QString anUnit;
  
  QString aFormat;
  int aType = DDS_DicItem::Unknown;
  Handle(DDS_Dictionary) aDict = DDS_Dictionary::Get();
  if ( !aDict.IsNull() )
  {
    anItem = aDict->GetDicItem( toAsciiString( id ) );
    if ( !anItem.IsNull() )
    {
      aType = anItem->GetType();
      aFormat = toQString( anItem->GetFormat( false ) );
      if ( convert )
        aNum = anItem->FromSI( aNum );
    }
  }

  return format( aFormat, aType, aNum );
}

/*!
  Format the specified string as data dictionary value.
*/
QString QDS_Datum::format( const QString& str, const QString& id, const bool convert )
{
  Handle(DDS_DicItem) anItem;
  QString aStr = str;
  QString anUnit;

  QString aFormat;
  int aType = DDS_DicItem::Unknown;
  Handle(DDS_Dictionary) aDict = DDS_Dictionary::Get();
  if ( !aDict.IsNull() )
  {
    anItem = aDict->GetDicItem( toAsciiString( id ) );
    if ( !anItem.IsNull() )
    {
      aType = anItem->GetType();
      aFormat = toQString( anItem->GetFormat( false ) );
      if ( convert )
        aStr = QString::number( anItem->FromSI( aStr.toDouble() ), 'f', 16 );
    }
  }

  return format( aFormat, aType, aStr );
}

/*!
  Format the given string accordance to data format.
*/
QString QDS_Datum::format( const QString& aFormat, const int aType, const int aValue )
{
  QString txt;

  if ( !aFormat.isEmpty() )
  {
    switch ( aType )
    {
    case DDS_DicItem::Float:
      txt = sprintf( aFormat, (double)aValue );
      txt = txt.stripWhiteSpace();
      break;
    case DDS_DicItem::Integer:
      txt = sprintf( aFormat, aValue );
      txt = txt.stripWhiteSpace();
      break;
    case DDS_DicItem::String:
    default:
      txt = sprintf( aFormat, aValue );
      break;
    }
  }
  else
    txt = QString().setNum( aValue );

  return txt;
}

/*!
  Format the given string accordance to data format.
*/
QString QDS_Datum::format( const QString& aFormat, const int aType, const double aValue )
{
  QString txt;

  if ( !aFormat.isEmpty() )
  {
    switch ( aType )
    {
    case DDS_DicItem::Float:
      txt = QString().sprintf( aFormat, aValue );
      txt = txt.stripWhiteSpace();
      break;
    case DDS_DicItem::Integer:
      txt = QString().sprintf( aFormat, (int)aValue );
      txt = txt.stripWhiteSpace();
      break;
    case DDS_DicItem::String:
    default:
      txt = QString().sprintf( aFormat, aValue );
      break;
    }
  }
  else
    txt = QString().setNum( aValue, 'g', 16 );

  return txt;
}

/*!
  Format the given string accordance to data format.
*/
QString QDS_Datum::format( const QString& aFormat, const int aType, const QString& aValue )
{
  QString txt = aValue;

  if ( aType != DDS_DicItem::String )
    txt = txt.stripWhiteSpace();

  if ( aFormat.isEmpty() || txt.isEmpty() )
    return txt;

  switch ( aType )
  {
  case DDS_DicItem::Float:
    txt = txt.replace( 'd', 'e' ).replace( 'D', 'E' );
    txt = sprintf( aFormat, txt.toDouble() );
    txt = txt.stripWhiteSpace();
    break;
  case DDS_DicItem::Integer:
    txt = sprintf( aFormat, txt.toInt() );
    txt = txt.stripWhiteSpace();
    break;
  case DDS_DicItem::String:
    txt = sprintf( aFormat, txt );
    break;
  }

  return txt;
}

/*!
  Wrapper around the standard sprintf function.
  Process some non standard flags from format string.
*/
QString QDS_Datum::sprintf( const QString& fmt, const int val )
{
  return QString().sprintf( canonicalFormat( fmt ), val );
}

/*!
  Wrapper around the standard sprintf function.
  Process some non standard flags from format string.
*/
QString QDS_Datum::sprintf( const QString& fmt, const double val )
{
  return QString().sprintf( canonicalFormat( fmt ), val );
}

/*!
  Wrapper around the standard sprintf function.
  Process some non standard flags from format string.
*/
QString QDS_Datum::sprintf( const QString& fmt, const QString& val )
{
  QString aFlags;
  QString aFormat = canonicalFormat( fmt, aFlags );

  QString txt = val;

  QRegExp rx( "^(%[0-9]*.?[0-9]*s)$" );
  if ( aFormat.find( rx ) != -1 )
  {
    // QString().sprintf() always expects string in UTF8 encoding, so we cannot use it here
    char* buf = new char[txt.length() + 1];
    ::sprintf( buf, aFormat.latin1(), (const char*)(txt.local8Bit()) );
    txt = QString::fromLocal8Bit( buf );

    delete[] buf;
  }

  if ( isDoubleFormat( aFormat ) )
  {
    /*bool isOk;
    double aVal = txt.toDouble( &isOk );
    if ( isOk )
    {
      txt = sprintf( aFormat, aVal );
      txt = txt.replace( 'e', 'D' );
    }*/
  }

  if ( aFlags.contains( "u", false ) )
    txt = txt.upper();
  if ( aFlags.contains( "l", false ) )
    txt = txt.lower();

  return txt;
}

/*!
  Returns the canonical sprintf format.
*/
QString QDS_Datum::canonicalFormat( const QString& fmt )
{
  QString flags;
  return canonicalFormat( fmt, flags );
}

/*!
  Returns the canonical sprintf format and non standard flags.
*/
QString QDS_Datum::canonicalFormat( const QString& fmt, QString& flags )
{
  QString newFmt = fmt;
  flags = QString::null;

  QRegExp rx( "^(%[0-9]*.?[0-9]*)([a-z,A-Z]+)[g|c|d|i|o|u|x|e|f|n|p|s|X|E|G]$" );
  if ( rx.search( newFmt ) >= 0 )
  {
    flags = rx.cap( 2 );
    newFmt.remove( rx.pos( 2 ), flags.length() );
  }
  return newFmt;
}

/*!
  Returns displayable units string for given DD ID
*/
QString QDS_Datum::units( const QString& id )
{
  QString anUnit;
  Handle(DDS_DicItem) anItem;

  Handle(DDS_Dictionary) aDict = DDS_Dictionary::Get();
  if ( !aDict.IsNull() )
  {
    anItem = aDict->GetDicItem( toAsciiString( id ) );
    if ( !anItem.IsNull() )
      anUnit = unitsToText( toQString( anItem->GetUnits() ) );
  }
  return anUnit;
}

/*!
  Get prefix string from format.
*/
QString QDS_Datum::prefix() const
{
  return QString::null;
}

/*!
  Get suffix string from format.
*/
QString QDS_Datum::suffix() const
{
  return QString::null;
}

/*!
  Get min value.
*/
QString QDS_Datum::minValue() const
{
  QString pref = prefix();
  QString suff = suffix();

  QString aMin = minimumValue().stripWhiteSpace();

  if ( !pref.isEmpty() && aMin.left( pref.length() ) == pref )
    aMin = aMin.mid( pref.length() );

  if ( !suff.isEmpty() && aMin.right( suff.length() ) == suff )
    aMin = aMin.mid( 0, aMin.length() - suff.length() );

  return aMin;
}

/*!
  Get max value.
*/
QString QDS_Datum::maxValue() const
{
  QString pref = prefix();
  QString suff = suffix();

  QString aMax = maximumValue().stripWhiteSpace();

  if ( !pref.isEmpty() && aMax.left( pref.length() ) == pref )
    aMax = aMax.mid( pref.length() );

  if ( !suff.isEmpty() && aMax.right( suff.length() ) == suff )
    aMax = aMax.mid( 0, aMax.length() - suff.length() );

  return aMax;
}

/*!
  Reset the numeric value cache.
*/
void QDS_Datum::invalidateCache()
{
  myTargetValue = QString::null;
}

QString QDS_Datum::removeAccel( const QString& src )
{
  QString trg = src;

  for ( uint i = 0; i < trg.length(); )
  {
    if ( trg.mid( i, 2 ) == QString( "&&" ) )
      i += 2;
    else if ( trg.at( i ) == '&' )
      trg.remove( i, 1 );
    else
      i++;
  }
  return trg;
}

bool QDS_Datum::isDoubleFormat( const QString& theFormat )
{
  if ( theFormat.length() > 0 )
  {
    QChar c = theFormat[ (int)( theFormat.length() - 1 ) ];
      return c == 'f' || c == 'g' || c == 'e' || c == 'G' || c == 'E';
  }
  else
    return false;
}

int QDS_Datum::flags() const
{
  return myFlags;
}

void QDS_Datum::initDatum() const
{
  if ( myInitialised )
    return;

  QDS_Datum* that = (QDS_Datum*)this;
  that->myInitialised = true;
  that->initialize();

  if ( parent() )
    parent()->removeEventFilter( this );
}

QDS_Datum::Wrapper* QDS_Datum::wrapper( QWidget* wid ) const
{
  if ( !wid )
    return 0;

  Wrapper* wrap = 0;
  for ( QMap<int, Wrapper*>::ConstIterator it = myWrapper.begin(); it != myWrapper.end() && !wrap; ++it )
  {
    if ( it.data() && it.data()->widget() == wid )
      wrap = it.data();
  }
  return wrap;
}

QDS_Datum::Wrapper* QDS_Datum::wrapper( const int id ) const
{
  Wrapper* wrap = 0;
  if ( myWrapper.contains( id ) )
    wrap = myWrapper[id];
  return wrap;
}

int QDS_Datum::wrapperType( QDS_Datum::Wrapper* wrap ) const
{
  int id = -1;
  for ( QMap<int, Wrapper*>::ConstIterator it = myWrapper.begin(); it != myWrapper.end() && id == -1; ++it )
  {
    if ( it.data() == wrap )
      id = it.key();
  }
  return id;
}
