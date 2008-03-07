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
#include "OB_ListItem.h"

#include "OB_ListView.h"

#include <SUIT_DataObject.h>

#include <qpainter.h>
#include <qwmatrix.h>

#include <iostream>
using namespace std;

#ifdef WNT
#define min(a, b)  (((a) < (b)) ? (a) : (b))
#endif

/*!
  Constructor
*/
template<class T>
ListItemF<T>::ListItemF( T* theT, SUIT_DataObject* obj ) :
myT( theT ),
myObject( obj )
{
}

/*!
  Paints item
*/
template<class T>
void ListItemF<T>::paintC( QPainter* p, QColorGroup& cg, int c, int w, int align )
{ 
//  QColorGroup colorGrp( cg );
  if ( myObject )
  {
    if ( myObject->color( SUIT_DataObject::Text ).isValid() )
      cg.setColor( QColorGroup::Text, myObject->color( SUIT_DataObject::Text ) );
    if ( myObject->color( SUIT_DataObject::Base ).isValid() )
      cg.setColor( QColorGroup::Base, myObject->color( SUIT_DataObject::Base ) );
    if ( myObject->color( SUIT_DataObject::Foreground ).isValid() )
      cg.setColor( QColorGroup::Foreground, myObject->color( SUIT_DataObject::Foreground ) );
    if ( myObject->color( SUIT_DataObject::Background ).isValid() )
      cg.setColor( QColorGroup::Background, myObject->color( SUIT_DataObject::Background ) );
    if ( myObject->color( SUIT_DataObject::Highlight ).isValid() )
      cg.setColor( QColorGroup::Highlight, myObject->color( SUIT_DataObject::Highlight ) );
    if ( myObject->color( SUIT_DataObject::HighlightedText ).isValid() )
      cg.setColor( QColorGroup::HighlightedText, myObject->color( SUIT_DataObject::HighlightedText ) );
  }

  
  p->fillRect( 0, 0, w, myT->height(), cg.brush( QColorGroup::Base ) );
  //int itemW = myT.width( p->fontMetrics(), myT.listView(), c );
    
  //myT.paintCell( p, colorGrp, c, itemW,  align );
}

/*!
  Paints focused item
*/
template<class T>
void ListItemF<T>::paintFoc( QPainter* p, QColorGroup& cg, const QRect& r )
{
  QRect rect = r;
  rect.setWidth( myT->width( p->fontMetrics(), myT->listView(), 0 ) );
  //myT.paintFocus( p, cg, rect );
}

/*!
  Set selection of item
  \param s - selected state
*/
template<class T>
void ListItemF<T>::setSel( bool s )
{
  QListView* lv = myT->listView();
  if ( s && lv && lv->inherits( "OB_ListView" ) )
  {
    OB_ListView* objlv = (OB_ListView*)lv;
    s = s && objlv->isOk( myT );
  }

  //myT.setSelected( s );
}

/*!
  Updates name and pixmap of item
*/
template<class T>
void ListItemF<T>::update()
{
  SUIT_DataObject* obj = dataObject();
  if ( !obj )
    return;

  QString n = obj->name();
  if( myT->text( 0 )!=n )
    myT->setText( 0, n );

  QPixmap p = obj->icon();
  int aIconW = p.width();
  if( aIconW > 0 )
  {
    if( aIconW > 20 )
    {
      QWMatrix aM;
      double aScale = 20.0 / aIconW;
      aM.scale( aScale, aScale );
      myT->setPixmap( 0, p.xForm( aM ) );
    }
    else
      myT->setPixmap( 0, p );
  }
  else if ( p.isNull() )
    myT->setPixmap( 0, p );

  myT->setDragEnabled( obj->isDragable() );
  myT->setDropEnabled( true );
}

/*!
  Constructor
*/
OB_ListItem::OB_ListItem( SUIT_DataObject* obj, QListView* parent )
: ListItemF<QListViewItem>( this, obj ),
 QListViewItem(parent)
{
  update();
}

/*!
  Constructor
*/
OB_ListItem::OB_ListItem( SUIT_DataObject* obj, QListViewItem* parent )
: ListItemF<QListViewItem>( this, obj ),
 QListViewItem(parent)
{
  update();
}

/*!
  Constructor
*/
OB_ListItem::OB_ListItem( SUIT_DataObject* obj, QListView* parent, QListViewItem* after )
: ListItemF<QListViewItem>( this, obj),
QListViewItem(parent, after )
{
  update();
}

/*!
  Constructor
*/
OB_ListItem::OB_ListItem( SUIT_DataObject* obj, QListViewItem* parent, QListViewItem* after )
: ListItemF<QListViewItem>( this,obj),
QListViewItem(parent, after )
{
  update();
}

/*!
  Destructor
*/
OB_ListItem::~OB_ListItem()
{
}

/*!
  Sets selection state of item
  \param s - new state
*/
void OB_ListItem::setSelected( bool s )
{
  setSel( s );
  QListViewItem::setSelected( s );
}

