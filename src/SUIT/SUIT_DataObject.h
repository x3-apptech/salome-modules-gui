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

// File   : SUIT_DataObject.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#ifndef SUIT_DATAOBJECT_H
#define SUIT_DATAOBJECT_H

#include "SUIT.h"

#include <Qtx.h>

#include <QList>
#include <QObject>
#include <QString>
#include <QPixmap>
#include <QFont>

class SUIT_DataObject;
class SUIT_DataObjectKey;

typedef QList<SUIT_DataObject*> DataObjectList;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class SUIT_EXPORT SUIT_DataObject
{
public:
  class Signal;

  //! Color role
  typedef enum {
    Text,              //!< editor foreground (text) color
    Base,              //!< editor background color
    Foreground,        //!< foreground (text) color
    Background,        //!< background color
    Highlight,         //!< highlight background color
    HighlightedText    //!< highlighted foreground (text) color
  } ColorRole;

  //! Column id
  enum
  {
    NameId,          //!< name column
    VisibilityId     //!< visibility state column
  };

  SUIT_DataObject( SUIT_DataObject* = 0 );
  virtual ~SUIT_DataObject();

  SUIT_DataObject*            root() const;
  SUIT_DataObject*            lastChild() const;
  SUIT_DataObject*            firstChild() const;

  int                         childCount() const;
  int                         childPos( const SUIT_DataObject* ) const;
  void                        moveChildPos( SUIT_DataObject*, int );
  SUIT_DataObject*            childObject( const int ) const;
  int                         level() const;
  int                         position() const;
  void                        setPosition(int);

  SUIT_DataObject*            nextBrother() const;
  SUIT_DataObject*            prevBrother() const;

  bool                        autoDeleteChildren() const;
  virtual void                setAutoDeleteChildren( const bool );

  virtual void                children( DataObjectList&, const bool = false ) const;
  virtual DataObjectList      children( const bool = false );

  void                        appendChild( SUIT_DataObject* );
  virtual void                insertChild( SUIT_DataObject*, int );
  virtual void                removeChild( SUIT_DataObject*, const bool = false );
  bool                        replaceChild( SUIT_DataObject*, SUIT_DataObject*, const bool = false );

  void                        reparentChildren( const SUIT_DataObject* );

  virtual SUIT_DataObject*    parent() const;
  virtual void                setParent( SUIT_DataObject* );
  virtual void                assignParent( SUIT_DataObject* );
  void                        insertChildAtPos( SUIT_DataObject* obj, int position );
  bool                        modified(){return _modified;};
  virtual void                setModified(bool modified);

  virtual QString             name() const;
  virtual QString             text( const int = NameId ) const;
  virtual QPixmap             icon( const int = NameId ) const;
  virtual QColor              color( const ColorRole, const int = NameId ) const;
  virtual QString             toolTip( const int = NameId ) const;
  virtual QString             statusTip( const int = NameId ) const;
  virtual QString             whatsThis( const int = NameId ) const;
  virtual QFont               font( const int = NameId ) const;
  virtual int                 alignment( const int = NameId ) const;

  virtual bool                expandable() const;
  virtual bool                isVisible() const;
  virtual bool                isDraggable() const;
  virtual bool                isDropAccepted() const;

  virtual bool                isEnabled() const;
  virtual bool                isSelectable() const;
  virtual bool                isCheckable( const int = NameId ) const;
  virtual bool                renameAllowed( const int = NameId ) const;
  virtual bool                setName(const QString& name);

  virtual bool                isOn( const int = NameId ) const;
  virtual void                setOn( const bool, const int = NameId );

  virtual bool                isOpen() const;
  virtual void                setOpen( const bool );

  virtual void                update();
  virtual bool                customSorting( const int = NameId ) const;
  virtual bool                compare( const QVariant&, const QVariant&, const int = NameId ) const;

  virtual SUIT_DataObjectKey* key() const;
  virtual int                 groupId() const;
  virtual QVariant            customData(Qtx::CustomDataType /*type*/);

  static Signal*              signal();
  static bool                 connect( const char*, QObject*, const char* );
  static bool                 disconnect( const char*, QObject*, const char* );

  void                        deleteLater();

  void                        dump( const int indent = 2 ) const; // dump to cout

private:
  SUIT_DataObject*            myParent;
  bool                        myOpen;
  bool                        myCheck;
  bool                        myAutoDel;
  DataObjectList              myChildren;
  bool                        _modified;

  static Signal*              mySignal;

  friend class SUIT_DataObject::Signal;
  friend class SUIT_DataObjectIterator;
};

class SUIT_EXPORT SUIT_DataObject::Signal : public QObject
{
  Q_OBJECT

public:
  Signal();
  virtual ~Signal();

private:
  void emitCreated( SUIT_DataObject* );
  void emitDestroyed( SUIT_DataObject* );
  void emitInserted( SUIT_DataObject*, SUIT_DataObject* );
  void emitRemoved( SUIT_DataObject*, SUIT_DataObject* );
  void emitModified( SUIT_DataObject* );

  void deleteLater( SUIT_DataObject* );

signals:
  void created( SUIT_DataObject* );
  void destroyed( SUIT_DataObject* );
  void inserted( SUIT_DataObject*, SUIT_DataObject* );
  void removed( SUIT_DataObject*, SUIT_DataObject* );
  void modified( SUIT_DataObject* );

  friend class SUIT_DataObject;

private:
  DataObjectList myDelLaterObjects;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif  // SUIT_DATAOBJECT_H
