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

// File   : SUIT_DataBrowser.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#include "SUIT_DataBrowser.h"
#include "SUIT_TreeModel.h"
#include <QtxTreeView.h>

#include <QShortcut>

/*!
  \class SUIT_DataBrowser
  \brief Object browser customization.
*/

/*!
  \brief Constructor.
  \param parent parent widget
*/
SUIT_DataBrowser::SUIT_DataBrowser( QWidget* parent )
: OB_Browser( parent )
{
  init( 0 );
}

/*!
  \brief Constructor.
  \param root root data object
  \param parent parent widget
*/
SUIT_DataBrowser::SUIT_DataBrowser( SUIT_DataObject* root, QWidget* parent )
: OB_Browser( parent )
{
  init( root );
}

/*!
  \brief Destructor.
*/
SUIT_DataBrowser::~SUIT_DataBrowser()
{
}

/*!
  \brief Get popup menu client type.
  \return popup client type
*/
QString SUIT_DataBrowser::popupClientType() const
{
  return "ObjectBrowser";
}

/*!
  \brief Get root object.
  \return root object
*/
SUIT_DataObject* SUIT_DataBrowser::root() const
{
  SUIT_ProxyModel* m = qobject_cast<SUIT_ProxyModel*>( model() );
  return m ? m->root() : 0;
}

/*!
  \brief Set root object.
  \param r new root object
*/
void SUIT_DataBrowser::setRoot( SUIT_DataObject* r )
{
  SUIT_ProxyModel* m = qobject_cast<SUIT_ProxyModel*>( model() );
  if ( m ) 
    m->setRoot( r );
}

/*!
  \brief Get 'auto-update tree' flag value.
  \return 'auto-update tree' flag value
  \sa setAutoUpdate(), updateTree()
*/
bool SUIT_DataBrowser::autoUpdate() const
{
  SUIT_ProxyModel* m = qobject_cast<SUIT_ProxyModel*>( model() );
  return m ? m->autoUpdate() : false;
}

/*!
  \brief Set 'auto-update tree' flag value.

  If this flag is set to \c true (by default), the object browser is updated
  automatically when data tree is changed.

  \param on 'auto-update tree' flag value
  \sa autoUpdate(), updateTree()
*/
void SUIT_DataBrowser::setAutoUpdate( const bool on )
{
  SUIT_ProxyModel* m = qobject_cast<SUIT_ProxyModel*>( model() );
  if ( m ) 
    m->setAutoUpdate( on );
}

/*!
  \brief Get 'updateModified' flag value.
  \return 'updateModified' flag value
*/
bool SUIT_DataBrowser::updateModified() const
{
  SUIT_ProxyModel* m = qobject_cast<SUIT_ProxyModel*>( model() );
  return m ? m->updateModified() : false;
}

/*!
  \brief Set 'updateModified' flag value.
  \param on 'updateModified' flag value
*/
void SUIT_DataBrowser::setUpdateModified( const bool on )
{
  SUIT_ProxyModel* m = qobject_cast<SUIT_ProxyModel*>( model() );
  if ( m ) 
    m->setUpdateModified( on );
}

/*!
  \brief Update object browser starting from the object \obj;
  open all branches automatically if \a autoOpen is \c true.
  \param obj starting object for updating
  \param autoOpen if \c true automatically open branches
*/
void SUIT_DataBrowser::updateTree( SUIT_DataObject* obj, const bool autoOpen )
{
  SUIT_ProxyModel* m = qobject_cast<SUIT_ProxyModel*>( model() );
  if ( m ) {
    m->updateTree( obj );
    openLevels();

    if (myAutoSizeFirstColumn)
      adjustFirstColumnWidth();
    if (myAutoSizeColumns)
      adjustColumnsWidth();
  }
  emit updated();
}

/*!
  \brief Get current key accelerator by id.
  \return current key accelerator
  \sa setShortcutKey(), requestUpdate(), requestRename()
*/
int SUIT_DataBrowser::shortcutKey(const int id) const
{
  return myShortcutMap.value(id)->key()[0];
}

/*!
  \brief Assign the key accelerator for the shortcut.
  
  \param id id of the shortcut
  \param key new key accelerator
  \sa shortcutKey(), requestUpdate(), requestRename()
*/
void SUIT_DataBrowser::setShortcutKey( const int id, const int key )
{ 
  ShortcutMap::iterator it = myShortcutMap.find( id );
  if( it != myShortcutMap.end() )
    (*it)->setKey(key);
}

/*!
  \brief Get list of selected data objects.
  \return list of the currently selected data objects
*/
DataObjectList SUIT_DataBrowser::getSelected() const
{
  DataObjectList lst;
  getSelected( lst );
  return lst;
}

