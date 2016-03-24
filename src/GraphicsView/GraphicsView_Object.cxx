// Copyright (C) 2013-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "GraphicsView_Object.h"

#include "GraphicsView_Scene.h"
#include "GraphicsView_ViewPort.h"

//=======================================================================
// Name    : GraphicsView_Object
// Purpose : Constructor
//=======================================================================
GraphicsView_Object::GraphicsView_Object( QGraphicsItem* theParent )
: QGraphicsItemGroup( theParent ),
  myPriority( 0 ),
  myIsOnTop( false ),
  myIsHighlighted( false ),
  myIsSelected( false ),
  myIsMoving( false ),
  myIsMovable( true )
{
}

//=======================================================================
// Name    : GraphicsView_Object
// Purpose : Destructor
//=======================================================================
GraphicsView_Object::~GraphicsView_Object()
{
  QListIterator<QGraphicsItem*> aChildIter( childItems() );
  while( aChildIter.hasNext() )
  {
    if( QGraphicsItem* aChild = aChildIter.next() )
    {
      removeFromGroup( aChild );
      if( QGraphicsScene* aScene = aChild->scene() )
        aScene->removeItem( aChild );
      delete aChild;
      aChild = 0;
    }
  }
}

//================================================================
// Function : addTo
// Purpose  : 
//================================================================
void GraphicsView_Object::addTo( GraphicsView_ViewPort* theViewPort )
{
  if( QGraphicsScene* aScene = theViewPort->scene() )
    aScene->addItem( this );
}

//================================================================
// Function : removeFrom
// Purpose  : 
//================================================================
void GraphicsView_Object::removeFrom( GraphicsView_ViewPort* theViewPort )
{
  if( QGraphicsScene* aScene = theViewPort->scene() )
    aScene->removeItem( this );
}

//================================================================
// Function : setName
// Purpose  : 
//================================================================
void GraphicsView_Object::setName( const QString& theName )
{
  myName = theName;
}

//================================================================
// Function : getRect
// Purpose  : 
//================================================================
QRectF GraphicsView_Object::getRect() const
{
  return sceneBoundingRect();
}

//================================================================
// Function : checkHighlight
// Purpose  : 
//================================================================
bool GraphicsView_Object::checkHighlight( double theX, double theY, QCursor& theCursor ) const
{
  return !getRect().isNull() && getRect().contains( theX, theY );
}

//================================================================
// Function : highlight
// Purpose  : 
//================================================================
bool GraphicsView_Object::highlight( double theX, double theY )
{
  QCursor aCursor;
  if( myIsHighlighted = isVisible() )
    myIsHighlighted = checkHighlight( theX, theY, aCursor );
  return myIsHighlighted;
}

//================================================================
// Function : unhighlight
// Purpose  : 
//================================================================
void GraphicsView_Object::unhighlight()
{
  myIsHighlighted = false;
}

//================================================================
// Function : select
// Purpose  : 
//================================================================
bool GraphicsView_Object::select( double theX, double theY, const QRectF& theRect )
{
  QCursor aCursor;
  if( myIsSelected = isVisible() )
  {
    if( !theRect.isNull() )
      myIsSelected = theRect.contains( getRect() );
    else
      myIsSelected = checkHighlight( theX, theY, aCursor );
  }
  return myIsSelected;
}

//================================================================
// Function : unselect
// Purpose  : 
//================================================================
void GraphicsView_Object::unselect()
{
  myIsSelected = false;
}

//================================================================
// Function : move
// Purpose  : 
//================================================================
void GraphicsView_Object::move( double theDX, double theDY, bool theIsAtOnce )
{
  if( !myIsMovable )
    return;

  if( theIsAtOnce )
  {
    finishMove( true );
    return;
  }

  myIsMoving = true;
  moveBy( theDX, theDY );
}

//================================================================
// Function : finishMove
// Purpose  : 
//================================================================
bool GraphicsView_Object::finishMove( bool theStatus )
{
  myIsMoving = false;
  if( theStatus )
    if( GraphicsView_Scene* aScene = dynamic_cast<GraphicsView_Scene*>( scene() ) )
      aScene->processRectChanged();
  return true;
}

//================================================================
// Function : setViewTransform
// Purpose  : 
//================================================================
void GraphicsView_Object::setViewTransform( const QTransform& theTransform )
{
  myViewTransform = theTransform;
}
