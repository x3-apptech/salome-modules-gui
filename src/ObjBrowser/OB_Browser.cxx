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
#include "OB_Browser.h"

#include "OB_Filter.h"
#include "OB_ListItem.h"
#include "OB_ListView.h"
#include "OB_FindDlg.h"

#include <SUIT_DataObjectIterator.h>
#include <SUIT_TreeSync.h>

#include <qcursor.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qpainter.h>
#include <qwmatrix.h>
#include <qlistview.h>
#include <qpopupmenu.h>
#include <qdatetime.h>

#include <time.h>

/*!
  \class  OB_Browser::ToolTip
  Tool tip for OB_Browser.
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

/*!
  Constructor
*/
OB_Browser::ToolTip::ToolTip( OB_Browser* b, QWidget* p )
: QToolTip( p ),
myBrowser( b )
{
}

/*!
  Destructor
*/
OB_Browser::ToolTip::~ToolTip()
{
}

/*!
  It is called when there is a possibility that a tool tip
  should be shown and must decide whether there is a tool tip for the point
  in the widget that this QToolTip object relates to.
  \param pos - point co-ordinates
*/
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


typedef SUIT_DataObject*   ObjPtr;
typedef OB_ListItem*       ItemPtr;

/*!
   \class  OB_BrowserSync
   Auxiliary class for synchronizing tree of SUIT_DataObjects and list view items
*/
class OB_BrowserSync
{
public:
  OB_BrowserSync( OB_Browser* );
  bool     isEqual( const ObjPtr&, const ItemPtr& ) const;
  ObjPtr   nullSrc() const;
  ItemPtr  nullTrg() const;
  ItemPtr  createItem( const ObjPtr&, const ItemPtr&, const ItemPtr&, const bool ) const;
  void     updateItem( const ObjPtr& , const ItemPtr& ) const;
  void     deleteItemWithChildren( const ItemPtr& ) const;
  void     children( const ObjPtr&, QValueList<ObjPtr>& ) const;
  void     children( const ItemPtr&, QValueList<ItemPtr>& ) const;
  ItemPtr  parent( const ItemPtr& ) const;
private:
  bool     needUpdate( const ItemPtr& ) const;
  OB_Browser*   myBrowser;
};


/*!
  Constructor
*/
OB_BrowserSync::OB_BrowserSync( OB_Browser* ob )
: myBrowser( ob )
{
}

/*!
  \return true if item must be updated
  \param item - item to be checked
*/
bool OB_BrowserSync::needUpdate( const ItemPtr& item ) const
{
  bool update = false;
  if ( item ) {
    SUIT_DataObject* obj = item->dataObject();
    if ( obj ) {
      // 1. check text
      update = ( item->text( 0 ) != obj->name() ) || myBrowser->needToUpdateTexts( item );

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
      }
    }
  }
  return update;
}

/*!
  Updates item
  \param p - item
*/
void OB_BrowserSync::updateItem( const ObjPtr& o, const ItemPtr& p ) const
{
  if ( p && needUpdate( p ) ) { 
    //    printf( "--- needUpdate for %s = true ---\n", p->text( 0 ).latin1() );
    myBrowser->updateText( p );
    p->update();
  }
  if( o && myBrowser->getUpdater() )
    {
      myBrowser->getUpdater()->update( o, p );
    }
}

/*!
  Creates item by SUIT object
  \param src - corresponding SUIT object
  \param parent - parent for item
  \param after - previous sibling for item
  \param prepend - item must be added to start of children list
*/
ItemPtr OB_BrowserSync::createItem( const ObjPtr& src,
				    const ItemPtr& parent, const ItemPtr& after,
				    const bool prepend ) const
{
  ItemPtr i = myBrowser ? dynamic_cast<ItemPtr>( myBrowser->createItem( src, parent, after, prepend ) ) : 0;
  if( i )
    i->setOpen( src->isOpen() );
  return i;
}

