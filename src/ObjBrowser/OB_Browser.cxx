#include "OB_Browser.h"

#include "OB_Filter.h"
#include "OB_ListItem.h"
#include "OB_ListView.h"

#include <qcursor.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qpainter.h>
#include <qwmatrix.h>
#include <qlistview.h>
#include <qpopupmenu.h>

/*!
    Class: OB_Browser::ToolTip
    Descr: Tool tip for OB_Browser.
*/

class OB_Browser::ToolTip : public QToolTip
{
public:
  ToolTip( OB_Browser* b, QWidget* p = 0 );
  virtual ~ToolTip();

  void        maybeTip( const QPoint& );

private:
  OB_Browser* myBrowser;
};

OB_Browser::ToolTip::ToolTip( OB_Browser* b, QWidget* p )
: QToolTip( p ),
myBrowser( b )
{
}

OB_Browser::ToolTip::~ToolTip()
{
}

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

/*!
    Class: OB_Browser
    Descr: Hierarchical tree object browser.
*/

OB_Browser::OB_Browser( QWidget* parent, SUIT_DataObject* root )
: QFrame( parent ),

myRoot( 0 ),
myTooltip( 0 ),
myAutoOpenLevel( 0 ),
myAutoUpdate( false ),
myAutoDelObjs( false ),
myRootDecorated( true )
{
  myView = new OB_ListView( QtxListView::HeaderAuto, this );
  myView->setAppropriate( myView->addColumn( "Data" ), false );
  myView->setSorting( -1 );
  myView->setRootIsDecorated( true );
  myView->setSelectionMode( QListView::Extended );
  myView->installEventFilter( this );
  myView->viewport()->installEventFilter( this );

  QVBoxLayout* main = new QVBoxLayout( this );
  main->addWidget( myView );

  myShowToolTips = true;
  myTooltip = new ToolTip( this, myView->viewport() );

  connect( myView, SIGNAL( dropped( QPtrList<QListViewItem>, QListViewItem*, int ) ),
           this, SLOT( onDropped( QPtrList<QListViewItem>, QListViewItem*, int ) ) );
  connect( myView, SIGNAL( selectionChanged() ), this, SIGNAL( selectionChanged() ) );
  connect( myView, SIGNAL( doubleClicked( QListViewItem* ) ),
           this, SLOT( onDoubleClicked( QListViewItem* ) ) );

  setRootObject( root );
}

OB_Browser::~OB_Browser()
{
  myItems.clear();
  delete myTooltip;
}

bool OB_Browser::rootIsDecorated() const
{
  return myRootDecorated;
}

void OB_Browser::setRootIsDecorated( const bool decor )
{
  if ( decor == rootIsDecorated() ) 
    return;

  myRootDecorated = decor;
  updateTree();
}

int OB_Browser::autoOpenLevel() const
{
  return myAutoOpenLevel;
}

void OB_Browser::setAutoOpenLevel( const int level )
{
  if ( myAutoOpenLevel == level )
    return;

  myAutoOpenLevel = level;

  autoOpenBranches();
}

bool OB_Browser::isShowToolTips()
{
  return myShowToolTips;
}

void OB_Browser::setShowToolTips( const bool theDisplay )
{
  myShowToolTips = theDisplay;
}

bool OB_Browser::isAutoUpdate() const
{
  return myAutoUpdate;
}

void OB_Browser::setAutoUpdate( const bool on )
{
  myAutoUpdate = on;
}

bool OB_Browser::isAutoDeleteObjects() const
{
  return myAutoDelObjs;
}

void OB_Browser::setAutoDeleteObjects( const bool on )
{
  myAutoDelObjs = on;
}

SUIT_DataObject* OB_Browser::getRootObject() const
{
  return myRoot;
}

void OB_Browser::setRootObject( SUIT_DataObject* theRoot )
{
  DataObjectKey curKey;
  DataObjectMap selObjs, openObjs;
  DataObjectKeyMap selKeys, openKeys;

  int selNum = numberOfSelected();

  SUIT_DataObject* curObj = 0;
  if ( theRoot )
    curObj = storeState( selObjs, openObjs, selKeys, openKeys, curKey );

  removeConnections( myRoot );
  if ( myRoot != theRoot && isAutoDeleteObjects() )
    delete myRoot;

  myRoot = theRoot;

  createConnections( myRoot );

  if ( myRoot )
    updateView( myRoot );
  else if ( listView() )
  {
    myItems.clear();
    listView()->clear();
  }

  restoreState( selObjs, openObjs, curObj, selKeys, openKeys, curKey );

  autoOpenBranches();

  if ( selNum != numberOfSelected() )
    emit selectionChanged();
}

