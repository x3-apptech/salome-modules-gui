// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File:   SUIT_TreeModel.cxx
// Author: Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#include "SUIT_Session.h"
#include "SUIT_TreeModel.h"
#include "SUIT_TreeSync.h"
#include "SUIT_DataObject.h"
#include "SUIT_ResourceMgr.h"

#include <QApplication>
#include <QHash>
#include <QMimeData>

SUIT_AbstractModel::SUIT_AbstractModel() : mySearcher( 0 )
{
}

SUIT_AbstractModel::operator const QAbstractItemModel*() const
{
  return dynamic_cast<const QAbstractItemModel*>( this );
}

SUIT_AbstractModel::operator QAbstractItemModel*()
{
  return dynamic_cast<QAbstractItemModel*>( this );
}

SUIT_AbstractModel::operator const QObject*() const
{
  return dynamic_cast<const QObject*>( this );
}

SUIT_DataSearcher* SUIT_AbstractModel::searcher() const
{
  return mySearcher;
}

void SUIT_AbstractModel::setSearcher( SUIT_DataSearcher* s )
{
  mySearcher = s;
}


/*!
  \class SUIT_TreeModel::TreeItem
  \brief Internal class used for tree view synchronizaton with data object tree.
  \internal
*/

class SUIT_TreeModel::TreeItem
{
public:
  TreeItem( SUIT_DataObject* obj, TreeItem* parent = 0, TreeItem* after = 0 );
  ~TreeItem();

  void                  insertChild( TreeItem* child, TreeItem* after = 0 );
  void                  removeChild( TreeItem* child );
  SUIT_DataObject*      dataObject() const;
  TreeItem*             parent() const;
  int                   position() const;
  void                  setPosition(int position) {_position=position;};
  int                   childCount() const;
  TreeItem*             child( const int i );
  QList<TreeItem*>      children() const;
  TreeItem*             nextSibling() const;
  TreeItem*             prevSibling() const;
  
private:
  TreeItem*             myParent;
  QList<TreeItem*>      myChildren;
  SUIT_DataObject*      myObj;
  int _position;
};

/*!
  \brief Constructor.
  \internal
  \param obj data object
  \param parent parent item
  \param after tree item after each this one should be inserted
*/
SUIT_TreeModel::TreeItem::TreeItem( SUIT_DataObject*          obj, 
                                    SUIT_TreeModel::TreeItem* parent,
                                    SUIT_TreeModel::TreeItem* after )
: myParent( parent ),
  myObj( obj ),
  _position(-1)
{
  // Add <this> to the parent's children list
  if ( myParent )
    myParent->insertChild( this, after );
}

/*!
  \brief Destructor. Deletes all child items recursively.
  \internal
*/
SUIT_TreeModel::TreeItem::~TreeItem()
{
  // Ensure that all children are deleted;
  // each child removes itself from the children list
  while( myChildren.count() )
    delete myChildren.at( 0 );

  // Remove this item from the parent's children list
  if ( myParent )
    myParent->removeChild( this );
}

/*!
  \brief Insert child item.
  \internal
  \param child child item being inserted
  \param after tree item after each \a child should be inserted
*/
void SUIT_TreeModel::TreeItem::insertChild( SUIT_TreeModel::TreeItem* child, 
                                            SUIT_TreeModel::TreeItem* after )
{
  if ( !child )
    return;

  int index = after ? after->position() + 1 : 0;
  myChildren.insert( index, child );
}

/*!
  \brief Remove child item.
  \internal
  \param child child item being removed
*/
void SUIT_TreeModel::TreeItem::removeChild( SUIT_TreeModel::TreeItem* child )
{
  if ( !child )
    return;
  myChildren.removeAll( child );
}

/*!
  \brief Get data object.
  \internal
  \return data object this item is associated to
*/
SUIT_DataObject* SUIT_TreeModel::TreeItem::dataObject() const
{
  return myObj;
}

/*!
  \brief Get parent item.
  \internal
  \return parent item
*/
SUIT_TreeModel::TreeItem* SUIT_TreeModel::TreeItem::parent() const
{
  return myParent;
}

/*!
  \brief Get position of this item in its parent's children list.
  \internal
  \return item position
*/
int SUIT_TreeModel::TreeItem::position() const
{
  return _position;
}

/*!
  \brief Get number of child items.
  \internal
  \return number of children
*/
int SUIT_TreeModel::TreeItem::childCount() const
{
  return myChildren.count();
}

/*!
  \brief Get child item by specified index.
  \internal
  \param i child item index
  \return child item or 0 if \a i is out of range
*/
SUIT_TreeModel::TreeItem* SUIT_TreeModel::TreeItem::child( const int i )
{
  return i >= 0 && i < myChildren.count() ? myChildren.at( i ) : 0;
}

/*!
  \brief Get all child items.
  \internal
  \return list of child items
*/
QList<SUIT_TreeModel::TreeItem*> SUIT_TreeModel::TreeItem::children() const
{
  return myChildren;
}

/*!
  \brief Get next sibling item.
  \internal
  \return next sibling item or 0 if there are no any
*/
SUIT_TreeModel::TreeItem* SUIT_TreeModel::TreeItem::nextSibling() const
{
  return parent() ? parent()->child( position()+1 ) : 0;
}

/*!
  \brief Get previous sibling item.
  \internal
  \return previous sibling item or 0 if there are no any
*/
SUIT_TreeModel::TreeItem* SUIT_TreeModel::TreeItem::prevSibling() const
{
  return parent() ? parent()->child( position()-1 ) : 0;
}

/*!
  \class SUIT_TreeModel::TreeSync
  \brief Functor class for synchronizing data tree and tree model 
         when the data tree is changed outside the model.
  \internal
*/

class SUIT_TreeModel::TreeSync
{
public:
  TreeSync( SUIT_TreeModel* );
  bool              isEqual( const ObjPtr&, const ItemPtr& ) const;
  ObjPtr            nullSrc() const;
  ItemPtr           nullTrg() const;
  ItemPtr           createItem( const ObjPtr&, const ItemPtr&, const ItemPtr& ) const;
  void              updateItem( const ObjPtr&, const ItemPtr& ) const;
  void              deleteItemWithChildren( const ItemPtr& ) const;
  QList<ObjPtr>     children( const ObjPtr& ) const;
  QList<ItemPtr>    children( const ItemPtr& ) const;
  ItemPtr           parent( const ItemPtr& ) const;
private:
  bool              needUpdate( const ItemPtr& ) const;
  SUIT_TreeModel*   myModel;
};

/*!
  \brief Constructor.
  \internal
  \param model tree model
*/
SUIT_TreeModel::TreeSync::TreeSync( SUIT_TreeModel* model )
: myModel( model )
{
}

/*!
  \brief Check if item corresponds to the specified data object.
  \internal
  \param obj data object
  \param item tree item 
  \return \c true if item corresponds to the data object
*/
bool SUIT_TreeModel::TreeSync::isEqual( const ObjPtr& obj, const ItemPtr& item ) const
{
  bool isRoot = obj == myModel->root() && item == myModel->rootItem(),
       isEq   = obj && item && item->dataObject() == obj;
  return isRoot || ( !obj && !item ) || isEq;
}

/*!
  \brief Get null data object.
  \internal
  \return null data object
*/
SUIT_TreeModel::ObjPtr SUIT_TreeModel::TreeSync::nullSrc() const
{
  return 0;
}

/*!
  \brief Get null tree item.
  \internal
  \return null tree item
*/
SUIT_TreeModel::ItemPtr SUIT_TreeModel::TreeSync::nullTrg() const
{
  return 0;
}

/*!
  \brief Create an item corresponding to the specified data object.
  \internal
  \param obj data object
  \param parent parent tree item
  \param after tree item after each new one should be inserted
  \return created item
*/
SUIT_TreeModel::ItemPtr SUIT_TreeModel::TreeSync::createItem( const ObjPtr&  obj,
                                                              const ItemPtr& parent, 
                                                              const ItemPtr& after ) const
{
  ItemPtr item = myModel ? myModel->createItem( obj, parent, after ) : 0;

  // Additional actions that can't be performed by the model, e.g. expanded state
  if( item )
    obj->update();
  return item;
}

/*!
  \brief Update tree item.
  \internal
  \param obj reference data object
  \param item tree item to be updated
*/
void SUIT_TreeModel::TreeSync::updateItem( const ObjPtr& obj, const ItemPtr& item ) const
{
  if( obj )
    obj->update();
  if ( item && needUpdate( item ) ) 
    myModel->updateItem( item, false );
}

