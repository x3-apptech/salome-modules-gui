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

// File   : OB_Browser.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#include "OB_Browser.h"

//#include "OB_Filter.h"
//#include "OB_ListItem.h"
//#include "OB_ListView.h"

#include <QtxTreeView.h>
#include <QtxSearchTool.h>
//#include <SUIT_DataObjectIterator.h>

#include <QAction>
#include <QMenu>
#include <QItemSelection>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QAbstractItemModel>
#include <QAbstractItemDelegate>
#include <QHeaderView>

#include <time.h>


/*!
  \class  OB_Browser::ToolTip
  Tool tip for OB_Browser.
*/

//TODO: ToolTip commented - to be removed or revised
/*
class OB_Browser::ToolTip : public QToolTip
{
public:
  ToolTip( OB_Browser* b, QWidget* p = 0 );
  virtual ~ToolTip();

  void        maybeTip( const QPoint& );

private:
  OB_Browser* myBrowser;
};
*/
/*!
  Constructor
*/
/*
OB_Browser::ToolTip::ToolTip( OB_Browser* b, QWidget* p )
: QToolTip( p ),
myBrowser( b )
{
}
*/
/*!
  Destructor
*/
/*
OB_Browser::ToolTip::~ToolTip()
{
}
*/
/*!
  It is called when there is a possibility that a tool tip
  should be shown and must decide whether there is a tool tip for the point
  in the widget that this QToolTip object relates to.
  \param pos - point co-ordinates
*/
/*
void OB_Browser::ToolTip::maybeTip( const QPoint& pos )
{
  if ( !parentWidget() || !myBrowser || !myBrowser->isShowToolTips() )
          return;

  QListView* lv = myBrowser->listView();

  QListViewItem* item = lv->itemAt( pos );
  SUIT_DataObject* obj = myBrowser->dataObject( item );
  if ( !obj )
    return;

  QString aText = obj->toolTip();

  if ( aText.isEmpty() )
    return;

  QRect aRect = lv->itemRect( item );

  tip( aRect, aText );
}
*/


/*!
  \class OB_Browser
  \brief Object browser widget which can be used to handle tree-like data model.

  The class OB_Browser implements public API of an object browser widget
  that can be used to display arbitrary application data in a hierarchical form.
  It is based on Qt4 model/view architecture. 

  Object browser can be used with conjuction of any custom item model inherited
  from QAbstractItemModel class (see Qt 4 reference manual).

  The class provides a functionality get/modify selection, drag-n-drop of the
  objects, etc.
*/

/*!
  \brief Constructor
  \param parent paren widget
  \param model data model
*/
OB_Browser::OB_Browser( QWidget* parent, QAbstractItemModel* model )
: QWidget( parent ),
  myAutoOpenLevel( 0 )
{
  // set-up tree view
  myView = new QtxTreeView( this );                                  // create tree view
  myView->setRootIsDecorated( true );                                // show root item
  myView->setSelectionMode( QAbstractItemView::ExtendedSelection );  // enable extended selection mode
  myView->setAllColumnsShowFocus( true );                            // focus is shown in all columns

  // enable drag-n-drop support
  myView->setDragDropMode( QAbstractItemView::DragDrop );            // enable both drag and drop operations
  myView->setDropIndicatorShown( true );                             // show drag indicator on dragging

  // set-up search tool
  mySearchTool = new QtxSearchTool( this, myView );                  // create search tool
  mySearchTool->setFrameStyle( QFrame::NoFrame | QFrame::Plain );    // do not show frame
  mySearchTool->setActivators( QtxSearchTool::StandardKey | QtxSearchTool::SlashKey ); // set activation mode
  mySearchTool->setSearcher( new QtxTreeViewSearcher( myView ) );    // assign searcher (for tree view)
  
  // layout widgets
  QVBoxLayout* main = new QVBoxLayout( this );
  main->addWidget( myView );
  main->addWidget( mySearchTool );
  main->setMargin( 0 );
  main->setSpacing( 3 );

  // TODO: decide what to do with tooltip
  //myShowToolTips = true;
  //myTooltip = new ToolTip( this, myView->viewport() );

  // TODO: drag-n-drop works differently - SUIT_TreeModel to be updated
  // and QTreeView needs some setup
  //connect( myView, SIGNAL( dropped( QPtrList<QListViewItem>, QListViewItem*, int ) ),
  //         this, SLOT( onDropped( QPtrList<QListViewItem>, QListViewItem*, int ) ) );
  setModel( model );

  connect( myView, SIGNAL( selectionChanged() ),
           this,   SIGNAL( selectionChanged() ) );
}