int OB_Browser::numberOfSelected() const
{
  int count = 0;
  if ( listView() )
  {
    for ( QListViewItemIterator it( listView() ); it.current(); ++it )
      if ( it.current()->isSelected() ) 
        count++;
  }
  return count;
}

DataObjectList OB_Browser::getSelected() const
{
  DataObjectList lst;
  getSelected( lst );
  return lst;
}

void OB_Browser::getSelected( DataObjectList& theObjList ) const
{
  theObjList.clear();

  if ( !listView() )
    return;

  for ( QListViewItemIterator it( listView() ); it.current(); ++it )
  {
    if ( it.current()->isSelected() ) 
    {
      SUIT_DataObject* obj = dataObject( it.current() );
      if ( obj )
	theObjList.append( obj );
    }
  }
}

void OB_Browser::setSelected( const SUIT_DataObject* theObject, const bool append )
{
  DataObjectList lst;
  lst.append( theObject );
  setSelected( lst, append );
}

void OB_Browser::setSelected( const DataObjectList& theObjLst, const bool append )
{
  QListView* lv = listView();

  if ( !lv )
    return;

  bool changed = false;
  bool block = lv->signalsBlocked();
  lv->blockSignals( true );

  QMap<QListViewItem*, int> map;
  for ( DataObjectListIterator itr( theObjLst ); itr.current(); ++itr )
    map.insert( listViewItem( itr.current() ), 0 );

  for ( QListViewItemIterator it( lv ); it.current(); ++it )
  {
    QListViewItem* item = it.current();
    if ( map.contains( item ) && !lv->isSelected( item ) )
    {
      changed = true;
      lv->setSelected( item, true );
    }
    if ( !append && !map.contains( item ) && lv->isSelected( item ) )
    {
      changed = true;
      lv->setSelected( item, false );
    }
  }

  lv->blockSignals( block );

  if ( changed )
  {
    int count = 0;
    QListViewItem* sel = 0;
    QListViewItem* cur = lv->currentItem();
    for ( QListViewItemIterator iter( lv ); iter.current() && !sel; ++iter, count++ )
    {
      if ( iter.current()->isSelected() && cur == iter.current() )
        sel = iter.current();
    }

    for ( QListViewItemIterator itr( lv ); itr.current() && !sel; ++itr )
    {
      if ( itr.current()->isSelected() )
	      sel = itr.current();
    }

    if ( sel )
      lv->setCurrentItem( sel );

    if ( sel && count == 1 )
      lv->ensureItemVisible( sel );

    emit selectionChanged();
  }
}

bool OB_Browser::isOpen( SUIT_DataObject* theObject ) const
{
  bool res = false;
  if ( listView() )
    res = listView()->isOpen( listViewItem( theObject ) );
  return res;
}

void OB_Browser::setOpen( SUIT_DataObject* theObject, const bool theOpen )
{
  if ( listView() )
    listView()->setOpen( listViewItem( theObject ), theOpen );
}

SUIT_DataObject* OB_Browser::dataObjectAt( const QPoint& pos ) const
{
  SUIT_DataObject* obj = 0;

  QListView* lv = listView();
  if ( lv )
    obj = dataObject( lv->itemAt( pos ) );

  return obj;
}

OB_Filter* OB_Browser::filter() const
{
  return myView->filter();
}

void OB_Browser::setFilter( OB_Filter* f )
{
  myView->setFilter( f );
}

int OB_Browser::addColumn( const QString& label, const int id, const int width )
{
  return addColumn( QIconSet(), label, id, width );
}

int OB_Browser::addColumn( const QIconSet& icon, const QString& label, const int id, const int width )
{
  QListView* lv = listView();
  if ( !lv )
    return -1;

  int theId = id;
  if ( theId < 0 )
  {
    while ( myColumnIds.contains( theId ) )
      theId++;
  }

  if ( myColumnIds.contains( theId ) )
    return -1; // can not reuse id

  int sec = -1;
  if ( icon.isNull() )
    sec = lv->addColumn( label, width );
  else
    sec = lv->addColumn( icon, label, width );

  if ( sec == -1 )
    return -1;

  myColumnIds.insert( theId, sec );
  updateText();

  return theId;
}

void OB_Browser::removeColumn( const int id )
{
  QListView* lv = listView();
  if ( !lv || !myColumnIds.contains( id ) )
    return;

  int sec = myColumnIds[id];
  lv->removeColumn( sec );

  // update map of column indeces
  myColumnIds.remove( id );
  for ( QMap<int, int>::iterator it = myColumnIds.begin(); it != myColumnIds.end(); ++it )
  {
    if ( it.key() > id )
      it.data()--;
  }
  updateText();
}

