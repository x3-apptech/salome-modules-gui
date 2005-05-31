#ifndef OB_LISTVIEW_H
#define OB_LISTVIEW_H

#include "OB.h"


#include <QtxListView.h>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

#include <qptrlist.h>

class OB_Filter;
class QListViewItem;
class SUIT_DataObject;

class OB_EXPORT OB_ListView : public QtxListView
{
  Q_OBJECT

public:
  OB_ListView( QWidget* = 0, const char* = 0, WFlags = 0 );
  OB_ListView( const int, QWidget* = 0, const char* = 0, WFlags = 0 );
  virtual ~OB_ListView();

  OB_Filter*              filter() const;
  void                    setFilter( OB_Filter* );

  virtual int             addColumn( const QString& label, int width = -1 );
  virtual int             addColumn( const QIconSet& iconset, const QString& label, int width = -1 );

  bool                    isOk( QListViewItem* ) const;

signals:
  void                    dropped( QPtrList<QListViewItem>, QListViewItem*, int );

protected:
  virtual QDragObject*    dragObject();
  virtual void            dropEvent( QDropEvent* );
  virtual void            dragMoveEvent( QDragMoveEvent* );
  virtual void            dragEnterEvent( QDragEnterEvent* );

private:
  void                    updateHeader();
  QListViewItem*          dropItem( QDropEvent* ) const;
  SUIT_DataObject*        dataObject( QListViewItem* ) const;
  bool                    isDropAccepted( QListViewItem* ) const;
  bool                    isDropAccepted( QListViewItem*, QListViewItem* ) const;

protected:
  QPtrList<QListViewItem> myItems;
  OB_Filter*              myFilter;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