/*!
  Deletes object with all children
  \param i - item
*/
void OB_BrowserSync::deleteItemWithChildren( const ItemPtr& i ) const
{
  if( myBrowser && myBrowser->myItems.contains( i->dataObject() ) )
  {
    myBrowser->removeReferences( i );
    delete i;
  }
}

/*!
  \return true if objects correspond each other at all
  \param p - suit object
  \param q - object browser item
*/
bool OB_BrowserSync::isEqual( const ObjPtr& p, const ItemPtr& q ) const
{
  bool isRoot = p==myBrowser->getRootObject() && !q,
       isEq = p && q && q->dataObject()==p;
  return isRoot || ( !p && !q ) || isEq;
}

/*!
  \return null suit object
*/
ObjPtr OB_BrowserSync::nullSrc() const
{
  return 0;
}

/*!
  \return null item
*/
ItemPtr OB_BrowserSync::nullTrg() const
{
  return 0;
}

/*!
  Fills list with children of SUIT object
  \param p - SUIT object
  \param ch - list to be filled
*/
void OB_BrowserSync::children( const ObjPtr& p, QValueList<ObjPtr>& ch ) const
{
  DataObjectList l;
  if( p )
  {
    p->children( l );
    ch.clear();
    for( SUIT_DataObject* o = l.first(); o; o = l.next() )
      ch.append( o );
  }
}

/*!
  Fills list with children of item
  \param p - item
  \param ch - list to be filled
*/
void OB_BrowserSync::children( const ItemPtr& p, QValueList<ItemPtr>& ch ) const
{
  for( QListViewItem* item = p ? p->firstChild() : myBrowser->listView()->firstChild(); item; item = item->nextSibling() )
  {
    ItemPtr p = dynamic_cast<ItemPtr>( item );
    if( p )
      ch.append( p );
  }
}

/*!
  \return parent of item
  \param p - item
*/
ItemPtr OB_BrowserSync::parent( const ItemPtr& p ) const
{
  return p ? dynamic_cast<ItemPtr>( p->parent() ) : 0;
}


/*!
  Constructor
*/
OB_Browser::OB_Browser( QWidget* parent, SUIT_DataObject* root )
: QFrame( parent ),

myRoot( 0 ),
myTooltip( 0 ),
myUpdater( 0 ),
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

  myFindDlg = new OB_FindDlg( this );
  myFindDlg->hide();

  QVBoxLayout* main = new QVBoxLayout( this );
  main->addWidget( myView, 1 );
  main->addWidget( myFindDlg, 0 );

  myShowToolTips = true;
  myTooltip = new ToolTip( this, myView->viewport() );

  connect( myView, SIGNAL( dropped( QPtrList<QListViewItem>, QListViewItem*, int ) ),
           this, SLOT( onDropped( QPtrList<QListViewItem>, QListViewItem*, int ) ) );
  connect( myView, SIGNAL( selectionChanged() ), this, SIGNAL( selectionChanged() ) );
  connect( myView, SIGNAL( doubleClicked( QListViewItem* ) ),
           this, SLOT( onDoubleClicked( QListViewItem* ) ) );

  setRootObject( root );

  setModified();
}

/*!
  Destructor
*/
OB_Browser::~OB_Browser()
{
  myItems.clear();
  delete myTooltip;
  setUpdater( 0 );
}

/*!
  \return true if root is decorated by +
*/
bool OB_Browser::rootIsDecorated() const
{
  return myRootDecorated;
}

/*!
  Sets state "root is recorated"
  \param decor - new value of state
*/
void OB_Browser::setRootIsDecorated( const bool decor )
{
  if ( decor == rootIsDecorated() ) 
    return;

  myRootDecorated = decor;
  updateTree( 0, false );
}

/*!
  \return number of levels to be auto opened on update tree
*/
int OB_Browser::autoOpenLevel() const
{
  return myAutoOpenLevel;
}

/*!
  Changes number of levels to be auto opened on update tree
  \param level - new number of levels
*/
void OB_Browser::setAutoOpenLevel( const int level )
{
  if ( myAutoOpenLevel == level )
    return;

  myAutoOpenLevel = level;
}

