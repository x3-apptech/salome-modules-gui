// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File   : QtxMap.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#ifndef QTXMAP_H
#define QTXMAP_H

template <class Key, class Value> class IMap;
template <class Key, class Value> class IMapIterator;
template <class Key, class Value> class IMapConstIterator;

/*!
  \brief Indexed map template class.
*/
template <class Key, class Value> class IMap
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
  
  QList<Key>   keys()   const { return myKeys; }
  QList<Value> values() const { QList<Value> l; for ( int i = 0; i < count(); i++ ) l.append( value( i ) ); return l; }
  bool         contains ( const Key& key ) const { return myData.contains( key ); }
  
  Iterator      begin()       { return Iterator( this );               }
  Iterator      end()         { return Iterator( this, count() );      }
  ConstIterator begin() const { return ConstIterator( this );          }
  ConstIterator end() const   { return ConstIterator( this, count() ); }
  
  Iterator insert( const Key& key, const Value& value, bool overwrite = true )
  { 
    if ( myData.find( key ) == myData.end() || overwrite )
    {
      if ( myData.find( key ) != myData.end() && overwrite )
        myKeys.removeAt( myKeys.indexOf( key ) );
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

  int           index( const Key& key ) const { return myKeys.indexOf( key );      }
  Iterator      at( const int index )         { return Iterator( this, index );      }
  ConstIterator at( const int index ) const   { return ConstIterator( this, index ); }

  Key& key( const int index )
  {
    if ( index < 0 || index >= (int)myKeys.count() ) 
      return dummyKey;
    return myKeys[index];
  }

  Value value( const int index )
  {
    if ( index < 0 || index >= (int)myKeys.count() ) 
      return dummyValue;
    return myData[ myKeys[index] ];
  }

  Value operator[]( const Key& key )
  {
    if ( myData.find( key ) == myData.end() )
      insert( key, Value() );
    return myData[ key ];
  }

  const Value operator[]( const Key& key ) const
  {
    if ( myData.find( key ) == myData.end() )
      return dummyValue;
    return myData[key];
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
      myData.remove( myKeys[index] );
      myKeys.removeAt( index );
    }
  }

private:
  QList<Key>      myKeys;
  QMap<Key,Value> myData;
  Key             dummyKey;
  Value           dummyValue;

  friend class IMapIterator<Key,Value>;
  friend class IMapConstIterator<Key,Value>;
};

/*!
  \brief Indexed map iterator template class.
*/
template <class Key, class Value> class IMapIterator
{
public:
  IMapIterator()                           : myMap( 0 ), myIndex( 0 )                                   { init(); }
  IMapIterator( const IMap<Key,Value>* m ) : myMap( const_cast< IMap<Key,Value>* >( m ) ), myIndex( 0 ) { init(); }
  IMapIterator( const IMapIterator& i )    : myMap( i.myMap ), myIndex( i.myIndex )                     { init(); }

  bool operator==( const IMapIterator& i ) { return !operator!=( i );                                   }
  bool operator!=( const IMapIterator& i ) { return !myMap || myMap != i.myMap || myIndex != i.myIndex; }
  
  operator bool() const { return myIndex >= 0; }

  const Key&   key() const  { return myMap->key( myIndex );   }
  Value&       value()       { return myMap->value( myIndex ); }
  const Value& value() const { return myMap->value( myIndex ); }

  Value& operator*() { return value(); }

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
  \brief Indexed map const iterator template class.
*/
template <class Key, class Value> class IMapConstIterator
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
  const Value value() const { return myMap->value( myIndex ); }
  
  const Value operator*() const { return value(); }

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

#endif // QTXMAP_H