/*!
  Paints focus
  \param p - painter
  \param cg - color group
  \param r - focus rectangle 
*/
void OB_ListItem::paintFocus( QPainter* p, const QColorGroup& cg, const QRect& r )
{
  QColorGroup col_group( cg );
	paintFoc( p, col_group, r );

  QRect R( r );
  if ( listView() && !listView()->allColumnsShowFocus() )
    R.setWidth( width( p->fontMetrics(), listView(), 0 ) );

  QListViewItem::paintFocus( p, col_group, R );
}

/*!
  Paints item
  \param p - painter
  \param cg - color group
  \param c - not used
  \param w - width of item
  \param align - alignment
*/
void OB_ListItem::paintCell( QPainter* p, const QColorGroup& cg, int c, int w, int align )
{
  QColorGroup col_group( cg );
	paintC( p, col_group, c ,w, align );

  int W = w;
  if ( listView() && !listView()->allColumnsShowFocus() )
    W = width( p->fontMetrics(), listView(), c );

	QListViewItem::paintCell( p, col_group, c, min( W, w ), align );
}

/*!
  \return custom RTTI info
*/
int OB_ListItem::RTTI()
{
  return 1000;
}

/*!
  \return custom RTTI info
*/
int OB_ListItem::rtti() const
{
  return RTTI();
}

/*!
  Sets item text
  \param column - column index
  \param text - new text
*/
void OB_ListItem::setText( int column, const QString& text )
{
  QListViewItem::setText( column, text );
  QFontMetrics fm = listView()->fontMetrics();
  int necessary = width( fm, listView(), column ),
      current = listView()->columnWidth( column );

  if( listView()->columnWidthMode( column )==QListView::Maximum && necessary>current )
    listView()->setColumnWidth( column, necessary );
}

/*!
  Constructor
*/
OB_CheckListItem::OB_CheckListItem( SUIT_DataObject* obj, QListView* parent, Type type )
: ListItemF<QCheckListItem>( this, obj),
QCheckListItem( parent, "", type )
{
  update();
}

/*!
  Constructor
*/
OB_CheckListItem::OB_CheckListItem( SUIT_DataObject* obj, QListViewItem* parent, Type type )
: ListItemF<QCheckListItem>( this, obj),
QCheckListItem( parent, "", type )
{
  update();
}

/*!
  Constructor
*/
OB_CheckListItem::OB_CheckListItem( SUIT_DataObject* obj, QListView* parent, QListViewItem* after, Type type )
: ListItemF<QCheckListItem>( this, obj),
#if defined(QT_VERSION) && QT_VERSION >= 0x030101
 QCheckListItem( parent, after, "", type )
#else
 QCheckListItem( parent, "", type )
#endif
{
  update();
}

/*!
  Constructor
*/
OB_CheckListItem::OB_CheckListItem( SUIT_DataObject* obj, QListViewItem* parent, QListViewItem* after, Type type )
: ListItemF<QCheckListItem>( this, obj),
#if defined(QT_VERSION) && QT_VERSION >= 0x030101
 QCheckListItem( parent, after, "", type )
#else
 QCheckListItem( parent, "", type )
#endif
{
  update();
}

/*!
  Destructor
*/
OB_CheckListItem::~OB_CheckListItem()
{
}

/*!
  Sets selection state of item
  \param s - new state
*/
void OB_CheckListItem::setSelected( bool s )
{
	setSel( s );
	QCheckListItem::setSelected( s );
}

/*!
  Paints focus
  \param p - painter
  \param cg - color group
  \param r - focus rectangle 
*/
void OB_CheckListItem::paintFocus( QPainter* p, const QColorGroup& cg, const QRect& r )
{
  QColorGroup col_group( cg );
	paintFoc( p, col_group, r );

  QRect R( r );
  if ( listView() && !listView()->allColumnsShowFocus() )
    R.setWidth( width( p->fontMetrics(), listView(), 0 ) );

	QCheckListItem::paintFocus( p, col_group, R );
}

/*!
  Paints item
  \param p - painter
  \param cg - color group
  \param c - not used
  \param w - width of item
  \param align - alignment
*/
void OB_CheckListItem::paintCell( QPainter* p, const QColorGroup& cg, int c, int w, int align )
{
  QColorGroup col_group( cg );
	paintC( p, col_group, c ,w, align );

  int W = w;
  if ( listView() && !listView()->allColumnsShowFocus() )
    W = width( p->fontMetrics(), listView(), c );

  QCheckListItem::paintCell( p, col_group, c, min( W, w ), align );
}

/*!
  \return custom RTTI info
*/
int OB_CheckListItem::RTTI()
{
  return OB_ListItem::RTTI() + 1;
}

/*!
  \return custom RTTI info
*/
int OB_CheckListItem::rtti() const
{
  return RTTI();
}

/*!
  SLOT: called if checked state is changed, changes corresponding state of SUIT object
*/
void OB_CheckListItem::stateChange( bool on )
{
  QCheckListItem::stateChange( on );

  if ( dataObject() )
    dataObject()->setOn( on );
}