/*!
  \brief Delete item with all children recursively.
  \internal
  \param item tree item
*/
void SUIT_TreeModel::TreeSync::deleteItemWithChildren( const ItemPtr& item ) const
{
  // NOTE: item is deleted inside removeItem()!
  myModel->removeItem( item );
}

/*!
  \brief Get all the children of the specified data object.
  \internal
  \param obj data object
  \return list of the children
*/
QList<SUIT_TreeModel::ObjPtr> SUIT_TreeModel::TreeSync::children( const ObjPtr& obj ) const
{
  QList<ObjPtr> ch;
  if ( obj )
    ch = obj->children();
  return ch;
}

/*!
  \brief Get all the children of the specified tree item.
  \internal
  \param item tree item
  \return list of the children
*/
QList<SUIT_TreeModel::ItemPtr> SUIT_TreeModel::TreeSync::children( const ItemPtr& item ) const
{
  QList<ItemPtr> ch;
  if ( item ) 
    ch = item->children();
  return ch;
}

/*!
  \brief Get item which is the parent for the specified item.
  \internal
  \param item tree item
  \return parent item
*/
SUIT_TreeModel::ItemPtr SUIT_TreeModel::TreeSync::parent( const ItemPtr& item ) const
{
  return item ? item->parent() : 0;
}

/*!
  \brief Check if the tree item needs updating.
  \internal
  \param item tree item to be checked
  \return \c true if item needs updating

  \todo finalize this method
*/
bool SUIT_TreeModel::TreeSync::needUpdate( const ItemPtr& item ) const
{
  bool update = false;
  if ( item ) {
    SUIT_DataObject* obj = item->dataObject();
    if ( obj ) {
      // TODO: find simplified way to check if an item is not up-to-date:
      // - use check-sum of various item data
      // - use "LastModified" time stamp in data objects and tree items - hardly possible, for sometimes data objects do not know that data changes...
      // ...
      update = true; // TEMPORARY!!!
      // 1. check text
/*      update = ( item->text( 0 ) != obj->name() ) || myBrowser->needToUpdateTexts( item );

      if ( !update ) { 
        // 2. check pixmap (compare serialNumber()-s)
        QPixmap objPix = obj->icon();
        const QPixmap* itemPix = item->pixmap( 0 );
        update = (  objPix.isNull() && (  itemPix && !itemPix->isNull() ) ) || 
                 ( !objPix.isNull() && ( !itemPix ||  itemPix->isNull() ) ); 
        if ( !update && !objPix.isNull() && itemPix && !itemPix->isNull() ) {
          int aIconW = objPix.width();
          if( aIconW > 20 ) {
            QWMatrix aM;
            double aScale = 20.0 / aIconW;
            aM.scale( aScale, aScale );
            objPix = objPix.xForm( aM );
          }
          update = ( objPix.serialNumber() != itemPix->serialNumber() );
        }
      }*/
    }
  }
  return update;
}

/*!
  \class SUIT_TreeModel
  \brief Implementation of the model/view API based on the tree of SUIT_DataObject class
  instances.

  The SUIT_TreeModel class does not support insertion/removal of rows. It is synchronized 
  automatically with the tree of data objects used by SUIT-based applications to 
  expose their data in a hierarchical form to the user.
*/

/*!
  \brief Constructor.
  \param parent parent object
*/
SUIT_TreeModel::SUIT_TreeModel( QObject* parent )
: QAbstractItemModel( parent ),
  myRoot( 0 ),
  myRootItem( 0 ),
  myAutoDeleteTree( false ),
  myAutoUpdate( true ),
  myUpdateModified( false )
{
  initialize();
}

/*!
  \brief Constructor.
  \param root root data object
  \param parent parent object
*/
SUIT_TreeModel::SUIT_TreeModel( SUIT_DataObject* root, QObject* parent )
: QAbstractItemModel( parent ),
  myRoot( root ),
  myRootItem( 0 ),
  myAutoDeleteTree( false ),
  myAutoUpdate( true ),
  myUpdateModified( false )
{
  initialize();
}

/*!
  \brief Destructor
*/
SUIT_TreeModel::~SUIT_TreeModel()
{
  if ( autoDeleteTree() ) {
    SUIT_DataObject::disconnect( SIGNAL( inserted( SUIT_DataObject*, SUIT_DataObject* ) ),
                                 this, SLOT( onInserted( SUIT_DataObject*, SUIT_DataObject* ) ) );
    SUIT_DataObject::disconnect( SIGNAL( removed( SUIT_DataObject*, SUIT_DataObject* ) ),
                                 this, SLOT( onRemoved( SUIT_DataObject*, SUIT_DataObject* ) ) );
    SUIT_DataObject::disconnect( SIGNAL( modifed( SUIT_DataObject* ) ),
                                 this, SLOT( onModified( SUIT_DataObject* ) ) );
    delete myRoot;
  }

  delete myRootItem;
}

/*!
  \brief Register new column in the model
  \param group_id - unique data object group identificator
  \param name - translated column name
  \param custom_id - custom column id that should be passed into method SUIT_DataObject::data()
 */
void SUIT_TreeModel::registerColumn( const int group_id, const QString& name, const int custom_id )
{
  bool found = false;
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
  beginResetModel();
#endif
  for ( int i=0, n=myColumns.size(); i<n && !found; i++ ) {
    if ( name == myColumns[i].myName ) {
      myColumns[i].myIds.insert( group_id, custom_id );
      found = true;
    }
  }
  if ( !found ) {
    ColumnInfo inf;
    inf.myName = name;
    inf.myIds.insert( group_id, custom_id );
    inf.myAppropriate = Qtx::Shown;
    inf.myHeaderFlags = Qtx::ShowAll;
    int n = myColumns.size();
    myColumns.resize( n+1 );
    myColumns[n] = inf;
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    endResetModel();
#else
    reset();
#endif
  }
}

/*!
  \brief Remove column from the model

  Please take into account that column is removed only for given group_id, it means
  that information of data objects with such group_id won't be shown.
  If there is not any registered group_id for this column after removing, the column will be hidden
  otherwise it continue to be shown

  \param group_id - unique data object identificator allowing the classification of objects 
  \param name - translated column name
 */
void SUIT_TreeModel::unregisterColumn( const int group_id, const QString& name )
{
  for ( int i = 0, n = myColumns.size(); i < n; i++ ) {
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    beginResetModel();
#endif
    if ( myColumns[i].myName == name ) {
      myColumns[i].myIds.remove( group_id );
      if ( myColumns[i].myIds.isEmpty() ) {
        myColumns.remove( i );
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
        endResetModel();
#else
        reset();
#endif
      }
      break;
    }
  }
}

/*!
  \brief Change column icon.

  \param name - column name
  \param icon - new icon of the specified column
*/
void SUIT_TreeModel::setColumnIcon( const QString& name, const QPixmap& icon )
{
  for ( int i = 0, n = myColumns.size(); i < n; i++ ) {
    if ( myColumns[i].myName == name ) {
      myColumns[i].myIcon = icon;
      break;
    }
  }
}

/*!
  \brief Get column icon.

  \param name - column name
  \return icon of the specified column
*/
QPixmap SUIT_TreeModel::columnIcon( const QString& name ) const
{
  QPixmap res;
  for ( int i = 0, n = myColumns.size(); i < n; i++ ) {
    if ( myColumns[i].myName == name ) {
      res = myColumns[i].myIcon;
      break;
    }
  }
  return res;
}

/*!
  \brief Change appropriate status
  
  Appropriate status determines if the column should appear in the tree view header popup menu
  (to show/hide the column).

  If appropriate status is not specified yet, the \c Shown value is taken,
  it means that column should be always visible.

  \param name - column name
  \param appr - new appropriate status
*/
void SUIT_TreeModel::setAppropriate( const QString& name, const Qtx::Appropriate appr )
{
  for ( int i = 0, n = myColumns.size(); i < n; i++ ) {
    if ( myColumns[i].myName == name && myColumns[i].myAppropriate != appr ) {
      myColumns[i].myAppropriate = appr;
      emit headerDataChanged( Qt::Horizontal, i, i );
      break;
    }
  }
}

/*!
  \brief Check if the column should appear in the tree view header popup menu
  (to show/hide the column).

  Default implementation (if appropriate status is not specified yet)
  returns \c Shown, it means that column should be always visible.

  \param name - column name
  \return appropriate status
*/
Qtx::Appropriate SUIT_TreeModel::appropriate( const QString& name ) const
{
  Qtx::Appropriate appr = Qtx::Shown;
  for ( int i = 0, n = myColumns.size(); i < n; i++ ) {
    if ( myColumns[i].myName == name ) {
      appr = myColumns[i].myAppropriate;
      break;
    }
  }
  return appr;
}


