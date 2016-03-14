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

#include "SUIT_DataObjectIterator.h"

/*!
  Constructor.
*/
SUIT_DataObjectIterator::SUIT_DataObjectIterator( SUIT_DataObject* root, const int det, const bool fromTrueRoot )
: myRoot( root ),
myDetourType( det )
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
        myChildrenIndexes.append(myDetourType == DepthLeft ? 0 : myCurrent->myChildren.size() - 1);
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
          int idx = myChildrenIndexes.last();
          if (myDetourType == DepthLeft && idx < aParent->myChildren.count() - 1) 
          {
            myChildrenIndexes.last()++;
            myCurrent = aParent->myChildren[idx + 1];
            exit = true;
          }
          else if (myDetourType == DepthRight && idx > 0) 
          {
            myChildrenIndexes.last()--;
            myCurrent = aParent->myChildren[idx - 1];
            exit = true;
          } else {
            myCurrent = aParent;
            myChildrenIndexes.removeLast();
          }
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
            myChildrenIndexes.append(myDetourType == BreadthLeft ? 0 : myCurrent->myChildren.size() - 1);
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
  return myChildrenIndexes.size();
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
    int idx = par->myChildren.indexOf( obj );
    if ( idx < par->myChildren.count() - 1 )
      return par->myChildren[idx + 1];
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
    return aList.first();
  else
    return aList.last();
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