/*!
  \brief Destructor.
*/
OB_Browser::~OB_Browser()
{
  //delete myTooltip;
  //setUpdater( 0 );
}

/*!
  \brief Get data model.
  \return data model
  \sa setModel()
*/
QAbstractItemModel* OB_Browser::model() const
{
  return myView->model();
}

/*!
  \brief Set data model.
  \param model data model
  \sa model()
*/
void OB_Browser::setModel( QAbstractItemModel* model )
{
  myView->setModel( model );
  myView->update();
  setModified();
}

/*!
  \brief Get current item delegate (items renderer).
  \return currently used item delegate
  \sa setItemDelegate()
*/
QAbstractItemDelegate* OB_Browser::itemDelegate() const
{
  return myView->itemDelegate();
}

/*!
  \brief Set item delegate (items renderer).
  \param d custom item delegate
  \sa itemDelegate()
*/
void OB_Browser::setItemDelegate( QAbstractItemDelegate* d )
{
  myView->setItemDelegate( d );
}

/*!
  \brief Check if controls for expanding and collapsing top-level items are shown.
  \return \c true if top-level items are decorated
  \sa setRootIsDecorated()
*/
bool OB_Browser::rootIsDecorated() const
{
  return myView->rootIsDecorated();
}

/*!
  \brief Show/hide controls for expanding and collapsing top-level items.
  \param decor if \c true, top-level items are decorated
  \sa rootIsDecorated()
*/
void OB_Browser::setRootIsDecorated( const bool decor )
{
  if ( decor != rootIsDecorated() )
    myView->setRootIsDecorated( decor );
}

/*
  \brief Check if "Sorting" popup menu command for the header is enabled.
  \return \c true if "Sorting" menu command is enabled
  \sa setSortMenuEnabled()
*/
bool OB_Browser::sortMenuEnabled() const
{
  return myView->sortMenuEnabled();
}

/*
  \brief Enable/disable "Sorting" popup menu command for the header.
  \param enableSortMenu if \c true, enable "Sorting" menu command
  \sa sortMenuEnabled()
*/
void OB_Browser::setSortMenuEnabled( const bool enabled )
{
  if ( enabled != sortMenuEnabled() )
    myView->setSortMenuEnabled( enabled );
}

/*!
  \brief Get search tool widget.
  \return search tool widget
  \sa isSearchToolEnabled(), setSearchToolEnabled()
*/
QtxSearchTool* OB_Browser::searchTool() const
{
  return mySearchTool;
}

/*!
  \brief Check if search tool is enabled.
  \return \c true if search tool is enabled
  \sa setSearchToolEnabled(), searchTool()
*/
bool OB_Browser::isSearchToolEnabled() const
{
  return mySearchTool->isEnabled();
}

/*!
  \brief Enable/disable search tool.
  \param enable pass \c true to enable search tool
  \sa isSearchToolEnabled(), searchTool()
*/
void OB_Browser::setSearchToolEnabled( const bool enable )
{
  if ( mySearchTool->isEnabled() == enable )
    return;

  mySearchTool->setEnabled( enable );
  if ( !mySearchTool->isEnabled() )
    mySearchTool->hide();
}

/*!
  \brief Get number of levels which should be automatically expanded
  when updating the data tree.
  \return number of levels to be auto-opened on tree updating
  \sa setAutoOpenLevel()
*/
int OB_Browser::autoOpenLevel() const
{
  return myAutoOpenLevel;
}

/*!
  \brief Set number of levels which should be automatically expanded
  when updating the data tree.
  \param levels number of levels to be auto-opened on tree updating
  \sa autoOpenLevel()
*/
void OB_Browser::setAutoOpenLevel( const int levels )
{
  if ( myAutoOpenLevel != levels )
    myAutoOpenLevel = levels;
}

/*!
  \brief Expand all branches to the specified number of levels.

  If \a levels value is negative, then autoOpenLevel() value is used instead.
  
  \param levels number of levels to be expanded
  \sa autoOpenLevel()
*/
void OB_Browser::openLevels( const int levels )
{
  myView->expandLevels( levels < 0 ? autoOpenLevel() : levels );
}