/*!
  \brief Set header flags.
  
  These flags allow show in the header of the column text (name of the column),
  icon or both text and icon.
  
  \param name - column name
  \param flags - header flags

*/
void SUIT_TreeModel::setHeaderFlags( const QString& name, const Qtx::HeaderViewFlags flags )
{
  for ( int i = 0, n = myColumns.size(); i < n; i++ ) {
    if ( myColumns[i].myName == name && myColumns[i].myHeaderFlags != flags ) {
      myColumns[i].myHeaderFlags = flags;
      emit headerDataChanged( Qt::Horizontal, i, i );
      break;
    }
  }
}

/*!
  \brief Get the  header flags.
  
  These flags allow show in the header of the column text (name of the column),
  icon or both text and icon.
  
  \param name - column name
  \return header flags
*/
Qtx::HeaderViewFlags SUIT_TreeModel::headerFlags( const QString& name ) const
{
  Qtx::HeaderViewFlags flags;
  for ( int i = 0, n = myColumns.size(); i < n; i++ ) {
    if ( myColumns[i].myName == name ) {
      flags = myColumns[i].myHeaderFlags;
      break;
    }
  }
  return flags;
}

/*!
  \brief Set visibility state of the object.
  
  \param id - column name
  \param state - visible state
  \param emitChanged - if set to false, blocks dataChanged() signal, this can be used to
  prevent emitting dataChanged() several times for the same data object
*/
void SUIT_TreeModel::setVisibilityState( const QString& id, Qtx::VisibilityState state, bool emitChanged )
{
  VisibilityMap::const_iterator it = myVisibilityMap.find( id );
  if ( it != myVisibilityMap.end() && it.value() == state )
    return;
  
  bool needSignal = false;
  if ( state != Qtx::UnpresentableState ) {
    myVisibilityMap.insert( id, state );
    needSignal = true;
  }
  else {
    needSignal = myVisibilityMap.remove( id ) > 0;
  }
  if ( emitChanged && needSignal ) {
    QModelIndexList lst;
    if ( searcher() ) {
      SUIT_DataObject* o = searcher()->findObject( id );
      if ( o ) lst << index( o );
    }
    else {
      lst = match( index( 0, root()->customData( Qtx::IdType ).toInt() ), DisplayRole, id, 1, Qt::MatchExactly | Qt::MatchRecursive );
    }
    if ( !lst.isEmpty() ) {
      QModelIndex idx = index( lst.first().row(), SUIT_DataObject::VisibilityId, lst.first().parent() );
      emit dataChanged( idx, idx );
    }
  }
}

/*!
  \brief Set visibility state for all objects.
  
  \param state - visible state
*/
void SUIT_TreeModel::setVisibilityStateForAll( Qtx::VisibilityState state )
{
  foreach( QString id, myVisibilityMap.keys() )
    setVisibilityState( id, state );
}

/*!
  \brief Get visibility state of the object.
  
  \param id - column name
  \return visible state
*/
Qtx::VisibilityState SUIT_TreeModel::visibilityState( const QString& id ) const
{
  VisibilityMap::const_iterator it = myVisibilityMap.find( id );
  return it != myVisibilityMap.end() ? it.value() : Qtx::UnpresentableState;
}

/*!
  \brief Get data tree root object.
  \return data tree root
  \sa setRoot()
*/
SUIT_DataObject* SUIT_TreeModel::root() const
{
  return myRoot;
}

/*!
  \brief Set data tree root object.
  \param r new data tree root
  \sa root()
*/
void SUIT_TreeModel::setRoot( SUIT_DataObject* r )
{
  if ( root() == r )
    return;

#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
  beginResetModel();
#endif

  if ( autoDeleteTree() ) {
    SUIT_DataObject::disconnect( SIGNAL( inserted( SUIT_DataObject*, SUIT_DataObject* ) ),
                                 this, SLOT( onInserted( SUIT_DataObject*, SUIT_DataObject* ) ) );
    SUIT_DataObject::disconnect( SIGNAL( removed( SUIT_DataObject*, SUIT_DataObject* ) ),
                                 this, SLOT( onRemoved( SUIT_DataObject*, SUIT_DataObject* ) ) );
    SUIT_DataObject::disconnect( SIGNAL( modified( SUIT_DataObject* ) ),
                                 this, SLOT( onModified( SUIT_DataObject* ) ) );
    delete myRoot;
    
    if ( myRootItem ) {
      QList<TreeItem*> items = myRootItem->children();
      for ( QList<TreeItem*>::iterator anIt = items.begin(); anIt != items.end(); anIt++ )
        delete *anIt;
    }
  }

  myRoot = r;

  //initialize();
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
  endResetModel();
#else
  reset();
#endif
  emit modelUpdated();
}

/*!
  \brief Get data for the specified model index and data role.
  \param index model index
  \param role data role
  \return requested data
  \sa setData()
*/
QVariant SUIT_TreeModel::data( const QModelIndex& index, int role ) const
{
  if ( !index.isValid() )
    return QVariant();

  SUIT_DataObject* obj = object( index );
  if ( !obj )
    return QVariant();

  QColor c;
  QVariant val;

  int obj_group_id = obj->groupId();
  const ColumnInfo& inf = myColumns[index.column()];

  int id = -1;
  if( inf.myIds.contains( 0 ) )
    id = inf.myIds[0];
  if( inf.myIds.contains( obj_group_id ) )
    id = inf.myIds[obj_group_id];

  if( id<0 )
    return QVariant();

  if ( obj )
  {
    switch ( role )
        {
    case DisplayRole:
      // data object text for the specified column
      val = obj->text( id );
      break;
    case EditRole:
      // data object text for the specified column (for editor)
      val = obj->text( id );
      break;
    case DecorationRole: {
      // icon
      if ( id == SUIT_DataObject::VisibilityId ) {
	// for visibility column, icon is defined specifically (using data object id)
	QString objId = objectId( index );
	if ( myVisibilityMap.contains( objId ) ) {
	  // visibility status is defined -> return proper icon
	  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();	  
	  val  = ( myVisibilityMap.value( objId ) == Qtx::ShownState ) ? 
	    resMgr->loadPixmap( "SUIT", tr( "ICON_DATAOBJ_VISIBLE" ) ) : 
	    resMgr->loadPixmap( "SUIT", tr( "ICON_DATAOBJ_INVISIBLE" ) );
	} 
	else {
	  // visibility status is undefined -> no icon
	  val = QIcon();
	}
      }
      else {
	// for other columns get icon from the object
	val = obj->icon( id );
      }
      break;
    }
    case ToolTipRole:
      // data object tooltip for the specified column
      val = obj->toolTip( id ); 
      break;
    case StatusTipRole:
      // data object status tip for the specified column
      val = obj->statusTip( id ); 
      break;
    case WhatsThisRole:
      // data object what's this info for the specified column
      val = obj->whatsThis( id ); 
      break;
    case FontRole:
      // data object font for the specified column
      val = obj->font( id ); 
      break;
    case TextAlignmentRole:
      // data object text alignment for the specified column
      val = obj->alignment( id ); 
      break;
    case BackgroundRole:
      // data background color for the specified column
      c = obj->color( SUIT_DataObject::Background, id );
      if( !c.isValid() ) // default value
            c = QApplication::palette().color( QPalette::Base );
      c.setAlpha( 0 );
      val = c; 
      break;
    case ForegroundRole:
      // data foreground (text) color for the specified column
      c = obj->color( SUIT_DataObject::Foreground, id );
      if( !c.isValid() ) // default value
            c = QApplication::palette().color( QPalette::Foreground );
      val = c; 
      break;
    case BaseColorRole:
      // editor background color for the specified column
      c = obj->color( SUIT_DataObject::Base, id );
      if( !c.isValid() ) // default value
            c = QApplication::palette().color( QPalette::Base );
      val = c; 
      break;
    case TextColorRole:
      // editor foreground (text) color for the specified column
      c = obj->color( SUIT_DataObject::Text, id );
      if( !c.isValid() ) // default value
            c = QApplication::palette().color( QPalette::Text );
      val = c; 
      break;
    case HighlightRole:
      // adta object highlighted background color for the specified column
      c = obj->color( SUIT_DataObject::Highlight, id );
      if( !c.isValid() ) // default value
            c = QApplication::palette().color( QPalette::Highlight );
      val = c; 
      break;
    case HighlightedTextRole:
      // data object highlighted foreground (text) color for the specified column
      c = obj->color( SUIT_DataObject::HighlightedText, id );
      if( !c.isValid() ) // default value
            c = QApplication::palette().color( QPalette::HighlightedText );
      val = c; 
      break;
    case CheckStateRole:
      // data object checked state for the specified column
      // NOTE! three-state check is not supported currently
      if( obj->isCheckable( id ) )
            val = obj->isOn( id ) ? Qt::Checked : Qt::Unchecked; 
      break;
    case SizeHintRole:
      // data size hint
      // NOTE! not supported currently
      break;
    default:
      break;
    } // ... switch ( role ) ...
  } // ... if ( obj ) ...
  return val;
}