/*!
  \brief Get list of selected data objects.
  \overload
  \param lst list to be filled with the currently selected data objects
*/
void SUIT_DataBrowser::getSelected( DataObjectList& lst ) const
{
  lst.clear();

  SUIT_ProxyModel* m = qobject_cast<SUIT_ProxyModel*>( model() );

  if ( m ) {
    QModelIndexList sel = selectedIndexes();
    QModelIndex idx;
  
    foreach( idx, sel ) {
      SUIT_DataObject* obj = m->object( idx );
      if ( obj )
        lst.append( obj );
    }
  }
}

/*!
  \brief Set selected object.
  \param obj data object to set selected
  \param append if \c true, the object is added to the current selection;
  otherwise the previous selection is first cleared
*/
void SUIT_DataBrowser::setSelected( const SUIT_DataObject* obj, const bool append )
{
  SUIT_ProxyModel* m = qobject_cast<SUIT_ProxyModel*>( model() );

  if ( m ) {
    QModelIndex index = m->index( obj );
    if ( index.isValid() )
      select( index, true, append );
  }
}

/*!
  \brief function to sort QModelIndexList with qSort
*/
bool modelIndexLessThan(const QModelIndex& lhs, const QModelIndex& rhs)
{
  QModelIndex lhs_parent=lhs.parent();
  QModelIndex rhs_parent=rhs.parent();
  if(lhs_parent < rhs_parent)return true;
  if(lhs_parent == rhs_parent) return lhs < rhs;
  return false;
}

/*!
  \brief Set list of selected data objects.
  \param lst list of the data object to set selected
  \param append if \c true, the objects are added to the current selection;
  otherwise the previous selection is first cleared
*/
void SUIT_DataBrowser::setSelected( const DataObjectList& lst, const bool append )
{
  SUIT_ProxyModel* m = qobject_cast<SUIT_ProxyModel*>( model() );

  if ( m ) {
    QModelIndexList indexes;
    SUIT_DataObject* obj;

    foreach( obj, lst ) {
      QModelIndex index = m->index( obj );
      if ( index.isValid() )
        indexes.append( index );
    }
    qSort(indexes.begin(), indexes.end(), modelIndexLessThan);

    select( indexes, true, append ); // if !indexes.isEmpty() ???
  }
}

/*!
  \brief Make the view item for specified data object is visible.
  \param obj data object
*/
void SUIT_DataBrowser::ensureVisible( SUIT_DataObject* obj )
{
  if ( !obj )
    return;

  DataObjectList lst;
  lst.append( obj );
  ensureVisible( lst );
}

/*!
  \brief Make the view items for specified data objects is visible.
  \param lst data object list
*/
void SUIT_DataBrowser::ensureVisible( const DataObjectList& lst )
{
  QtxTreeView* tv = treeView();
  SUIT_AbstractModel* treeModel = dynamic_cast<SUIT_AbstractModel*>( model() );
  if ( !tv || !treeModel )
    return;

  for ( DataObjectList::const_iterator it = lst.begin(); it != lst.end(); ++it )
  {
    QModelIndex idx = treeModel->index( *it );
    if ( idx.isValid() )
      tv->scrollTo( idx );
  }
}

/*!
  \brief Add custom actions to the popup menu.
  \param menu popup menu
*/
void SUIT_DataBrowser::contextMenuPopup( QMenu* menu )
{
  createPopupMenu( menu );
}

/*!
  \brief Set 'auto-size first column' flag value.

  If this flag is set to \c true (by default), the first column width is resized
  to its contents.

  \param on 'auto-size first column' flag value
  \sa setAutoSizeColumns()
*/
void SUIT_DataBrowser::setAutoSizeFirstColumn( const bool on )
{
  myAutoSizeFirstColumn = on;
}

/*!
  \brief Set 'auto-size columns' flag value.

  If this flag is set to \c true (by default is false), columns width except 
  the first column is resized to its contents.

  \param on 'auto-size columns' flag value
  \sa setAutoSizeFirstColumn()
*/
void SUIT_DataBrowser::setAutoSizeColumns( const bool on )
{
  myAutoSizeColumns = on;
}

/*!
  \brief Process context menu request event.
  \param e context menu event
*/
void SUIT_DataBrowser::contextMenuEvent( QContextMenuEvent* e )
{
  contextMenuRequest( e );
}

/*!
  \brief Set 'resize on expand item' flag value.

  If this flag is set to \c true (by default is false), after
  expanding an item columns will be resized to its contents.

  \param on 'resize on expand item' flag value
*/
void SUIT_DataBrowser::setResizeOnExpandItem( const bool on )
{
  myResizeOnExpandItem = on;
}