/*!
  Opens branches from 1 to \alevels. If parameter value negative then autoOpenLevel() value will be used.
  \sa autoOpenLevel()
*/
void OB_Browser::openLevels( const int levels )
{
  int level = levels < 0 ? autoOpenLevel() : levels;
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

/*!
  \return state "are tooltips shown"
*/
bool OB_Browser::isShowToolTips()
{
  return myShowToolTips;
}

/*!
  Sets new value of state "are tooltips shown"
  \param theDisplay - new value
*/
void OB_Browser::setShowToolTips( const bool theDisplay )
{
  myShowToolTips = theDisplay;
}

/*!
  \return true if object browser automatically updates tree after SUIT object removing
*/
bool OB_Browser::isAutoUpdate() const
{
  return myAutoUpdate;
}

/*!
  Sets new value of "auto update": whether object browser automatically updates tree after SUIT object removing
*/
void OB_Browser::setAutoUpdate( const bool on )
{
  myAutoUpdate = on;
}

/*!
  \return true if object browser must delete old tree on setRootObject(), replaceTree()
  \sa setRootObject(), replaceTree()
*/
bool OB_Browser::isAutoDeleteObjects() const
{
  return myAutoDelObjs;
}

/*!
  Sets whether object browser must delete old tree on setRootObject(), replaceTree()
  \sa setRootObject(), replaceTree()
*/
void OB_Browser::setAutoDeleteObjects( const bool on )
{
  myAutoDelObjs = on;
}

/*!
  \return updater of browser
*/
OB_Updater* OB_Browser::getUpdater() const
{
  return myUpdater;
}

/*!
  \sets new updater of browser
*/
void OB_Browser::setUpdater( OB_Updater* theUpdate )
{
  if( myUpdater )
    delete myUpdater;
  myUpdater = theUpdate;
}

/*!
  \return root SUIT object of browser
*/
SUIT_DataObject* OB_Browser::getRootObject() const
{
  return myRoot;
}

/*!
  Sets new root SUIT object of browser
  \param theRoot - new root object
*/
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

  setModified();

  if ( selNum != numberOfSelected() )
    emit selectionChanged();
}

/*!
  \return number of selected items
*/
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

/*!
  \return list of selected objects
*/
DataObjectList OB_Browser::getSelected() const
{
  DataObjectList lst;
  getSelected( lst );
  return lst;
}

/*!
  Fills list with selected objects
*/
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

/*!
  Sets selected object
  \param theObject - new selected object
  \param append - if it is true, then other selected objects are left as selected,
  otherwise only 'theObject' will be selected
*/
void OB_Browser::setSelected( const SUIT_DataObject* theObject, const bool append )
{
  DataObjectList lst;
  lst.append( theObject );
  setSelected( lst, append );
}

/*!
  Sets selected objects
  \param theObjLst - new selected objects
  \param append - if it is true, then other selected objects are left as selected,
  otherwise only 'theObjLst' will be selected
*/
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

/*!
  \return true if item corresponding to object is opened
  \param theObject - object to be checked
*/
bool OB_Browser::isOpen( SUIT_DataObject* theObject ) const
{
  bool res = false;
  if ( listView() )
    res = listView()->isOpen( listViewItem( theObject ) );
  return res;
}

/*!
  Sets opened state of item
  \param theObject - object corresponding to item
  \param theOpen - new opened state
*/
void OB_Browser::setOpen( SUIT_DataObject* theObject, const bool theOpen )
{
  if ( listView() )
    listView()->setOpen( listViewItem( theObject ), theOpen );
}

/*!
  \return SUIT object correspondint to item at position 'pos'
  \param pos - position
*/
SUIT_DataObject* OB_Browser::dataObjectAt( const QPoint& pos ) const
{
  SUIT_DataObject* obj = 0;

  QListView* lv = listView();
  if ( lv )
    obj = dataObject( lv->itemAt( pos ) );

  return obj;
}

/*!
  \return filter of list view
*/
OB_Filter* OB_Browser::filter() const
{
  return myView->filter();
}