/*!
  \brief Set data for the specified model index and data role.
  \param index model index
  \param value new data value
  \param role data role
  \return \c true if data is set
  \sa data()
*/
bool SUIT_TreeModel::setData( const QModelIndex& index, 
                              const QVariant& value, int role )
{
  if ( index.isValid() && value.isValid() ) {
    SUIT_DataObject* obj = object( index );
    if ( obj ) {
      // NOTE! only 'check state' data is supported by default
      switch ( role ) {
      case CheckStateRole:
        // checked state
        if ( obj->isCheckable( index.column() ) ) {
          obj->setOn( value.toBool(), index.column() );
          emit( dataChanged( index, index ) );
          return true;
        }
        break;
      case EditRole: {
	    QString val = value.toString();
	    bool mod = obj->name() != val;
        if ( !val.isEmpty() && obj->setName(val) ) {
          emit( dataChanged( index, index ) );
          if (mod)
            emit ( renamed(obj) );
          return true;
        }
        return false;
        break;
      }
      default:
        break;
      }
    }
  }
  return QAbstractItemModel::setData( index, value, role );
}

/*!
  \brief Get data flags for specified model index.
  \param index model index
  \return data flags
*/
Qt::ItemFlags SUIT_TreeModel::flags( const QModelIndex& index ) const
{
  Qt::ItemFlags f = 0;

  if (!index.isValid())
    //return Qt::ItemIsDropEnabled; // items can be dropped into the top level of the model
    return f;

  SUIT_DataObject* obj = object(index);

  if (obj) {
    // data object is enabled
    if (obj->isEnabled())
      f = f | Qt::ItemIsEnabled;

    // data object is selectable
    if (obj->isSelectable())
      f = f | Qt::ItemIsSelectable;

    // data object is checkable
    if (obj->isCheckable(index.column()))
      f = f | Qt::ItemIsUserCheckable;
    
    // data object can be renamed
    if (obj->renameAllowed(index.column()))
      f = f | Qt::ItemIsEditable;
    
    // data object can be dragged
    if (obj->isDraggable())
      f = f | Qt::ItemIsDragEnabled;
    
    // another data object(s) can be dropped on this one
    if (obj->isDropAccepted())
      f = f | Qt::ItemIsDropEnabled;
  }

  return f;
}

Qt::DropActions SUIT_TreeModel::supportedDropActions() const
{
  return Qt::CopyAction | Qt::MoveAction;
}

/*!
  \brief Get header data (can be used in any data view).
  \param column column number
  \param orientation header orientation
  \param role data role
  \return header data
*/
QVariant SUIT_TreeModel::headerData( int column, Qt::Orientation orientation, int role ) const
{
  QVariant d;
  // NOTE! only horizontal header is supported
  if ( root() && orientation == Qt::Horizontal )
  {
    switch ( role )
        {
    case DisplayRole:
      // column title
      if((myColumns[column].myHeaderFlags & Qtx::ShowText) || 
	 (myColumns[column].myHeaderFlags == Qtx::ShowAll)) 
	d = myColumns[column].myName;
      else
	d = QString();
      break;
    case DecorationRole:
      // column icon
      if((myColumns[column].myHeaderFlags & Qtx::ShowIcon) || 
	 (myColumns[column].myHeaderFlags == Qtx::ShowAll)) 
	d = myColumns[column].myIcon;
      else
	d = QIcon();      
      break;
    case AppropriateRole:
      // appropriate flag (can column be hidden via context popup menu)
      d = myColumns[column].myAppropriate;
      break;
    default:
      break;
    }
  }
  return d;
}

/*!
  \brief Create model index.
  \param row data row
  \param column data column
  \param parent parent model index
  \return model index
*/
QModelIndex SUIT_TreeModel::index( int row, int column, 
                                   const QModelIndex& parent ) const
{
  if( hasIndex( row, column, parent ) )
  {
    TreeItem* parentItem = treeItem( parent );
    if( parentItem )
    {
      TreeItem* childItem = parentItem->child( row );
      if( childItem )
        return createIndex( row, column, childItem );
    }
  }
  return QModelIndex();
}

/*!
  \brief Get parent model index.
  \param index model index
  \return parent model index
*/
QModelIndex SUIT_TreeModel::parent( const QModelIndex& index ) const
{
  if ( !index.isValid() )
    return QModelIndex();

  TreeItem* childItem = treeItem( index );
  TreeItem* parentItem = childItem ? childItem->parent() : 0;

  if ( !parentItem || parentItem == rootItem() )
    return QModelIndex();

  return createIndex( parentItem->position(), 0, parentItem );
}

/*!
  \brief Get number of data columns.
  \param parent parent model index (not used)
  \return data columns number
  \sa rowCount()
*/
int SUIT_TreeModel::columnCount( const QModelIndex& /*parent*/ ) const
{
  return myColumns.size();
}

/*!
  \brief Get number of data rows (children of the specified model index).
  \param parent parent model index
  \return data rows (children) number
  \sa columnCount()
*/
int SUIT_TreeModel::rowCount( const QModelIndex& parent ) const
{
  // Commented by rnv in the frame of the 
  // "20830: EDF 1357 GUI : Hide/Show Icon" imp
  // if ( parent.column() > 0 )
  // return 0;

  TreeItem* parentItem = treeItem( parent );

  return parentItem ? parentItem->childCount() : 0;
}

/*!
  \brief Get data object by the specified model index.
  \param index model index
  \return data object corresponding to the model index
*/
SUIT_DataObject* SUIT_TreeModel::object( const QModelIndex& index ) const
{
  return object( treeItem( index ) );
}

/*!
  \brief Get model index by the specified data object.
  \param obj data object
  \param column data object column
  \return model index
*/
QModelIndex SUIT_TreeModel::index( const SUIT_DataObject* obj, int column ) const
{
  if ( obj == root() )
    return QModelIndex();

  TreeItem* item = treeItem( obj );

  return item ? createIndex( item->position(), column, item ) : QModelIndex();
}

/*!
  \brief Get 'auto-delete data tree' flag value.
  \return 'auto-delete data tree' flag value
  \sa setAutoDeleteTree()
*/
bool SUIT_TreeModel::autoDeleteTree() const
{
  return myAutoDeleteTree;
}

/*!
  \brief Set 'auto-delete data tree' flag value.

  If this flag is set to \c true, the data tree is deleted when
  the tree model is destroyed. Default value for this flag is \c false.

  \param on 'auto-delete data tree' flag value
  \sa autoDeleteTree()
*/
void SUIT_TreeModel::setAutoDeleteTree( const bool on )
{
  myAutoDeleteTree = on;
}

/*!
  \brief Get 'auto-update tree' flag value.
  \return 'auto-update tree' flag value
  \sa setAutoUpdate(), updateTree()
*/
bool SUIT_TreeModel::autoUpdate() const
{
  return myAutoUpdate;
}

/*!
  \brief Set 'auto-update tree' flag value.

  If this flag is set to \c true (by default), the model is updated
  automatically when data tree is changed.

  \param on 'auto-update tree' flag value
  \sa autoUpdate(), updateTree()
*/
void SUIT_TreeModel::setAutoUpdate( const bool on )
{
  if ( myAutoUpdate == on )
    return;

  SUIT_DataObject::disconnect( SIGNAL( inserted( SUIT_DataObject*, SUIT_DataObject* ) ),
                               this, SLOT( onInserted( SUIT_DataObject*, SUIT_DataObject* ) ) );
  SUIT_DataObject::disconnect( SIGNAL( removed( SUIT_DataObject*, SUIT_DataObject* ) ),
                               this, SLOT( onRemoved( SUIT_DataObject*, SUIT_DataObject* ) ) );
  SUIT_DataObject::disconnect( SIGNAL( modified( SUIT_DataObject* ) ),
                               this, SLOT( onModified( SUIT_DataObject* ) ) );
  myAutoUpdate = on;

  if ( myAutoUpdate ) {
    SUIT_DataObject::connect( SIGNAL( inserted( SUIT_DataObject*, SUIT_DataObject* ) ),
                              this, SLOT( onInserted( SUIT_DataObject*, SUIT_DataObject* ) ) );
    SUIT_DataObject::connect( SIGNAL( removed( SUIT_DataObject*, SUIT_DataObject* ) ),
                              this, SLOT( onRemoved( SUIT_DataObject*, SUIT_DataObject* ) ) );
    SUIT_DataObject::connect( SIGNAL( modified( SUIT_DataObject* ) ),
                              this, SLOT( onModified( SUIT_DataObject* ) ) );

    updateTree();
  }
}

