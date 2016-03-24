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

// File   : SUIT_DataObject.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#include <QVariant>

#include "SUIT_DataObject.h"
#include "SUIT_DataObjectKey.h"
#include <cstdio>

SUIT_DataObject::Signal* SUIT_DataObject::mySignal = 0;

/*!
  \class SUIT_DataObject
  \brief Data object representing the data instance in the tree-like hierarchy.

  Data object represents uniform data tree structure recommended to use in the
  SUIT-based applications.
*/

/*!
  \brief Constructor.

  Creates the data object with the specified parent.
  To create the top-level object, pass 0 as parameter.

  \param p parent object
*/
SUIT_DataObject::SUIT_DataObject( SUIT_DataObject* p )
: myParent( 0 ),
  myOpen( false ),
  myCheck( false ),
  myAutoDel( true ),
  _modified( false )
{
  setParent( p );
  signal()->emitCreated( this );
}

/*!
  \brief Destructor.

  Destroys all the children if "auto-delete children" flag is set.
*/
SUIT_DataObject::~SUIT_DataObject()
{
  SUIT_DataObject* p = myParent;

  myParent = 0;

  if ( p )
    p->removeChild( this );

  signal()->emitDestroyed( this );

  for ( DataObjectList::iterator it = myChildren.begin(); it != myChildren.end(); ++it )
    (*it)->myParent = 0;

  if ( autoDeleteChildren() )
  {
    for ( DataObjectList::iterator itr = myChildren.begin(); itr != myChildren.end(); ++itr )
      delete *itr;
  }
}

/*!
  \brief Get the root object.
  \return root object of the data tree
*/
SUIT_DataObject* SUIT_DataObject::root() const
{
  return parent() ? parent()->root() : (SUIT_DataObject*)this;
}

/*!
  \brief Get the first child object.
  \return first child object or 0 if there are no children
  \sa lastChild()
*/
SUIT_DataObject* SUIT_DataObject::firstChild() const
{
  SUIT_DataObject* child = 0;
  if ( !myChildren.isEmpty() )
    child = myChildren.first();
  return child;
}

/*!
  \brief Get the last child object.
  \return last child object or 0 if there are no children
  \sa firstChild()
*/
SUIT_DataObject* SUIT_DataObject::lastChild() const
{
  SUIT_DataObject* child = 0;
  if ( !myChildren.isEmpty() )
    child = myChildren.last();
  return child;
}

/*!
  \brief Get the number of the child objects.
  \return number of the children
*/
int SUIT_DataObject::childCount() const
{
  return myChildren.count();
}

/*!
  \brief Get the index of the specified object in the child list.
  \param obj child object
  \return subobject position or -1 if it does not belong to this object
*/
int SUIT_DataObject::childPos( const SUIT_DataObject* obj ) const
{
  return myChildren.indexOf( (SUIT_DataObject*)obj );
}

/*!
  \brief Moves the child position from current to new one.
  \param theObj child object
  \param theNewPos child objectnew position in the children list

*/
void SUIT_DataObject::moveChildPos( SUIT_DataObject* theObj, int theNewPos)
{
  if (myChildren.size() <= 1)  return;

  int aNewPos = theNewPos;
  if (aNewPos < 0) aNewPos = 0;
  if (aNewPos > (myChildren.size() - 1)) aNewPos = myChildren.size() - 1;

  if (myChildren.removeOne(theObj))
    myChildren.insert(aNewPos, theObj);
}


/*!
  \brief Get child object by the specified index.
  \param idx child object index
  \return child object or 0 if index is out of range
*/
SUIT_DataObject* SUIT_DataObject::childObject( const int idx ) const
{
  SUIT_DataObject* child = 0;

  if ( idx >= 0 && idx < myChildren.count() )
    child = myChildren.at( idx );

  return child;
}

/*!
  \brief Get the object level in the tree structure.

  Root object has level 0.

  \return object level.
*/
int SUIT_DataObject::level() const
{
  int lev = 0;
  SUIT_DataObject* p = parent();
  while ( p ) {
    p = p->parent();
    lev++;
  }
  return lev;
}