/*!
  \return state "are tooltips shown"
*/
/*
bool OB_Browser::isShowToolTips()
{
  return myShowToolTips;
}
*/
/*!
  Sets new value of state "are tooltips shown"
  \param theDisplay - new value
*/
/*
void OB_Browser::setShowToolTips( const bool theDisplay )
{
  myShowToolTips = theDisplay;
}
*/

/*!
  \brief Get number of selected items.
  \return number of selected items
*/
int OB_Browser::numberOfSelected() const
{
  // we take selection by rows
  return myView->selectionModel() ? myView->selectionModel()->selectedRows().count() : 0;
}

/*!
  \brief Get all selected items.
  \return unsorted list of selected indexes with no duplicates
*/
QModelIndexList OB_Browser::selectedIndexes() const
{
  // we take selection by rows
  return myView->selectionModel() ? myView->selectionModel()->selectedRows() : QModelIndexList();
}

/*!
  \brief Get selection containing information about selected ranges.
  \return QItemSelection instance
*/
const QItemSelection OB_Browser::selection() const
{
  static QItemSelection emptySel;
  QItemSelection sel = emptySel;
  if ( myView->selectionModel() )
    sel = myView->selectionModel()->selection();
  return sel;
}

/*!
  \brief Select/deselect specified model index.
  \param index model index to be selected/deselected
  \param on if \c true, the index will be selected, otherwise - deselected
  \param keepSelection if \c true (default) the previous selection is kept, 
  otherwise it is first cleared
*/
void OB_Browser::select( const QModelIndex& index, const bool on, const bool keepSelection )
{
  if ( myView->selectionModel() ) {
    QItemSelectionModel::SelectionFlags f = on ? QItemSelectionModel::Select : QItemSelectionModel::Deselect;
    f = f | QItemSelectionModel::Rows;
    if ( !keepSelection )
      f = f | QItemSelectionModel::Clear;

    myView->selectionModel()->select( index, f );
  }
}

/*!
  \brief Select/deselect specified model indices.
  \param indexes model indices to be selected/deselected
  \param on if \c true, the indices will be selected, otherwise - deselected
  \param keepSelection if \c true (default) the previous selection is kept, 
  otherwise it is first cleared
*/
void OB_Browser::select( const QModelIndexList& indexes, const bool on, const bool keepSelection )
{
  bool blocked = myView->signalsBlocked();
  myView->blockSignals( true );

  QModelIndex idx;

  if ( !indexes.isEmpty() ) {
    QItemSelection mysel;
    // select by range if indexes are contiguous
    QModelIndex first=indexes.at(0);
    QModelIndex last=first;
    if (indexes.size() > 1) {
      for (int i = 1; i < indexes.size(); ++i) 
      {
        idx=indexes.at(i);
        if(idx.parent().row()==last.parent().row() && idx.parent().column()==last.parent().column() &&
           idx.row()==last.row()+1 && idx.column()==last.column())
        {
          // index is contiguous to last: extend the range
          last=idx;
        }
        else
        {
          // index idx is not contiguous: create a new range
          mysel.select(first,last);
          first=idx;
          last=idx;
        }
      }
    }
    mysel.select(first,last);

    if ( myView->selectionModel() ) {
      QItemSelectionModel::SelectionFlags f = on ? QItemSelectionModel::Select : QItemSelectionModel::Deselect;
      f = f | QItemSelectionModel::Rows;
      if ( !keepSelection )
        f = f | QItemSelectionModel::Clear;
      myView->selectionModel()->select( mysel, f );
    }
  }
  else if ( !keepSelection )
  {
    myView->clearSelection();
  }

  myView->blockSignals( blocked );
  emit( selectionChanged() );
}

/*!
  \brief Check if specified model index is expanded or collapsed.
  \param index model index
  \return \c true if model index is expanded
  \sa setOpen()
*/
bool OB_Browser::isOpen( const QModelIndex& index ) const
{
  return index.isValid() && model() && model()->hasChildren( index ) && myView->isExpanded( index );
}

/*!
  \brief Expand/collapse the specified model index.
  \param index model index
  \param open if \c true, the index will be expanded, otherwse - collapsed
  \sa isOpen()
*/
void OB_Browser::setOpen( const QModelIndex& index, const bool open )
{
  myView->setExpanded( index, open );  // hasChildren() ???
}

