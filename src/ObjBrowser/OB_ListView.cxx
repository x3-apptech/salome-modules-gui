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
#include "OB_ListView.h"

#include "OB_Filter.h"
#include "OB_ListItem.h"

#include <SUIT_DataObject.h>

#include <qdragobject.h>

/*!
  Constructor
*/
OB_ListView::OB_ListView( QWidget* parent, const char* name, WFlags f )
: QtxListView( parent, name, f ),
myFilter( 0 )
{
}

/*!
  Constructor
*/
OB_ListView::OB_ListView( const int state, QWidget* parent, const char* name, WFlags f )
: QtxListView( state, parent, name, f ),
myFilter( 0 )
{
}

/*!
  Destructor
*/
OB_ListView::~OB_ListView()
{
  delete myFilter;
}

/*!
  \return filter
*/
OB_Filter* OB_ListView::filter() const
{
  return myFilter;
}

/*!
  Changes filter
  \param f - new filter
*/
void OB_ListView::setFilter( OB_Filter* f )
{
  if ( myFilter == f )
    return;

  delete myFilter;
  myFilter = f;
}

/*!
  \return true if item passes filter
*/
bool OB_ListView::isOk( QListViewItem* item ) const
{
  bool ok = true;
  SUIT_DataObject* obj = dataObject( item );
  if ( obj && filter() )
    ok = filter()->isOk( obj );
  return ok;
}

/*!
  Creates new drag object
*/
QDragObject* OB_ListView::dragObject()
{
  myItems.clear();

  for ( QListViewItemIterator it( this ); it.current(); ++it )
    if ( it.current()->isSelected() )
      myItems.append( it.current() );

  return new QTextDrag( "", this );
}

/*!
  Custom drag enter event filter
*/
void OB_ListView::dragEnterEvent( QDragEnterEvent* e )
{
  e->accept();
}

/*!
  Custom drag move event filter
*/
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

/*!
  Custom drop event filter
*/
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

/*!
  Custom key press event filter
*/
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

/*!
  Finds item, in that dragged objects are dropped by QDropEvent
  \return tree item
*/
QListViewItem* OB_ListView::dropItem( QDropEvent* e ) const
{
  QListViewItem* item = 0;
  if ( e )
    item = itemAt( QPoint( e->pos().x() - viewport()->x(), e->pos().y() - viewport()->y() ) );

  return item;
}

/*!
  \return SUIT object by tree item
  \param item - tree item
*/
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

/*!
  \return true if it is possible to drop into item
  \param item - tree item to be checked
*/
bool OB_ListView::isDropAccepted( QListViewItem* item ) const
{
  bool res = true;

  for ( QPtrListIterator<QListViewItem> it( myItems ); it.current() && res; ++it )
    res = res && isDropAccepted( it.current(), item );

  return res;
}

/*!
  \return true if it is possible to drop one item into other
  \param drag - dragged item
  \param drop - destination item
*/
bool OB_ListView::isDropAccepted( QListViewItem* drag, QListViewItem* drop ) const
{
  SUIT_DataObject* dragObj = dataObject( drag );
  SUIT_DataObject* dropObj = dataObject( drop );

  if ( !dragObj || !dropObj )
    return false;

  return dropObj->isDropAccepted( dragObj );
}

/*!
  Sets column width
  \param col - column index
  \param width - column width
*/ 
void OB_ListView::setColumnWidth( int col, int width )
{
  int max = columnMaxWidth( col );
  if ( max>0 && width>max )
    width = max;

  QtxListView::setColumnWidth( col, width );
}

/*!
  \return column max width
  \param col - column index
*/ 
int OB_ListView::columnMaxWidth( const int col ) const
{
  int res = -1;
  if( myMaxColWidth.contains( col ) )
    res = myMaxColWidth[col];
  else if( myMaxColRatio.contains( col ) )
    res = int( myMaxColRatio[col]*height() );
  return res;
}

/*!
  Changes column max width
  \param col - column index
  \param w - column max width
*/ 
void OB_ListView::setColumnMaxWidth( const int col, const int w )
{
  myMaxColWidth.insert( col, w );
}

/*!
  \return column max ratio
  \param col - column index
*/ 
double OB_ListView::columnMaxRatio( const int col ) const
{
  double res = 0.0;
  if( myMaxColRatio.contains( col ) )
    res = myMaxColRatio[col];
  return res;
}

/*!
  Changes column max ratio
  \param col - column index
  \param w - column max ratio
*/ 
void OB_ListView::setColumnMaxRatio( const int col, const double r )
{
  myMaxColRatio.insert( col, r );
}