/*!
  \brief Get the position of the data object in its parent's children list
  \return data object position
*/
int SUIT_DataObject::position() const
{
  return myParent ? myParent->childPos( this ) : 0;
}

/*!
  \brief Sets new position of the object in parent's list
*/
void SUIT_DataObject::setPosition(int theNewPos)
{
  if (theNewPos == position())  return;
  if (!myParent)  return;
  myParent->moveChildPos(this, theNewPos);
}

/*!
  \brief Get the next sibling data object in the children list.
  \return child object or 0 if there is no next sibling
  \sa prevBrother()
*/
SUIT_DataObject* SUIT_DataObject::nextBrother() const
{
  return myParent ? myParent->childObject( myParent->childPos( this ) + 1 ) : 0;
}

/*!
  \brief Get the previous sibling data object in the children list.
  \return child object or 0 if there is no previous sibling
  \sa nextBrother()
*/
SUIT_DataObject* SUIT_DataObject::prevBrother() const
{
  return myParent ? myParent->childObject( myParent->childPos( this ) - 1 ) : 0;
}

/*!
  \brief Get "auto-delete children" flag.
  \return \c true if the object should delete all its children on destroying
  \sa setAutoDeleteChildren()
*/
bool SUIT_DataObject::autoDeleteChildren() const
{
  return myAutoDel;
}

/*!
  \brief Set "auto-delete children" flag.

  If this flag is on (default), the object will delete
  all its children on destroying.

  \param on new flag value
  \sa autoDeleteChildren()
*/
void SUIT_DataObject::setAutoDeleteChildren( const bool on )
{
  myAutoDel = on;
}

/*!
  \brief Get all children.

  If parameter \a rec is \c true then function collects all
  the children recursively.

  \param lst returning list of children
  \param rec if \c true collect all children recursively
*/
void SUIT_DataObject::children( DataObjectList& lst, const bool rec ) const
{
  for ( DataObjectList::const_iterator it = myChildren.begin(); it != myChildren.end(); ++it )
  {
    lst.append( *it );
    if ( rec )
      (*it)->children( lst, rec );
  }
}

/*!
  \brief Get all children.
  \override

  If parameter \a rec is \c true then function collects all
  the children recursively.

  \param rec if \c true collect all children recursively
  \return list of children
*/
DataObjectList SUIT_DataObject::children( const bool rec )
{
  DataObjectList lst;
  children( lst, rec );
  return lst;
}

/*!
  \brief Add new child object to the end of the children list.
  \param obj child object being added
*/
void SUIT_DataObject::appendChild( SUIT_DataObject* obj )
{
  insertChild( obj, myChildren.count() );
}

/*!
  \brief Insert new child object to the list of the children.
  \param obj child object being added
  \param position child position
*/
void SUIT_DataObject::insertChild( SUIT_DataObject* obj, int position )
{
  if ( !obj || myChildren.contains( obj ) )
    return;

  int pos = position < 0 ? myChildren.count() : position;
  myChildren.insert( qMin( pos, (int)myChildren.count() ), obj );
  obj->setParent( this );
  signal()->emitInserted( obj, this );
}

/*!
  \brief Insert new child object into the list of the children (faster version of insertChild without signal).
  \param obj child object being added
  \param position child position
*/
void SUIT_DataObject::insertChildAtPos( SUIT_DataObject* obj, int position )
{
  if ( !obj )return;
  int pos = position < 0 ? myChildren.count() : position;
  myChildren.insert( qMin( pos, (int)myChildren.count() ), obj );
  obj->assignParent( this );
}

/*!
  \brief Remove the specified child object reference.
  \param obj child object being removed
  \param del if \c true, the child object is destroyed
*/
void SUIT_DataObject::removeChild( SUIT_DataObject* obj, const bool del )
{
  if ( !obj )
    return;

  if ( myChildren.removeAll( obj ) ) {
    signal()->emitRemoved( obj, this );
    obj->setParent( 0 );

    if ( del )
      obj->deleteLater();
  }
}

