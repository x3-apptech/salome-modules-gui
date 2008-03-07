// Copyright (C) 2005  CEA/DEN, EDF R&D, OPEN CASCADE, PRINCIPIA R&D
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

#include <TColStd_SequenceOfAsciiString.hxx>

/*!
  class: QDS_Datum::Wrapper
  descr: Wrapper widget for sub widgets. [internal]
*/

class QDS_Datum::Wrapper : public QWidget
{
public:
  Wrapper( QWidget* = 0 );
  virtual ~Wrapper();

  QWidget*      widget() const;
  void          setWidget( QWidget* );

  virtual void  setGeometry( int x, int y, int w, int h );
  virtual void  setSizePolicy( QSizePolicy );

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

  myWid = wid;

  if ( !myWid )
    return;

  if ( myWid->parent() != this )
    myWid->reparent( this, QPoint( 0, 0 ) );

  setTabOrder( this, myWid );
  setFocusProxy( myWid );

  myWid->updateGeometry();
  updateGeometry();
}

void QDS_Datum::Wrapper::setSizePolicy( QSizePolicy sp )
{
  QWidget::setSizePolicy( sp );

  if ( widget() )
    widget()->setSizePolicy( sp );
}

void QDS_Datum::Wrapper::setGeometry( int x, int y, int w, int h )
{
  QWidget::setGeometry( x, y, w, h );

  if ( widget() && widget()->size() != size() )
    widget()->setGeometry( 0, 0, width(), height() );
}

/*!
  \class QDS_Datum

  This is a base class for control using the data dictionary. Datum is successor of QObject (not QWidget).
  This object can have three sub widgets named as Label, Control and Units. User can skip creation of
  some of them manipulate by parameter \aflags. Label widget display label of datum, Control widget allow
  to input value, Units widget display units of measure in the active system.
  
  These widgets constructs under parent of datum. If this parent has layout which can automaticaly add child
  widgets (see QLayout::setAutoAdd()) then these subwidgets will be placed in following order: first widget
  is Label, second - Control, third - Unints. User can add these widgets to layout manually using methods
  QDS_Datum::addTo() or QDS_Datum::widget(). In last case User can retrieve desired widget and place it into
  layout.

  If use QGroupBox as parent widget for datum object then all subwidgets will be arranged automatically by
  group box according to column and orientation properties of QGroupBox.

  For example:
    QGroupBox* box = new QGroupBox( 3, Qt::Horizontal, "datum box" );
    QDS_Datum* d1  = new QDS_Datum( "datum_1", box, All );
    QDS_Datum* d2  = new QDS_Datum( "datum_2", box, All );
    QDS_Datum* d3  = new QDS_Datum( "datum_3", box, All );

  In this example we create the QGroupBox with 3 horizontal columns. All created datum widgets will be
  placed automatically three widgets in a row. Each datum will be placed from up to bottom one by one.

  Datum value is stored as string. User can get/set this value in different kinds:
    \liAs string  - methods stringValue()/setStringValue().
    \liAs integer - methods integerValue()/setIntegerValue(). Given value converted to/from SI.
    \liAs double  - methods doubleValue()/setDoubleValue(). Given value converted to/from SI.
    \liAs variant - methods value()/setValue().

  User can perform some actions on datum subwidgets using following methods: isEnabled(),
  setEnabled(), show(), hide(), setShown(), setFocus(), setAlignment().
*/

/*!
  Constructor. Create datum object with datum identifier \aid under widget \aparent. Parameter \aflags
  define behaviour of datum and set of created subwidgets. Default value of this parameter is QDS::All.
  Parameter \acomp specify the component name which will be used during search of dictionary item.

  Datum register self in the static list by QDS::insertDatum().
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

/*!
  Destructor. Destroy all subwidget.
  Datum unregister self from the static list by QDS::removeDatum().
*/
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

/*!
  Overloaded operator allow to retrieve main subwidget named Control.
*/
QDS_Datum::operator QWidget*() const
{
  return widget( Control );
}

/*!
  Returns the datum id.
*/
QString QDS_Datum::id() const
{
  initDatum();

  return myId;
}

/*!
  Returns the datum type of value.
*/
int QDS_Datum::type() const
{
  initDatum();

  int res = DDS_DicItem::Unknown;
  if ( !myDicItem.IsNull() )
    res = myDicItem->GetType();
  return res;
}

/*!
  Returns the datum label string.
*/
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

