#ifndef OB_BROWSER_H
#define OB_BROWSER_H

#include "OB.h"

#include <qframe.h>

#include <qmap.h>
#include <qlistview.h>

#include <SUIT_DataObject.h>
#include <SUIT_DataObjectKey.h>
#include <SUIT_PopupClient.h>

#ifdef WNT
#pragma warning( disable:4251 )
#endif

class QToolTip;
class OB_Filter;
class OB_ListView;

class OB_EXPORT OB_Browser : public QFrame, public SUIT_PopupClient
{
	Q_OBJECT

  class ToolTip;

public:
  OB_Browser( QWidget* = 0, SUIT_DataObject* theRoot = 0 );
  virtual ~OB_Browser();

  virtual QString   popupClientType() const { return QString( "ObjectBrowser" ); }

  SUIT_DataObject*  getRootObject() const;
  virtual void      setRootObject( SUIT_DataObject* theRoot = 0 );

  int               numberOfSelected() const;

  DataObjectList    getSelected() const;
  void              getSelected( DataObjectList& ) const;

  virtual void      setSelected( const SUIT_DataObject* theObject, const bool = false );
  virtual void      setSelected( const DataObjectList& theObjLst, const bool = false );

  bool              isOpen( SUIT_DataObject* theObject ) const;
  virtual void      setOpen( SUIT_DataObject* theObject, const bool theOpen = true );

  bool              isAutoUpdate() const;
  virtual void      setAutoUpdate( const bool );

  bool              isAutoDeleteObjects() const;
  virtual void      setAutoDeleteObjects( const bool );

  virtual void      updateTree( SUIT_DataObject* = 0 );
  virtual void      replaceTree( SUIT_DataObject*, SUIT_DataObject* );

  bool              isShowToolTips();
  void              setShowToolTips( const bool theDisplay );

  void              adjustWidth();

  SUIT_DataObject*  dataObjectAt( const QPoint& ) const;

  OB_Filter*        filter() const;
  void              setFilter( OB_Filter* );

  bool              rootIsDecorated() const;
  void              setRootIsDecorated( const bool );

  int               autoOpenLevel() const;
  void              setAutoOpenLevel( const int );

  virtual int       addColumn( const QString&, const int id = -1, const int width = -1 );
  virtual int       addColumn( const QIconSet&, const QString&, const int id = -1, const int width = -1 );
  virtual void      removeColumn( const int id );

  void              setNameTitle( const QString& );
  virtual void      setNameTitle( const QIconSet&, const QString& );
  void              setColumnTitle( const int id, const QString& );
  virtual void      setColumnTitle( const int id, const QIconSet&, const QString& );

  QString           nameTitle() const;
  QString           columnTitle( const int ) const;

  bool              isColumnVisible( const int ) const;
  virtual void      setColumnShown( const int, const bool );

  virtual void      setWidthMode( QListView::WidthMode );

  QValueList<int>   columns() const;

  bool              appropriateColumn( const int ) const;
  virtual void      setAppropriateColumn( const int, const bool );

  virtual bool      eventFilter(QObject* watched, QEvent* e);

  QListView*        listView() const;

  virtual void      contextMenuPopup( QPopupMenu* );

signals:
  void              selectionChanged();
  void              doubleClicked( SUIT_DataObject* );
  void              dropped( DataObjectList, SUIT_DataObject*, int );

private slots:
  void              onExpand();
  void              onColumnVisible( int );
  void              onDestroyed( SUIT_DataObject* );
  void              onDoubleClicked ( QListViewItem* );
  void              onDropped( QPtrList<QListViewItem>, QListViewItem*, int );

protected:
  void              adjustWidth( QListViewItem* );
  virtual void      updateView( const SUIT_DataObject* theStartObj = 0 );
  virtual void      updateText();

  virtual void      keyPressEvent( QKeyEvent* );

  SUIT_DataObject*  dataObject( const QListViewItem* ) const;
  QListViewItem*    listViewItem( const SUIT_DataObject* ) const;

private:
  typedef QMap<SUIT_DataObject*, QListViewItem*> ItemMap;
  typedef SUIT_DataObjectKeyHandle               DataObjectKey;
  typedef QMap<SUIT_DataObject*, int>            DataObjectMap;
  typedef QMap<DataObjectKey, int>               DataObjectKeyMap;

private:
  void              expand( QListViewItem* );
  bool              hasClosed( QListViewItem* ) const;

  void              autoOpenBranches();
  void              openBranch( QListViewItem*, const int );

  void              removeReferences( QListViewItem* );
  void              removeConnections( SUIT_DataObject* );
  void              createConnections( SUIT_DataObject* );
  void              removeObject( SUIT_DataObject*, const bool = true );

  void              updateText( QListViewItem* );

  DataObjectKey     objectKey( QListViewItem* ) const;
  DataObjectKey     objectKey( SUIT_DataObject* ) const;

  QListViewItem*    createTree( const SUIT_DataObject*, QListViewItem*, QListViewItem* = 0 );
  QListViewItem*    createItem( const SUIT_DataObject*, QListViewItem*, QListViewItem* = 0 );

  SUIT_DataObject*  storeState( DataObjectMap&, DataObjectMap&,
                                DataObjectKeyMap&, DataObjectKeyMap&, DataObjectKey& ) const;
  void              restoreState( const DataObjectMap&, const DataObjectMap&, const SUIT_DataObject*,
                                  const DataObjectKeyMap&, const DataObjectKeyMap&, const DataObjectKey& );

private:
  OB_ListView*      myView;
  SUIT_DataObject*  myRoot;
  ItemMap           myItems;
  QToolTip*         myTooltip;
  QMap<int, int>    myColumnIds;
  bool              myAutoUpdate;
  bool              myAutoDelObjs;
  bool              myShowToolTips;
  bool              myRootDecorated;
  int               myAutoOpenLevel;

  friend class OB_Browser::ToolTip;
};

#endif