/*!
  \brief Get 'updateModified' flag value.
  \return 'updateModified' flag value
*/
bool SUIT_TreeModel::updateModified() const
{
  return myUpdateModified;
}
/*!
  \brief Set 'updateModified' flag value.
  \param on 'updateModified' flag value
*/
void SUIT_TreeModel::setUpdateModified(const bool on)
{
  myUpdateModified=on;
}

/*!
  \brief Check if the specified column supports custom sorting.
  \param column column index on which data is being sorted
  \return \c true if column requires custom sorting
  \sa lessThan()
*/
bool SUIT_TreeModel::customSorting( const int column ) const
{
  return root() ? root()->customSorting( column ) : false;
}

void SUIT_TreeModel::forgetObject( const SUIT_DataObject* obj )
{
  removeItem( treeItem( obj ) );
}

/*!
  \brief Compares two model indexes for the sorting purposes.

  This method is called only for those columns for which customSorting()
  method returns \c true.

  \param left first index to compare
  \param right second index to compare
  \return result of the comparison
  \sa customSorting()
*/
bool SUIT_TreeModel::lessThan( const QModelIndex& left, const QModelIndex& right ) const
{
  QVariant ldata = data( left );
  QVariant rdata = data( right );
  return root() ? root()->compare( ldata, rdata, left.column() ) : false;
}

/*!
  \brief Get item delegate for the model.
  \return new item delegate
*/
QAbstractItemDelegate* SUIT_TreeModel::delegate() const
{
  return new SUIT_ItemDelegate( const_cast<SUIT_TreeModel*>( this ) );
}


void SUIT_TreeModel::emitClicked( SUIT_DataObject* obj, const QModelIndex& index) {
  int obj_group_id = obj->groupId();
  const ColumnInfo& inf = myColumns[index.column()];

  int id = -1;
  if( inf.myIds.contains( 0 ) )
    id = inf.myIds[0];
  if( inf.myIds.contains( obj_group_id ) )
    id = inf.myIds[obj_group_id];
  emit clicked(obj, id);
}

/*!
  \brief Update tree model.

  Call this method when data tree is changed outside the model.
  If the 'auto-update' flag is set to \c true, the model
  is updated automatically when the data tree is changed.

  \param index starting index for the updating
  \sa setAutoUpdate()
*/
void SUIT_TreeModel::updateTree( const QModelIndex& index )
{
  updateTree( object( index ) );
}


void SUIT_TreeModel::updateTreeModel(SUIT_DataObject* obj,TreeItem* item)
{
  int kobj=0;
  int kitem=0;
  int nobjchild=obj->childCount();
  SUIT_DataObject* sobj=obj->childObject(kobj);
  TreeItem* sitem = item->child(kitem);

  while(kobj < nobjchild)
    {
      if(sitem==0)
        {
          //end of item list
	  sitem=createItemAtPos(sobj,item,kitem);
          updateTreeModel(sobj,sitem);
          kobj++;
          kitem++;
          sobj=obj->childObject(kobj);
          sitem = item->child(kitem);
        }
      else if(sitem->dataObject() != sobj)
        {
          if(treeItem(sobj))
            {
              // item : to remove
              removeItem(sitem);
              sitem = item->child(kitem);
            }
          else
            {
              // obj : new object
              sitem=createItemAtPos(sobj,item,kitem);
	      updateTreeModel(sobj,sitem);
              kobj++;
              kitem++;
              sobj=obj->childObject(kobj);
              sitem = item->child(kitem);
            }
        }
      else
        {
          //obj and item are synchronised : go to next ones
          updateTreeModel(sobj,sitem);
          if(sobj->modified()) updateItem(sitem, true);
          sobj->update();
          kobj++;
          kitem++;
          sobj=obj->childObject(kobj);
          sitem = item->child(kitem);
        }
    }
  //remove remaining items
  for(int i = item->childCount(); i > kitem;i--)
    {
      sitem = item->child(i-1);
      removeItem(sitem);
    }
}

/*!
  \brief Update tree model.

  Call this method when data tree is changed outside the model.
  If the 'auto-update' flag is set to \c true, the model
  is updated automatically when the data tree is changed.

  \param obj starting data object for the updating
  \sa setAutoUpdate()
*/
void SUIT_TreeModel::updateTree( SUIT_DataObject* obj )
{
  if ( !obj )
    obj = root();

  else if ( obj->root() != root() )
    return;

  if(updateModified())
    {
      updateTreeModel(obj,treeItem( obj ));
    }
  else
    {
      synchronize<ObjPtr,ItemPtr,SUIT_TreeModel::TreeSync>( obj,
                                                            treeItem( obj ),
                                                            SUIT_TreeModel::TreeSync( this ) );
    }
  emit modelUpdated();
}

/*!
  \brief Initialize tree model.
*/
void SUIT_TreeModel::initialize()
{
  SUIT_DataObject::disconnect( SIGNAL( inserted( SUIT_DataObject*, SUIT_DataObject* ) ),
                               this, SLOT( onInserted( SUIT_DataObject*, SUIT_DataObject* ) ) );
  SUIT_DataObject::disconnect( SIGNAL( removed( SUIT_DataObject*, SUIT_DataObject* ) ),
                               this, SLOT( onRemoved( SUIT_DataObject*, SUIT_DataObject* ) ) );
  SUIT_DataObject::disconnect( SIGNAL( modified( SUIT_DataObject* ) ),
                               this, SLOT( onModified( SUIT_DataObject* ) ) );
  if ( autoUpdate() ) {
    SUIT_DataObject::connect( SIGNAL( inserted( SUIT_DataObject*, SUIT_DataObject* ) ),
                              this, SLOT( onInserted( SUIT_DataObject*, SUIT_DataObject* ) ) );
    SUIT_DataObject::connect( SIGNAL( removed( SUIT_DataObject*, SUIT_DataObject* ) ),
                              this, SLOT( onRemoved( SUIT_DataObject*, SUIT_DataObject* ) ) );
    SUIT_DataObject::connect( SIGNAL( modified( SUIT_DataObject* ) ),
                              this, SLOT( onModified( SUIT_DataObject* ) ) );
  }

  myItems.clear(); // ????? is it really necessary

  if ( !myRootItem )
    myRootItem = new TreeItem( 0 );

  registerColumn( 0, QObject::tr( "NAME_COLUMN" ), SUIT_DataObject::NameId );

  QString visCol = QObject::tr( "VISIBILITY_COLUMN" );
  registerColumn( 0, visCol, SUIT_DataObject::VisibilityId );

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  setColumnIcon( visCol, resMgr->loadPixmap( "SUIT", tr( "ICON_DATAOBJ_VISIBLE" ) ));
  setHeaderFlags( visCol, Qtx::ShowIcon);

  updateTree();
}

/*!
  \brief Get root tree item.
  \return root tree item
*/
SUIT_TreeModel::TreeItem* SUIT_TreeModel::rootItem() const
{
  return myRootItem;
}

/*!
  \brief Get tree item corresponding to the specified model index.
  \param index model index
  \return tree item or root item if index is invalid
*/
SUIT_TreeModel::TreeItem* SUIT_TreeModel::treeItem( const QModelIndex& index ) const
{
  return index.isValid() ? static_cast<TreeItem*>( index.internalPointer() ) : rootItem();
}

/*!
  \brief Get tree item corresponding to the specified data object.
  \param obj data object
  \return tree item or 0 if there is no tree item corresponding to \a obj
*/
SUIT_TreeModel::TreeItem* SUIT_TreeModel::treeItem( const SUIT_DataObject* obj ) const
{
  TreeItem* item = 0;

  if ( obj == root() )
    item = rootItem();
  else if ( myItems.contains( const_cast<SUIT_DataObject*>( obj ) ) )
    item = myItems[ const_cast<SUIT_DataObject*>( obj ) ];

  return item;
}