/*!
  \brief Adjust first column width to its contents.
*/
void OB_Browser::adjustWidth()
{
  myView->resizeColumnToEncloseContents( 0 );
}

/*!
  \brief Adjust first column width to its contents.
*/
void OB_Browser::adjustFirstColumnWidth()
{
  myView->resizeColumnToEncloseContents( 0 );
}

/*!
  \brief Adjust all columns width to its contents except the first column.
*/
void OB_Browser::adjustColumnsWidth()
{
  for ( int aCol = 1; aCol < myView->header()->count(); aCol++ ) {
    if ( myView->columnWidth( aCol ) > 0 )
      myView->resizeColumnToEncloseContents( aCol );
  }
}

/*!
  \return SUIT object correspondint to item at position 'pos'
  \param pos - position
*/
/* TODO: removed - QTreeView::indexAt() should be used
SUIT_DataObject* OB_Browser::dataObjectAt( const QPoint& pos ) const
{
  SUIT_DataObject* obj = 0;

  QListView* lv = listView();
  if ( lv )
    obj = dataObject( lv->itemAt( pos ) );

  return obj;
}
*/
/*!
  \return filter of list view
*/
/* TODO: removed
OB_Filter* OB_Browser::filter() const
{
  return myView->filter();
}
*/
/*!
  Changes filter of list view
  \param f - new filter
*/
/* TODO: removed
void OB_Browser::setFilter( OB_Filter* f )
{
  myView->setFilter( f );
}
*/
/*!
  Sets global width mode
  \param mode - new width mode
*/
/* TODO: removed
void OB_Browser::setWidthMode( QListView::WidthMode mode )
{
  for ( int i = 0, n = myView->columns(); i < n; i++ )
    if( mode!=QListView::Maximum || myView->columnWidth( i )>0 )
      myView->setColumnWidthMode( i, mode );
}
*/
/*!
  Updates tree
  \param obj - start object
  \param autoOpen - to open automatically branches of autoOpenLevel()
  \sa autoOpenLevel()
*/
/* TODO: removed
void OB_Browser::updateTree( SUIT_DataObject* obj, const bool autoOpen )
{
//  QTime t1 = QTime::currentTime();

  if ( !obj && !(obj = getRootObject()) )
    return;

  DataObjectKey curKey;
  DataObjectMap selObjs, openObjs;
  DataObjectKeyMap selKeys, openKeys;

  int selNum = numberOfSelected();

  SUIT_DataObject* curObj = storeState( selObjs, openObjs, selKeys, openKeys, curKey );

  updateView( obj );

  restoreState( selObjs, openObjs, curObj, selKeys, openKeys, curKey );

  if ( autoOpen )
    autoOpenBranches();

  setModified();

  if ( selNum != numberOfSelected() )
    emit selectionChanged();

//  QTime t2 = QTime::currentTime();
//  qDebug( QString( "update tree time = %1 msecs" ).arg( t1.msecsTo( t2 ) ) );
}
*/
/*!
  Replaces part of tree starting at object 'src' by tree starting at object 'trg'
*/
/* TODO: removed
void OB_Browser::replaceTree( SUIT_DataObject* src, SUIT_DataObject* trg )
{
  if ( !src || !trg || src == trg || src->root() != getRootObject() )
    return;

  DataObjectKey curKey;
  DataObjectMap selObjs, openObjs;
  DataObjectKeyMap selKeys, openKeys;

  int selNum = numberOfSelected();

  SUIT_DataObject* curObj = storeState( selObjs, openObjs, selKeys, openKeys, curKey );

  SUIT_DataObject* parent = src->parent();
  int pos = parent ? parent->childPos( src ) : -1;

  src->setParent( 0 );

  removeConnections( src );
  if ( isAutoDeleteObjects() )
    delete src;

  if ( parent && pos != -1 )
    parent->insertChild( trg, pos );

  trg->setParent( parent );

  updateView( trg );
  createConnections( trg );

  restoreState( selObjs, openObjs, curObj, selKeys, openKeys, curKey );

  setModified();

  if ( selNum != numberOfSelected() )
    emit selectionChanged();
}
*/
/*!
  Adjusts width by item
  \param item
*/
/*
void OB_Browser::adjustWidth( QListViewItem* item )
{
  while ( item )
  {
    item->widthChanged( 0 );
    if ( item->isOpen() )
      adjustWidth( item->firstChild() );
    item = item->nextSibling();
  }
}
*/

