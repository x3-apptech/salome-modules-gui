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

// File:   SUIT_TreeModel.h
// Author: Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#ifndef SUIT_TREEMODEL_H
#define SUIT_TREEMODEL_H

#include "SUIT.h"

#include <Qtx.h>

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QModelIndex>
#include <QItemDelegate>
#include <QVariant>
#include <QMap>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class SUIT_DataObject;
class SUIT_TreeModel;
class QMimeData;

class SUIT_EXPORT SUIT_DataSearcher
{
public:
  virtual SUIT_DataObject* findObject( const QString& ) const = 0;
};

class SUIT_EXPORT SUIT_AbstractModel
{
  SUIT_DataSearcher* mySearcher;

public:
  SUIT_AbstractModel();

  operator const QAbstractItemModel*() const;
  operator QAbstractItemModel*();
  operator const QObject*() const;

  virtual SUIT_DataObject*      root() const = 0;
  virtual void                  setRoot( SUIT_DataObject* ) = 0;
  virtual SUIT_DataObject*      object( const QModelIndex& = QModelIndex() ) const = 0;
  virtual QModelIndex           index( const SUIT_DataObject*, int = 0 ) const = 0;
  virtual bool                  autoDeleteTree() const = 0;
  virtual void                  setAutoDeleteTree( const bool ) = 0;
  virtual bool                  autoUpdate() const = 0;
  virtual void                  setAutoUpdate( const bool ) = 0;
  virtual bool                  updateModified() const = 0;
  virtual void                  setUpdateModified( const bool ) = 0;
  virtual QAbstractItemDelegate* delegate() const = 0;
  virtual bool                  customSorting( const int ) const = 0;
  virtual bool                  lessThan( const QModelIndex& left, const QModelIndex& right ) const = 0;
  virtual void                  forgetObject( const SUIT_DataObject* ) = 0;

  virtual void                  updateTree( const QModelIndex& ) = 0;
  virtual void                  updateTree( SUIT_DataObject* = 0 ) = 0;

  virtual void                  registerColumn( const int group_id, const QString& name, const int custom_id ) = 0;
  virtual void                  unregisterColumn( const int group_id, const QString& name ) = 0;
  virtual void                  setColumnIcon( const QString& name, const QPixmap& icon ) = 0;
  virtual QPixmap               columnIcon( const QString& name ) const = 0;
  virtual void                  setAppropriate( const QString& name, const Qtx::Appropriate appr ) = 0;
  virtual Qtx::Appropriate      appropriate( const QString& name ) const = 0;
  virtual void                  setVisibilityState(const QString& id, Qtx::VisibilityState state, bool emitChanged = true) = 0;
  virtual void                  setVisibilityStateForAll(Qtx::VisibilityState state) = 0;
  virtual Qtx::VisibilityState  visibilityState(const QString& id) const = 0;
  virtual void                  setHeaderFlags( const QString& name, const Qtx::HeaderViewFlags flags ) = 0;
  virtual Qtx::HeaderViewFlags  headerFlags( const QString& name ) const = 0;
  virtual void                  emitClicked( SUIT_DataObject* obj, const QModelIndex& index) = 0;

  virtual SUIT_DataSearcher*    searcher() const;
  virtual void                  setSearcher( SUIT_DataSearcher* );
};


class SUIT_EXPORT SUIT_TreeModel : public QAbstractItemModel, public SUIT_AbstractModel
{
  Q_OBJECT

private:
  class TreeItem;
  class TreeSync;
  typedef SUIT_DataObject*          ObjPtr;
  typedef SUIT_TreeModel::TreeItem* ItemPtr;
 
public:
  //! Data role
  typedef enum {
    DisplayRole         = Qt::DisplayRole,       //!< text label
    DecorationRole      = Qt::DecorationRole,    //!< icon
    EditRole            = Qt::EditRole,          //!< edit mode
    ToolTipRole         = Qt::ToolTipRole,       //!< tooltip
    StatusTipRole       = Qt::StatusTipRole,     //!< status tip
    WhatsThisRole       = Qt::WhatsThisRole,     //!< what's this info
    FontRole            = Qt::FontRole,          //!< font
    TextAlignmentRole   = Qt::TextAlignmentRole, //!< text alignment
    BackgroundRole      = Qt::BackgroundRole,    //!< background color
    ForegroundRole      = Qt::ForegroundRole,    //!< text color
    CheckStateRole      = Qt::CheckStateRole,    //!< check state
    SizeHintRole        = Qt::SizeHintRole,      //!< size hint
    BaseColorRole       = Qt::UserRole,          //!< (editor) background color
    TextColorRole,                               //!< (editor) text color    (Qt::UserRole + 1)
    HighlightRole,                               //!< highlight color        (Qt::UserRole + 2)
    HighlightedTextRole,                         //!< highlighted text color (Qt::UserRole + 3)
    AppropriateRole     = Qtx::AppropriateRole   //!< appropriate flag       (Qt::UserRole + 100)
  } Role;