/*!
  \brief Replace the specified child object by another object.
  \param src child object being replaced
  \param trg new child object
  \param del if \c true, the previous object is destroyed
  \return \c true if the object has been replaced
*/
bool SUIT_DataObject::replaceChild( SUIT_DataObject* src, SUIT_DataObject* trg, const bool del )
{
  if ( !src || !trg )
    return false;

  int idx = childPos( trg );
  removeChild( trg );

  int pos = childPos( src );
  if ( pos < 0 )
  {
    if ( idx >= 0 )
      insertChild( trg, idx );
    return false;
  }

  insertChild( trg, pos );
  removeChild( src );

  if ( del )
    src->deleteLater();

  return true;
}

/*!
  \brief Change the parent for all children from specified object to this one.
  \param obj object which children to be reparented
*/
void SUIT_DataObject::reparentChildren( const SUIT_DataObject* obj )
{
  if ( !obj )
    return;

  DataObjectList lst;
  obj->children( lst );
  for ( DataObjectList::iterator it = lst.begin(); it != lst.end(); ++it )
    (*it)->setParent( this );
}

/*!
  \brief Get the parent object.
  \return parent object or 0 if this is top-level item
*/
SUIT_DataObject* SUIT_DataObject::parent() const
{
  return myParent;
}

/*!
  \brief Change the parent object.
  \param p new parent object
*/
void SUIT_DataObject::setParent( SUIT_DataObject* p )
{
  if ( p == parent() )
    return;

  if ( parent() )
    parent()->removeChild( this );

  myParent = p;

  if ( parent() )
    parent()->appendChild( this );
}

void SUIT_DataObject::assignParent( SUIT_DataObject* p )
{
  if ( p == myParent )
    return;

  myParent = p;
}

/*!
  \brief Sets modification state of the object.

  When the object has been modified (modified is set to true)
  a signal is emitted to notify the tree model and eventually redraw the data object.

  \param modified modified state 
*/
void SUIT_DataObject::setModified(bool modified)
{
  if ( _modified == modified )
    return;

  _modified = modified;
  if ( _modified )
    signal()->emitModified( this );
}

/*!
  \brief Get data object name.

  This method should be re-implemented in the subclasses.
  Default implementation returns null string.

  \return object name
*/
QString SUIT_DataObject::name() const
{
  return QString();
}

/*!
  \brief Get object text data for the specified column.

  This method can be re-implemented in the subclasses.
  Default implementation returns null string.

  Column with \a id = 0 (NameId) is supposed to be used
  to get the object name (as it does the default implementation).

  \param id column id
  \return object text data
*/
QString SUIT_DataObject::text( const int id ) const
{
  return id == NameId ? name() : QString();
}

/*!
  \brief Get data object icon for the specified column.

  This method can be re-implemented in the subclasses.
  Default implementation returns null pixmap.

  The parameter \a id specifies the column identificator

  \param id column id
  \return object icon for the specified column
*/
QPixmap SUIT_DataObject::icon( const int /*id*/ ) const
{
  return QPixmap();
}

/*!
  \brief Get data object color for the specified column.

  This method can be re-implemented in the subclasses.
  Default implementation returns null color.

  The parameter \a id specifies the column identificator

  \param role color role
  \param id column id
  \return object color for the specified column
*/
QColor SUIT_DataObject::color( const ColorRole /*role*/, const int /*id*/ ) const
{
  return QColor();
}

/*!
  \brief Get data object tooltip for the specified column.

  This method can be re-implemented in the subclasses.
  Default implementation returns null string.

  The parameter \a id specifies the column identificator
  (to display, for example, in the tree view widget).

  \param id column id
  \return object tooltip for the specified column
*/
QString SUIT_DataObject::toolTip( const int /*id*/ ) const
{
  return QString();
}

/*!
  \brief Get data object status tip for the specified column.

  This method can be re-implemented in the subclasses.
  Default implementation returns null string.

  The parameter \a id specifies the column identificator

  \param id column id
  \return object status tip for the specified column
*/
QString SUIT_DataObject::statusTip( const int /*id*/ ) const
{
  return QString();
}

/*!
  \brief Get data object "what's this" information for the
         specified column.

  This method can be re-implemented in the subclasses.
  Default implementation returns null string.

  The parameter \a id specifies the column identificator

  \param id column id
  \return object "what's this" information for the specified column
*/
QString SUIT_DataObject::whatsThis( const int /*id*/ ) const
{
  return QString();
}