/*!
  \remove all items referencing current (through data objects)
*/
/* TODO:
void OB_Browser::removeReferences( QListViewItem* item )
{
  if ( !item )
    return;

  SUIT_DataObject* obj = dataObject( item );
  obj->disconnect( this, SLOT( onDestroyed( SUIT_DataObject* ) ) );
  myItems.remove( obj );

  QListViewItem* i = item->firstChild();
  while ( i )
  {
    removeReferences( i );
    i = i->nextSibling();
  }
}
*/
/*!
  Connects all children to SLOT onDestroyed
*/
/* TODO: move to SUIT_TreeModel
void OB_Browser::createConnections( SUIT_DataObject* obj )
{
  if ( !obj )
    return;

  DataObjectList childList;
  obj->children( childList, true );

  childList.prepend( obj );

  for ( DataObjectListIterator it( childList ); it.current(); ++it )
    it.current()->connect( this, SLOT( onDestroyed( SUIT_DataObject* ) ) );
}
*/
/*!
  Disconnects all children from SLOT onDestroyed
*/
/* TODO: move to SUIT_TreeModel
void OB_Browser::removeConnections( SUIT_DataObject* obj )
{
  if ( !obj )
    return;

  DataObjectList childList;
  obj->children( childList, true );

  childList.prepend( obj );

  for ( DataObjectListIterator it( childList ); it.current(); ++it )
    it.current()->disconnect( this, SLOT( onDestroyed( SUIT_DataObject* ) ) );
}
*/
/*!
  Stores states (opened, selected) of current tree items
  \return current item
  \param selObjs, selKeys - maps of selected objects
  \param openObjs, openKeys - maps of opened objects
  \param curKey - map of current objects
*/
/* TODO: to be revised
SUIT_DataObject* OB_Browser::storeState( DataObjectMap& selObjs, DataObjectMap& openObjs,
                                         DataObjectKeyMap& selKeys, DataObjectKeyMap& openKeys,
                                         DataObjectKey& curKey ) const
{
  QListView* lv = listView();
  if ( !lv )
    return 0;

  SUIT_DataObject* curObj = dataObject( lv->currentItem() );

  curKey = objectKey( curObj );

  for ( QListViewItemIterator it( lv ); it.current(); ++it )
  {
    SUIT_DataObject* obj = dataObject( it.current() );
    if ( !obj )
      continue;

    selObjs.insert( obj, lv->isSelected( it.current() ) );
    openObjs.insert( obj, lv->isOpen( it.current() ) );
    if ( lv->isSelected( it.current() ) )
      selKeys.insert( objectKey( obj ), 0 );
    if ( lv->isOpen( it.current() ) )
      openKeys.insert( objectKey( obj ), 0 );
  }

  return curObj;
}
*/
/*!
  Restores states (opened, selected) of current tree items
  \param selObjs, selKeys - maps of selected objects
  \param openObjs, openKeys - maps of opened objects
  \param curKey - map of current objects
*/
/* TODO: to be revised
void OB_Browser::restoreState( const DataObjectMap& selObjs, const DataObjectMap& openObjs,
                               const SUIT_DataObject* curObj, const DataObjectKeyMap& selKeys,
                               const DataObjectKeyMap& openKeys, const DataObjectKey& curKey )
{
  QListView* lv = listView();
  if ( !lv )
    return;

  bool block = lv->signalsBlocked();
  lv->blockSignals( true );

  QListViewItem* curItem = 0;
  for ( QListViewItemIterator it( lv ); it.current(); ++it )
  {
    QListViewItem* item = it.current();
    SUIT_DataObject* obj = dataObject( item );

    if ( !obj )
      continue;

    DataObjectKey key = objectKey( obj );

    if ( selObjs.contains( obj ) )
    {
      if ( selObjs[obj] && !lv->isSelected( item ) )
        lv->setSelected( item, true );
    }
    else if ( !key.isNull() && selKeys.contains( key ) && !lv->isSelected( item ) )
      lv->setSelected( item, true );

    if ( openObjs.contains( obj ) )
    {
      bool parentOpen = true;
      if( item && item->parent() )
        parentOpen = item->parent()->isOpen();

      if ( openObjs[obj] && parentOpen )
        lv->setOpen( item, true );
    }
    else if ( !key.isNull() && openKeys.contains( key ) )
    {
      bool parentOpen = true;
      if( item && item->parent() )
        parentOpen = item->parent()->isOpen();

      if( parentOpen )
        lv->setOpen( item, true );
    }

    if ( !curItem && ( curObj == obj || ( !curKey.isNull() && curKey == key )) )
      curItem = item;
  }

  if ( curItem )
    lv->setCurrentItem( curItem );

  lv->blockSignals( block );
}
*/
/*!
  Creates object key by tree item
*/
/* TODO: move to SUIT_TreeModel
OB_Browser::DataObjectKey OB_Browser::objectKey( QListViewItem* i ) const
{
  return objectKey( dataObject( i ) );
}
*/
/*!
  Creates object key by SUIT object
*/
/* TODO: move to SUIT_TreeModel
OB_Browser::DataObjectKey OB_Browser::objectKey( SUIT_DataObject* obj ) const
{
  if ( !obj )
    return 0;

  return DataObjectKey( obj->key() );
}
*/