void OB_Browser::setNameTitle( const QString& label )
{
  setNameTitle( QIconSet(), label );
}

void OB_Browser::setNameTitle( const QIconSet& icon, const QString& label )
{
  QListView* lv = listView();
  if ( !lv )
    return;

  if ( icon.isNull() )
    lv->setColumnText( 0, label );
  else
    lv->setColumnText( 0, icon, label );
}

void OB_Browser::setColumnTitle( const int id, const QString& label )
{
  setColumnTitle( id, QIconSet(), label );
}

void OB_Browser::setColumnTitle( const int id, const QIconSet& icon, const QString& label )
{
  QListView* lv = listView();
  if ( !lv || !myColumnIds.contains( id ) )
    return;

  if ( icon.isNull() )
    lv->setColumnText( myColumnIds[id], label );
  else
    lv->setColumnText( myColumnIds[id], icon, label );
}

QString OB_Browser::nameTitle() const
{
  return myView->columnText( 0 );
}

QString OB_Browser::columnTitle( const int id ) const
{
  QString txt;
  if ( myColumnIds.contains( id ) )
    txt = myView->columnText( myColumnIds[id] );
  return txt;
}

bool OB_Browser::isColumnVisible( const int id ) const
{
  return myColumnIds.contains( id ) && myView->isShown( myColumnIds[id] );
}

void OB_Browser::setColumnShown( const int id, const bool on )
{
  if ( !myColumnIds.contains( id ) )
    return;

  myView->setShown( myColumnIds[id], on );
}

void OB_Browser::setWidthMode( QListView::WidthMode mode )
{
  for ( int i = 0, n = myView->columns(); i < n; i++ )
    myView->setColumnWidthMode( i, mode );
}

QValueList<int> OB_Browser::columns() const
{
  QValueList<int> lst;
  for ( QMap<int, int>::ConstIterator it = myColumnIds.begin(); it != myColumnIds.end(); ++it )
    lst.append( it.key() );
  return lst;
}

bool OB_Browser::appropriateColumn( const int id ) const
{
  bool res = false;
  if ( myColumnIds.contains( id ) )
    res = myView->appropriate( myColumnIds[id] );
  return res;
}

void OB_Browser::setAppropriateColumn( const int id, const bool on )
{
  if ( !myColumnIds.contains( id ) )
    return;

  myView->setAppropriate( myColumnIds[id], on );
}

void OB_Browser::updateTree( SUIT_DataObject* obj )
{
  if ( !obj && !(obj = getRootObject()) )
    return;

  DataObjectKey curKey;
  DataObjectMap selObjs, openObjs;
  DataObjectKeyMap selKeys, openKeys;

  int selNum = numberOfSelected();

  SUIT_DataObject* curObj = storeState( selObjs, openObjs, selKeys, openKeys, curKey );

  updateView( obj );

  restoreState( selObjs, openObjs, curObj, selKeys, openKeys, curKey );

  autoOpenBranches();

  if ( selNum != numberOfSelected() )
    emit selectionChanged();
}

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

  autoOpenBranches();

  if ( selNum != numberOfSelected() )
    emit selectionChanged();
}

void OB_Browser::updateView( const SUIT_DataObject* theStartObj )
{
  QListView* lv = listView();
  if ( !lv )
    return;

  if ( !theStartObj || theStartObj->root() != getRootObject() )
    return;

  QListViewItem* after = 0;
  QListViewItem* parent = 0;
  QListViewItem* startItem = listViewItem( theStartObj );

  if ( theStartObj->parent() )
    parent = listViewItem( theStartObj->parent() );

  QListViewItem* prv = 0;
  QListViewItem* cur = parent ? parent->firstChild() : lv->firstChild();
  while ( !after && cur )
  {
    if ( cur == startItem )
      after = prv;

    prv = cur;
    cur = cur->nextSibling();
  }

  QPtrList<QListViewItem> delList;
  if ( !startItem && theStartObj == getRootObject() )
  {
    for ( QListViewItem* item = lv->firstChild(); item; item = item->nextSibling() )
      delList.append( item );
  }
  else
    delList.append( startItem );

  for ( QPtrListIterator<QListViewItem> it( delList ); it.current(); ++it )
  {
    removeReferences( it.current() );
    delete it.current();
  }

  // for myRoot object, if myShowRoot==false, then creating multiple top-level QListViewItem-s
  // (which will correspond to myRoot's children = Modules).  
  if ( rootIsDecorated() && theStartObj == myRoot )
  {
    DataObjectList lst;
    theStartObj->children( lst );
    DataObjectListIterator it ( lst );
    // iterating backward to preserve the order of elements in the tree
    for ( it.toLast(); it.current(); --it )
      createTree( it.current(), 0, 0 );
  }
  else
    createTree( theStartObj, parent, after ? after : parent );
}