/*!
  \brief Get data object corresponding to the specified tree item.
  \param item tree item
  \return data object or 0 if there is no data object corresponding to \a item
*/
SUIT_DataObject* SUIT_TreeModel::object( const SUIT_TreeModel::TreeItem* item ) const
{
  if ( item == rootItem() )
    return root();
 
  SUIT_DataObject* obj = item ? item->dataObject() : 0;
  return myItems.contains( obj ) ? obj : 0;
}

/*!
  \brief Get unique object identifier
  
  Object identifier is customized via the Qtx::IdType custom data

  \param index model index
  \return object identifier or null string if it isn't specified
  \sa SUIT_DataObject::customData()
*/
QString SUIT_TreeModel::objectId( const QModelIndex& index ) const
{
  QString objId;
  if ( index.isValid() ) {
    SUIT_DataObject* obj = object( index );
    if ( obj ) {
      int anId = obj->customData( Qtx::IdType ).toInt(); 
      objId = data( createIndex( index.row(), anId, index.internalPointer() ) ).toString();
    }
  }
  return objId;
}

/*!
  \brief Create an item corresponding to the data object.
  \param obj source data object
  \param parent parent tree item
  \param after tree item after which new item should be inserted
  \return created tree item or 0 if item could not be created
*/
SUIT_TreeModel::TreeItem* SUIT_TreeModel::createItem( SUIT_DataObject* obj,
                                                      SUIT_TreeModel::TreeItem* parent, 
                                                      SUIT_TreeModel::TreeItem* after )
{
  if ( !obj )
    return 0;

  SUIT_DataObject* parentObj = object( parent );
  QModelIndex parentIdx = index( parentObj );

  SUIT_DataObject* afterObj = after ? object( after ) : 0;
  int row = afterObj ? afterObj->position() + 1 : 0;

  beginInsertRows( parentIdx, row, row );

  myItems[ obj ] = new TreeItem( obj, parent, after );

  for(int pos=row;pos < parent->childCount();pos++)
    parent->child(pos)->setPosition(pos);

  endInsertRows();

  obj->setModified(false);

  return myItems[ obj ];
}

/*!
  \brief Create an item corresponding to the data object.
  \param obj source data object
  \param parent parent tree item
  \param pos tree item position into which new item should be inserted
  \return created tree item or 0 if item could not be created
*/
SUIT_TreeModel::TreeItem* SUIT_TreeModel::createItemAtPos( SUIT_DataObject* obj,
                                                           SUIT_TreeModel::TreeItem* parent,
                                                           int pos )
{
  if ( !obj )
    return 0;

  SUIT_DataObject* parentObj = object( parent );
  QModelIndex parentIdx = index( parentObj );

  int row = pos ;
  SUIT_TreeModel::TreeItem* after = pos>0 ? parent->child(pos-1) : 0 ;

  beginInsertRows( parentIdx, row, row );

  SUIT_TreeModel::TreeItem* item = new TreeItem( obj, parent, after );
  myItems[ obj ] = item;

  for(int ppos=row;ppos < parent->childCount();ppos++)
    parent->child(ppos)->setPosition(ppos);

  endInsertRows();

  obj->setModified(false);

  return item;
}

/*!
  \brief Update tree item.
  \param item tree item to be updated
  \param emitLayoutChanged if signal about changed layout should be emitted
*/
void SUIT_TreeModel::updateItem( SUIT_TreeModel::TreeItem* item, bool emitLayoutChanged )
{
  if ( !item )
    return;
  
  SUIT_DataObject* obj = object( item );
  if ( !obj )
    return;
  
  // update all columns corresponding to the given data object
  /*To avoid crashes we should update any persistent model indexes before emitting layoutChanged(). In other words, when the structure changes:
      -  emit layoutAboutToBeChanged
      - Remember the QModelIndex that will change
      - call changePersistentIndex()
      - emit layoutChanged
  */

    emit layoutAboutToBeChanged();

    // Remember the QModelIndex that will change
    QModelIndexList fromIndexes;
    QModelIndexList toIndexes;
    for (int i = 0; i < columnCount() - 1; ++i) {
        fromIndexes.append( index( obj, i ));
        toIndexes.append(QModelIndex());
    }
    //changePersistentIndexList(fromIndexes, toIndexes); // Limitation: can lead to loss of selection

    emit dataChanged( toIndexes.first(), toIndexes.last() );
    obj->setModified(false);
    if ( emitLayoutChanged )
      emit layoutChanged();
}

/*!
  \brief Remove tree item (recursively).
  \param item tree item to be removed
*/
void SUIT_TreeModel::removeItem( SUIT_TreeModel::TreeItem* item )
{
  if ( !item )
    return;

  // Remove list view items from <myItems> recursively for all children.
  // Otherwise, "delete item" line below will destroy all item's children,
  // and <myItems> will contain invalid pointers
  while( item->childCount() )
    removeItem( item->child( 0 ) );

  SUIT_DataObject* obj = object( item );
  
  // Warning! obj can be deleted at this point!

  TreeItem* parent=item->parent();
  SUIT_DataObject* parentObj = object( parent );
  QModelIndex parentIdx = index( parentObj, 0 );
  int row = item->position();
  
  beginRemoveRows( parentIdx, row, row );
  myItems.remove( obj );

  if ( obj == root() )
    setRoot( 0 );
  else if ( parent )
    {
      parent->removeChild( item );
      for(int pos=row;pos < parent->childCount();pos++)
        parent->child(pos)->setPosition(pos);
    }

  delete item;

  endRemoveRows();
}

/*!
  \brief Called when the data object is inserted to the tree.
  \param object data object being inserted
  \param parent parent data object
*/
void SUIT_TreeModel::onInserted( SUIT_DataObject* /*object*/, SUIT_DataObject* parent )
{
  if ( autoUpdate() )
    updateTree( parent );
}

/*!
  \brief Called when the data object is removed from the tree.
  \param object data object being removed
  \param parent parent data object
*/
void SUIT_TreeModel::onRemoved( SUIT_DataObject* /*object*/, SUIT_DataObject* parent )
{
  if ( autoUpdate() )
    updateTree( parent );
}

/*!
  \brief Called when the data object is modified. TreeSync is not used here for maximum efficiency.
  It is assumed that it is up to the application to decide when its data objects are modified.
  \param obj data object that has been modified
*/
void SUIT_TreeModel::onModified( SUIT_DataObject* obj )
{
  if ( autoUpdate() )
  {
    QModelIndex firstIdx = index( obj, 0 );
    QModelIndex lastIdx  = index( obj, columnCount() - 1 );
    emit dataChanged( firstIdx, lastIdx );
    obj->setModified(false);
  }
}

/*!
  \brief Drag and Drop support.
*/
QStringList SUIT_TreeModel::mimeTypes() const
{
  QStringList types;
  types << "application/vnd.text.list";
  return types;
}

/*!
  \brief Called when the data objects are exported(dragged) from the tree.
  \param indexes the list of exported objects
*/
QMimeData* SUIT_TreeModel::mimeData( const QModelIndexList& indexes ) const
{
  QMimeData* mimeData = new QMimeData();
  QByteArray encodedData;

  QDataStream stream( &encodedData, QIODevice::WriteOnly );

  foreach ( QModelIndex index, indexes ) {
    QString id = objectId( index );
    // we have to check only 0 column in order to avoid repeating items in the drag object
    // - QTreeView tries to drag indices for all visible columns
    if ( index.isValid() && index.column() == 0 && !id.isEmpty() )
      stream << id;
  }

  mimeData->setData( "application/vnd.text.list", encodedData );
  return mimeData;
}

bool SUIT_TreeModel::dropMimeData( const QMimeData* data, Qt::DropAction action,
                                   int row, int column, const QModelIndex& parent )
{
  if ( action == Qt::IgnoreAction )
    // do nothing with data
    return false;

  if ( !data->hasFormat( "application/vnd.text.list" ) )
    // not supported data dropped
    return false;

  if ( !parent.isValid() )
    // dropping into the top level of the model is not allowed
    return false;

  // get parent object
  SUIT_DataObject* pobj = object( parent );
  if ( !pobj )
    // invalid parent
    return false;

  // decode mime data and collect data objects being dropped
  QByteArray encodedData = data->data( "application/vnd.text.list" );
  QDataStream stream( &encodedData, QIODevice::ReadOnly );
  
  DataObjectList objects;

  while ( !stream.atEnd() ) {
    QString id;
    stream >> id;
    if ( !id.isEmpty() && searcher() ) {
      SUIT_DataObject* obj = searcher()->findObject( id );
      if ( obj ) objects << obj;
    }
  }

  // emit signal
  emit dropped( objects, pobj, row, action );

  // return true if there's any to drop
  return !objects.isEmpty();
}

