// Copyright (C) 2005  CEA/DEN, EDF R&D, OPEN CASCADE, PRINCIPIA R&D
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

#ifndef SUIT_TREE_SYNC_HEADER
#define SUIT_TREE_SYNC_HEADER

#include <qptrlist.h>
#include <qvaluelist.h>

/*!
  \struct DiffItem
  \brief Struct representing difference between items
*/
template <class SrcItem, class TrgItem>
struct DiffItem
{
  SrcItem  mySrc;
  /*! 
    \var mySrc
    if it is null, then this item is to deleted
  */
  TrgItem  myTrg;
  /*!
    \var myTrg
    if it is null, then this item is to added
    if both fields aren't null, then this item is to update
  */
};

/*!
  \brief synchronizes two trees
*/
template <class SrcItem, class TrgItem, class TreeData>
TrgItem synchronize( const SrcItem&, const TrgItem&, const TreeData& );

/*!
  \brief compares children 
*/
template <class SrcItem, class TrgItem, class TreeData>
void diffSiblings( const SrcItem&, const TrgItem&,
                   QValueList < DiffItem < SrcItem,TrgItem > >&,
                   const TreeData& );

/*!
  \brief create item with children (subtree)
*/
template <class SrcItem, class TrgItem, class TreeData>
TrgItem createSubTree( const SrcItem&, const TrgItem&, const TrgItem&, const bool, const TreeData& );

/*!
  \brief find equal element in list
*/
template <class SrcItem, class TrgItem, class TreeData>
const typename QValueList<TrgItem>::const_iterator findEqual( const QValueList<TrgItem>& l,
							      const typename QValueList<TrgItem>::const_iterator& first,
							      const SrcItem& it,
							      const TreeData& td );




/*!
  Synchronizes two trees by comparing corresponding items
  \param r1 - start item from first tree
  \param r2 - start item from second tree
  \param td - auxiliary class providing following methods:
  <ul>
  <li> bool     isEqual( const SrcItem&, const TrgItem& ) const - returns true if items are equal
  <li> SrcItem  nullSrc() const - returns null SrcItem
  <li> TrgItem  nullTrg() const - returns null TrgItem
  <li> TrgItem  createItem( 
    <ol>
      <li> const SrcItem& src,    - corresponding SrcItem
      <li> const TrgItem& parent, - parent TrgItem
      <li> const TrgItem& after,  - TrgItem after that new item must be added
      <li> const bool prepend     - whether new item must be added as first 
    </ol>
    ) const - creates new TrgItem
  <li> void     updateItem( const TrgItem& ) const - updates TrgItem without recreation
  <li> void     deleteItemWithChildren( const TrgItem& ) const - deletes TrgItem with all children
  <li> void     children( const SrcItem&, QValueList<SrcItem>& ) const - fills list with children
  <li> void     children( const TrgItem&, QValueList<TrgItem>& ) const - fills list with children
  <li> SrcItem  parent( const SrcItem& ) const - return parent SrcItem
  <li> TrgItem  parent( const TrgItem& ) const - return parent SrcItem
  </ul>
*/
template <class SrcItem, class TrgItem, class TreeData>
TrgItem synchronize( const SrcItem& r1, const TrgItem& r2, const TreeData& td )
{
  if( td.isEqual( r1, r2 ) )
  {
    // update items themselves
    td.updateItem( r1, r2 );

    // iterate 'siblings' (direct children) 
    QValueList< DiffItem< SrcItem, TrgItem > > d;
    diffSiblings( r1, r2, d, td );

    typename QValueList< DiffItem< SrcItem, TrgItem > >::const_iterator anIt = d.begin(), aLast = d.end();
    TrgItem lastItem = td.nullTrg();
    //    TrgItem tail = td.nullTrg();
    for( ; anIt!=aLast; anIt++ )
    {
      const DiffItem<SrcItem,TrgItem>& item = *anIt;
      if( item.mySrc==td.nullSrc() )
	if( item.myTrg==td.nullTrg() )
	  qDebug( "error: both null" );
        else
	  //to delete
	  td.deleteItemWithChildren( item.myTrg );
      else {
	if( item.myTrg==td.nullTrg() )
	{
	  //to add
	  TrgItem nitem = createSubTree( item.mySrc, r2, lastItem, lastItem==td.nullTrg(), td );
	  if( nitem!=td.nullTrg() )
	    lastItem = nitem;
	}
        else
	{
	  //to update
	  td.updateItem( item.mySrc, item.myTrg );
	  synchronize( item.mySrc, item.myTrg, td );
	  lastItem = item.myTrg;
	}
      }
    }

    return r2;
  }
  else
  {
    TrgItem new_r2 = createSubTree( r1, td.parent( r2 ), r2, false, td );
    if( r2!=td.nullTrg() )
      td.deleteItemWithChildren( r2 );
    return new_r2;
  }
}

