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
// See http://www.salome-platform.org/
//
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

  bool                    isOk( QListViewItem* ) const;
  virtual void            setColumnWidth( int, int );
  int                     columnMaxWidth( const int ) const;
  void                    setColumnMaxWidth( const int, const int );
  double                  columnMaxRatio( const int ) const;
  void                    setColumnMaxRatio( const int, const double );

signals:
  void                    dropped( QPtrList<QListViewItem>, QListViewItem*, int );

protected:
  virtual QDragObject*    dragObject();
  virtual void            dropEvent( QDropEvent* );
  virtual void            dragMoveEvent( QDragMoveEvent* );
  virtual void            dragEnterEvent( QDragEnterEvent* );

  virtual void            keyPressEvent( QKeyEvent* );

private:
  void                    updateHeader();
  QListViewItem*          dropItem( QDropEvent* ) const;
  SUIT_DataObject*        dataObject( QListViewItem* ) const;
  bool                    isDropAccepted( QListViewItem* ) const;
  bool                    isDropAccepted( QListViewItem*, QListViewItem* ) const;

protected:
  QPtrList<QListViewItem> myItems;
  OB_Filter*              myFilter;
  QMap<int,int>           myMaxColWidth;
  QMap<int,double>        myMaxColRatio;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