  SUIT_TreeModel( QObject* = 0 );
  SUIT_TreeModel( SUIT_DataObject*, QObject* = 0 );
  ~SUIT_TreeModel();

  SUIT_DataObject*       root() const;
  void                   setRoot( SUIT_DataObject* );

  virtual QVariant       data( const QModelIndex&, int = DisplayRole ) const;
  virtual bool           setData( const QModelIndex&, const QVariant&, int = EditRole );
  virtual Qt::ItemFlags  flags( const QModelIndex& ) const;
  virtual QVariant       headerData( int, Qt::Orientation, int = Qt::DisplayRole ) const;

  virtual Qt::DropActions supportedDropActions() const;

  virtual QModelIndex    index( int, int, const QModelIndex& = QModelIndex() ) const;
  virtual QModelIndex    parent( const QModelIndex& ) const;

  virtual int              columnCount( const QModelIndex& = QModelIndex() ) const;
  virtual int              rowCount( const QModelIndex& = QModelIndex() ) const;
  virtual void             registerColumn( const int group_id, const QString& name, const int custom_id );
  virtual void             unregisterColumn( const int group_id, const QString& name );
  virtual void             setColumnIcon( const QString& name, const QPixmap& icon );
  virtual QPixmap          columnIcon( const QString& name ) const;
  virtual void             setAppropriate( const QString& name, const Qtx::Appropriate appr );
  virtual Qtx::Appropriate appropriate( const QString& name ) const;
  virtual void                  setVisibilityState(const QString& id, Qtx::VisibilityState state, bool emitChanged = true);
  virtual void                  setVisibilityStateForAll(Qtx::VisibilityState state);
  virtual Qtx::VisibilityState  visibilityState(const QString& id) const;
  virtual void                  setHeaderFlags( const QString& name, const Qtx::HeaderViewFlags flags );
  virtual Qtx::HeaderViewFlags  headerFlags( const QString& name ) const;
  virtual void           emitClicked( SUIT_DataObject* obj, const QModelIndex& index);


  SUIT_DataObject*       object( const QModelIndex& = QModelIndex() ) const;
  QModelIndex            index( const SUIT_DataObject*, int = 0 ) const;

  bool                   autoDeleteTree() const;
  void                   setAutoDeleteTree( const bool );

  bool                   autoUpdate() const;
  void                   setAutoUpdate( const bool );

  bool                   updateModified() const;
  void                   setUpdateModified( const bool );

  virtual bool           customSorting( const int ) const;
  virtual bool           lessThan( const QModelIndex& left, const QModelIndex& right ) const;
  virtual void           forgetObject( const SUIT_DataObject* );

  QAbstractItemDelegate* delegate() const;


  virtual void           updateTreeModel(SUIT_DataObject*,TreeItem*);

  virtual QStringList    mimeTypes() const;
  virtual QMimeData*     mimeData (const QModelIndexList& indexes) const;
  virtual bool           dropMimeData (const QMimeData *data, Qt::DropAction action,
                                       int row, int column, const QModelIndex &parent);

public slots:
  virtual void           updateTree( const QModelIndex& );
  virtual void           updateTree( SUIT_DataObject* = 0 );

signals:
  void modelUpdated();
  void clicked( SUIT_DataObject*, int );
  void dropped( const QList<SUIT_DataObject*>&, SUIT_DataObject*, int, Qt::DropAction );
  void renamed( SUIT_DataObject* );

private:
  void                   initialize();

  TreeItem*              rootItem() const;
  TreeItem*              treeItem( const QModelIndex& ) const;
  TreeItem*              treeItem( const SUIT_DataObject* ) const;
  SUIT_DataObject*       object( const TreeItem* ) const;
  QString                objectId( const QModelIndex& = QModelIndex() ) const;

