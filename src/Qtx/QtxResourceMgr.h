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
#ifndef QTX_RESOURCEMGR_H
#define QTX_RESOURCEMGR_H

#include "Qtx.h"

#include <qmap.h>
#include <qcolor.h>
#include <qfont.h>
#include <qpixmap.h>
#include <qstringlist.h>
#include <qtranslator.h>
#include <qvaluelist.h>

class QPixmap;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

/*!
  Class: QtxResourceMgr
*/

class QTX_EXPORT QtxResourceMgr
{
  class IniFormat;
  class XmlFormat;
  class Resources;

public:
  class Format;

  template <class Key, class Value> class IMap;
  template <class Key, class Value> class IMapIterator;
  template <class Key, class Value> class IMapConstIterator;

#ifdef QTX_NO_INDEXED_MAP
  typedef QMap<QString, QString> Section;
#else
  typedef IMap<QString, QString> Section;
#endif

public:
  QtxResourceMgr( const QString&, const QString& = QString::null );
  virtual ~QtxResourceMgr();

  QString         appName() const;
  QStringList     dirList() const;

  bool            checkExisting() const;
  virtual void    setCheckExisting( const bool );

  bool            isPixmapCached() const;
  void            setIsPixmapCached( const bool );

  void            clear();

  void            setIgnoreUserValues( const bool = true );
  bool            ignoreUserValues() const;

  bool            value( const QString&, const QString&, int& ) const;
  bool            value( const QString&, const QString&, double& ) const;
  bool            value( const QString&, const QString&, bool& ) const;
  bool            value( const QString&, const QString&, QColor& ) const;
  bool            value( const QString&, const QString&, QFont& ) const;  
  bool            value( const QString&, const QString&, QString&, const bool = true ) const;

  int             integerValue( const QString&, const QString&, const int = 0 ) const;
  double          doubleValue( const QString&, const QString&, const double = 0 ) const;
  bool            booleanValue( const QString&, const QString&, const bool = false ) const;
  QFont           fontValue( const QString&, const QString&, const QFont& = QFont() ) const;
  QColor          colorValue( const QString&, const QString&, const QColor& = QColor() ) const;
  QString         stringValue( const QString&, const QString&, const QString& = QString::null ) const;

  bool            hasSection( const QString& ) const;
  bool            hasValue( const QString&, const QString& ) const;

  void            setValue( const QString&, const QString&, const int );
  void            setValue( const QString&, const QString&, const double );
  void            setValue( const QString&, const QString&, const bool );
  void            setValue( const QString&, const QString&, const QFont& );
  void            setValue( const QString&, const QString&, const QColor& );
  void            setValue( const QString&, const QString&, const QString& );

  void            remove( const QString& );
  void            remove( const QString&, const QString& );

  QString         currentFormat() const;
  void            setCurrentFormat( const QString& );

  Format*         format( const QString& ) const;
  void            installFormat( Format* );
  void            removeFormat( Format* );

  QStringList     options() const;
  QString         option( const QString& ) const;
  void            setOption( const QString&, const QString& );

  QPixmap         defaultPixmap() const;
  virtual void    setDefaultPixmap( const QPixmap& );

  QString         resSection() const;
  QString         langSection() const;

  QPixmap         loadPixmap( const QString&, const QString& ) const;
  QPixmap         loadPixmap( const QString&, const QString&, const bool ) const;
  QPixmap         loadPixmap( const QString&, const QString&, const QPixmap& ) const;
  void            loadLanguage( const QString& = QString::null, const QString& = QString::null );

  void            raiseTranslators( const QString& );
  void            removeTranslators( const QString& );
  void            loadTranslator( const QString&, const QString& );
  void            loadTranslators( const QString&, const QStringList& );

  QString         path( const QString&, const QString&, const QString& ) const;

  bool            load();
  bool            import( const QString& );
  bool            save();

  QStringList     sections() const;
  QStringList     parameters( const QString& ) const;

  void            refresh();

protected:
  virtual void    setDirList( const QStringList& );
  virtual void    setResource( const QString&, const QString&, const QString& );

  virtual QString userFileName( const QString&, const bool = true ) const;
  virtual QString globalFileName( const QString& ) const;

private:
  void            initialize( const bool = true ) const;
  QString         substMacro( const QString&, const QMap<QChar, QString>& ) const;

private:
  typedef QPtrList<Resources>           ResList;
  typedef QPtrList<Format>              FormatList;
  typedef QMap<QString, QString>        OptionsMap;
  typedef QPtrListIterator<Resources>   ResListIterator;
  typedef QPtrListIterator<Format>      FormatListIterator;

  typedef QPtrList<QTranslator>         TransList;
  typedef QMap<QString, TransList>      TransListMap;
  typedef QPtrListIterator<QTranslator> TransListIterator;

private:
  QString         myAppName;
  QStringList     myDirList;
  FormatList      myFormats;
  OptionsMap      myOptions;
  ResList         myResources;
  bool            myCheckExist;
  TransListMap    myTranslator;
  QPixmap         myDefaultPix;
  bool            myIsPixmapCached;

