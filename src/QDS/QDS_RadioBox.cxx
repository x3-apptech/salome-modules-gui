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
#include "QDS_RadioBox.h"

#include <DDS_Dictionary.h>

#include <TCollection_AsciiString.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfExtendedString.hxx>

#include <qobjectlist.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>

/*
  \class QDS_RadioBox
  
  Datum with control corresponding to button group with set of exclusive radio buttons.
  This control used for datum with enumerable values. It can be used for datum which has
  type of value 'List'. Each radio button of combobox defined two properties:
  integer identifier and string name. All operations on radio buttons performed via identifier.

  If datum label text is specified then it displayed in group box title.
*/

/*!
  Constructor. Create radio button box datum object with datum identifier \aid under widget \aparent.
  Parameter \aflags define behaviour of datum and set of created subwidgets. Default value of this
  parameter is QDS::Control. Parameter \acomp specify the component name which will be used during
  search of dictionary item.
*/
QDS_RadioBox::QDS_RadioBox( const QString& id, QWidget* parent, const int flags, const QString& comp )
: QDS_Datum( id, parent, flags & ~( Label | Units ), comp )
{
}

/*!
  Destructor.
*/
QDS_RadioBox::~QDS_RadioBox()
{
}

/*!
  Returns number of buttons in radio box. If total is 'false' then only
  visible buttons are taken into account otherwise all buttons.
*/
int QDS_RadioBox::count( bool total ) const
{
  if ( total )
    return myValue.count();
  else
  {
    QPtrList<QRadioButton> bList;
    buttons( bList );
    return bList.count();
  }
}

/*!
  Returns list of button identifiers \aids. If \atotal is 'false' then only visible
  buttons are taken into account otherwise all buttons.
*/
void QDS_RadioBox::values( QValueList<int>& ids, bool total ) const
{
  ids.clear();
  for ( QIntList::const_iterator it = myDataIds.begin(); it != myDataIds.end(); ++it )
    if ( total || ( myState.contains( *it ) && myState[*it] ) )
      ids.append( *it );
}

/*!
  Returns visible state of button specified by \aid.
*/
bool QDS_RadioBox::state( const int id ) const
{
  bool state = false;
  if ( myState.contains( id ) )
    state = myState[id];
  return state;
}

/*!
  Sets the visible state of button specified by \aid. If \aid is -1 then specified
  state will be set to all buttons. If \aappend is set then keep status for other
  buttons otherwise status of other buttons will be cleared.
*/
void QDS_RadioBox::setState( const bool on, const int id, const bool append )
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
  Sets the visible state of buttons specified by list of identifiers \aids.
  If \aappend is set then keep status for other buttons otherwise status of other
  buttons will be cleared.
*/
void QDS_RadioBox::setState( const bool on, const QValueList<int>& ids, const bool append )
{
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
    updateRadioBox();
}

/*!
  Sets the custom user buttons into the radio box. User buttons like standard dictionary
  button from list will be added into the radio box. This functionality allow to user override
  buttons.
*/
void QDS_RadioBox::setValues( const QValueList<int>& ids, const QStringList& names )
{
  if ( ids.count() != names.count() )
    return;

  myUserIds = ids;
  myUserNames = names;
}

/*!
  This is an overloaded member function, provided for convenience.
  It behaves essentially like the above function. It creates
  QValueList (0, 1, 2 ... ) and call previous method
*/
void QDS_RadioBox::setValues( const QStringList& names )
{
  QValueList< int > ids;
  for ( int i = 0, n = names.count(); i < n; i++ )
    ids.append( i );
  setValues( ids, names );
}

/*!
  Returns string from radio box. Reimplemented. String which contains identifier of
  currently selected button returned.
*/
QString QDS_RadioBox::getString() const
{
  QString res;
  QButtonGroup* bg = buttonGroup();
  if ( bg )
  {
    int id = bg->selectedId();
    if ( id != -1 )
      res = QString::number( id );
  }
  return res;
}