QListViewItem* OB_Browser::createTree( const SUIT_DataObject* obj,
                                       QListViewItem* parent, QListViewItem* after )
{
  if ( !obj )
    return 0;
  
  QListViewItem* item = createItem( obj, parent, after );

  DataObjectList lst;
  obj->children( lst );
  for ( DataObjectListIterator it ( lst ); it.current(); ++it )
    createTree( it.current(), item );

  if ( item )
    item->setOpen( obj->isOpen() );

  return item;
}

QListViewItem* OB_Browser::createItem( const SUIT_DataObject* o,
                                       QListViewItem* parent, QListViewItem* after )
{
  QListView* lv = listView();

  if ( !lv || !o )
    return 0;

  QListViewItem* item = 0;
  SUIT_DataObject* obj = (SUIT_DataObject*)o;

  int type = -1;

  switch ( obj->checkType() )
  {
  case SUIT_DataObject::CheckBox:
    type = QCheckListItem::CheckBox;
    break;
  case SUIT_DataObject::RadioButton:
    type = QCheckListItem::RadioButton;
    break;
  }

  if ( parent )
  {
    if ( parent->childCount() && !after )
    {
      after = parent->firstChild();
      while ( after->nextSibling() )
        after = after->nextSibling();
    }

    if ( after )
    {
      if ( type == -1 )
        item = new OB_ListItem( obj, parent, after );
      else
        item = new OB_CheckListItem( obj, parent, after, (QCheckListItem::Type)type );
    }
    else
    {
      if ( type == -1 )
        item = new OB_ListItem( obj, parent );
      else
        item = new OB_CheckListItem( obj, parent,  (QCheckListItem::Type)type );
    }
  }
  else // ROOT item
  {
    if ( type == -1 )
      item = new OB_ListItem( obj, lv );
    else
      item = new OB_CheckListItem( obj, lv,  (QCheckListItem::Type)type );
  }

  myItems.insert( obj, item );
  obj->connect( this, SLOT( onDestroyed( SUIT_DataObject* ) ) );

  updateText( item );

  return item;
}

void OB_Browser::adjustWidth()
{
  if ( !listView() )
    return;

  listView()->setColumnWidth( 0, 0 );
  if ( listView()->firstChild() )
    adjustWidth( listView()->firstChild() );
}

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

SUIT_DataObject* OB_Browser::dataObject( const QListViewItem* item ) const
{
  SUIT_DataObject* obj = 0;

  if ( item && item->rtti() == OB_ListItem::RTTI() )
    obj = ((OB_ListItem*)item)->dataObject();
  else if ( item && item->rtti() == OB_CheckListItem::RTTI() )
    obj = ((OB_CheckListItem*)item)->dataObject();

  return obj;
}

QListViewItem* OB_Browser::listViewItem( const SUIT_DataObject* obj ) const
{
  QListViewItem* item = 0;

  if ( myItems.contains( (SUIT_DataObject*)obj ) )
    item = myItems[(SUIT_DataObject*)obj];

  return item;
}

QListView* OB_Browser::listView() const
{
  return myView;
}

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
      if ( openObjs[obj] )
        lv->setOpen( item, true );
    }
    else if ( !key.isNull() && openKeys.contains( key ) )
      lv->setOpen( item, true );

    if ( !curItem && ( curObj == obj || ( !curKey.isNull() && curKey == key )) )
      curItem = item;
  }

  if ( curItem )
    lv->setCurrentItem( curItem );

  lv->blockSignals( block );
}

OB_Browser::DataObjectKey OB_Browser::objectKey( QListViewItem* i ) const
{
  return objectKey( dataObject( i ) );
}

OB_Browser::DataObjectKey OB_Browser::objectKey( SUIT_DataObject* obj ) const
{
  if ( !obj )
    return 0;

  return DataObjectKey( obj->key() );
}

void OB_Browser::keyPressEvent( QKeyEvent* e )
{
  if ( e->key() == Qt::Key_F5 )
    updateTree();

  QFrame::keyPressEvent( e );
}

void OB_Browser::onExpand()
{
  DataObjectList selected;
  getSelected( selected );
  for ( DataObjectListIterator itr( selected ); itr.current(); ++itr )
    expand( listViewItem( itr.current() ) );
}

void OB_Browser::onColumnVisible( int id )
{
  setColumnShown( id, !isColumnVisible( id ) );
}