  bool            myIsIgnoreUserValues;

  friend class QtxResourceMgr::Format;
};

/*!
  Class: QtxResourceMgr::Format
*/

class QTX_EXPORT QtxResourceMgr::Format
{
public:
  Format( const QString& );
  virtual ~Format();

  QString                format() const;

  QStringList            options() const;
  QString                option( const QString& ) const;
  void                   setOption( const QString&, const QString& );

  bool                   load( Resources* );
  bool                   save( Resources* );

protected:
  virtual bool           load( const QString&, QMap<QString, Section>& ) = 0;
  virtual bool           save( const QString&, const QMap<QString, Section>& ) = 0;

private:
  QString                myFmt;
  QMap<QString, QString> myOpt;
};

/*!
  Class: QtxResourceMgr::Resources
*/

class QtxResourceMgr::Resources
{
public:
  Resources( const QtxResourceMgr*, const QString& );
  virtual ~Resources();

  QString                file() const;
  void                   setFile( const QString& );

  QString                value( const QString&, const QString&, const bool ) const;
  void                   setValue( const QString&, const QString&, const QString& );

  bool                   hasSection( const QString& ) const;
  bool                   hasValue( const QString&, const QString& ) const;

  void                   removeSection( const QString& );
  void                   removeValue( const QString&, const QString& );

  QPixmap                loadPixmap( const QString&, const QString&, const QString& ) const;
  QTranslator*           loadTranslator( const QString&, const QString&, const QString& ) const;

  QString                environmentVariable( const QString&, int&, int& ) const;
  QString                makeSubstitution( const QString&, const QString&, const QString& ) const;

  void                   clear();

  QStringList            sections() const;
  QStringList            parameters( const QString& ) const;

  QString                path( const QString&, const QString&, const QString& ) const;

protected:
  QtxResourceMgr*        resMgr() const;

private:
  Section&               section( const QString& );
  const Section&         section( const QString& ) const;

  QString                fileName( const QString&, const QString&, const QString& ) const;

private:
  typedef QMap<QString, Section> SectionMap;

private:
  SectionMap             mySections;
  QString                myFileName;
  QMap<QString,QPixmap>  myPixmapCache;
  QtxResourceMgr*        myMgr;

  friend class QtxResourceMgr::Format;
};

/*!
  Class: QtxResourceMgr::IMapIterator
*/

template <class Key, class Value> class QtxResourceMgr::IMapIterator
{
public:
  IMapIterator()                           : myMap( 0 ), myIndex( 0 )                                   { init(); }
  IMapIterator( const IMap<Key,Value>* m ) : myMap( const_cast< IMap<Key,Value>* >( m ) ), myIndex( 0 ) { init(); }
  IMapIterator( const IMapIterator& i )    : myMap( i.myMap ), myIndex( i.myIndex )                     { init(); }

  bool operator==( const IMapIterator& i ) { return !operator!=( i );                                   }
  bool operator!=( const IMapIterator& i ) { return !myMap || myMap != i.myMap || myIndex != i.myIndex; }
  
  operator bool() const { return myIndex >= 0; }

  const Key&   key() const  { return myMap->key( myIndex );   }
  Value&       data()       { return myMap->value( myIndex ); }
  const Value& data() const { return myMap->value( myIndex ); }

  Value& operator*() { return data(); }

  IMapIterator& operator++()      { myIndex++; init(); return *this;                     }
  IMapIterator  operator++( int ) { IMapIterator i = *this; myIndex++; init(); return i; }
  IMapIterator& operator--()      { myIndex--; init(); return *this;                     }
  IMapIterator  operator--( int ) { IMapIterator i = *this; myIndex--; init(); return i; }

private:
  IMapIterator( const IMap<Key,Value>* m, const int index ) : myMap( const_cast< IMap<Key,Value>* >( m ) ), myIndex( index ) { init(); }
  void init() { if ( !myMap || myIndex >= myMap->count() ) myIndex = -1; }

private:
  IMap<Key,Value>* myMap;
  int              myIndex;

  friend class IMap<Key, Value>;
  friend class IMapConstIterator<Key, Value>;
};

/*!
  Class: QtxResourceMgr::IMapConstIterator
*/