/*!
  \brief Initialize object browser.
  \param root root data object
*/
void SUIT_DataBrowser::init( SUIT_DataObject* root )
{
  SUIT_ProxyModel* m = new SUIT_ProxyModel( root, this );
  connect( m, SIGNAL( modelUpdated() ), this, SLOT( onModelUpdated() ) );
  
  setModel( m );
  setItemDelegate( qobject_cast<SUIT_ProxyModel*>( model() )->delegate() );
  connect( treeView(), SIGNAL( sortingEnabled( bool ) ), 
           model(),    SLOT( setSortingEnabled( bool ) ) );
  connect( treeView(), SIGNAL( clicked( const QModelIndex& ) ), 
           this,       SLOT( onClicked( const QModelIndex& ) ) );
  connect( treeView(), SIGNAL( doubleClicked( const QModelIndex& ) ), 
           this,       SLOT( onDblClicked( const QModelIndex& ) ) );
  connect( treeView(), SIGNAL( expanded( const QModelIndex& ) ), 
           this,       SLOT( onExpanded( const QModelIndex& ) ) );
  connect( this      , SIGNAL( requestRename() ),
	   this      , SLOT ( onStartEditing() ));

  myShortcutMap.insert(UpdateShortcut , new QShortcut( Qt::Key_F5, this, SIGNAL( requestUpdate() ), SIGNAL( requestUpdate() ) ) );
  myShortcutMap.insert(RenameShortcut , new QShortcut( Qt::Key_F2, this, SIGNAL( requestRename() ), SIGNAL( requestRename() ) ) );

  myAutoSizeFirstColumn = true;
  myAutoSizeColumns = false;
  myResizeOnExpandItem = false;
}

/*!
  \fn void SUIT_DataBrowser::requestUpdate();
  \brief The signal is emitted when the key accelerator
  assigned for the update operation is pressed by the user.

  By default, \c [F5] key is assigned for the update operation.
  The key accelerator can be changed with the setShortcutKey() method.

  \sa shortcutKey(), setShortcutKey()
*/

/*!
  \fn void SUIT_DataBrowser::clicked( SUIT_DataObject* o );
  \brief This signal is emitted when a mouse button is clicked.

  The data object the mouse was clicked on is specified by \a o.
  The signal is only emitted when the object is valid.

  \param o data object which is clicked
*/

/*!
  \fn void SUIT_DataBrowser::doubleClicked( SUIT_DataObject* o );
  \brief This signal is emitted when a mouse button is double-clicked.

  The data object the mouse was double-clicked on is specified by \a o.
  The signal is only emitted when the object is valid.

  \param o data object which is double-clicked
*/

/*!
  \brief Update internal modification time just after data model update
*/
void SUIT_DataBrowser::onModelUpdated()
{
  setModified();
}

/*!
  \brief Called when item is clicked in the tree view
  \internal
  
  Emits signal clicked( SUIT_DataObject* );
*/
void SUIT_DataBrowser::onClicked( const QModelIndex& index )
{
  SUIT_ProxyModel* m = qobject_cast<SUIT_ProxyModel*>( model() );

  if ( m ) {
    SUIT_DataObject* obj = m->object( index );
    if ( obj ) { 
      emit( clicked( obj ) );
      m->emitClicked(obj, index);
    }
  }
}

/*!
  \brief Called when item is double-clicked in the tree view
  \internal
  
  Emits signal doubleClicked( SUIT_DataObject* );
*/
void SUIT_DataBrowser::onDblClicked( const QModelIndex& index )
{
  SUIT_ProxyModel* m = qobject_cast<SUIT_ProxyModel*>( model() );

  if ( m ) {
    SUIT_DataObject* obj = m->object( index );
    if ( obj ) emit( doubleClicked( obj ) );
  }
}

/*!
  \brief Called when item specified by index is expanded.
  \internal
*/
void SUIT_DataBrowser::onExpanded( const QModelIndex& index )
{
  if (myResizeOnExpandItem) {
    adjustFirstColumnWidth();
    adjustColumnsWidth();
  }
}

/*!
  \brief Make editable selected item in place.
  \internal
*/
void SUIT_DataBrowser::onStartEditing() {
  DataObjectList sel = getSelected();
  SUIT_ProxyModel* m = qobject_cast<SUIT_ProxyModel*>( model() );
  if(treeView() && m && sel.count() == 1){
    treeView()->edit(m->index( sel.first(), SUIT_DataObject::NameId ));
  }
}