/*!
  Returns the datum units string.
*/
QString QDS_Datum::units() const
{
  initDatum();

  QString unitStr;
  if ( !myDicItem.IsNull() )
    unitStr = toQString( myDicItem->GetUnits() );
  return unitStr;
}

/*!
  Returns the datum value filter string.
*/
QString QDS_Datum::filter() const
{
  initDatum();

  QString fltr;
  if ( !myDicItem.IsNull() )
    fltr = toQString( myDicItem->GetFilter() );
  return fltr;
}

/*!
  Returns the datum value format string.
*/
QString QDS_Datum::format() const
{
  initDatum();

  QString fmtStr;
  if ( !myDicItem.IsNull() )
    fmtStr = toQString( myDicItem->GetFormat( false ) );
  return fmtStr;
}

/*!
  Returns the datum default value string.
*/
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

/*!
  Returns the datum minimum value string.
*/
QString QDS_Datum::minimumValue() const
{
  initDatum();

  QString min;
  if ( !myDicItem.IsNull() && myDicItem->HasData( DDS_DicItem::MinValue ) )
      min = format( format(), type(), myDicItem->GetMinValue() );
  return min;
}

/*!
  Returns the datum maximum value string.
*/
QString QDS_Datum::maximumValue() const
{
  initDatum();

  QString max;
  if ( !myDicItem.IsNull() && myDicItem->HasData( DDS_DicItem::MaxValue ) )
    max = format( format(), type(), myDicItem->GetMaxValue() );
  return max;
}

/*!
  Returns the datum long description.
*/
QString QDS_Datum::longDescription() const
{
  initDatum();

  QString ldStr;
  if ( !myDicItem.IsNull() )
    ldStr = toQString( myDicItem->GetLongDescription() );
  return ldStr;
}

/*!
  Returns the datum short description.
*/
QString QDS_Datum::shortDescription() const
{
  initDatum();

  QString sdStr;
  if ( !myDicItem.IsNull() )
    sdStr = toQString( myDicItem->GetLongDescription() );
  return sdStr;
}

/*!
  Returns the list of option names.
*/
QStringList QDS_Datum::options() const
{
  QStringList res;
  if ( !dicItem().IsNull() )
  {
    TColStd_SequenceOfAsciiString lst;
    dicItem()->GetOptionNames( lst );
    for ( int i = 1; i <= lst.Length(); i++ )
      res.append( toQString( lst.Value( i ) ) );
  }
  return res;
}

/*!
  Returns the option specified by \aname as QVariant.
  If option not exist then not valid QVariant returned.
*/
QVariant QDS_Datum::option( const QString& name ) const
{
  QVariant res;
  if ( !dicItem().IsNull() )
    res = QVariant( toQString( dicItem()->GetOption( toAsciiString( name ) ) ) );
  return res;
}

/*!
  Returns the option specified by \aname as QString.
  If option not exist then empty string returned.
*/
QString QDS_Datum::optionString( const QString& name ) const
{
  QString res;
  if ( !dicItem().IsNull() )
    res = toQString( dicItem()->GetOption( toAsciiString( name ) ) );
  return res;
}

/*!
  Returns the option specified by \aname as double.
  If option not exist then 0 returned.
*/
double QDS_Datum::optionDouble( const QString& name ) const
{
  double res = 0;
  QVariant opt = option( name );
  if ( opt.isValid() && opt.canCast( QVariant::Double ) )
    res = opt.toDouble();
  return res;
}

/*!
  Returns the option specified by \aname as integer.
  If option not exist then 0 returned.
*/
int QDS_Datum::optionInteger( const QString& name ) const
{
  int res = 0;
  QVariant opt = option( name );
  if ( opt.isValid() && opt.canCast( QVariant::Int ) )
    res = opt.toInt();
  return res;
}

/*!
  Returns the datum value as variant (QVariant object).
*/
QVariant QDS_Datum::value() const
{
  QVariant val;
  if ( !isEmpty() )
    val = stringValue();
  return val;
}

/*!
  Returns the datum value as string (QString object).
*/
QString QDS_Datum::stringValue() const
{
  initDatum();

  if ( getString() == myTargetValue )
    return mySourceValue;
  else
    return getString();
}

/*!
  Returns the datum value as double. This value converted from units of measure in active unit system
  to units of measure in unit system "SI".
*/
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

/*!
  Returns the datum value as integer. This value converted from units of measure in active unit system
  to units of measure in unit system "SI".
*/
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

