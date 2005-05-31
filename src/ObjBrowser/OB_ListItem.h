
#ifndef OB_LISTITEM_H
#define OB_LISTITEM_H

#include "OB.h"

#include <qlistview.h>

class SUIT_DataObject;

/* 
  base template class for ListViewItems and CheckListItems
*/

template<class T> class ListItem : public T
{
public:
  ListItem( SUIT_DataObject*, QListView* );
  ListItem( SUIT_DataObject*, QListViewItem* );
  ListItem( SUIT_DataObject*, QListView*, QListViewItem* );
  ListItem( SUIT_DataObject*, QListViewItem*, QListViewItem* );

  ListItem( SUIT_DataObject*, QListView*, int );
  ListItem( SUIT_DataObject*, QListViewItem*, int );
  ListItem( SUIT_DataObject*, QListView*, QListViewItem*, int );
  ListItem( SUIT_DataObject*, QListViewItem*, QListViewItem*, int );

  virtual void            setSelected( bool s );
  inline SUIT_DataObject* dataObject() const { return myObject; }
  virtual void            paintFocus( QPainter* p, const QColorGroup& cg, const QRect& r );
  virtual void            paintCell( QPainter* p, const QColorGroup& cg, int c, int w, int align );

private:
  void                     update();

private:
  SUIT_DataObject* myObject;
};

/* 
   ListViewItem class
*/

class OB_EXPORT OB_ListItem : public ListItem<QListViewItem>
{
public:
	OB_ListItem( SUIT_DataObject*, QListView* );
	OB_ListItem( SUIT_DataObject*, QListViewItem* );
	OB_ListItem( SUIT_DataObject*, QListView*, QListViewItem* );
	OB_ListItem( SUIT_DataObject*, QListViewItem*, QListViewItem* );

	virtual ~OB_ListItem();

  virtual int      rtti() const;

  static int       RTTI();
};

/* 
   CheckListItem class
*/

class OB_EXPORT OB_CheckListItem : public ListItem<QCheckListItem>
{
public:
  OB_CheckListItem( SUIT_DataObject*, QListView*, Type = CheckBox );
  OB_CheckListItem( SUIT_DataObject*, QListViewItem*, Type = CheckBox );
  OB_CheckListItem( SUIT_DataObject*, QListView*, QListViewItem*, Type = CheckBox );
  OB_CheckListItem( SUIT_DataObject*, QListViewItem*, QListViewItem*, Type = CheckBox );

  virtual ~OB_CheckListItem();

  virtual int      rtti() const;

  static int       RTTI();

protected:
  void             stateChange( bool );

private:
  void             update();
};

#endif
