
#ifndef SUIT_TREE_SYNC_HEADER
#define SUIT_TREE_SYNC_HEADER

#include <qptrlist.h>
#include <qvaluelist.h>

template <class SrcItem, class TrgItem>
struct DiffItem
{
  SrcItem  mySrc;  //if it is null, then this item is to deleted
  TrgItem  myTrg;  //if it is null, then this item is to added
  //if both fields aren't null, then this item is to update
};

template <class SrcItem, class TrgItem, class TreeData>
TrgItem synchronize( const SrcItem&, const TrgItem&, const TreeData& );

template <class SrcItem, class TrgItem, class TreeData>
void diffSiblings( const SrcItem&, const TrgItem&,
                   QValueList < DiffItem < SrcItem,TrgItem > >&,
                   const TreeData& );

template <class SrcItem, class TrgItem, class TreeData>
TrgItem createSubTree( const SrcItem&, const TrgItem&, const TrgItem&, const bool, const TreeData& );

template <class SrcItem, class TrgItem, class TreeData>
const typename QValueList<TrgItem>::const_iterator findEqual( const QValueList<TrgItem>& l,
							      const typename QValueList<TrgItem>::const_iterator& first,
							      const SrcItem& it,
							      const TreeData& td );





//int gSync = 0;
template <class SrcItem, class TrgItem, class TreeData>
TrgItem synchronize( const SrcItem& r1, const TrgItem& r2, const TreeData& td )
{
  if( td.isEqual( r1, r2 ) )
  {
    // update items themselves
    td.updateItem( r2 );

    // iterate 'siblings' (direct children) 
    QValueList< DiffItem< SrcItem, TrgItem > > d;
    diffSiblings( r1, r2, d, td );

    typename QValueList< DiffItem< SrcItem, TrgItem > >::const_iterator anIt = d.begin(), aLast = d.end();
    bool isFirst = true;
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
	  lastItem = createSubTree( item.mySrc, r2, lastItem, isFirst, td );
	}
        else
	{
	  //to update
	  td.updateItem( item.myTrg );
	  synchronize( item.mySrc, item.myTrg, td );
	  lastItem = item.myTrg;
	}
	isFirst = false;
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

template <class SrcItem, class TrgItem, class TreeData>
void diffSiblings( const SrcItem& src, const TrgItem& trg,
		   QValueList < DiffItem < SrcItem,TrgItem > >& d,
		   const TreeData& td )
{
  if( src==td.nullSrc() || trg==td.nullTrg() )
    return;

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
