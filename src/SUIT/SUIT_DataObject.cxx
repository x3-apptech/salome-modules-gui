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
#include "SUIT_DataObject.h"

#include <qobject.h>

#include "SUIT_DataObjectKey.h"

/*!
    Constructor
*/

SUIT_DataObject::SUIT_DataObject( SUIT_DataObject* p )
: myParent( 0 ),
  mySignal( 0 ),
  myOpen( false ),
  myCheck( false )
{
  myChildren.setAutoDelete( true );

  setParent( p );
}

/*!
    Destructor
*/

SUIT_DataObject::~SUIT_DataObject()
{
  SUIT_DataObject* p = myParent;

  myParent = 0;

  if ( p )
    p->removeChild( this );

  if ( mySignal )
  {
    mySignal->emitSignal();
    mySignal->setOwner( 0 );
  }

  for ( QPtrListIterator<SUIT_DataObject> it( myChildren ); it.current(); ++it )
    it.current()->myParent = 0;

  delete mySignal;
}

/*!
    Returns the root object.
*/

SUIT_DataObject* SUIT_DataObject::root() const
{
  return parent() ? parent()->root() : (SUIT_DataObject*)this;
}

/*!
    Returns the first child object.
*/

SUIT_DataObject* SUIT_DataObject::firstChild() const
{
  SUIT_DataObject* child = 0;
  if ( !myChildren.isEmpty() )
    child = myChildren.getFirst();
  return child;
}

/*!
    Returns the last child object.
*/

SUIT_DataObject* SUIT_DataObject::lastChild() const
{
  SUIT_DataObject* child = 0;
  if ( !myChildren.isEmpty() )
    child = myChildren.getLast();
  return child;
}

/*!
    Returns the number of the child objects.
*/

int SUIT_DataObject::childCount() const
{
  return myChildren.count();
}

/*!
    Returns the index of the specified object in the child list or -1.
*/

int SUIT_DataObject::childPos( const SUIT_DataObject* obj ) const
{
  int res = -1;

  int i = 0;
  for ( DataObjectListIterator it( myChildren ); it.current() && res == -1; ++it, i++ )
  {
    if ( it.current() == obj )
      res = i;
  }

  return res;
}

/*!
    Returns the child object with specified index.
*/

SUIT_DataObject* SUIT_DataObject::childObject( const int idx ) const
{
  SUIT_DataObject* child = 0;

  if ( idx>= 0 && idx < (int)myChildren.count() )
  {
    SUIT_DataObject* that = (SUIT_DataObject*)this;
    child = that->myChildren.at( idx );
  }

  return child;
}

/*!
    Returns the level of the object in the data tree.
    0 means that object is top-level.
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
    Returns the next data object in the child list of the parent.
*/

SUIT_DataObject* SUIT_DataObject::nextBrother() const
{
  return myParent ? myParent->childObject( myParent->childPos( this ) + 1 ) : 0;
}

/*!
    Returns the previous data object in the child list of the parent.
*/

SUIT_DataObject* SUIT_DataObject::prevBrother() const
{
  return myParent ? myParent->childObject( myParent->childPos( this ) - 1 ) : 0;
}

/*!
    Returns 'true' if the object will delete children during destroying
*/

bool SUIT_DataObject::autoDeleteChildren() const
{
  return myChildren.autoDelete();
}

/*!
    Specify should the object delete children during destroying
*/

void SUIT_DataObject::setAutoDeleteChildren( const bool on )
{
  myChildren.setAutoDelete( on );
}

/*!
    Returns the list of the child objects. if 'rec' is 'true' then function get all sub children.
*/

void SUIT_DataObject::children( DataObjectList& lst, const bool rec ) const
{
  for ( DataObjectListIterator it( myChildren ); it.current(); ++it )
  {
    lst.append( it.current() );
    if ( rec )
      it.current()->children( lst, rec );
  }
}

/*!
    Returns the list of the child objects. if 'rec' is 'true' then function get all sub children.
*/

DataObjectList SUIT_DataObject::children( const bool rec )
{
  DataObjectList lst;
  children( lst, rec );
  return lst;
}

/*!
    Append new child object to the end of the children list
*/

void SUIT_DataObject::appendChild( SUIT_DataObject* theObj ) 
{
  insertChild( theObj, myChildren.count() );
}

/*!
    Insert new child object to the children list at specified position
*/

void SUIT_DataObject::insertChild( SUIT_DataObject* theObj, int thePosition )
{
  if ( !theObj || myChildren.find( theObj ) != -1 )
    return;

  int pos = thePosition < 0 ? myChildren.count() : thePosition;
  myChildren.insert( QMIN( pos, (int)myChildren.count() ), theObj );
  theObj->setParent( this );
}

/*!
    Removes the specified child object reference.
*/

void SUIT_DataObject::removeChild( SUIT_DataObject* theObj )
{
  if ( !theObj )
    return;

  bool ad = myChildren.autoDelete();
  myChildren.setAutoDelete( false );

  if ( myChildren.remove( theObj ) )
    theObj->setParent( 0 );

  myChildren.setAutoDelete( ad );
}