/*!
  \class SUIT_ProxyModel
  \brief Proxy model which can be used above the SUIT_TreeModel class
  to enable custom sorting/filtering of the data.

  The SUIT_TreeModel class does not support custom sorting/filtering of the data.
  To use these features, the SUIT_ProxyModel class can be used as top-level
  wrapper for the SUIT_DataObject-based data tree model.
*/

/*!
  \brief Constructor.
  \param parent parent object
*/
SUIT_ProxyModel::SUIT_ProxyModel( QObject* parent )
: QSortFilterProxyModel( parent ),
  mySortingEnabled( true )
{
  SUIT_TreeModel* model = new SUIT_TreeModel( this );
  connect( model, SIGNAL( modelUpdated() ), this, SIGNAL( modelUpdated() ) );
  connect( model, SIGNAL( clicked( SUIT_DataObject*, int ) ), this, SIGNAL(clicked( SUIT_DataObject*, int ) ) );
  connect( model, SIGNAL( dropped( const QList<SUIT_DataObject*>&, SUIT_DataObject*, int, Qt::DropAction ) ),
           this,  SIGNAL( dropped( const QList<SUIT_DataObject*>&, SUIT_DataObject*, int, Qt::DropAction ) ) );
  connect( model, SIGNAL( renamed( SUIT_DataObject* ) ), this, SIGNAL( renamed( SUIT_DataObject* ) ) );
  setSourceModel( model );
  setDynamicSortFilter( true );
}

/*!
  \brief Constructor.
  \param root root data object
  \param parent parent object
*/
SUIT_ProxyModel::SUIT_ProxyModel( SUIT_DataObject* root, QObject* parent )
: QSortFilterProxyModel( parent ),
  mySortingEnabled( true )
{
  SUIT_TreeModel* model = new SUIT_TreeModel( root, this );
  connect( model, SIGNAL( modelUpdated() ), this, SIGNAL( modelUpdated() ) );
  connect( model, SIGNAL( clicked( SUIT_DataObject*, int ) ), this, SIGNAL( clicked( SUIT_DataObject*, int ) ) );
  connect( model, SIGNAL( dropped( const QList<SUIT_DataObject*>&, SUIT_DataObject*, int, Qt::DropAction ) ),
           this,  SIGNAL( dropped( const QList<SUIT_DataObject*>&, SUIT_DataObject*, int, Qt::DropAction ) ) );
  connect( model, SIGNAL( renamed( SUIT_DataObject* ) ), this, SIGNAL( renamed( SUIT_DataObject* ) ) );
  setSourceModel( model );
  setDynamicSortFilter( true );
}

/*!
  \brief Constructor.
  \param model tree model
  \param parent parent object
*/
SUIT_ProxyModel::SUIT_ProxyModel( SUIT_AbstractModel* model, QObject* parent )
: QSortFilterProxyModel( parent ),
  mySortingEnabled( true )
{
  connect( *model, SIGNAL( modelUpdated() ), this, SIGNAL( modelUpdated() ) );
  connect( *model, SIGNAL( clicked( SUIT_DataObject*, int ) ), this, SIGNAL( clicked( SUIT_DataObject*, int ) ) );
  connect( *model, SIGNAL( dropped( const QList<SUIT_DataObject*>&, SUIT_DataObject*, int, Qt::DropAction ) ),
           this,   SIGNAL( dropped( const QList<SUIT_DataObject*>&, SUIT_DataObject*, int, Qt::DropAction ) ) );
  connect( *model, SIGNAL( renamed( SUIT_DataObject* ) ), this, SIGNAL( rename( SUIT_DataObject* ) ) );
  setSourceModel( *model );
  setDynamicSortFilter( true );
}

/*!
  \brief Destructor.
*/
SUIT_ProxyModel::~SUIT_ProxyModel()
{
}

/*!
  \brief Get data tree root object.
  \return data tree root
  \sa setRoot()
*/
SUIT_DataObject* SUIT_ProxyModel::root() const
{
  return treeModel() ? treeModel()->root() : 0;
}

/*!
  \brief Set data tree root object.
  \param r new data tree root
  \sa root()
*/
void SUIT_ProxyModel::setRoot( SUIT_DataObject* r )
{
  if ( treeModel() )
    treeModel()->setRoot( r );
}

/*!
  \brief Get data object by the specified model index.
  \param index model index
  \return data object corresponding to the model index
*/
SUIT_DataObject* SUIT_ProxyModel::object( const QModelIndex& index ) const
{
  return treeModel() ? treeModel()->object( mapToSource( index ) ) : 0;
}

/*!
  \brief Get model index by the specified data object.
  \param obj data object
  \param column data object column
  \return model index
*/
QModelIndex SUIT_ProxyModel::index( const SUIT_DataObject* obj, int column ) const
{
  return treeModel() ? mapFromSource( treeModel()->index( obj, column ) ) : QModelIndex();
}

/*!
  \brief Get 'auto-delete data tree' flag value.
  \return 'auto-delete data tree' flag value
  \sa setAutoDeleteTree()
*/
bool SUIT_ProxyModel::autoDeleteTree() const
{
  return treeModel() ? treeModel()->autoDeleteTree() : false;
}

/*!
  \brief Set 'auto-delete data tree' flag value.

  If this flag is set to \c true, the data tree is deleted when
  the tree model is destroyed. Default value for this flag is \c false.

  \param on 'auto-delete data tree' flag value
  \sa autoDeleteTree()
*/
void SUIT_ProxyModel::setAutoDeleteTree( const bool on )
{
  if ( treeModel() )
    treeModel()->setAutoDeleteTree( on );
}

/*!
  \brief Get 'auto-update tree' flag value.
  \return 'auto-update tree' flag value
  \sa setAutoUpdate(), updateTree()
*/
bool SUIT_ProxyModel::autoUpdate() const
{
  return treeModel() ? treeModel()->autoUpdate() : false;
}

/*!
  \brief Get 'updateModified' flag value.
  \return 'updateModified' flag value
*/
bool SUIT_ProxyModel::updateModified() const
{
  return treeModel() ? treeModel()->updateModified() : false;
}
/*!
  \brief Set 'updateModified' flag value.

  If this flag is set to \c true (default=false), the model is updated by updateTreeModel that 
  uses the isModified flag to update only modified objects

  \param on 'updateModified' flag value
*/
void SUIT_ProxyModel::setUpdateModified( const bool on )
{
  if ( treeModel() )
    treeModel()->setUpdateModified( on );
}

/*!
  \brief Set 'auto-update tree' flag value.

  If this flag is set to \c true (by default), the model is updated
  automatically when data tree is changed.

  \param on 'auto-update tree' flag value
  \sa autoUpdate(), updateTree()
*/
void SUIT_ProxyModel::setAutoUpdate( const bool on )
{
  if ( treeModel() )
    treeModel()->setAutoUpdate( on );
}

/*!
  \brief Check if sorting is enabled.
  \return \c true if sorting is enabled
  \sa setSortingEnabled()
*/
bool SUIT_ProxyModel::isSortingEnabled() const
{
  return mySortingEnabled;
}

SUIT_DataSearcher* SUIT_ProxyModel::searcher() const
{
  return treeModel() ? treeModel()->searcher() : 0;
}

void SUIT_ProxyModel::setSearcher( SUIT_DataSearcher* s )
{
  if ( treeModel() ) treeModel()->setSearcher( s );
}

/*!
  \brief Get item delegate for the model.
  \return new item delegate
*/
QAbstractItemDelegate* SUIT_ProxyModel::delegate() const
{
  return treeModel() ? treeModel()->delegate() : 0;
}

/*!
  \brief Update tree model.

  Call this method when data tree is changed outside the model.
  If the 'auto-update' flag is set to \c true, the model
  is updated automatically when the data tree is changed.

  \param index starting index for the updating
  \sa setAutoUpdate()
*/
void SUIT_ProxyModel::updateTree( const QModelIndex& index )
{
  if ( treeModel() )
    treeModel()->updateTree( mapToSource( index ) );
}

/*!
  \brief Update tree model.

  Call this method when data tree is changed outside the model.
  If the 'auto-update' flag is set to \c true, the model
  is updated automatically when the data tree is changed.

  \param obj starting data object for the updating
  \sa setAutoUpdate()
*/
void SUIT_ProxyModel::updateTree( SUIT_DataObject* obj )
{
  if ( treeModel() )
    treeModel()->updateTree( obj );
}