/*!
  \brief Get data object font for the specified column.

  This method can be re-implemented in the subclasses.
  Default implementation returns application default font.

  The parameter \a id specifies the column identificator

  \param id column id
  \return object font for the specified column
*/
QFont SUIT_DataObject::font( const int /*id*/ ) const
{
  return QFont();
}

/*!
  \brief Get data object text alignment for the specified column.

  This method can be re-implemented in the subclasses.
  Default implementation returns default alignment which
  is Qt:AlignLeft.

  The parameter \a id specifies the column identificator
  (to display, for example, in the tree view widget).

  \param id column id
  \return object text alignment flags for the specified column
*/
int SUIT_DataObject::alignment( const int /*id*/ ) const
{
  return Qt::AlignLeft;
}

bool SUIT_DataObject::expandable() const
{
  return true;
}

/*!
  \brief Check if the object is visible.

  This method can be re-implemented in the subclasses.
  Default implementation returns \c true (all objects are visible by default).

  \return \c true if this object is displayable or \c false otherwise
*/
bool SUIT_DataObject::isVisible() const
{
  return true;
}

/*!
  \brief Check if the object is draggable.

  This method can be re-implemented in the subclasses.
  Default implementation returns \c false (all objects could not be dragged).

  \return \c true if it is possible to drag this object
*/
bool SUIT_DataObject::isDraggable() const
{
  return false;
}

/*!
  \brief Check if the drop operation for this object is possible.

  This method can be re-implemented in the subclasses.
  Default implementation returns \c false (drop operation is not allowed).

  \return \c true if it is possible to drop one or more objects (currently selected) to this object
*/
bool SUIT_DataObject::isDropAccepted() const
{
  return false;
}

/*!
  \brief Check if this object is enabled.

  This method can be re-implemented in the subclasses.
  Default implementation returns \c true (all objects are enabled).

  \return \c true if the user can interact with the item
*/
bool SUIT_DataObject::isEnabled() const
{
  return true;
}

/*!
  \brief Check if this object is selectable.

  This method can be re-implemented in the subclasses.
  Default implementation returns \c true (all objects are selectable).

  \return \c true if the item can be selected
*/
bool SUIT_DataObject::isSelectable() const
{
  return true;
}

/*!
  \brief Check if this object is checkable for the specified column.

  This method can be re-implemented in the subclasses.
  Default implementation returns \c false (all objects are not checkable).

  \param id column id
  \return \c true if the item can be checked or unchecked by the user
  \sa isOn(), setOn()
*/
bool SUIT_DataObject::isCheckable( const int /*id*/ ) const
{
  return false;
}

/*!
  \brief Check if this object is can't be renamed in place

  This method can be re-implemented in the subclasses.
  Default implementation returns \c false (all objects can not be renamed).

  \param id column id
  \return \c true if the item can be renamed by the user in place (e.g. in the Object browser)
*/
bool SUIT_DataObject::renameAllowed( const int /*id*/ ) const
{
  return false;
}

/*!
  \brief Set name of the this object.

  This method can be re-implemented in the subclasses.
  Default implementation returns \c false.

  \return \c true if rename operation finished successfully, \c false otherwise.
*/
bool SUIT_DataObject::setName(const QString& /*name*/) {
  return false;
}

/*!
  \brief Get the checked state of the object (if it is checkable)
  for the specified column.

  Default implementation supports the checked state for the first
  ("Name") column only.

  \param id column id
  \return checked state of the object for the specified column
  \sa setOn(), isCheckable()
*/
bool SUIT_DataObject::isOn( const int id ) const
{
  return id == NameId && myCheck;
}

/*!
  \brief Set the checked state of the object (if it is checkable)
  for the specified column.

  Default implementation supports the checked state for the first
  ("Name") column only.

  \param on new checked state of the object for the specified column
  \param id column id
  \sa isOn(), isCheckable()
*/
void SUIT_DataObject::setOn( const bool on, const int id )
{
  if( id == NameId )
    myCheck = on;
}

/*!
  \brief Get the "opened" state of the object.
  \return "opened" state of the object
  \sa setOpen()
*/
bool SUIT_DataObject::isOpen() const
{
  return myOpen;
}

