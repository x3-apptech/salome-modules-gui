// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
// File:      QtxComboBox.cxx
// Author:    Sergey TELKOV

#include "QtxComboBox.h"

#include <QStandardItemModel>
#include <QLineEdit>
#include <QEvent>
#include <QApplication>

/*!
  \class QtxComboBox::Model
  \brief Internal view model, used to process 'cleared' state of the combo box.
  \internal
*/
class QtxComboBox::Model : public QStandardItemModel
{
public:
  Model( QObject* parent = 0 );
  ~Model();
  void setCleared( const bool );

  QVariant data( const QModelIndex&, int = Qt::DisplayRole ) const;

private:
  bool   myCleared;
};

/*!
  \brief Constructor.
  \internal
  \param parent parent object
*/
QtxComboBox::Model::Model( QObject* parent )
  : QStandardItemModel( 0, 1, parent ),
    myCleared( false )
{
}

/*!
  \brief Destructor.
  \internal
*/
QtxComboBox::Model::~Model()
{
}

/*!
  \brief Set 'cleared' state.
  \param isClear new 'cleared' state
  \internal
*/
void QtxComboBox::Model::setCleared( const bool isClear )
{
  if ( myCleared == isClear )
    return;
  
  myCleared = isClear;
}

/*!
  \brief Get model data.
  \param index model index
  \param role data role
  \return data of \a role for given \a index
  \internal
*/
QVariant QtxComboBox::Model::data( const QModelIndex& index, int role ) const
{
  return ( myCleared && ( role == Qt::DisplayRole || role == Qt::DecorationRole ) ) ?
    QVariant() : QStandardItemModel::data( index, role );
}

/*!
  \class QtxComboBox::ClearEvent
  \brief Custom event, used to process 'cleared' state of the combo box
  in editable mode.
  \internal
*/

#define CLEAR_EVENT QEvent::Type( QEvent::User + 123 )

class QtxComboBox::ClearEvent : public QEvent
{
public:
  ClearEvent();
};

/*!
  \brief Constructor.
  \internal
*/
QtxComboBox::ClearEvent::ClearEvent() : QEvent( CLEAR_EVENT )
{
}

/*!
  \class QtxComboBox
  \brief Enhanced version of Qt combo box class.

  In addition to the QComboBox class, QtxComboBox supports 
  adding/removing the items with the associated unique identifiers.
  It also provides a way to set "cleared" state to the combo box -
  when no item is selected.
*/

/*!
  \brief Constructor.
  \param parent parent widget
*/
QtxComboBox::QtxComboBox( QWidget* parent )
: QComboBox( parent ),
  myCleared( false )
{
  connect( this, SIGNAL( currentIndexChanged( int ) ),  this, SLOT( onCurrentChanged( int ) ) );
  setModel( new Model( this ) );
}

/*!
  \brief Destructor.
*/
QtxComboBox::~QtxComboBox()
{
}

/*!
  \brief Check if combo box is in "cleared" state.
  \return \c true if combo box is in "cleared" state or \c false otherwise
*/
bool QtxComboBox::isCleared() const
{
  return myCleared;
}

/*!
  \brief Set "cleared" state.
  \param isClear new "cleared" state
*/
void QtxComboBox::setCleared( const bool isClear )
{
  if ( myCleared == isClear )
    return;
    
  myCleared = isClear;

  if ( lineEdit() )
    lineEdit()->setText( myCleared ? QString( "" ) : itemText( currentIndex() ) );

  update();
}

/*!
  \brief Get current item's identifier.
  \return current item's identifier
*/
QVariant QtxComboBox::currentId() const
{
  return id( currentIndex() );
}

/*!
  \brief Set current item by identifier.
  \param ident item's identifier
*/
void QtxComboBox::setCurrentId( const QVariant& ident )
{
  setCurrentIndex( index( ident ) );
}

/*!
  \brief Assign identifier to specified item.
  \param idx item's index
  \param ident item's identifier
*/
void QtxComboBox::setId( const int idx, const QVariant& ident )
{
  setItemData( idx, ident, (Qt::ItemDataRole)IdRole );
}

/*!
  \brief Customize paint event.
  \param e paint event
*/
void QtxComboBox::paintEvent( QPaintEvent* e )
{
  Model* m = dynamic_cast<Model*>( model() );
  if ( m )
    m->setCleared( myCleared );
  QComboBox::paintEvent( e );
  if ( m )
    m->setCleared( false );
}

/*!
  \brief Customize child addition/removal event.
  \param e child event
*/
void QtxComboBox::childEvent( QChildEvent* e )
{
  if ( ( e->added() || e->polished() ) && qobject_cast<QLineEdit*>( e->child() ) )
    QApplication::postEvent( this, new ClearEvent() );
}

/*!
  \brief Process custom events
  \param e custom event
*/
void QtxComboBox::customEvent( QEvent* e )
{
  if ( e->type() == CLEAR_EVENT && lineEdit() && myCleared )
    lineEdit()->setText( "" );
}

/*!
  \brief Called when current item is changed (by user or programmatically).
  \param idx index of item being set current
*/
void QtxComboBox::onCurrentChanged( int idx )
{
  if ( idx != -1 )
  {
    resetClear();
    QVariant ident = id( idx );
    emit activatedId( id( idx ) );
    if ( ident.type() == QVariant::Int )
      emit activatedId( ident.toInt() );
    else if ( ident.type() == QVariant::String )
      emit activatedId( ident.toString() );
  }
}

/*!
  \brief Reset "cleared" state and update combo box.
*/
void QtxComboBox::resetClear()
{
  if ( !myCleared )
    return;
  
  myCleared = false;
  update();
}

/*!
  \brief Get item's identifier by index.
  \param idx item's index
  \return item's identifier or invalid QVariant if index is out of range
  or identifier is not assigned to item
*/
QVariant QtxComboBox::id( const int idx ) const
{
  return itemData( idx, (Qt::ItemDataRole)IdRole );
}

/*!
  \brief Get item index by identifier.
  \param ident item's identifier
  \return item's index or -1 if \a ident is invalid of if item is not found
*/
int QtxComboBox::index( const QVariant& ident ) const
{
  if ( !ident.isValid() ) return -1;
  return findData( ident, (Qt::ItemDataRole)IdRole );
}

/*!
  \brief Check if item has assigned identifier.
  \param idx item's index
  \return \c true if item with given index has identifier of \c false otherwise
*/
bool QtxComboBox::hasId( const int idx ) const
{
  QVariant v = itemData( idx, (Qt::ItemDataRole)IdRole );
  return !v.isNull();
}

/*!
  \fn void QtxComboBox::activatedId( QVariant ident )
  \brief Emitted when item with identificator \a ident is activated.
  \param ident item's identifier
*/

/*!
  \fn void QtxComboBox::activatedId( int ident )
  \brief Emitted when item with integer identificator \a ident is activated.
  \param ident item's identifier
*/

/*!
  \fn void QtxComboBox::activatedId( QString ident )
  \brief Emitted when item with string identificator \a ident is activated.
  \param ident item's identifier
*/