/*!
  \brief Get tree view widget.
  \return tree view widget of the object browser
*/
QtxTreeView* OB_Browser::treeView() const
{
  return myView;
}

/*!
  \brief Process context menu request event.
  \param e context menu event
*/
void OB_Browser::contextMenuEvent( QContextMenuEvent* e )
{
  QMenu* popup = new QMenu();
  
  createPopupMenu( popup );

  Qtx::simplifySeparators( popup );

  if ( !popup->actions().isEmpty() )
    popup->exec( e->globalPos() );
  delete popup;
}

/*!
  \brief Get the time of the latest updating.
  \return latest updating time
*/
unsigned long OB_Browser::getModifiedTime() const
{ 
  return myModifiedTime; 
}

/*!
  \brief Update the time of the latest updating.
*/
void OB_Browser::setModified()
{
  myModifiedTime = clock();
}

/*!
  \brief Called when "Expand all" popup menu command is activated.
  
  Expands all selected items recursively.
*/
void OB_Browser::onExpandAll()
{
  QModelIndexList indexes = selectedIndexes();
  QModelIndex index;
  disconnect( treeView(), SIGNAL( expanded( const QModelIndex& ) ), 
           this,       SLOT( onExpanded( const QModelIndex& ) ) );
  
  foreach ( index, indexes ) {
    myView->expandAll( index );
  }
  connect( treeView(), SIGNAL( expanded( const QModelIndex& ) ), 
           this,       SLOT( onExpanded( const QModelIndex& ) ) );
  emit(onExpanded( index));
}

/*!
  \brief Called when "Collapse all" popup menu command is activated.
  
  Collapse all selected items recursively.
*/
void OB_Browser::onCollapseAll()
{
  QModelIndexList indexes = selectedIndexes();
  QModelIndex index;

  foreach ( index, indexes ) {
    myView->collapseAll( index );
  }
}

/*!
  SLOT: called if SUIT object is destroyed
*/
/* TODO: moved to SUIT_TreeModel
void OB_Browser::onDestroyed( SUIT_DataObject* obj )
{
  removeObject( obj );
}
*/
/*!
  SLOT: called on finish of drag-n-drop operation
  \param items - dragged items
  \param item - destination (item on that they were dropped)
  \param action - QDropEvent::Action
*/
  // TODO: drag-n-drop works differently - SUIT_TreeModel to be updated
  // and QTreeView needs some setup