void SUIT_ProxyModel::forgetObject( const SUIT_DataObject* obj )
{
  if ( treeModel() )
    treeModel()->forgetObject( obj );
}

/*!
  \brief Compares two model indexes for the sorting purposes.
  \param left first index to compare
  \param right second index to compare
  \return result of the comparison
*/
bool SUIT_ProxyModel::lessThan( const QModelIndex& left, const QModelIndex& right ) const
{
  if ( !isSortingEnabled() && left.isValid() && right.isValid() ) {
    return left.row() < right.row();
  }
  if ( treeModel() && treeModel()->customSorting( left.column() ) ) {
    return treeModel()->lessThan( left, right );
  }
  return QSortFilterProxyModel::lessThan( left, right );
}

/*!
  \brief Check if the specified column supports custom sorting.
  \param column column index on which data is being sorted
  \return \c true if column requires custom sorting
  \sa lessThan()
*/
bool SUIT_ProxyModel::customSorting( const int column ) const
{
  return treeModel() ? treeModel()->customSorting( column ) : false;
}

/*!
  \brief Enable/disable sorting.
  \param enabled new flag state
  \sa isSortingEnabled()
*/
void SUIT_ProxyModel::setSortingEnabled( bool enabled )
{
  mySortingEnabled = enabled;
  clear();
}

/*
  \brief Get tree model.
  \return tree model
*/
SUIT_AbstractModel* SUIT_ProxyModel::treeModel() const
{
  return dynamic_cast<SUIT_AbstractModel*>( sourceModel() );
}

/*!
  \brief Filter rows
  \param sourceRow row index of the source data model
  \param sourceParent parent model index of the source data model
  \return \c true if the specified row should be filtered out (i.e. not displayed) or \c false otherwise
*/
bool SUIT_ProxyModel::filterAcceptsRow( int sourceRow, const QModelIndex& sourceParent ) const
{
  SUIT_DataObject* o = treeModel()->object( sourceModel()->index( sourceRow, 0, sourceParent ) );
  SUIT_DataObject* p = o ? o->parent() : 0;
  return ( !p || p->expandable() ) && o && o->isVisible();
}

/*!
  \brief Register new column in the model
  \param group_id - unique data object identificator allowing the classification of objects 
  \param name - translated column name
  \param custom_id - custom column id that should be passed into method SUIT_DataObject::data()
 */
void SUIT_ProxyModel::registerColumn( const int group_id, const QString& name, const int custom_id )
{
  if( treeModel() )
    treeModel()->registerColumn( group_id, name, custom_id );
}

/*!
  \brief Remove column from the model

  Please take into account that column is removed only for given group_id, it means
  that information of data objects with such group_id won't be shown.
  If there is not any registered group_id for this column after removing, the column will be hidden
  otherwise it continue to be shown

  \param group_id - unique data object identificator allowing the classification of objects 
  \param name - translated column name
 */
void SUIT_ProxyModel::unregisterColumn( const int group_id, const QString& name )
{
  if( treeModel() )
    treeModel()->unregisterColumn( group_id, name );
}

/*!
  \brief Change column icon.

  \param name - column name
  \param icon - new icon of the specified column
*/
void SUIT_ProxyModel::setColumnIcon( const QString& name, const QPixmap& icon )
{
  if( treeModel() )
    treeModel()->setColumnIcon( name, icon );
}

/*!
  \brief Get column icon.

  \param name - column name
  \return icon of the specified column
*/
QPixmap SUIT_ProxyModel::columnIcon( const QString& name ) const
{
  return treeModel() ? treeModel()->columnIcon( name ) : QPixmap();
}

/*!
  \brief Change appropriate status
  
  Appropriate status determines if the column should appear in the tree view header popup menu
  (to show/hide the column).

  If appropriate status is not specified yet, the \c Shown value is taken,
  it means that column should be always visible.

  \param name - column name
  \param appr - new appropriate status
*/
void SUIT_ProxyModel::setAppropriate( const QString& name, const Qtx::Appropriate appr )
{
  if( treeModel() )
    treeModel()->setAppropriate( name, appr );
}

/*!
  \brief Check if the column should appear in the tree view header popup menu
  (to show/hide the column).

  Default implementation (if appropriate status is not specified yet)
  returns \c Shown, it means that column should be always visible.

  \param name - column name
  \return appropriate status
*/
Qtx::Appropriate SUIT_ProxyModel::appropriate( const QString& name ) const
{
  return treeModel() ? treeModel()->appropriate( name ) : Qtx::Shown;
}

/*!
  \brief Set header flags.
  
  These flags allow show in the header of the column text (name of the column),
  icon or both text and icon.
  
  \param name - column name
  \param flags - header flags

*/
void SUIT_ProxyModel::setHeaderFlags( const QString& name, const Qtx::HeaderViewFlags flags )
{
  if(treeModel())
    treeModel()->setHeaderFlags(name, flags);
}

/*!
  \brief Get the  header flags.
  
  These flags allow show in the header of the column text (name of the column),
  icon or both text and icon.
  
  \param name - column name
  \return header flags
*/
Qtx::HeaderViewFlags SUIT_ProxyModel::headerFlags( const QString& name ) const
{
  return treeModel() ? treeModel()->headerFlags( name ) : Qtx::ShowAll;
}

/*!
  \brief Set visibility state of the object.
  
  \param id - column name
  \param state - visible state
  \param emitChanged - if set to false, blocks dataChanged() signal, this can be used to
  prevent emitting dataChanged() several times for the same data object
*/
void SUIT_ProxyModel::setVisibilityState(const QString& id, Qtx::VisibilityState state, bool emitChanged ) {
  if(treeModel())
    treeModel()->setVisibilityState(id,state,emitChanged);
}

/*!
  \brief Set visibility state for all objects.
  
  \param id - column name
  \param state - visible state
*/
void SUIT_ProxyModel::setVisibilityStateForAll(Qtx::VisibilityState state)
{
  if(treeModel())
    treeModel()->setVisibilityStateForAll(state);
}

/*!
  \brief Get visibility state of the object.
  
  \param id - column name
  \return visible state
*/
Qtx::VisibilityState SUIT_ProxyModel::visibilityState(const QString& id) const
{
  return treeModel() ? treeModel()->visibilityState(id) : Qtx::UnpresentableState;
}

void SUIT_ProxyModel::emitClicked( SUIT_DataObject* obj, const QModelIndex& index)
{
  if(treeModel())
    treeModel()->emitClicked(obj,index);
}

/*!
  \class SUIT_ItemDelegate
  \brief An SUIT_DataObject-based item delegate class.

  This class can be used to render the SUIT_DataObject-based item
  in the widgets like QTreeView and others.
  Refer to the Qt 4 documentation, model/view architecture 
  section for more details).
*/

/*!
  \brief Constructor.
  \param parent parent object
*/
SUIT_ItemDelegate::SUIT_ItemDelegate( QObject* parent )
: QItemDelegate( parent )
{
}

/*!
  \brief Render the item in the widget.

  Customizes the item colors for the specific roles.

  \param painter painter
  \param option painting option
  \param index model index being rendered
*/
void SUIT_ItemDelegate::paint( QPainter* painter, 
                               const QStyleOptionViewItem& option,
                               const QModelIndex& index ) const
{
  QStyleOptionViewItem opt = option;
  if ( index.isValid() ) {
    // Note: we check into account only custom roles; other roles are process
    //       correctly by the QItemDelegate class
    QVariant val = index.data( SUIT_TreeModel::BaseColorRole );
    if ( val.isValid() && val.value<QColor>().isValid() ) {
      QColor aBase = val.value<QColor>();
      aBase.setAlpha( 0 );
      opt.palette.setBrush( QPalette::Base, val.value<QColor>() );
    }
    val = index.data( SUIT_TreeModel::TextColorRole );
    if ( val.isValid() && val.value<QColor>().isValid() )
      opt.palette.setBrush( QPalette::Text, val.value<QColor>() );
    val = index.data( SUIT_TreeModel::HighlightRole );
    if ( val.isValid() && val.value<QColor>().isValid() )
      opt.palette.setBrush( QPalette::Highlight, val.value<QColor>() );
    val = index.data( SUIT_TreeModel::HighlightedTextRole );
    if ( val.isValid() && val.value<QColor>().isValid() )
      opt.palette.setBrush( QPalette::HighlightedText, val.value<QColor>() );      
  }
  QItemDelegate::paint( painter, opt, index );
}

QSize SUIT_ItemDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    QSize size = QItemDelegate::sizeHint ( option, index );
#if QT_VERSION >= 0x040500
    size.setHeight( size.height() + 1 );
#endif
    return size;
}