void OB_Browser::onDestroyed( SUIT_DataObject* obj )
{
  removeObject( obj );
}

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

void OB_Browser::updateText( QListViewItem* item )
{
  SUIT_DataObject* obj = dataObject( item );
  if ( !obj )
    return;

  for( QMap<int, int>::iterator it = myColumnIds.begin(); it != myColumnIds.end(); ++it )
    item->setText( it.data(), obj->text( it.key() ) );
}

bool OB_Browser::eventFilter( QObject* o, QEvent* e )
{
  if ( o == myView && e->type() == QEvent::ContextMenu )
  {
    QContextMenuEvent* ce = (QContextMenuEvent*)e;
    if ( ce->reason() != QContextMenuEvent::Mouse )
      contextMenuRequest( ce );
    return true;
  }
  if ( o == myView->viewport() && e->type() == QEvent::MouseButtonRelease )
  {
    QMouseEvent* me = (QMouseEvent*)e;
    if ( me->button() == RightButton )
    {
      QContextMenuEvent ce( QContextMenuEvent::Mouse, me->pos(), me->globalPos(), me->state() );
      contextMenuRequest( &ce );
      return true;
    }
  }

  return QFrame::eventFilter( o, e );
}

void OB_Browser::contextMenuPopup( QPopupMenu* menu )
{
/*  QValueList<int> cols;
  for ( QMap<int, int>::ConstIterator it = myColumnIds.begin(); it != myColumnIds.end(); ++it )
  {
    if ( appropriateColumn( it.key() ) )
      cols.append( it.key() );
  }

  uint num = menu->count();
  menu->setCheckable( true );
  for ( QValueList<int>::const_iterator iter = cols.begin(); iter != cols.end(); ++iter )
  {
    QString name = columnTitle( *iter );
    if ( name.isEmpty() )
      continue;

    int id = menu->insertItem( name, this, SLOT( onColumnVisible( int ) ) );
    menu->setItemChecked( id, isColumnVisible( *iter ) );
    menu->setItemParameter( id, *iter );
  }
  if ( menu->count() != num )
    menu->insertSeparator();*/

  DataObjectList selected;
  getSelected( selected );

  bool closed = false;
  for ( DataObjectListIterator itr( selected ); itr.current() && !closed; ++itr )
    closed = hasClosed( listViewItem( itr.current() ) );

  if ( closed )
  {
    menu->insertItem( tr( "MEN_EXPAND_ALL" ), this, SLOT( onExpand() ) );
    menu->insertSeparator();
  }
}

void OB_Browser::expand( QListViewItem* item )
{
  if ( !item )
    return;

  item->setOpen( true );
  for ( QListViewItem* child = item->firstChild(); child; child = child->nextSibling() )
    expand( child );
}

bool OB_Browser::hasClosed( QListViewItem* item ) const
{
  if ( !item )
    return false;

  if ( item->childCount() && !item->isOpen() )
    return true;

  bool has = false;
  for ( QListViewItem* child = item->firstChild(); child && !has; child = child->nextSibling() )
    has = hasClosed( child );

  return has;
}

void OB_Browser::removeObject( SUIT_DataObject* obj, const bool autoUpd )
{
  if ( !obj )
    return;

  // Removing list view items from <myItems> recursively for all children.
  // Otherwise, "delete item" line will destroy all item's children,
  // and <myItems> will contain invalid pointers (see ~QListViewItem() description in Qt docs)
  DataObjectList childList;
  obj->children( childList );
  for ( DataObjectListIterator it( childList ); it.current(); ++it )
    removeObject( it.current(), false );

  QListViewItem* item = listViewItem( obj );

  obj->disconnect( this, SLOT( onDestroyed( SUIT_DataObject* ) ) );
  myItems.remove( obj );

  if ( obj == myRoot )
  {
    // remove all child list view items
    setRootObject( 0 );
    return;
  }

  if ( !autoUpd )
    return;

  if ( isAutoUpdate() )
  {
    SUIT_DataObject* pObj = item && item->parent() ? dataObject( item->parent() ) : 0;
    updateTree( pObj );
  }
  else
    delete item;
}

void OB_Browser::autoOpenBranches()
{
  int level = autoOpenLevel();
  QListView* lv = listView();
  if ( !lv || level < 1 )
    return;

  QListViewItem* item = lv->firstChild();
  while ( item )
  {
    openBranch( item, level );
    item = item->nextSibling();
  }
}

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

void OB_Browser::onDoubleClicked( QListViewItem* item )
{
  if ( item )
    emit doubleClicked( dataObject( item ) );
}