/*!
  Changes filter of list view
  \param f - new filter
*/
void OB_Browser::setFilter( OB_Filter* f )
{
  myView->setFilter( f );
}

/*!
  Adds new column to list view
  \param label - title of column
  \param id - id of column
  \param width - width of column
*/
int OB_Browser::addColumn( const QString& label, const int id, const int width )
{
  return addColumn( QIconSet(), label, id, width );
}

/*!
  Adds new column to list view
  \param icon - icon of column
  \param label - title of column
  \param id - id of column
  \param width - width of column
*/
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

/*!
  Removes column
  \param id - id of column
*/
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

/*!
  Sets title of first column (name column)
  \param label - new title
*/
void OB_Browser::setNameTitle( const QString& label )
{
  setNameTitle( QIconSet(), label );
}

/*!
  Sets title and icon of first column (name column)
  \param icon - new icon
  \param label - new title
*/
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

/*!
  Sets title of column
  \param id - column id
  \param label - new column title
*/
void OB_Browser::setColumnTitle( const int id, const QString& label )
{
  setColumnTitle( id, QIconSet(), label );
}

/*!
  Sets title and icon of column
  \param id - column id
  \param icon - new column icon
  \param label - new column title
*/
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

/*!
  \return title of first column (name column)
*/
QString OB_Browser::nameTitle() const
{
  return myView->columnText( 0 );
}

/*!
  \return title of first column (name column)
  \param id - column id
*/
QString OB_Browser::columnTitle( const int id ) const
{
  QString txt;
  if ( myColumnIds.contains( id ) )
    txt = myView->columnText( myColumnIds[id] );
  return txt;
}

/*!
  \return true if column is visible
  \param id - column id
*/
bool OB_Browser::isColumnVisible( const int id ) const
{
  return myColumnIds.contains( id ) && myView->isShown( myColumnIds[id] );
}

/*!
  Sets visibility of column
  \param id - column id
  \param on - new visibility state
*/
void OB_Browser::setColumnShown( const int id, const bool on )
{
  if ( !myColumnIds.contains( id ) )
    return;

  myView->setShown( myColumnIds[id], on );
  if( !on )
    myView->setColumnWidthMode( myColumnIds[id], QListView::Manual );
}

/*!
  Sets global width mode
  \param mode - new width mode
*/
void OB_Browser::setWidthMode( QListView::WidthMode mode )
{
  for ( int i = 0, n = myView->columns(); i < n; i++ )
    if( mode!=QListView::Maximum || myView->columnWidth( i )>0 )
      myView->setColumnWidthMode( i, mode );
}

/*!
  \return list of columns ids
*/
QValueList<int> OB_Browser::columns() const
{
  QValueList<int> lst;
  for ( QMap<int, int>::ConstIterator it = myColumnIds.begin(); it != myColumnIds.end(); ++it )
    lst.append( it.key() );
  return lst;
}

/*!
  \return true if it is possible to show/hide column by popup
  \param id - column id
*/
bool OB_Browser::appropriateColumn( const int id ) const
{
  bool res = false;
  if ( myColumnIds.contains( id ) )
    res = myView->appropriate( myColumnIds[id] );
  return res;
}

/*!
  Sets "appropriate state": is it possible to show/hide column by popup
  \param id - column id
  \param on - new state
*/
void OB_Browser::setAppropriateColumn( const int id, const bool on )
{
  if ( !myColumnIds.contains( id ) )
    return;

  myView->setAppropriate( myColumnIds[id], on );
}

/*!
  Updates tree
  \param obj - start object
  \param autoOpen - to open automatically branches of autoOpenLevel()
  \sa autoOpenLevel()
*/
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

/*!
  Replaces part of tree starting at object 'src' by tree starting at object 'trg'
*/
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

