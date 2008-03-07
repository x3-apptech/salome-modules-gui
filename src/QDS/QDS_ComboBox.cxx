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
#include "QDS_ComboBox.h"

#include <DDS_Dictionary.h>

#include <TCollection_AsciiString.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfExtendedString.hxx>

#include <qlineedit.h>

/*
  \class QDS_ComboBox
  
  Datum with control corresponding to combo box. This control used for datum with enumerable values.
  It can be used for datum which has type of value 'List'. Each item of combobox defined two properties:
  integer identifier and string name. All operations on items performed via identifier.
*/

/*!
  Constructor. Create combobox datum object with datum identifier \aid under widget \aparent. Parameter \aflags
  define behaviour of datum and set of created subwidgets. Default value of this parameter is QDS::All.
  Parameter \acomp specify the component name which will be used during search of dictionary item.
*/
QDS_ComboBox::QDS_ComboBox( const QString& id, QWidget* parent, const int flags, const QString& comp )
: QDS_Datum( id, parent, flags, comp )
{
}

/*!
  Destructor.
*/
QDS_ComboBox::~QDS_ComboBox()
{
}

/*!
  Returns true if ComboBox allow to edit current text.
*/
bool QDS_ComboBox::editable() const
{
  if ( comboBox() && comboBox()->lineEdit() )
    return !comboBox()->lineEdit()->isReadOnly();
  else
    return false;
}

/*!
  Sets the possibily of current text editing.
*/
void QDS_ComboBox::setEditable( const bool on )
{
  QComboBox* aCombo = comboBox();
  if ( aCombo )
    aCombo->setEditable( on );
  if ( aCombo && aCombo->lineEdit() )
  {
    aCombo->lineEdit()->setReadOnly( !on );
    aCombo->clearValidator();
    if ( on )
      aCombo->setValidator( validator() );
  }
}

/*!
  Returns number of items in ComboBox. If \atotal is 'false' then only
  visible items are taken into account otherwise all items.
*/
int QDS_ComboBox::count( bool total ) const
{
  if ( total )
    return myValue.count();
  else if ( comboBox() )
    return comboBox()->count();
  else
    return 0;
}

/*!
  Returns list of list item identifiers \aids. If \atotal is 'false' then only visible items
  are taken into account otherwise all items.
*/
void QDS_ComboBox::values( QValueList<int>& ids, bool total ) const
{
  ids.clear();
  for ( QIntList::const_iterator it = myDataIds.begin(); it != myDataIds.end(); ++it )
    if ( total || ( myState.contains( *it ) && myState[*it] ) )
      ids.append( *it );
}

/*!
  Returns the current id as integer. Reimplemented.
*/
int QDS_ComboBox::integerValue() const
{
  QComboBox* cb = comboBox();
  QString cur = getString();
  if ( cb && cb->count() > 0 && cb->currentItem() >= 0 )
    cur = cb->text( cb->currentItem() );

  if ( cb && cur == getString() )
    return getId( cb->currentItem() );
  else
    return getId( getString() );
}

/*!
  Returns the current id as double. Reimplemented.
*/
double QDS_ComboBox::doubleValue() const
{
  initDatum();

  QComboBox* cb = comboBox();
  QString cur = getString();
  if ( cb && cb->count() > 0 && cb->currentItem() >= 0 )
    cur = cb->text( cb->currentItem() );

  if ( cb && cur == getString() )
    return getId( cb->currentItem() );
  else
    return getId( getString() );
}

/*!
  Set the current item acording to specified id. Reimplemented.
*/
void QDS_ComboBox::setIntegerValue( const int id )
{
  initDatum();

  if ( myValue.contains( id ) )
    setString( myValue[id] );
  else 
    setString( "" );
}

/*!
  Get the integer part of specified value and use it as new current identifier. Reimplemented.
*/
void QDS_ComboBox::setDoubleValue( const double val )
{
  initDatum();

  int id = (int)val;
  if ( myValue.contains( id ) )
    setString( myValue[id] );
  else if ( id == -1 )
    setString( "" );
}