template <class Key, class Value> class QtxResourceMgr::IMapConstIterator
{
public:
  IMapConstIterator()                                    : myMap( 0 ), myIndex( 0 )                                    { init(); }
  IMapConstIterator( const IMap<Key,Value>* m )          : myMap( const_cast< IMap<Key,Value>* >( m )  ), myIndex( 0 ) { init(); }
  IMapConstIterator( const IMapConstIterator& i )        : myMap( i.myMap ), myIndex( i.myIndex )                      { init(); }
  IMapConstIterator( const IMapIterator<Key, Value>& i ) : myMap( i.myMap ), myIndex( i.myIndex )                      { init(); }
  
  bool operator==( const IMapConstIterator& i ) { return !operator!=( i );                                   }
  bool operator!=( const IMapConstIterator& i ) { return !myMap || myMap != i.myMap || myIndex != i.myIndex; }
  
  operator bool() const { return myIndex >= 0; }
  
  const Key&   key() const  { return myMap->key( myIndex );   }
  const Value& data() const { return myMap->value( myIndex ); }
  
  const Value& operator*() const { return data(); }
  
  IMapConstIterator& operator++()      { myIndex++; init(); return *this;                          }
  IMapConstIterator  operator++( int ) { IMapConstIterator i = *this; myIndex++; init(); return i; }
  IMapConstIterator& operator--()      { myIndex--; init(); return *this;                          }
  IMapConstIterator  operator--( int ) { IMapConstIterator i = *this; myIndex--; init(); return i; }
  
private:
  IMapConstIterator( const IMap<Key,Value>* m, const int index ): myMap( const_cast< IMap<Key,Value>* >( m ) ), myIndex( index ) { init(); }
  void init() { if ( !myMap || myIndex >= myMap->count() ) myIndex = -1; }
  
private:
  IMap<Key,Value>* myMap;
  int              myIndex;
  
  friend class IMap<Key,Value>;
};

/*!
  Class: QtxResourceMgr::IMap
*/

template <class Key, class Value> class QtxResourceMgr::IMap
{
public:
  typedef IMapIterator<Key,Value>      Iterator;
  typedef IMapConstIterator<Key,Value> ConstIterator;

public:
  IMap() {}
  IMap( const IMap& m ) : myKeys( m.myKeys ), myData( m.myData ) {}
  IMap& operator=( const IMap& m ) { myKeys = m.myKeys; myData = m.myData; return *this; }
  
  int  count() const   { return myData.count(); }
  int  size() const    { return myData.count(); }
  bool empty() const   { return myData.empty(); }
  bool isEmpty() const { return myData.empty(); }
  
  void clear() { myKeys.clear(); myData.clear(); }
  
  QValueList<Key>   keys()   const { return myKeys; }
  QValueList<Value> values() const { QValueList<Value> l; for ( int i = 0; i < count(); i++ ) l.append( value( i ) ); return l; }
  bool              contains ( const Key& key ) const { return myData.contains( key ); }
  
  Iterator      begin()       { return Iterator( this );               }
  Iterator      end()         { return Iterator( this, count() );      }
  ConstIterator begin() const { return ConstIterator( this );          }
  ConstIterator end() const   { return ConstIterator( this, count() ); }
  
  Iterator insert( const Key& key, const Value& value, bool overwrite = true )
  { 
    if ( myData.find( key ) == myData.end() || overwrite )
    {
      if ( myData.find( key ) != myData.end() && overwrite )
        myKeys.remove( myKeys.find( key ) );
      myKeys.append( key );
      myData[key] = value;
    }
    return Iterator( this, index( key ) );
  }

  Iterator replace( const Key& key, const Value& value )
  { 
    if ( myData.find( key ) == myData.end() )
      myKeys.append( key );
    myData[ key ] = value;
    return Iterator( this, index( key ) );
  }

  int           index( const Key& key ) const { return myKeys.findIndex( key );      }
  Iterator      at( const int index )         { return Iterator( this, index );      }
  ConstIterator at( const int index ) const   { return ConstIterator( this, index ); }

  Key& key( const int index )
  {
    if ( index < 0 || index >= (int)myKeys.count() ) 
      return dummyKey;
    return myKeys[index];
  }

  Value& value( const int index )
  {
    if ( index < 0 || index >= (int)myKeys.count() ) 
      return dummyValue;
    return myData[ myKeys[index] ];
  }

  Value& operator[]( const Key& key )
  {
    if ( myData.find( key ) == myData.end() )
      insert( key, Value() );
    return myData[ key ];
  }

  const Value& operator[]( const Key& key ) const
  {
    if ( myData.find( key ) == myData.end() )
      return dummyValue;
    return myData[ key ];
  }

  void erase( Iterator it )     { remove( it );    }
  void erase( const Key& key )  { remove( key );   }
  void erase( const int index ) { remove( index ); }
  void remove( Iterator it )    { if ( it.myMap != this ) return; remove( it.myIndex ); }
  void remove( const Key& key ) { remove( index( key ) ); }
  void remove( const int index )
  {
    if ( index >= 0 && index < (int)myKeys.count() )
    {
      myData.remove( myKeys[ index ] );
      myKeys.remove( myKeys.at( index ) );
    }
  }

private:
  QValueList<Key> myKeys;
  QMap<Key,Value> myData;
  Key             dummyKey;
  Value           dummyValue;

  friend class IMapIterator<Key,Value>;
  friend class IMapConstIterator<Key,Value>;
};

#endif
