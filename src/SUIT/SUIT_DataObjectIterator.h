#ifndef SUIT_DATAOBJECT_ITERATOR_H
#define SUIT_DATAOBJECT_ITERATOR_H

#include "SUIT_DataObject.h"

/*!
  Class provide data object iterator.
*/
class SUIT_EXPORT SUIT_DataObjectIterator
{
public:
  typedef enum { DepthLeft, DepthRight, BreadthLeft, BreadthRight, User } DetourType;

  SUIT_DataObjectIterator( SUIT_DataObject* root, const int det, const bool fromTrueRoot = false );

  virtual void      operator++();
  SUIT_DataObject*  current() const;

  int               depth() const;
  int               detour() const;

protected: 
  SUIT_DataObject*  parent( SUIT_DataObject* ) const;
  SUIT_DataObject*  extreme( DataObjectList&, bool FromLeft ) const;
  SUIT_DataObject*  globalSibling( SUIT_DataObject*, bool next ) const;

protected:
  SUIT_DataObject*  myRoot;
  SUIT_DataObject*  myCurrent;

private:
  SUIT_DataObject*  myExtremeChild;
  int               myDetourType, myCurrentLevel;
};

class SUIT_DataObjectLevelIterator : public SUIT_DataObjectIterator
{
public:
  SUIT_DataObjectLevelIterator( SUIT_DataObject* root, const int start, const int end = 0,
                                  const bool LeftToRight = true );
  virtual void      operator++();

protected:
  int myStartLevel;
  int myEndLevel;
};

#endif
