//  SALOME QxGraph : build Supervisor viewer into desktop
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "QxGraph_Prs.h"

#include "QxGraph_Canvas.h"
#include "QxGraph_Def.h" // for debug only

#include "SUIT_Session.h" // for debug only

/*!
  Constructor
*/
QxGraph_Prs::QxGraph_Prs(QxGraph_Canvas* theCanvas):
  myCanvas(theCanvas),
  myDMode(0),
  needUpdate(true)
{
  myCanvas->addPrs(this);
}

/*!
  Destructor
*/
QxGraph_Prs::~QxGraph_Prs()
{
  for ( DMode2ItemList::iterator it1 = myDisplayMap.begin();
	it1 != myDisplayMap.end();
	it1++ )
  {
    for ( std::list<QCanvasItem*>::iterator it2 = (*it1).second.begin();
	  it2 != (*it1).second.end();
	  it2++ )
    {
      QCanvasItem* anItem = *it2;
      if ( anItem ) delete anItem;
    }
  }
      
  myDisplayMap.clear();
}

/*!
  Add item to display in the view with index theDMode
*/
void QxGraph_Prs::addItem(QCanvasItem* theItem, int theDMode)
{
  if ( theDMode == -1 ) // add item for the current display mode
    myDisplayMap[myDMode].push_back(theItem);
  else
    myDisplayMap[theDMode].push_back(theItem);
}

/*!
  Remove item from the view with index theDMode
*/
void QxGraph_Prs::removeItem(QCanvasItem* theItem, int theDMode)
{
  if ( theDMode == -1 ) // remove item from the current display mode
    myDisplayMap[myDMode].remove(theItem);
  else
    myDisplayMap[theDMode].remove(theItem);
}

/*! Adds all the items of this presentation for the current display mode
 *  to the canvas.
 */
void QxGraph_Prs::show()
{
  if ( isToUpdate() ) 
    update();

  for ( std::list<QCanvasItem*>::iterator it = myDisplayMap[myDMode].begin();
	it != myDisplayMap[myDMode].end();
	it++ )
  {
    QCanvasItem* anItem = *it;
    if ( anItem )
    {
      anItem->setCanvas( myCanvas );
      anItem->show();
    }
  }
}

/*! Removes all the items belonging to this presentation from the canvas.
 */
void QxGraph_Prs::hide()
{
  for ( DMode2ItemList::iterator it1 = myDisplayMap.begin();
	it1 != myDisplayMap.end();
	it1++ )
  {
    for ( std::list<QCanvasItem*>::iterator it2 = (*it1).second.begin();
	  it2 != (*it1).second.end();
	  it2++ )
    {
      QCanvasItem* anItem = *it2;
      if ( anItem )
      {
	anItem->setCanvas( 0 );
      }
    }
  }
}

/*! Prepare for full recomputation of the presentation
 */
void QxGraph_Prs::setToUpdate( const bool theFlag )
{
  needUpdate = theFlag;
}

/*! Re-fills the presentation with items.
 *  Base implementation just resets <needUpdate> flag.
 *  It should be called at the end by re-implementations.
 */
void QxGraph_Prs::update()
{
  setToUpdate( false );
}

/*!
  Add a QCanvasRectangle item for display mode DMode
*/
QCanvasItem* QxGraph_Prs::addRectangleItem(QRect theRect, int theDMode)
{
  QCanvasRectangle* aRectItem;
  if ( myCanvas )
  {
    QCanvasRectangle* aRectItem = new QCanvasRectangle(theRect, myCanvas);
    aRectItem->setZ(0);
    aRectItem->show();
    myCanvas->update();
    
    // test drawing features: brush, pen ...
    QBrush aBr(SUIT_Session::session()->resourceMgr()->colorValue( "QxGraph", "NodeBody", RECTANGLE_BODY ));
    aRectItem->setBrush(aBr);
  }
  addItem(aRectItem);
  return aRectItem;
}

/*!
  Add a QCanvasPolygon item for display mode DMode
*/
QCanvasItem* QxGraph_Prs::addPolygonItem(QPointArray thePA, int theDMode)
{
  QCanvasPolygon* aPolyItem;
  if ( myCanvas )
  {
    aPolyItem = new QCanvasPolygon(myCanvas);
    aPolyItem->setZ(0);
    aPolyItem->setPoints(thePA);
    aPolyItem->show();
    myCanvas->update();
    
    // test drawing features: brush, pen ...
    QBrush aBr(SUIT_Session::session()->resourceMgr()->colorValue( "QxGraph", "NodeBody", RECTANGLE_BODY ));
    aPolyItem->setBrush(aBr);
    QPen aPen(Qt::black,2);
    aPolyItem->setPen(aPen);
  }
  addItem(aPolyItem);
  return aPolyItem;
}

/*!
  Add a QCanvasLine item for display mode DMode
*/
QCanvasItem* QxGraph_Prs::addLineItem(QPoint theStart, QPoint theEnd, int theDMode)
{
  QCanvasLine* aLineItem;
  if ( myCanvas )
  {
    aLineItem = new QCanvasLine(myCanvas);
    aLineItem->setZ(0);
    aLineItem->setPoints(theStart.x(), theStart.y(), theEnd.x(), theEnd.y());
    aLineItem->show();
    myCanvas->update();
  
    // test drawing features: brush, pen ...
    QPen aPen(Qt::black,2);
    aLineItem->setPen(aPen);
  }
  addItem(aLineItem);
  return aLineItem;
}

/*!
  Add a QCanvasEllipse item for display mode DMode
*/
QCanvasItem* QxGraph_Prs::addEllipseItem(int theW, int theH, int theStartAngle, int theAngle, int theDMode)
{
  QCanvasEllipse* aEllipseItem;
  if ( myCanvas )
  {
    aEllipseItem = new QCanvasEllipse(theW, theH, theStartAngle, theAngle, myCanvas);
    aEllipseItem->setZ(0);
    aEllipseItem->show();
    myCanvas->update();
    
    // test drawing features: brush, pen ...
    QBrush aBr(SUIT_Session::session()->resourceMgr()->colorValue( "QxGraph", "NodeBody", RECTANGLE_BODY ));
    aEllipseItem->setBrush(aBr);
    QPen aPen(Qt::black,2);
    aEllipseItem->setPen(aPen);
  }
  addItem(aEllipseItem);
  return aEllipseItem;
}

/*!
  Add a QCanvasText item for display mode DMode
*/
QCanvasItem* QxGraph_Prs::addTextItem(QString theText, int theDMode)
{
  QCanvasText* aTextItem;
  if ( myCanvas )
  {
    aTextItem = new QCanvasText(theText, myCanvas);
    aTextItem->setZ(0);
    aTextItem->show();
    myCanvas->update();
    
    // test drawing features: font, color, text flags ...
    aTextItem->setColor(Qt::darkBlue);
    //aTextItem->setFont(QFont("Times"/*"Helvetica"*/, 14, QFont::Normal, true));
  }
  addItem(aTextItem);
  return aTextItem;
}
