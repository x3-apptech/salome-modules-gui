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

// File   : OB_Browser.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#ifndef OB_BROWSER_H
#define OB_BROWSER_H

#include "OB.h"

#include <QWidget>
#include <QMap>
#include <QModelIndex>
#include <QItemSelection>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QAbstractItemModel;
class QAbstractItemDelegate;
class QToolTip;
class QMenu;
class QtxTreeView;
class QtxSearchTool;

class OB_EXPORT OB_Browser : public QWidget
{
  Q_OBJECT

  // TODO: commented - to be removed or revised
  //class ToolTip;

public:
  OB_Browser( QWidget* = 0, QAbstractItemModel* = 0 );
  virtual ~OB_Browser();

  QAbstractItemModel*    model() const;
  void                   setModel( QAbstractItemModel* );

  QAbstractItemDelegate* itemDelegate() const;
  void                   setItemDelegate( QAbstractItemDelegate* );

  bool                   rootIsDecorated() const;
  void                   setRootIsDecorated( const bool );

  bool                   sortMenuEnabled() const;
  void                   setSortMenuEnabled( const bool );

  QtxSearchTool*         searchTool() const;
  bool                   isSearchToolEnabled() const;
  void                   setSearchToolEnabled( const bool );

  int                    autoOpenLevel() const;
  void                   setAutoOpenLevel( const int );
  void                   openLevels( const int = -1 );

  //bool                 isShowToolTips();
  //void                 setShowToolTips( const bool theDisplay );

  int                    numberOfSelected() const;
  QModelIndexList        selectedIndexes() const;
  const QItemSelection   selection() const;

  virtual void           select( const QModelIndex&, const bool, const bool = true );
  virtual void           select( const QModelIndexList&, const bool, const bool = true );

  bool                   isOpen( const QModelIndex& ) const;
  virtual void           setOpen( const QModelIndex& theObject, const bool theOpen = true );

  void                   adjustWidth();
  void                   adjustFirstColumnWidth();
  void                   adjustColumnsWidth();

  // san - To be revised or removed
  // QTreeView::indexAt() should be used
  //SUIT_DataObject*  dataObjectAt( const QPoint& ) const;

  // san - Removed
  //OB_Filter*          filter() const;
  //void                setFilter( OB_Filter* );

  // TODO: QTreeView::resizeColumnToContents() can be used instead
  //virtual void      setWidthMode( QListView::WidthMode );

  unsigned long          getModifiedTime() const;
  void                   setModified();

  // san - moved to SUIT_TreeModel
  //OB_Updater*       getUpdater() const;
  //virtual void      setUpdater( OB_Updater* theUpdate = 0 );

  QtxTreeView*           treeView() const;

  QByteArray             getOpenStates( int theColumn ) const;
  void                   setOpenStates( const QByteArray&, int theColumn );

  typedef QMap<QString, bool> MapOfOpenStates;
  void                   openStates( bool isGet, MapOfOpenStates&, const QModelIndex& theIndex, int theColumn );

signals:
  void                   selectionChanged();
  //void                   doubleClicked( SUIT_DataObject* );
  //void                   dropped( DataObjectList, SUIT_DataObject*, int );

private slots:
  void                   onExpandAll();
  void                   onCollapseAll();
  virtual void           onExpanded( const QModelIndex& ) {}
  //void                   onDestroyed( SUIT_DataObject* );
  //void                   onDoubleClicked ( QListViewItem* );
  //void                   onDropped( QPtrList<QListViewItem>, QListViewItem*, int );

protected:
  //void              adjustWidth( QListViewItem* );
  //virtual void      updateText();

  virtual void           contextMenuEvent( QContextMenuEvent* );
  virtual void           createPopupMenu( QMenu* );

private:
  //typedef QMap<SUIT_DataObject*, QListViewItem*> ItemMap;
  //typedef SUIT_DataObjectKeyHandle               DataObjectKey;
  //typedef QMap<SUIT_DataObject*, int>            DataObjectMap;
  //typedef QMap<DataObjectKey, int>               DataObjectKeyMap;

private:
  bool                   hasCollased( const QModelIndex& ) const;
  bool                   hasExpanded( const QModelIndex& ) const;

  //void              autoOpenBranches();
  //void              openBranch( QListViewItem*, const int );

  //void              removeReferences( QListViewItem* );
  //void              removeConnections( SUIT_DataObject* );
  //void              createConnections( SUIT_DataObject* );
  //void              removeObject( SUIT_DataObject*, const bool = true );

  //void              updateText( QListViewItem* );
  //bool              needToUpdateTexts( QListViewItem* ) const;

  //DataObjectKey     objectKey( QListViewItem* ) const;
  //DataObjectKey     objectKey( SUIT_DataObject* ) const;

  //QListViewItem*    createTree( const SUIT_DataObject*, QListViewItem*, QListViewItem* = 0, const bool = false );
  //QListViewItem*    createItem( const SUIT_DataObject*, QListViewItem*, QListViewItem* = 0, const bool = false );

  //SUIT_DataObject*  storeState( DataObjectMap&, DataObjectMap&,
  //                              DataObjectKeyMap&, DataObjectKeyMap&, DataObjectKey& ) const;
  //void              restoreState( const DataObjectMap&, const DataObjectMap&, const SUIT_DataObject*,
  //                                const DataObjectKeyMap&, const DataObjectKeyMap&, const DataObjectKey& );

private:
  QtxTreeView*           myView;
  QtxSearchTool*         mySearchTool;
  // TODO: decide what to do with tooltip
  //QToolTip*           myTooltip;
  //QMap<int, int>      myColumnIds;
  // TODO: decide what to do with tooltip
  //bool                myShowToolTips;
  int                    myAutoOpenLevel;
  unsigned long          myModifiedTime;

  // TODO: decide what to do with tooltip
  //friend class OB_Browser::ToolTip;
};

#endif  // OB_BROWSER_H