/*!
  Returns visible state of item specified by \aid.
*/
bool QDS_ComboBox::state( const int id ) const
{
  bool state = false;
  if ( myState.contains( id ) )
    state = myState[id];
  return state;
}

/*!
  Sets the visible state of item specified by \aid. If \aid is -1 then specified
  state will be set to all items. If \aappend is set then keep status for other items
  otherwise status of other items will be cleared.
*/
void QDS_ComboBox::setState( const bool on, const int id, const bool append )
{
  QValueList<int> lst;
  if ( id < 0 )
  {
    for ( IdStateMap::Iterator it = myState.begin(); it != myState.end(); ++it )
      lst.append( it.key() );
  }
  else
    lst.append( id );

  setState( on, lst, append );
}

/*!
  Sets the visible state of items specified by list of identifiers \aids.
  If \aappend is set then keep status for other items otherwise status of other
  items will be cleared.
*/
void QDS_ComboBox::setState( const bool on, const QValueList<int>& ids, const bool append )
{
  initDatum();

  if ( ids.isEmpty() && append )
    return;

  bool changed = false;

  QMap<int, int> aMap;
  for ( uint i = 0; i < ids.count(); i++ )
    aMap.insert( *ids.at( i ), 0 );

  for ( IdStateMap::Iterator it = myState.begin(); it != myState.end(); ++it )
  {
    if ( aMap.contains( it.key() ) )
    {
      if ( it.data() != on )
      {
        it.data() = on;
        changed = true;
      }
    }
    else if ( !append && it.data() == on )
    {
      it.data() = !on;
      changed = true;
    }
  }
  if ( changed )
    updateComboBox();
}

/*!
  Sets the custom user items into the combo box. User items like standard dictionary
  list items will be added into the combobox. This functionality allow to user override
  items.
*/
void QDS_ComboBox::setValues( const QValueList<int>& ids, const QStringList& names )
{
  initDatum();

  if ( ids.count() != names.count() )
    return;

  myUserIds = ids;
  myUserNames = names;
}

/*!
  This is an overloaded member function, provided for convenience.
  It behaves essentially like the above function. It creates
  QValueList (0, 1, 2 ... ) and call previous method.
*/
void QDS_ComboBox::setValues( const QStringList& names )
{
  initDatum();

  QValueList< int > ids;
  for ( int i = 0, n = names.count(); i < n; i++ )
    ids.append( i );
  setValues( ids, names );
}

/*!
  Sets the active item as item with default id. If default
  not defined then first item will be used.
*/
void QDS_ComboBox::reset()
{
  int id = -1;
  QString aDefValue = defaultValue();
  if ( !aDefValue.isEmpty() )
    id = aDefValue.toInt();

  if ( id == -1 )
    id = getId( 0 );

  setIntegerValue( id );
}

/*!
  Returns identifier from given ComboBox string item.
*/
int QDS_ComboBox::stringToValue( const QString& str ) const
{
  return getId( str );
}

/*!
  Returns ComboBox string item from given identifier.
*/
QString QDS_ComboBox::valueToString( const int val ) const
{
  QString str;
  if ( myValue.contains( val ) )
    str = myValue[val];
  return str;
}

/*!
  Returns string from QComboBox widget. Reimplemented.
*/
QString QDS_ComboBox::getString() const
{
  QString res;
  QtxComboBox* cb = comboBox();
  if ( cb )
  {
    if ( !cb->editable() )
    {
      if ( !cb->isCleared() )
        res = cb->currentText(); 
    }
    else
      res = cb->lineEdit()->text();
  }
  return res;
}