/*!
    Replaces the specified child object by another object.
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
    Transfer the all children from specified object 'obj' to self.
*/

void SUIT_DataObject::reparentChildren( const SUIT_DataObject* obj )
{
  if ( !obj )
    return;

  DataObjectList lst;
  obj->children( lst );
  for ( DataObjectListIterator it( lst ); it.current(); ++it )
    it.current()->setParent( this );
}

/*!
    Set the parent object. Remove itself from current parent children
    and append itself to the new parent children list.
*/

void SUIT_DataObject::setParent( SUIT_DataObject* theParent )
{
  if ( theParent == parent() )
    return;

  if ( parent() )
    parent()->removeChild( this );

  myParent = theParent;

  if ( parent() )
    parent()->appendChild( this );
}

/*!
    Returns the parent object.
*/

SUIT_DataObject* SUIT_DataObject::parent() const
{
  return myParent;
}


/*!
    Connect to signal destroyed( SUIT_DataObject* ).
*/

bool SUIT_DataObject::connect( QObject* reciever, const char* slot )
{
  if ( !reciever || !slot )
    return false;

  if ( !mySignal )
    mySignal = new Signal( this );

  QObject::disconnect( mySignal, SIGNAL( destroyed( SUIT_DataObject* ) ), reciever, slot );
  return QObject::connect( mySignal, SIGNAL( destroyed( SUIT_DataObject* ) ), reciever, slot );
}

/*!
    Disconnect from signal destroyed( SUIT_DataObject* ).
*/

bool SUIT_DataObject::disconnect( QObject* reciever, const char* slot )
{
  if ( !reciever || !slot )
    return false;

  if ( !mySignal )
    return true;

  return QObject::disconnect( mySignal, SIGNAL( destroyed( SUIT_DataObject* ) ), reciever, slot );
}

/*!
    Returns object name
*/

void SUIT_DataObject::deleteLater()
{
  if ( !mySignal )
    mySignal = new Signal( this );
  
  mySignal->emitSignal();
  mySignal->deleteLater();
}

/*!
    Returns object name
*/

QString SUIT_DataObject::name() const
{
  return QString::null;
}

/*!
    Returns object icon
*/

QPixmap SUIT_DataObject::icon() const
{
  return QPixmap();
}

/*!
    Returns object text
*/

QString SUIT_DataObject::text( const int ) const
{
  return QString::null;
}

/*!
    Returns object color
*/

QColor SUIT_DataObject::color( const ColorRole ) const
{
  return QColor();
}

/*!
    Returns object tool tip
*/

QString SUIT_DataObject::toolTip() const
{
  return QString::null;
}

/*!
    Returns 'true' if it is possible to drag this object
*/

bool SUIT_DataObject::isDragable() const
{
  return false;
}

/*!
    Returns 'true' if it is possible to drop an object "obj" to this object.
*/

bool SUIT_DataObject::isDropAccepted( SUIT_DataObject* )
{
  return false;
}

/*!
    Returns type of check possibility.
*/

SUIT_DataObject::CheckType SUIT_DataObject::checkType() const
{
  return None;
}

/*!
    Returns the checked state of the object.
*/

bool SUIT_DataObject::isOn() const
{
  return myCheck;
}

/*!
    Sets the checked state of the object.
*/

void SUIT_DataObject::setOn( const bool on )
{
  myCheck = on;
}

/*!
    \return the opened state of the object (used in Object Browser).
*/
bool SUIT_DataObject::isOpen() const
{
  return myOpen;
}

/*!
    Sets the opened state of the object (used in Object Browser).
*/
void SUIT_DataObject::setOpen( const bool on )
{
  myOpen = on;
}

/*!
    Returns object personal indentification key.
*/

SUIT_DataObjectKey* SUIT_DataObject::key() const
{
  return 0;
}

/*!
   Dump this data object and its children to cout
*/
void SUIT_DataObject::dump( const int indent ) const
{
  QString strIndent = QString().fill( ' ', indent ); // indentation string 
  printf( "%s%s\n", strIndent.latin1(), name().latin1() );
  for ( DataObjectListIterator it( myChildren ); it.current(); ++it ) // iterate all children
    it.current()->dump( indent + 2 );  // dump every child with indent + 2 spaces
}

/*!
  Class: SUIT_DataObject::Signal [Internal]
*/

SUIT_DataObject::Signal::Signal( SUIT_DataObject* o )
: QObject(),
myOwner( o )
{
}

/*!
  Destructor.
*/
SUIT_DataObject::Signal::~Signal()
{
  SUIT_DataObject* o = myOwner;
  myOwner = 0;
  if ( o )
  {
    o->mySignal = 0;
    delete o;
  }
}

/*!
  Set owner \a o.
*/
void SUIT_DataObject::Signal::setOwner( SUIT_DataObject* o )
{
  myOwner = o;
}

/*!
  emit signal destroed owner.
*/
void SUIT_DataObject::Signal::emitSignal()
{
  if ( myOwner )
    emit destroyed( myOwner );
}