/*!
  Finds equal element in list
  \return iterator
  \param l - list to search
  \param first - start iterator 
  \param it - item to be found
  \param td - tree data object (provides auxiliary methods)
*/
template <class SrcItem, class TrgItem, class TreeData>
const typename QValueList<TrgItem>::const_iterator findEqual( const QValueList<TrgItem>& l,
							      const typename QValueList<TrgItem>::const_iterator& first,
							      const SrcItem& it,
							      const TreeData& td )
{
  typename QValueList<TrgItem>::const_iterator cur = first, last = l.end();
  for( ; cur!=last; cur++ )
    if( td.isEqual( it, *cur ) )
      return cur;
  return last;
}

/*!
  Compares children of objects src and trg
  \param src - SrcItem to be checked
  \param trg - TrgItem to be checked
  \param d - map of difference to be filled
  \param td - tree data object (provides auxiliary methods)
*/
template <class SrcItem, class TrgItem, class TreeData>
void diffSiblings( const SrcItem& src, const TrgItem& trg,
		   QValueList < DiffItem < SrcItem,TrgItem > >& d,
		   const TreeData& td )
{
  //if( src==td.nullSrc() || trg==td.nullTrg() )
  //  return;

  QValueList<SrcItem> src_ch;
  QValueList<TrgItem> trg_ch;
  td.children( src, src_ch );
  td.children( trg, trg_ch );

  typename QValueList<SrcItem>::const_iterator src_it = src_ch.begin(), src_last = src_ch.end();
  typename QValueList<TrgItem>::const_iterator cur = trg_ch.begin(), trg_last = trg_ch.end();

  for( ; src_it!=src_last; src_it++ )
  {
    typename QValueList<TrgItem>::const_iterator f =
      findEqual<SrcItem, TrgItem, TreeData>( trg_ch, cur, *src_it, td );
    if( f!=trg_last )  //is found
    {
      //mark all items before found as "to be deleted"
      for( typename QValueList<TrgItem>::const_iterator it = cur; it!=f; it++ )
      {
	DiffItem<SrcItem,TrgItem> ndiff;
	ndiff.mySrc = td.nullSrc();
	ndiff.myTrg = *it; //to delete;
	d.append( ndiff );
      }
      cur = f;
      DiffItem<SrcItem,TrgItem> ndiff;
      ndiff.mySrc = *src_it;
      ndiff.myTrg = *cur; //update this item
      d.append( ndiff );
      cur++;
    }
    else //not found
    {
      DiffItem<SrcItem,TrgItem> ndiff;
      ndiff.mySrc = *src_it;
      ndiff.myTrg = td.nullTrg(); //add this item
      d.append( ndiff );
    }
  }
  for( ; cur!=trg_last; cur++ )
  {
    DiffItem<SrcItem,TrgItem> ndiff;
    ndiff.mySrc = td.nullSrc();
    ndiff.myTrg = *cur; //to delete;
    d.append( ndiff );
  }
}

/*!
  Creates sub-tree
  \return root of just created sub-tree
  \param src - corresponding SrcItem
  \param parent - parent of new TrgItem
  \param after - TrgItem, after that new item must be added
  \param asFirst - true if TrgItem must be added as first
  \param td - tree data object (provides auxiliary methods)
*/
template <class SrcItem, class TrgItem, class TreeData>
TrgItem createSubTree( const SrcItem& src, const TrgItem& parent,
		       const TrgItem& after, const bool asFirst,
		       const TreeData& td )
{
  if( src==td.nullSrc() )
    return td.nullTrg();

  TrgItem nitem = td.createItem( src, parent, after, asFirst );
  if( nitem==td.nullTrg() )
    return nitem;

  QValueList<SrcItem> ch;
  td.children( src, ch );
  typename QValueList<SrcItem>::const_iterator anIt = ch.begin(), aLast = ch.end();
  for( ; anIt!=aLast; anIt++ )
    createSubTree( *anIt, nitem, td.nullTrg(), false, td );

  return nitem;
}

#endif