/*!
  Sets the string into QComboBox widget. Reimplemented.
*/
void QDS_ComboBox::setString( const QString& txt )
{
  QtxComboBox* cb = comboBox();
  if ( !cb )
    return;

  bool isClear = cb->isCleared();
  
  int idx = -1;
  for ( int i = 0; i < cb->count() && idx == -1; i++ )
    if ( cb->text( i ) == txt )
      idx = i;

  int old = cb->currentItem();
  if ( idx != -1 )
    cb->setCurrentItem( idx );
  else if ( txt.isEmpty() )
  {
    if ( !cb->editable() )
      cb->setCurrentText( txt );
    else
      cb->lineEdit()->setText( txt );
  }
  if ( isClear != txt.isEmpty() || ( !isClear && old != cb->currentItem() ) )
  {
    onParamChanged();
    QString str = getString();
    emit activated( integerValue() );
    emit activated( str );
    emit paramChanged();
    emit paramChanged( str );
  }
}

/*!
  Returns pointer to QtxComboBox widget.
*/
QtxComboBox* QDS_ComboBox::comboBox() const
{
  return ::qt_cast<QtxComboBox*>( controlWidget() );
}

/*!
  Create QComboBox widget as control subwidget. Reimplemented.
*/
QWidget* QDS_ComboBox::createControl( QWidget* parent )
{
  QtxComboBox* cb = new QtxComboBox( parent );
  cb->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );
  connect( cb, SIGNAL( textChanged( const QString& ) ), this,
           SLOT( onTextChanged( const QString& ) ) );
  connect( cb, SIGNAL( activated( int ) ), this, SLOT( onActivated( int ) ) );
  return cb;
}

/*!
  Notification about active unit system changing. Reimplemented from QDS_Datum.
  Update combobox content.
*/
void QDS_ComboBox::unitSystemChanged( const QString& system )
{
  QDS_Datum::unitSystemChanged( system );

  Handle(TColStd_HArray1OfInteger) anIds;
  Handle(TColStd_HArray1OfExtendedString) aValues, anIcons;

  Handle(DDS_DicItem) aDicItem = dicItem();
  if ( !aDicItem.IsNull() )
    aDicItem->GetListOfValues( aValues, anIds, anIcons );

  myValue.clear();
  myIcons.clear();
  myDataIds.clear();

  QMap<int, QString> userMap;
  QIntList::iterator iIt = myUserIds.begin();
  QStringList::iterator sIt = myUserNames.begin();
  for ( ; iIt != myUserIds.end() && sIt != myUserNames.end(); ++iIt, ++sIt )
    userMap.insert( *iIt, *sIt );

  if ( !anIds.IsNull() && !aValues.IsNull() &&
       anIds->Length() == aValues->Length() )
  {
    for ( int i = anIds->Lower(); i <= anIds->Upper(); i++ )
    {
      QString aValue;
      QPixmap aPixmap;
      int id = anIds->Value( i );
      if ( userMap.contains( id  ) )
        aValue = userMap[id];
      else
      {
        aValue = toQString( aValues->Value( i ) );
        if ( !anIcons.IsNull() && i <= anIcons->Upper() )
        {
          QString anIconId = toQString( anIcons->Value( i ) );
          if ( anIconId != "" )
            aPixmap = QPixmap( anIconId );
        }
      }

      myDataIds.append( id );
      myValue.insert( id, aValue );
      myState.insert( id, true );
      if ( !aPixmap.isNull() )
        myIcons.insert( id, aPixmap );
    }
  }

  for ( iIt = myUserIds.begin(); iIt != myUserIds.end(); ++iIt )
  {
    int id = *iIt;
    if ( !myValue.contains( id  ) )
    {
      myDataIds.append( id );
      myValue.insert( id, userMap[id] );
    }
  }

  QIntList del, add;
  for ( IdStateMap::Iterator it1 = myState.begin(); it1 != myState.end(); ++it1 )
    if ( !myValue.contains( it1.key() ) )
      del.append( it1.key() );

  for ( IdValueMap::Iterator it2 = myValue.begin(); it2 != myValue.end(); ++it2 )
    if ( !myState.contains( it2.key() ) )
      add.append( it2.key() );

  for ( QIntList::iterator iter1 = del.begin(); iter1 != del.end(); ++iter1 )
    myState.remove( *iter1 );

  for ( QIntList::iterator iter2 = add.begin(); iter2 != add.end(); ++iter2 )
    myState.insert( *iter2, true );

  updateComboBox();
}