/*!
  \brief Set the "opened" state of the object.
  \param on new "opened" state of the object
  \sa isOpen()
*/
void SUIT_DataObject::setOpen( const bool on )
{
  myOpen = on;
}

/*!
  \brief Check if the specified column supports custom sorting.

  This method can be re-implemented in the subclasses.
  Default implementation returns false ("Name" column does not require
  custom sorting).

  \param id column id
  \return \c true if column sorting should be customized
  \sa compare()
*/
bool SUIT_DataObject::customSorting( const int /*id*/ ) const
{
  return false;
}

/*!
  \brief Compares data from two items for sorting purposes.

  This method can be re-implemented in the subclasses.
  Default implementation returns false ("Name" column does not require
  custom sorting).

  This method is called only for those columns for which customSorting()
  method returns \c true.

  \param left first data to compare
  \param right second data to compare
  \param id column id
  \return result of the comparison
  \sa customSorting()
*/
bool SUIT_DataObject::compare( const QVariant& /*left*/, const QVariant& /*right*/,
                               const int /*id*/ ) const
{
  return false;
}

/*!
  \brief Get the object unique indentification key.

  This method can be re-implemented in the subclasses.
  Default implementation returns 0.

  \return object key
*/
SUIT_DataObjectKey* SUIT_DataObject::key() const
{
  return 0;
}

/*!
  \brief Get global signal handler.
  \return the only instance of the signal handler
*/
SUIT_DataObject::Signal* SUIT_DataObject::signal()
{
  if ( !mySignal )
    mySignal = new Signal();
  return mySignal;
}

/*!
  \brief Connect to the signal handlerx
  \param sig signal name
  \param reciever signal receiver object
  \param slot slot name
  \return \c true if connection is successfull
*/
bool SUIT_DataObject::connect( const char* sig, QObject* reciever, const char* slot )
{
  if ( !reciever || !slot )
    return false;

  signal()->disconnect( signal(), sig, reciever, slot );
  return signal()->connect( signal(), sig, reciever, slot );
}

/*!
  \brief Disconnect from the signal handler
  \param sig signal name
  \param reciever signal receiver object
  \param slot slot name
  \return \c true if disconnection is successfull
*/
bool SUIT_DataObject::disconnect( const char* sig, QObject* reciever, const char* slot )
{
  if ( !reciever || !slot )
    return false;
  return signal()->disconnect( signal(), sig, reciever, slot );
}

/*!
  \brief Schedule this object for the late deleting.

  The object will be deleted when control returns to the event loop.
  Note that entering and leaving a new event loop (e.g., by opening
  a modal dialog) will not perform the deferred deletion; for the object
  to be deleted, the control must return to the event loop from which
  deleteLater() was called.
*/
void SUIT_DataObject::deleteLater()
{
  if ( parent() )
    parent()->removeChild( this, false ); // to avoid infinite loop!
  signal()->deleteLater( this );
}

/*!
  \brief Dump the object tree recursively to the standard output.
  \param indent current indentation level
*/
void SUIT_DataObject::dump( const int indent ) const
{
  QString strIndent = QString().fill( ' ', indent ); // indentation string
  printf( "%s%s\n", strIndent.toLatin1().data(), name().toLatin1().data() );
  for ( DataObjectList::const_iterator it = myChildren.begin(); it != myChildren.end(); ++it )
    (*it)->dump( indent + 2 );
}

/*!
  \class SUIT_DataObject::Signal
  \brief Watcher class, responsible for the emitting signals on behalf of
  the data objects.

  SUIT_DataObject class does not inherit from QObject for the performance
  reasons, so it can not use signals/slots mechanism directly.
  Instead it uses the only Signal object to emit the signals when the data
  object is created, destroyed, inserted to the parent object or removed
  from it.

  If some object needs to handle, for example, data object destroying, it can
  use SUIT_DataObject::signal() method to connect the signal:
  \code
  MyHandler* h = new MyHandler();
  h->connect( SUIT_DataObject::signal(), SIGNAL(destroyed(SUIT_DataObject*)),
              h, SLOT(onDestroyed(SUIT_DataObject*)) );
  \endcode
  The same can be done by using static method SUIT_DataObject::connect().
  For example,
  \code
  MyHandler* h = new MyHandler();
  SUIT_DataObject::connect( SIGNAL(destroyed(SUIT_DataObject*)),
                            h, SLOT(onDestroyed(SUIT_DataObject*)));
  \endcode
*/