/*!
  Sets the string into radio box. Reimplemented. Button with identifier from specified
  string \atxt became selected in radio box.
*/
void QDS_RadioBox::setString( const QString& txt )
{
  QButtonGroup* bg = buttonGroup();
  if ( !bg )
    return;

  int oldId = bg->selectedId();

  if ( txt.isEmpty() )
  {
    QPtrList<QRadioButton> bList;
    buttons( bList );
    for ( QPtrListIterator<QRadioButton> it( bList ); it.current(); ++it )
      it.current()->setChecked( false );
  }
  else
  {
    bool ok;
    int id = txt.toInt( &ok );
    if ( !ok )
      id = -1;

    bool block = signalsBlocked();
    blockSignals( true );
    bg->setButton( id );
    blockSignals( block );
  }

  int newId = bg->selectedId();

  if ( oldId != newId )
  {
    onParamChanged();
    QString str = getString();
    emit activated( newId );
    emit paramChanged();
    emit paramChanged( str );
  }
}

/*!
  Returns pointer to QButtonGroup widget.
*/
QButtonGroup* QDS_RadioBox::buttonGroup() const
{
  return ::qt_cast<QButtonGroup*>( controlWidget() );
}

/*!
  Create QButtonGroup widget as control subwidget.
*/
QWidget* QDS_RadioBox::createControl( QWidget* parent )
{
  QButtonGroup* bg = new QButtonGroup( 1, Qt::Vertical, "", parent );
  bg->setExclusive( true );
  bg->setRadioButtonExclusive( true );
  return bg;
}

/*!
  Notification about active unit system changing. Reimplemented from QDS_Datum.
  Update radio box content.
*/
void QDS_RadioBox::unitSystemChanged( const QString& system )
{
  QDS_Datum::unitSystemChanged( system );

  Handle(TColStd_HArray1OfInteger) anIds;
  Handle(TColStd_HArray1OfExtendedString) aValues, anIcons;

  Handle(DDS_DicItem) aDicItem = dicItem();
  if ( !aDicItem.IsNull() )
    aDicItem->GetListOfValues( aValues, anIds, anIcons );

  myValue.clear();
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
      int id = anIds->Value( i );
      if ( userMap.contains( id  ) )
        aValue = userMap[id];
      else
        aValue = toQString( aValues->Value( i ) );

      myDataIds.append( id );
      myValue.insert( id, aValue );
      myState.insert( id, true );
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

  QButtonGroup* bg = buttonGroup();
  if ( bg )
    bg->setTitle( label() );

  updateRadioBox();
}

/*!
  Notify about activation radio button.
*/
void QDS_RadioBox::onToggled( bool on )
{
  if ( !on )
    return;

  onParamChanged();
  emit paramChanged();
  QString str = getString();
  emit paramChanged( str );
}

/*!
  Updates RadioBox after have change of visible state or buttons have been inserted/removed.
*/
void QDS_RadioBox::updateRadioBox()
{
  QButtonGroup* bg = buttonGroup();
  if ( !bg )
    return;

  int curId = bg->selectedId();

  QPtrList<QRadioButton> bList;
  buttons( bList );
  for ( QPtrListIterator<QRadioButton> itr( bList ); itr.current(); ++itr )
    delete itr.current();

  for ( QIntList::const_iterator it = myDataIds.begin(); it != myDataIds.end(); ++it )
  {
    int id = *it;
    if ( !myValue.contains( id ) || !myState.contains( id ) || !myState[id] )
      continue;

    QRadioButton* rb = new QRadioButton( myValue[id], bg );
    bg->insert( rb, id );

    connect( rb, SIGNAL( toggled( bool ) ), this, SLOT( onToggled( bool ) ) );
  }

  if ( curId != -1 )
  {
    int id = curId;
    if ( !bg->find( id ) )
    {
      QPtrList<QRadioButton> bList;
      buttons( bList );
      if ( !bList.isEmpty() )
        id = bg->id( bList.getFirst() );
    }

    bool block = signalsBlocked();
    blockSignals( true );
    bg->setButton( id );
    blockSignals( block );
  }

  if ( curId != bg->selectedId() )
  {
    onParamChanged();
    emit paramChanged();
    emit paramChanged( getString() );
  }
}

/*!
  Returns the list of the radio buttons from the button group.
*/
void QDS_RadioBox::buttons( QPtrList<QRadioButton>& lst ) const
{
  lst.setAutoDelete( false );
  lst.clear();

  QButtonGroup* bg = buttonGroup();
  if ( !bg )
    return;

  QObjectList* objs = bg->queryList( "QRadioButton" );
  if ( objs )
  {
    for ( QObjectListIt it( *objs ); it.current(); ++it )
    {
      QRadioButton* rb = ::qt_cast<QRadioButton*>( it.current() );
      if ( rb )
        lst.append( rb );
    }
  }
  delete objs;
}