/*!
  Notify about text changing in line edit of ComboBox.
*/
void QDS_ComboBox::onTextChanged( const QString& )
{
  onParamChanged();
  emit paramChanged();
  QString str = getString();
  emit paramChanged( str );
}

/*!
  Notify about activation combobox item.
*/
void QDS_ComboBox::onActivated( int idx )
{
  if ( comboBox() )
    comboBox()->setCurrentItem( comboBox()->currentItem() );

  int id = getId( idx );
  if ( id != -1 )
  {
    onParamChanged();
    QString str = getString();
    emit activated( id );
    emit activated( str );
    emit paramChanged();
    emit paramChanged( str );
  }
}

/*!
  Updates ComboBox after have change of visible state or items have been inserted/removed.
*/
void QDS_ComboBox::updateComboBox()
{
  QtxComboBox* cb = comboBox();

  int curId = -1;

  bool isClear = false;

  if ( cb )
  {
    isClear = cb->isCleared();

    curId = getId( cb->currentItem() );
    cb->clear();
  }

  myIndex.clear();

  int idx = 0;
  for ( QIntList::const_iterator it = myDataIds.begin(); it != myDataIds.end(); ++it )
  {
    int id = *it;
    if ( !myValue.contains( id ) || !myState.contains( id ) || !myState[id] )
      continue;

    myIndex.insert( id, idx++ );
    if ( cb )
    {
      if ( myIcons.contains( id ) )
        cb->insertItem( myIcons[id], myValue[id] );
      else
        cb->insertItem( myValue[id] );
    }
  }

  if ( cb && cb->count() )
  {
    cb->setFont( cb->font() );
    cb->updateGeometry();

    if ( isClear )
      cb->setCurrentText( "" );
    else
    {
      if ( getIndex( curId ) != -1 )
        cb->setCurrentItem( getIndex( curId ) );
      if ( curId != getId( cb->currentItem() ) )
        onActivated( cb->currentItem() );
    }
  }
}

/*!
  Returns index of ComboBox item according to id.
*/
int QDS_ComboBox::getIndex( const int id ) const
{
  int idx = -1;
  if ( myIndex.contains( id ) )
    idx = myIndex[id];
  return idx;
}

/*!
  Returns index of ComboBox item according to string.
*/
int QDS_ComboBox::getIndex( const QString& str ) const
{
  int idx = -1;
  QComboBox* cb = comboBox();
  if ( cb )
  {
    for ( int i = 0; i < cb->count() && idx == -1; i++ )
      if ( cb->text( i ) == str )
        idx = i;
  }
  return idx;
}

/*!
  Returns id according to ComboBox item index.
*/
int QDS_ComboBox::getId( const int idx ) const
{
  int id = -1;
  IdIndexMap::ConstIterator it = myIndex.begin();
  for (; it != myIndex.end() && id == -1; ++it )
    if ( it.data() == idx )
      id = it.key();
  return id;
}

/*!
  Returns id according to ComboBox item string.
*/
int QDS_ComboBox::getId( const QString& str ) const
{
  int id = -1;
  int candidate = -1;
  IdValueMap::ConstIterator it = myValue.begin();
  for (; it != myValue.end() && id == -1; ++it )
  {
    if ( it.data() == str )
    {
      if ( state( it.key() ) )
        id = it.key();
      else
        candidate = it.key();
    }
  }
  if ( id == -1 )
    id = candidate;

  return id;
}