/*
void OB_Browser::onDropped( QPtrList<QListViewItem> items, QListViewItem* item, int action )
{
  SUIT_DataObject* obj = dataObject( item );
  if ( !obj )
    return;

  DataObjectList lst;
  for ( QPtrListIterator<QListViewItem> it( items ); it.current(); ++it )
  {
    SUIT_DataObject* o = dataObject( it.current() );
    if ( o )
      lst.append( o );
  }

  if ( !lst.isEmpty() )
    emit dropped( lst, obj, action );
}
*/
/*!
  Updates texts of items
*/
/* TODO: to be removed
void OB_Browser::updateText()
{
  if ( myColumnIds.isEmpty() )
    return;

  QListView* lv = listView();
  if ( !lv )
    return;

  for ( QListViewItemIterator it( lv ); it.current(); ++it )
  {
    SUIT_DataObject* obj = dataObject( it.current() );
    if ( !obj )
      continue;

    for( QMap<int, int>::iterator itr = myColumnIds.begin(); itr != myColumnIds.end(); ++itr )
      it.current()->setText( itr.data(), obj->text( itr.key() ) );
  }
}
*/
/*!
  \return true if item must be updated
  \param item - item to be checked
*/
/* TODO: to be revised
bool OB_Browser::needToUpdateTexts( QListViewItem* item ) const
{
  SUIT_DataObject* obj = dataObject( item );
  if ( !obj )
    return false;

  for( QMap<int, int>::const_iterator it = myColumnIds.begin(); it != myColumnIds.end(); ++it )
    if( item->text( it.data() ) != obj->text( it.key() ) )
      return true;
  return false;
}
*/
/*!
  Updates texts of item
  \param item - item to be updated
*/
/* TODO: to be revised
void OB_Browser::updateText( QListViewItem* item )
{
  SUIT_DataObject* obj = dataObject( item );
  if ( !obj )
    return;

  for( QMap<int, int>::iterator it = myColumnIds.begin(); it != myColumnIds.end(); ++it )
    item->setText( it.data(), obj->text( it.key() ) );
}
*/

/*!
  \brief Add custom actions to the popup menu.
  \param menu popup menu
*/
void OB_Browser::createPopupMenu( QMenu* menu )
{
  menu->addSeparator();

  QModelIndexList indexes = selectedIndexes();

  bool closed = false, opened = false;
  
  for ( QModelIndexList::Iterator it = indexes.begin(); 
        it != indexes.end() && !closed; ++it ) {
    closed = hasCollased( *it );
  }

  for ( QModelIndexList::Iterator it = indexes.begin(); 
        it != indexes.end() && !opened; ++it ) {
    opened = hasExpanded( *it );
  }

  if ( closed )
    menu->addAction( tr( "MEN_EXPAND_ALL" ), this, SLOT( onExpandAll() ) );
  if ( opened )
    menu->addAction( tr( "MEN_COLLAPSE_ALL" ), this, SLOT( onCollapseAll() ) );

  if ( isSearchToolEnabled() ) {
    menu->addSeparator();
    menu->addAction( tr( "MEN_FIND" ), searchTool(), SLOT( find() ), QKeySequence(Qt::CTRL + Qt::Key_F) );
    menu->addSeparator();
  }
}

/*!
  Expands item with all it's children
*/
/* TODO: to be revised
void OB_Browser::expand( QListViewItem* item )
{
  if ( !item )
    return;

  item->setOpen( true );
  for ( QListViewItem* child = item->firstChild(); child; child = child->nextSibling() )
    expand( child );
}
*/
/*!
  \brief Check if model index is collapsed or has collapsed children.
  \return \c true if item or one of its children is collapsed
*/
bool OB_Browser::hasCollased( const QModelIndex& index ) const
{
  bool result = false;

  if ( index.isValid() && model() ) {
    bool hasChildren = model()->hasChildren( index );
    result = hasChildren && !myView->isExpanded( index );
    if ( !result && hasChildren ) {
      int rows = model()->rowCount( index );
      for ( int i = 0; i < rows && !result; i ++ ) {
        QModelIndex child = model()->index( i, 0, index );
        result = hasCollased( child );
      }
    }
  }
  return result;
}

/*!
  \brief Check if model index is expanded or has expanded children.
  \return \c true if item or one of its children is expanded
*/
bool OB_Browser::hasExpanded( const QModelIndex& index ) const
{
  bool result = false;

  if ( index.isValid() && model() ) {
    bool hasChildren = model()->hasChildren( index );
    result = hasChildren && myView->isExpanded( index );
    if ( !result && hasChildren ) {
      int rows = model()->rowCount( index );
      for ( int i = 0; i < rows && !result; i ++ ) {
        QModelIndex child = model()->index( i, 0, index );
        result = hasExpanded( child );
      }
    }
  }
  return result;
}

