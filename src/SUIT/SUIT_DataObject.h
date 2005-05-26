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

class SUIT_EXPORT SUIT_DataObject  
{
public:
  class Signal;

  typedef enum { None, RadioButton, CheckBox } CheckType;
  typedef enum { Text, Base, Foreground, Background, Highlight } ColorRole;

  SUIT_DataObject( SUIT_DataObject* = 0 );
  virtual ~SUIT_DataObject();

  SUIT_DataObject*            root() const;
  SUIT_DataObject*            lastChild() const;
  SUIT_DataObject*            firstChild() const;

  int                         childCount() const;
  int                         childPos( const SUIT_DataObject* ) const;
  SUIT_DataObject*            childObject( const int ) const;

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

  QString                     text() const { return text( 0 ); }
  QColor                      color() const { return color( Foreground ); }
  virtual QString             name() const;
  virtual QPixmap             icon() const;
  virtual QString             text( const int ) const;
  virtual QColor              color( const ColorRole ) const;
  virtual QString             toolTip() const;

  virtual SUIT_DataObject*    parent() const;
  virtual void                setParent( SUIT_DataObject* );

  virtual bool                isDragable() const;
  virtual bool                isDropAccepted( SUIT_DataObject* obj );

  virtual CheckType           checkType() const;

  virtual bool                isOn() const;
  virtual void                setOn( const bool );

  virtual SUIT_DataObjectKey* key() const;

  bool                        connect( QObject*, const char* );
  bool                        disconnect( QObject*, const char* );
  
  void                        dump( const int indent = 2 ) const; // dump to cout

private:
  bool                        myCheck;
  Signal*                     mySignal;
  SUIT_DataObject*            myParent;
  DataObjectList              myChildren;

  friend class SUIT_DataObjectIterator;
};

class SUIT_DataObject::Signal : public QObject
{
  Q_OBJECT

public:
  Signal( SUIT_DataObject* o ) : QObject(), myOwner( o ) {};
  virtual ~Signal() { emit destroyed( myOwner ); };

signals:
  void                        destroyed( SUIT_DataObject* );

private:
  SUIT_DataObject*            myOwner;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