/*!
  Updates view
  \param startObj - start object
*/
void OB_Browser::updateView( SUIT_DataObject* startObj )
{
  QListView* lv = listView();
  if ( !lv )
    return;

  if ( !startObj || startObj->root() != getRootObject() )
    return;

  //qDebug( "updateView:" );
  //startObj->dump();

  if ( startObj == myRoot )
  {
    OB_BrowserSync sync( this );
    synchronize<ObjPtr,ItemPtr,OB_BrowserSync>( myRoot, 0, sync );
  }
  else
  {
    OB_BrowserSync sync( this );
    OB_ListItem* startItem = dynamic_cast<OB_ListItem*>( listViewItem( startObj ) );
    synchronize<ObjPtr,ItemPtr,OB_BrowserSync>( startObj, startItem, sync );
  }
}

/*!
  Creates new list item
  \return new item
  \param o - corresponding SUIT object
  \param parent - parent item
  \param after - item after that new item must be added
  \param prepend - new item must be added as first
*/
QListViewItem* OB_Browser::createItem( const SUIT_DataObject* o, QListViewItem* parent,
				       QListViewItem* after, const bool prepend )
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
    if ( after ) 
    {
      if ( type == -1 )
	item = new OB_ListItem( obj, parent, after );
      else
	item = new OB_CheckListItem( obj, parent, after, (QCheckListItem::Type)type );
    }
    else if ( prepend )
    {
      if ( type == -1 )
	item = new OB_ListItem( obj, parent );
      else
	item = new OB_CheckListItem( obj, parent,  (QCheckListItem::Type)type );
    }
    else // append
    {
      after = parent->firstChild();
      while ( after && after->nextSibling() )
	after = after->nextSibling();
      if ( type == -1 )
	item = new OB_ListItem( obj, parent, after );
      else
	item = new OB_CheckListItem( obj, parent, after, (QCheckListItem::Type)type );
    }
  }
  else
  {
    if ( after ) 
    {
      if ( type == -1 )
	item = new OB_ListItem( obj, lv, after );
      else
	item = new OB_CheckListItem( obj, lv, after, (QCheckListItem::Type)type );
    }
    else if ( prepend )
    {
      if ( type == -1 )
	item = new OB_ListItem( obj, lv );
      else
	item = new OB_CheckListItem( obj, lv,  (QCheckListItem::Type)type );
    }
    else // append
    {
      after = lv->firstChild();
      while ( after && after->nextSibling() )
	after = after->nextSibling();
      if ( type == -1 )
	item = new OB_ListItem( obj, lv, after );
      else
	item = new OB_CheckListItem( obj, lv, after, (QCheckListItem::Type)type );
    }
  }

  myItems.insert( obj, item );
  obj->connect( this, SLOT( onDestroyed( SUIT_DataObject* ) ) );
  updateText( item );

  return item;
}

/*!
  Adjusts width by root item
*/
void OB_Browser::adjustWidth()
{
  if ( !listView() )
    return;

  listView()->setColumnWidth( 0, 0 );
  if ( listView()->firstChild() )
    adjustWidth( listView()->firstChild() );
}

/*!
  Adjusts width by item
  \param item
*/
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

/*!
  \return SUIT object corresponding to item
  \param item
*/
SUIT_DataObject* OB_Browser::dataObject( const QListViewItem* item ) const
{
  SUIT_DataObject* obj = 0;

  if ( item && item->rtti() == OB_ListItem::RTTI() )
    obj = ((OB_ListItem*)item)->dataObject();
  else if ( item && item->rtti() == OB_CheckListItem::RTTI() )
    obj = ((OB_CheckListItem*)item)->dataObject();

  return obj;
}

/*!
  \return item corresponding to SUIT object
  \param obj - SUIT object
*/
QListViewItem* OB_Browser::listViewItem( const SUIT_DataObject* obj ) const
{
  QListViewItem* item = 0;

  if ( myItems.contains( (SUIT_DataObject*)obj ) )
    item = myItems[(SUIT_DataObject*)obj];

  return item;
}

/*!
  \return list view of object browser
*/
QListView* OB_Browser::listView() const
{
  return myView;
}

/*!
  \remove all items referencing current (through data objects)
*/
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

/*!
  Connects all children to SLOT onDestroyed
*/
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