/*!
  Returns the text from datum. Text consist of label, string value and units.
*/
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

/*!
  Returns false if datum control has inputted value.
*/
bool QDS_Datum::isEmpty() const
{
  return stringValue().isEmpty();
}

/*!
  Reset datum state and set default value as current.
*/
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

/*!
  Clear the control.
*/
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

/*!
  Set varian value (QVariant object) into datum.
*/
void QDS_Datum::setValue( const QVariant& val )
{
  if ( val.isValid() && val.canCast( QVariant::String ) )
    setStringValue( val.toString() );
  else
    clear();
}

/*!
  Set string value (QString object) into datum.
*/
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

/*!
  Set double value into datum. This value converted from units of measure in unit system "SI"
  to units of measure in active unit system. Format the value using datum format if it required.
*/
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

/*!
  Set integer value into datum. This value converted from units of measure in unit system "SI"
  to units of measure in active unit system. Format the value using datum format if it required.
*/
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
  Returns true if all subwidgets specified by \aelement enabled.
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
  Enable/Disable subwidgets specified by \aelement.
  Possible values of \aelement: Label, Control, Units or their combinations.
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

/*!
  Show/hide subwidgets specified by \aelement.
  Possible values of \aelement: Label, Control, Units or their combinations.
*/
void QDS_Datum::setShown( const bool visible, const int flags )
{
  initDatum();

  uint flag = Units;
  while ( flag )
  {
    if ( flags & flag && widget( flag ) )
      widget( flag )->setShown( visible );
    flag = flag >> 1;
  }
}

/*!
  Show subwidgets specified by \aelement.
  Possible values of \aelement: Label, Control, Units or their combinations.
*/
void QDS_Datum::show( const int element )
{
  setShown( true, element );
}

/*!
  Hide subwidgets specified by \aelement.
  Possible values of \aelement: Label, Control, Units or their combinations.
*/
void QDS_Datum::hide( const int element )
{
  setShown( false, element );
}

/*!
  Returns subwidget specified by \aelement.
  Possible values of \aelement: Label, Control, Units.
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
  and display warning message box if parameter \amsgBox is set.
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
  Add widgets to the vertical box layout.
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
  Add widgets to the horizaontal box layout.
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

/*!
  Perform delayed initialisation. Reimplemented for internal reasons.
*/
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
  Notify about parameter value changing.
*/
void QDS_Datum::onParamChanged()
{
}

/*!
  Perform delayed initialization.
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
  Returns QLabel widget which contains dictionary item label.
*/
QLabel* QDS_Datum::labelWidget() const
{
  initDatum();
  return myLabel;
}

/*!
  Returns QLabel widget which contains dictionary item units.
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
  Creates QLabel widget for dictionary item label.
*/
QLabel* QDS_Datum::createLabel( QWidget* parent )
{
  return new QLabel( parent );
}

/*!
  Creates QLabel widget for dictionary item units.
*/
QLabel* QDS_Datum::createUnits( QWidget* parent )
{
  return new QLabel( parent );
}

/*!
  Creates and returns validator accordance to datum type of value.
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
  Retrieves information from dictionary and create subwidgets using virtual mechanism.
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

/*!
  Notification about active unit system changing. Update label and units texts.
*/
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
  Format the specified integer as dictionary item value.
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
  Format the specified double as dictionary item value.
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
  Format the specified string as dictionary item value.
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
  Format the given string accordance to dictionary item format.
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
  Format the given string accordance to dictionary item format.
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
  Format the given string accordance to dictionary item format.
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
  Returns displayable units string for given dictionary item id
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

/*!
  Remove the acceleartor tags '&' from specified label string \asrc.
*/
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

/*!
  Returns true if given format string \atheFormat has specificator for double values.
*/
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

/*!
  Returns datum flags.
*/
int QDS_Datum::flags() const
{
  return myFlags;
}

/*!
  Perform intialization if it needed. [internal]
*/
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

/*!
  Return wrapper for specified subwidget. [internal]
*/
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

/*!
  Return wrapper for specified subwidget name. [internal]
*/
QDS_Datum::Wrapper* QDS_Datum::wrapper( const int id ) const
{
  Wrapper* wrap = 0;
  if ( myWrapper.contains( id ) )
    wrap = myWrapper[id];
  return wrap;
}

/*!
  Return subwidget name for specified wrapper. [internal]
*/
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
