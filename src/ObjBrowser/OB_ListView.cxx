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
#include "OB_ListView.h"

#include "OB_Filter.h"
#include "OB_ListItem.h"

#include <SUIT_DataObject.h>

#include <qdragobject.h>

OB_ListView::OB_ListView( QWidget* parent, const char* name, WFlags f )
: QtxListView( parent, name, f ),
myFilter( 0 )
{
}

OB_ListView::OB_ListView( const int state, QWidget* parent, const char* name, WFlags f )
: QtxListView( state, parent, name, f ),
myFilter( 0 )
{
}

OB_ListView::~OB_ListView()
{
  delete myFilter;
}

OB_Filter* OB_ListView::filter() const
{
  return myFilter;
}

void OB_ListView::setFilter( OB_Filter* f )
{
  if ( myFilter == f )
    return;

  delete myFilter;
  myFilter = f;
}

bool OB_ListView::isOk( QListViewItem* item ) const
{
  bool ok = true;
  SUIT_DataObject* obj = dataObject( item );
  if ( obj && filter() )
    ok = filter()->isOk( obj );
  return ok;
}

QDragObject* OB_ListView::dragObject()
{
  myItems.clear();

  for ( QListViewItemIterator it( this ); it.current(); ++it )
    if ( it.current()->isSelected() )
      myItems.append( it.current() );

  return new QTextDrag( "", this );
}

void OB_ListView::dragEnterEvent( QDragEnterEvent* e )
{
  e->accept();
}

void OB_ListView::dragMoveEvent( QDragMoveEvent* e )
{
  QListViewItem* item = dropItem( e );

  if ( isDropAccepted( item ) )
  {
    setCurrentItem( item );
    e->accept( true );
  }
  else
    e->accept( false );
}

void OB_ListView::dropEvent( QDropEvent* e )
{
  QListViewItem* item = dropItem( e );
  if ( isDropAccepted( item ) )
  {
    e->accept();
    emit dropped( myItems, item, e->action() );
  }
  myItems.clear();
}

void OB_ListView::keyPressEvent( QKeyEvent* ke )
{
  if ( ( ke->key() == Qt::Key_Plus || ke->key() == Qt::Key_Minus ) && ke->state() & ControlButton )
  {
    bool isOpen = ke->key() == Qt::Key_Plus;
    for ( QListViewItemIterator it( this ); it.current(); ++it )
      if ( it.current()->childCount() )
        it.current()->setOpen( isOpen );
  }
  else
    QtxListView::keyPressEvent( ke );
}

QListViewItem* OB_ListView::dropItem( QDropEvent* e ) const
{
  QListViewItem* item = 0;
  if ( e )
    item = itemAt( QPoint( e->pos().x() - viewport()->x(), e->pos().y() - viewport()->y() ) );

  return item;
}

SUIT_DataObject* OB_ListView::dataObject( QListViewItem* item ) const
{
  if ( !item )
    return 0;

  SUIT_DataObject* obj = 0;

  if ( dynamic_cast<OB_ListItem*>( item ) )
    obj = dynamic_cast<OB_ListItem*>( item )->dataObject();
  else if ( dynamic_cast<OB_CheckListItem*>( item ) )
    obj = dynamic_cast<OB_CheckListItem*>( item )->dataObject();

  return obj;
}

bool OB_ListView::isDropAccepted( QListViewItem* item ) const
{
  bool res = true;

  for ( QPtrListIterator<QListViewItem> it( myItems ); it.current() && res; ++it )
    res = res && isDropAccepted( it.current(), item );

  return res;
}

bool OB_ListView::isDropAccepted( QListViewItem* drag, QListViewItem* drop ) const
{
  SUIT_DataObject* dragObj = dataObject( drag );
  SUIT_DataObject* dropObj = dataObject( drop );

  if ( !dragObj || !dropObj )
    return false;

  return dropObj->isDropAccepted( dragObj );
}

void OB_ListView::setColumnWidth( int col, int width )
{
  int max = columnMaxWidth( col );
  if( max>0 && width>max )
    width = max;
  QListView::setColumnWidth( col, width );
}

int OB_ListView::columnMaxWidth( const int col ) const
{
  int res = -1;
  if( myMaxColWidth.contains( col ) )
    res = myMaxColWidth[col];
  else if( myMaxColRatio.contains( col ) )
    res = int( myMaxColRatio[col]*height() );
  return res;
}

void OB_ListView::setColumnMaxWidth( const int col, const int w )
{
  myMaxColWidth.insert( col, w );
}

double OB_ListView::columnMaxRatio( const int col ) const
{
  double res = 0.0;
  if( myMaxColRatio.contains( col ) )
    res = myMaxColRatio[col];
  return res;
}

void OB_ListView::setColumnMaxRatio( const int col, const double r )
{
  myMaxColRatio.insert( col, r );
}