/*!
  Disconnects all children from SLOT onDestroyed
*/
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

/*!
  Stores states (opened, selected) of current tree items
  \return current item
  \param selObjs, selKeys - maps of selected objects
  \param openObjs, openKeys - maps of opened objects
  \param curKey - map of current objects
*/
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

/*!
  Restores states (opened, selected) of current tree items
  \param selObjs, selKeys - maps of selected objects
  \param openObjs, openKeys - maps of opened objects
  \param curKey - map of current objects
*/
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

/*!
  Creates object key by tree item
*/
OB_Browser::DataObjectKey OB_Browser::objectKey( QListViewItem* i ) const
{
  return objectKey( dataObject( i ) );
}

/*!
  Creates object key by SUIT object
*/
OB_Browser::DataObjectKey OB_Browser::objectKey( SUIT_DataObject* obj ) const
{
  if ( !obj )
    return 0;

  return DataObjectKey( obj->key() );
}

/*!
  Custom key press event handler, updates tree by F5
*/
void OB_Browser::keyPressEvent( QKeyEvent* e )
{
  if ( e->key() == Qt::Key_F5 )
    updateTree( 0, false );

  QFrame::keyPressEvent( e );
}

/*!
  SLOT: called if action "Expand all" is activated
*/
void OB_Browser::onExpand()
{
  DataObjectList selected;
  getSelected( selected );
  for ( DataObjectListIterator itr( selected ); itr.current(); ++itr )
    expand( listViewItem( itr.current() ) );
}

/*!
  SLOT: called if action "Show/hide column" is activated by popup
*/
void OB_Browser::onColumnVisible( int id )
{
  setColumnShown( id, !isColumnVisible( id ) );
}

/*!
  SLOT: called if SUIT object is destroyed
*/
void OB_Browser::onDestroyed( SUIT_DataObject* obj )
{
  removeObject( obj );
}

/*!
  SLOT: called on finish of drag-n-drop operation
  \param items - dragged items
  \param item - destination (item on that they were dropped)
  \param action - QDropEvent::Action
*/
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

/*!
  Updates texts of items
*/
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

/*!
  \return true if item must be updated
  \param item - item to be checked
*/
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

/*!
  Updates texts of item
  \param item - item to be updated
*/
void OB_Browser::updateText( QListViewItem* item )
{
  SUIT_DataObject* obj = dataObject( item );
  if ( !obj )
    return;

  for( QMap<int, int>::iterator it = myColumnIds.begin(); it != myColumnIds.end(); ++it )
    item->setText( it.data(), obj->text( it.key() ) );
}

/*!
  Custom event filter
*/
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

/*!
  Adds custom actions to popup
  \param menu - popup menu
*/
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

/*!
  Expands item with all it's children
*/
void OB_Browser::expand( QListViewItem* item )
{
  if ( !item )
    return;

  item->setOpen( true );
  for ( QListViewItem* child = item->firstChild(); child; child = child->nextSibling() )
    expand( child );
}

/*!
  \return true if item or one of it's children isn't opened
*/
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

/*!
  Removes SUIT object
  \param obj - SUIT object to be removed
  \param autoUpd - auto tree updating
*/
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

/*!
  Opens branches from 1 to autoOpenLevel()
  \sa autoOpenLevel()
*/
void OB_Browser::autoOpenBranches()
{
  openLevels();
}

/*!
  Opens branch
  \param item
  \param level
*/
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

/*!
  SLOT: called on double click on item, emits signal
*/
void OB_Browser::onDoubleClicked( QListViewItem* item )
{
  if ( item )
    emit doubleClicked( dataObject( item ) );
}

/*!
  Stores time of last modification
*/
void OB_Browser::setModified()
{
  myModifiedTime = clock();
}

OB_ObjSearch* OB_Browser::getSearch() const
{
  return myFindDlg->getSearch();
}

void OB_Browser::setSearch( OB_ObjSearch* s )
{
  myFindDlg->setSearch( s );
}

void OB_Browser::enableSearch( const bool on )
{
  myFindDlg->setShown( on );
}