/*!
  Removes SUIT object
  \param obj - SUIT object to be removed
  \param autoUpd - auto tree updating
*/
/* TODO: moved to SUIT_TreeModel
void OB_Browser::removeObject( SUIT_DataObject* obj, const bool autoUpd )
{
  if ( !obj )
    return;

  // Removing list view items from <myItems> recursively for all children.
  // Otherwise, "delete item" line will destroy all item's children,
  // and <myItems> will contain invalid pointers (see ~QListViewItem() description in Qt docs)
  DataObjectList childList;
  obj->children( childList, true );
  for ( DataObjectListIterator it( childList ); it.current(); ++it )
  {
    it.current()->disconnect( this, SLOT( onDestroyed( SUIT_DataObject* ) ) );
    myItems.remove( it.current() );
  }

  QListViewItem* item = listViewItem( obj );

  obj->disconnect( this, SLOT( onDestroyed( SUIT_DataObject* ) ) );
  myItems.remove( obj );

  if ( obj == myRoot )
  {
    // remove all child list view items
    setRootObject( 0 );
    return;
  }

  if( !autoUpd )
    return;

  if ( isAutoUpdate() )
  {
    SUIT_DataObject* pObj = item && item->parent() ? dataObject( item->parent() ) : 0;
    updateTree( pObj, false );
  }

  delete item;
}
*/
/*!
  Opens branches from 1 to autoOpenLevel()
  \sa autoOpenLevel()
*/
/* TODO: to be revised
void OB_Browser::autoOpenBranches()
{
  openLevels();
}
*/
/*!
  Opens branch
  \param item
  \param level
*/
/* TODO: to be revised
void OB_Browser::openBranch( QListViewItem* item, const int level )
{
  if ( level < 1 )
    return;

  while ( item )
  {
    item->setOpen( true );
    openBranch( item->firstChild(), level - 1 );
    item = item->nextSibling();
  }
}
*/
/*!
  SLOT: called on double click on item, emits signal
*/
/* TODO: to be revised
void OB_Browser::onDoubleClicked( QListViewItem* item )
{
  if ( item )
    emit doubleClicked( dataObject( item ) );
}
*/

/*!
  \fn void OB_Browser::selectionChanged();
  \brief Emitted when selection is changed in the Object Browser.
*/

QByteArray OB_Browser::getOpenStates( int theColumn ) const
{
  QByteArray aData;
  QDataStream aStream( &aData, QIODevice::WriteOnly );
  MapOfOpenStates aMap;
  const_cast<OB_Browser*>( this )->openStates( true, aMap, QModelIndex(), theColumn );
  MapOfOpenStates::const_iterator anIt = aMap.begin(), aLast = aMap.end();
  for( ; anIt!=aLast; anIt++ )
  {
    QString anEntry = anIt.key();
    qint32 anOpenAttr = anIt.value() ? 1 : 0;
    aStream << anEntry << anOpenAttr;
  }
  return aData;
}

void OB_Browser::setOpenStates( const QByteArray& theData, int theColumn )
{
  QByteArray* aData = const_cast<QByteArray*>( &theData );
  QDataStream aStream( aData, QIODevice::ReadOnly );
  MapOfOpenStates aMap;
  while( !aStream.atEnd() )
  {
    QString anEntry;
    qint32 anOpenAttr;
    aStream >> anEntry >> anOpenAttr;
    bool isOpen = anOpenAttr!=0;
    aMap[anEntry] = isOpen;
  }
  openStates( false, aMap, QModelIndex(), theColumn );
}

void OB_Browser::openStates( bool isGet, MapOfOpenStates& theMap, const QModelIndex& theIndex, int theColumn )
{
  if( theIndex.isValid() )
  {
    QString anEntry = theIndex.sibling( theIndex.row(), theColumn ).data().toString();
    bool isOpen;
    if( isGet )
    {
      isOpen = treeView()->isExpanded( theIndex );
      theMap[anEntry] = isOpen;
    }
    else
    {
      isOpen = theMap.contains( anEntry ) ? theMap[anEntry] : false;
      treeView()->setExpanded( theIndex, isOpen );
    }
  }

  const QAbstractItemModel* aModel = model();

  int n = aModel->rowCount( theIndex );
  for( int i=0; i<n; i++ )
  {
    QModelIndex aChild = aModel->index( i, 0, theIndex );
    openStates( isGet, theMap, aChild, theColumn );
  }
}
