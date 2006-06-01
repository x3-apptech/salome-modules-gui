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
#ifndef SUIT_DATAOBJECT_H
#define SUIT_DATAOBJECT_H

#include <qobject.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qptrlist.h>

#include "SUIT.h"

class SUIT_DataObject;
class SUIT_DataObjectKey;

typedef QPtrList<SUIT_DataObject>         DataObjectList;
typedef QPtrListIterator<SUIT_DataObject> DataObjectListIterator;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

/*!
  \class SUIT_DataObject
  Data Object represents uniform data tree structure recommended to use in SUIT-based applications
  Many of standard classes (DataModel,ObjectBrowser) deal with SUIT_DataObjects
*/
class SUIT_EXPORT SUIT_DataObject  
{
public:
  class Signal;

  typedef enum { None, RadioButton, CheckBox } CheckType;
  typedef enum { Text, Base, Foreground, Background, Highlight, HighlightedText } ColorRole;

  SUIT_DataObject( SUIT_DataObject* = 0 );
  virtual ~SUIT_DataObject();

  SUIT_DataObject*            root() const;
  SUIT_DataObject*            lastChild() const;
  SUIT_DataObject*            firstChild() const;

  int                         childCount() const;
  int                         childPos( const SUIT_DataObject* ) const;
  SUIT_DataObject*            childObject( const int ) const;
  int                         level() const;

  SUIT_DataObject*            nextBrother() const;
  SUIT_DataObject*            prevBrother() const;

  bool                        autoDeleteChildren() const;
  virtual void                setAutoDeleteChildren( const bool );

  virtual void                children( DataObjectList&, const bool = false ) const;
  virtual DataObjectList      children( const bool = false );
  
  void                        appendChild( SUIT_DataObject* );
  virtual void                removeChild( SUIT_DataObject* );
  virtual void                insertChild( SUIT_DataObject*, int thePosition );
  bool                        replaceChild( SUIT_DataObject*, SUIT_DataObject*, const bool = false );

  void                        reparentChildren( const SUIT_DataObject* );

  virtual QString             text( const int ) const;
  virtual QColor              color( const ColorRole ) const;

  virtual QString             name() const;
  virtual QPixmap             icon() const;
  virtual QString             toolTip() const;

  virtual SUIT_DataObject*    parent() const;
  virtual void                setParent( SUIT_DataObject* );

  virtual bool                isDragable() const;
  virtual bool                isDropAccepted( SUIT_DataObject* obj );

  virtual CheckType           checkType() const;

  virtual bool                isOn() const;
  virtual void                setOn( const bool );

  virtual bool                isOpen() const;
  virtual void                setOpen( const bool );

  virtual SUIT_DataObjectKey* key() const;

  bool                        connect( QObject*, const char* );
  bool                        disconnect( QObject*, const char* );

  void                        deleteLater();
  
  void                        dump( const int indent = 2 ) const; // dump to cout

private:
  bool                        myOpen;
  bool                        myCheck;
  Signal*                     mySignal;
  SUIT_DataObject*            myParent;
  DataObjectList              myChildren;

  friend class SUIT_DataObject::Signal;
  friend class SUIT_DataObjectIterator;
};

/*!
  \class SUIT_DataObject::Signal
  Auxiliary class providing functionality to use signals of data object state change
  SUIT_DataObject cannot have signals, because it isn't QObject, but
  methods connect/disconnect of SUIT_DataObject with help of this it is possible
  to emulate Qt signal processing
*/
class SUIT_DataObject::Signal : public QObject
{
  Q_OBJECT

public:
  Signal( SUIT_DataObject* );
  virtual ~Signal();

  void                        emitSignal();
  void                        setOwner( SUIT_DataObject* o );

signals:
  void                        destroyed( SUIT_DataObject* );

private:
  SUIT_DataObject*            myOwner;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
