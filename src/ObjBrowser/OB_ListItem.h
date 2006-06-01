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

#ifndef OB_LISTITEM_H
#define OB_LISTITEM_H

#include "OB.h"

#include <qlistview.h>

class SUIT_DataObject;

/*!
  \class ListItemF
  Base template class for ListViewItems and CheckListItems
*/
template<class T> class ListItemF
{
public:
	ListItemF( T*, SUIT_DataObject* );
  /*ListItem( SUIT_DataObject*, QListView* );
  ListItem( SUIT_DataObject*, QListViewItem* );
  ListItem( SUIT_DataObject*, QListView*, QListViewItem* );
  ListItem( SUIT_DataObject*, QListViewItem*, QListViewItem* );

  ListItem( SUIT_DataObject*, QListView*, int );
  ListItem( SUIT_DataObject*, QListViewItem*, int );
  ListItem( SUIT_DataObject*, QListView*, QListViewItem*, int );
  ListItem( SUIT_DataObject*, QListViewItem*, QListViewItem*, int );*/

  void            setSel( bool s );
  inline SUIT_DataObject* dataObject() const { return myObject; }
  void            paintFoc( QPainter* p, QColorGroup& cg, const QRect& r );
  void            paintC( QPainter* p, QColorGroup& cg, int c, int w, int align );

//protected:
  void                     update();

protected:
  SUIT_DataObject* myObject;
  T*  myT;
};

/*!
  \class OB_ListItem
  Provides custom list item for Object Browser tree
*/
class OB_EXPORT OB_ListItem : public ListItemF<QListViewItem>, public QListViewItem
{
public:
	OB_ListItem( SUIT_DataObject*, QListView* );
	OB_ListItem( SUIT_DataObject*, QListViewItem* );
	OB_ListItem( SUIT_DataObject*, QListView*, QListViewItem* );
	OB_ListItem( SUIT_DataObject*, QListViewItem*, QListViewItem* );

	virtual ~OB_ListItem();

  virtual void            setSelected( bool s );
  virtual void            paintFocus( QPainter* p, const QColorGroup& cg, const QRect& r );
  virtual void            paintCell( QPainter* p, const QColorGroup& cg, int c, int w, int align );

  virtual void setText ( int column, const QString & text );

  virtual int      rtti() const;

  static int       RTTI();
};

/*!
  \class OB_CheckListItem
  Provides custom list item with check box
*/
class OB_EXPORT OB_CheckListItem : public ListItemF<QCheckListItem>, public QCheckListItem
{
public:
  OB_CheckListItem( SUIT_DataObject*, QListView*, Type = CheckBox );
  OB_CheckListItem( SUIT_DataObject*, QListViewItem*, Type = CheckBox );
  OB_CheckListItem( SUIT_DataObject*, QListView*, QListViewItem*, Type = CheckBox );
  OB_CheckListItem( SUIT_DataObject*, QListViewItem*, QListViewItem*, Type = CheckBox );

  virtual ~OB_CheckListItem();

  virtual void            setSelected( bool s );
  virtual void            paintFocus( QPainter* p, const QColorGroup& cg, const QRect& r );
  virtual void            paintCell( QPainter* p, const QColorGroup& cg, int c, int w, int align );


  virtual int      rtti() const;

  static int       RTTI();

protected:
  void             stateChange( bool );

//private:
//  void             update();
};

#endif
