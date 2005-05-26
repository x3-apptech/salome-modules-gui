#include "SUIT_DataObjectIterator.h"

SUIT_DataObjectIterator::SUIT_DataObjectIterator( SUIT_DataObject* root, const int det, const bool fromTrueRoot )
: myRoot( root ),
myDetourType( det ),
myCurrentLevel( 0 )
{
  if ( myRoot && fromTrueRoot )
    myRoot = myRoot->root();

  myCurrent = myExtremeChild = myRoot;
}

SUIT_DataObject* SUIT_DataObjectIterator::parent( SUIT_DataObject* obj ) const
{
  SUIT_DataObject* result = 0;
  if ( obj && obj != myRoot )
    result = obj->parent();
  return result;
}

void SUIT_DataObjectIterator::operator++()
{
  SUIT_DataObject* aNext = 0;
  SUIT_DataObject* aParent = 0;

  bool exit;

  if ( myCurrent )
  {
    if ( myDetourType == DepthLeft || myDetourType == DepthRight )
    {
            //Depth detour algorithm
      if ( myCurrent->myChildren.count() > 0 )
      {
        myCurrent = extreme( myCurrent->myChildren, myDetourType == DepthLeft );
        myCurrentLevel++;
      }
      else do
      {
        exit = false;
        aParent = parent( myCurrent );
        if ( !aParent )
        {
          myCurrent = 0; //the tree is passed completely
          exit = true;
        }
        else
        {
          aParent->myChildren.find( myCurrent );
          if ( myDetourType == DepthLeft )
            myCurrent = aParent->myChildren.next();
          else
            myCurrent = aParent->myChildren.prev();
          if ( !myCurrent )
          {
            myCurrent = aParent;
            myCurrentLevel--;
          }
          else
            exit = true;
        }
      }
      while ( !exit );
    }
    else if ( myDetourType == BreadthLeft || myDetourType == BreadthRight )
    {
      //Breadth detour algorithm
      aNext = globalSibling( myCurrent, myDetourType == BreadthLeft );
      if ( !aNext )
      {
        myCurrent = 0;
        for ( SUIT_DataObject* cur = myExtremeChild; cur && !myCurrent; cur = globalSibling( cur, myDetourType == BreadthLeft ) )
        {
          if ( cur->myChildren.count() > 0 )
          {
            myExtremeChild = extreme( cur->myChildren, myDetourType == BreadthLeft );
            myCurrent = myExtremeChild;
            myCurrentLevel++;
          }
        }
      }
      else
        myCurrent = aNext;
    }
  }
}

SUIT_DataObject* SUIT_DataObjectIterator::current() const
{
  return myCurrent;
}

int SUIT_DataObjectIterator::depth() const
{
  return myCurrentLevel;
}

int SUIT_DataObjectIterator::detour() const
{
  return myDetourType;
}

SUIT_DataObject* SUIT_DataObjectIterator::globalSibling( SUIT_DataObject* obj, bool next ) const
{
  SUIT_DataObject* par;

  if ( obj && ( par = parent( obj ) ) )
  {
    par->myChildren.find( obj );
    if ( par->myChildren.next() )
      return par->myChildren.current();
    else
    {
      for ( ; par; par = globalSibling( par, next ) )
      {
        if ( par->myChildren.count() > 0 )
          return extreme( par->myChildren, next );
      }
    }
    return 0;
  }
  else
    return 0;
}

SUIT_DataObject* SUIT_DataObjectIterator::extreme( DataObjectList& aList, bool FromLeft ) const
{
  if ( FromLeft )
    return aList.getFirst();
  else
    return aList.getLast();
}


SUIT_DataObjectLevelIterator::SUIT_DataObjectLevelIterator( SUIT_DataObject* root,
                                                            int start, int end, bool LeftToRight )
: SUIT_DataObjectIterator( root, LeftToRight ? BreadthLeft : BreadthRight )
{
  myStartLevel = start;
  if ( end > start )
    myEndLevel = end;
  else
    myEndLevel = myStartLevel;

  while ( current() && depth() < myStartLevel )
    SUIT_DataObjectIterator::operator++();
}

void SUIT_DataObjectLevelIterator::operator++()
{
  if ( myCurrent )
  {
    SUIT_DataObjectIterator::operator++();
    if ( depth() > myEndLevel )
      myCurrent = 0;
  }
}
