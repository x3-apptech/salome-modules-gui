// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

//  SALOME QxGraph : build Supervisor viewer into desktop
//
#include "QxGraph_Canvas.h"
#include "QxGraph_Def.h"
#include "QxGraph_Prs.h"

#include <SUIT_ResourceMgr.h>

#include <qobjectlist.h>
#include <QxGraph_CanvasView.h>

/*!
  Constructor
*/
QxGraph_Canvas::QxGraph_Canvas(SUIT_ResourceMgr* theMgr) :
  QCanvas()
{
  printf("Construct QxGraph_Canvas\n");

  resize(GRAPH_WIDTH, GRAPH_HEIGHT);
  setDoubleBuffering(true);

  QColor aColor = theMgr->colorValue( "QxGraph", "Background", DEF_BACK_COLOR );
  setBackgroundColor(aColor);
}

/*!
  Destructor
*/
QxGraph_Canvas::~QxGraph_Canvas()
{
  /*QObjectList* aNodeList = queryList("QObject");
  QObjectListIt aIt(*aNodeList);
  QObject* anObj;
  while ( (anObj = aIt.current()) != 0 ) {
    ++aIt;
    aNodeList->removeRef(anObj);
    delete anObj;
  }
  delete aNodeList;*/

  myPrsList.setAutoDelete(true);
  myPrsList.clear();
}

/*!
  Add view
*/
void QxGraph_Canvas::addView(QCanvasView* theView)
{
  myViews.append( theView );
  QCanvas::addView(theView);
  theView->setPaletteBackgroundColor(backgroundColor().light(120)); 
}

/*!
  Remove view
*/
void QxGraph_Canvas::removeView( QCanvasView* theView )
{
  myViews.remove( theView );
}

/*!
  Remove item
*/
void QxGraph_Canvas::removeItem( QCanvasItem* theItem )
{
  QValueList< QCanvasView* >::iterator anIter;
  for ( anIter = myViews.begin(); anIter != myViews.end(); ++anIter )
  {
    QxGraph_CanvasView* aView = dynamic_cast<QxGraph_CanvasView*>( *anIter );
    if ( aView )
      aView->itemRemoved( theItem );
  }

  QCanvas::removeItem( theItem );
}

/*!
  Get Prs object with index theIndex
*/
QxGraph_Prs* QxGraph_Canvas::getPrs(int theIndex)
{
  QxGraph_Prs* aRetPrs = 0;
  if ( theIndex >= 0 && theIndex < myPrsList.count() )
    aRetPrs = myPrsList.at(theIndex);
  return aRetPrs;
}