/*!
  \brief Constructor.
*/
SUIT_DataObject::Signal::Signal()
: QObject()
{
}

/*!
  \brief Destructor.

  Destroys data object which are scheduled for the deleting with the deleteLater().
*/
SUIT_DataObject::Signal::~Signal()
{
  for ( DataObjectList::Iterator it = myDelLaterObjects.begin();
        it != myDelLaterObjects.end(); ++it ) {
    delete *it;
  }
  myDelLaterObjects.clear();
}

/*!
  \brief Emit signal about data object creation.
  \param object data object being created
*/
void SUIT_DataObject::Signal::emitCreated( SUIT_DataObject* object )
{
  if ( object )
    emit created( object );
}

/*!
  \brief Emit signal about data object destroying.
  \param object data object being destroyed
*/
void SUIT_DataObject::Signal::emitDestroyed( SUIT_DataObject* object )
{
  if ( object ) {
    if ( myDelLaterObjects.contains( object ) )
      // object is being destroyed after calling deleteLater():
      // the signal has been already emitted from deleteLater()
      // we should avoid repeating of the object destroying from
      // the Signal destructor
      myDelLaterObjects.removeAll( object );
    else
      // object is being destroyed directly or via deleteLater()
      emit destroyed( object );
  }
}

/*!
  \brief Emit signal about data object adding to the parent data object.
  \param object data object being added
  \param parent parent data object
*/
void SUIT_DataObject::Signal::emitInserted( SUIT_DataObject* object, SUIT_DataObject* parent )
{
  emit( inserted( object, parent ) );
}

/*!
  \brief Emit signal about data object removed from the parent data object.
  \param object data object being removed
  \param parent parent data object
*/
void SUIT_DataObject::Signal::emitRemoved( SUIT_DataObject* object, SUIT_DataObject* parent )
{
  emit( removed( object, parent ) );
}

/*!
  \brief Emit a signal to notify that the data object has been modified.
  \param object data object that has been modified
*/
void SUIT_DataObject::Signal::emitModified( SUIT_DataObject* object )
{
  emit( modified( object ) );
}

/*!
  \brief Schedule data object for the late deleting.
  \param object data object to be deleted later
*/
void SUIT_DataObject::Signal::deleteLater( SUIT_DataObject* object )
{
  if ( !myDelLaterObjects.contains( object ) ) {
    emitDestroyed( object );
    myDelLaterObjects.append( object );
  }
}

/*!
  \brief Updates necessary internal fields of data object
*/
void SUIT_DataObject::update()
{
}

/*!
  \brief return unique group identificator

  Groups of data objects are used for column information search.
  Each column of data model has one or several registered group id
  If object has the same group id as one of registered, the information
  will be shown; the custom id of column will be passed into data() method
  in order to identify column from point of view of data object

 */
int SUIT_DataObject::groupId() const
{
  return 0;
}
/*!
  \brief return custom data for data object.
 */
QVariant SUIT_DataObject::customData(Qtx::CustomDataType /*type*/) {
  return QVariant();
}
/*!
  \fn void SUIT_DataObject::Signal::created( SUIT_DataObject* object );
  \brief Emitted when data object is created.
  \param object data object being created
*/

/*!
  \fn void SUIT_DataObject::Signal::destroyed( SUIT_DataObject* object );
  \brief Emitted when data object is destroyed.
  \param object data object being destroyed
*/

/*!
  \fn void SUIT_DataObject::Signal::inserted( SUIT_DataObject* object, SUIT_DataObject* parent );
  \brief Emitted when data object is inserted to the parent data object.
  \param object data object being created
  \param parent parent data object
*/

/*!
  \fn void SUIT_DataObject::Signal::removed( SUIT_DataObject* object, SUIT_DataObject* parent );
  \brief Emitted when data object is removed from the parent data object.
  \param object data object being removed
  \param parent parent data object
*/
