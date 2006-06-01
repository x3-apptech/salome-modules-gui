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
#include "SUIT_DataObjectIterator.h"

/*!
  Constructor.
*/
SUIT_DataObjectIterator::SUIT_DataObjectIterator( SUIT_DataObject* root, const int det, const bool fromTrueRoot )
: myRoot( root ),
myDetourType( det ),
myCurrentLevel( 0 )
{
  if ( myRoot && fromTrueRoot )
    myRoot = myRoot->root();

  myCurrent = myExtremeChild = myRoot;
}

/*!
  Gets parent for object \a obj.
*/
SUIT_DataObject* SUIT_DataObjectIterator::parent( SUIT_DataObject* obj ) const
{
  SUIT_DataObject* result = 0;
  if ( obj && obj != myRoot )
    result = obj->parent();
  return result;
}

/*!
  Increment operator.
*/
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

/*!
  Gets current data object.
*/
SUIT_DataObject* SUIT_DataObjectIterator::current() const
{
  return myCurrent;
}

/*!
  Gets depth of current lavel.
*/
int SUIT_DataObjectIterator::depth() const
{
  return myCurrentLevel;
}

/*!
  Gets detour type.
*/
int SUIT_DataObjectIterator::detour() const
{
  return myDetourType;
}

/*!
  Gets global sibling for object \a obj
*/
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

/*!
 * Gets first or last data object from list.
 * Get firls, if \a FromLeft == true, else last.
 */
SUIT_DataObject* SUIT_DataObjectIterator::extreme( DataObjectList& aList, bool FromLeft ) const
{
  if ( FromLeft )
    return aList.getFirst();
  else
    return aList.getLast();
}

/*!
  Constructor.
*/
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

/*!
  Increment operator.
*/
void SUIT_DataObjectLevelIterator::operator++()
{
  if ( myCurrent )
  {
    SUIT_DataObjectIterator::operator++();
    if ( depth() > myEndLevel )
      myCurrent = 0;
  }
}