  TreeItem*              createItem( SUIT_DataObject*, TreeItem* = 0, TreeItem* = 0 );
  TreeItem*              createItemAtPos( SUIT_DataObject*, TreeItem* = 0, int pos=0 );
  void                   updateItem( TreeItem*, bool emitLayoutChanged );
  void                   removeItem( TreeItem* );

private slots:
  void                   onInserted( SUIT_DataObject*, SUIT_DataObject* );
  void                   onRemoved( SUIT_DataObject*, SUIT_DataObject* );
  void                   onModified( SUIT_DataObject* );

private:
  typedef QMap<SUIT_DataObject*, TreeItem*> ItemMap;
  typedef struct
  {
    QString myName;
        QMap<int,int> myIds;
        QPixmap myIcon;
        Qtx::HeaderViewFlags myHeaderFlags;
        Qtx::Appropriate myAppropriate;

  } ColumnInfo;

  typedef QMap<QString,Qtx::VisibilityState> VisibilityMap;
  
  SUIT_DataObject*    myRoot;
  TreeItem*           myRootItem;
  ItemMap             myItems;
  VisibilityMap       myVisibilityMap;
  bool                myAutoDeleteTree;
  bool                myAutoUpdate;
  bool                myUpdateModified;
  QVector<ColumnInfo> myColumns;

  friend class SUIT_TreeModel::TreeSync;
};

class SUIT_EXPORT SUIT_ProxyModel : public QSortFilterProxyModel, public SUIT_AbstractModel
{
  Q_OBJECT

public:
  SUIT_ProxyModel( QObject* = 0 );
  SUIT_ProxyModel( SUIT_DataObject*, QObject* = 0 );
  SUIT_ProxyModel( SUIT_AbstractModel*, QObject* = 0 );
  ~SUIT_ProxyModel();

  SUIT_DataObject*       root() const;
  void                   setRoot( SUIT_DataObject* );

  SUIT_DataObject*       object( const QModelIndex& = QModelIndex() ) const;
  QModelIndex            index( const SUIT_DataObject*, int = 0 ) const;

  bool                   autoDeleteTree() const;
  void                   setAutoDeleteTree( const bool );

  bool                   autoUpdate() const;
  void                   setAutoUpdate( const bool );

  bool                   updateModified() const;
  void                   setUpdateModified( const bool );

  bool                   isSortingEnabled() const;
  bool                   customSorting( const int ) const;

  virtual void             forgetObject( const SUIT_DataObject* );
  virtual bool             lessThan( const QModelIndex&, const QModelIndex& ) const;
  virtual void             registerColumn( const int group_id, const QString& name, const int custom_id );
  virtual void             unregisterColumn( const int group_id, const QString& name );
  virtual void             setColumnIcon( const QString& name, const QPixmap& icon );
  virtual QPixmap          columnIcon( const QString& name ) const;
  virtual void             setAppropriate( const QString& name, const Qtx::Appropriate appr );
  virtual Qtx::Appropriate appropriate( const QString& name ) const;
  virtual void                  setVisibilityState(const QString& id, Qtx::VisibilityState state, bool emitChanged = true);
  virtual void                  setVisibilityStateForAll(Qtx::VisibilityState state);
  virtual Qtx::VisibilityState  visibilityState(const QString& id) const;
  virtual void                  setHeaderFlags( const QString& name, const Qtx::HeaderViewFlags flags );
  virtual Qtx::HeaderViewFlags  headerFlags( const QString& name ) const;
  virtual void             emitClicked( SUIT_DataObject* obj, const QModelIndex& index);

  virtual SUIT_DataSearcher*    searcher() const;
  virtual void                  setSearcher( SUIT_DataSearcher* );

  QAbstractItemDelegate* delegate() const;

public slots:
  virtual void           updateTree( const QModelIndex& );
  virtual void           updateTree( SUIT_DataObject* = 0 );
  void                   setSortingEnabled( bool );

signals:
  void modelUpdated();
  void clicked( SUIT_DataObject*, int );
  void dropped( const QList<SUIT_DataObject*>&, SUIT_DataObject*, int, Qt::DropAction );
  void renamed( SUIT_DataObject* );

protected:
  SUIT_AbstractModel*    treeModel() const;
  virtual bool           filterAcceptsRow( int, const QModelIndex& ) const;

private:
  bool                   mySortingEnabled;
};

class SUIT_EXPORT SUIT_ItemDelegate : public QItemDelegate
{
  Q_OBJECT

public:
  SUIT_ItemDelegate( QObject* = 0 );
  
  virtual void paint( QPainter*, const QStyleOptionViewItem&,
                      const QModelIndex& ) const;
                      
  virtual QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif // SUIT_TREEMODEL_H
