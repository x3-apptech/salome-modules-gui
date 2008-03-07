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
// File:      QtxWorkstack.cxx
// Author:    Sergey TELKOV

#include "QtxWorkstack.h"

#include <qstyle.h>
#include <qimage.h>
#include <qaction.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qiconset.h>
#include <qpainter.h>
#include <qsplitter.h>
#include <qpopupmenu.h>
#include <qobjectlist.h>
#include <qpushbutton.h>
#include <qwidgetstack.h>
#include <qapplication.h>
#include <qinputdialog.h>
#include <qevent.h>
#include <qregexp.h>

#define DARK_COLOR_LIGHT      250

/*!
  Constructor
*/
QtxWorkstack::QtxWorkstack( QWidget* parent )
: QWidget( parent ),
myWin( 0 ),
myArea( 0 ),
myWorkWin( 0 ),
myWorkArea( 0 )
{
  myActionsMap.insert( SplitVertical,   new QAction( tr( "Split vertically" ),   0, this ) );
  myActionsMap.insert( SplitHorizontal, new QAction( tr( "Split horizontally" ), 0, this ) );
  myActionsMap.insert( Close,           new QAction( tr( "Close" ),       0, this ) );
  myActionsMap.insert( Rename,          new QAction( tr( "Rename" ),      0, this ) );

  connect( myActionsMap[SplitVertical], SIGNAL( activated() ), this, SLOT( splitVertical() ) );
  connect( myActionsMap[SplitHorizontal], SIGNAL( activated() ), this, SLOT( splitHorizontal() ) );
  connect( myActionsMap[Close], SIGNAL( activated() ), this, SLOT( onCloseWindow() ) );
  connect( myActionsMap[Rename], SIGNAL( activated() ), this, SLOT( onRename() ) );

  QVBoxLayout* base = new QVBoxLayout( this );
  mySplit = new QSplitter( this );
  mySplit->setChildrenCollapsible( false );
  base->addWidget( mySplit );
}

/*!
  Destructor
*/
QtxWorkstack::~QtxWorkstack()
{
}

/*!
  \return list of all widgets in all areas
*/
QWidgetList QtxWorkstack::windowList() const
{
  QPtrList<QtxWorkstackArea> lst;
  areas( mySplit, lst, true );

  QWidgetList widList;
  for ( QPtrListIterator<QtxWorkstackArea> it( lst ); it.current(); ++it )
  {
    QWidgetList wids = it.current()->widgetList();
    for ( QWidgetListIt itr( wids ); itr.current(); ++itr )
      widList.append( itr.current() );
  }

  return widList;
}

/*!
  \return list of all widgets in active area
*/
QWidgetList QtxWorkstack::splitWindowList() const
{
  return myArea ? myArea->widgetList() : QWidgetList();
}

/*!
  \return active widget
*/
QWidget* QtxWorkstack::activeWindow() const
{
  return myWin;
}

/*!
  Splits widgets
  \param o - orientation (Qt::Orientation)
*/
void QtxWorkstack::split( const int o )
{
  QtxWorkstackArea* area = myWorkArea;
  if ( !area )
    area = activeArea();
  if ( !area )
    return;

  if ( area->widgetList().count() < 2 )
    return;

  QWidget* curWid = area->activeWidget();
  if ( !curWid )
    return;

  QSplitter* s = splitter( area );
  QPtrList<QtxWorkstackArea> areaList;
  areas( s, areaList );

  QPtrList<QSplitter> splitList;
  splitters( s, splitList );

  QSplitter* trg = 0;
  if ( areaList.count() + splitList.count() < 2 || s->orientation() == o )
    trg = s;

  if ( !trg )
    trg = wrapSplitter( area );

  if ( !trg )
    return;

  trg->setOrientation( (Orientation)o );

  QtxWorkstackArea* newArea = createArea( 0 );
  insertWidget( newArea, trg, area );

  area->removeWidget( curWid );
  newArea->insertWidget( curWid );

  distributeSpace( trg );

  curWid->show();
  curWid->setFocus();
}

/*!
 \brief Split workarea of the given widget on two parts.
 \param wid  - widget, belonging to this workstack
 \param o    - orientation of splitting (Qt::Horizontal or Qt::Vertical)
 \param type - type of splitting, see <VAR>SplitType</VAR> enumeration
*/
void QtxWorkstack::Split (QWidget* wid, const Qt::Orientation o, const SplitType type)
{
  if (!wid) return;

  // find area of the given widget
  QtxWorkstackArea* area = NULL;
  QPtrList<QtxWorkstackArea> allAreas;
  areas(mySplit, allAreas, true);

  QPtrListIterator<QtxWorkstackArea> it (allAreas);
  for (; it.current() && !area; ++it) {
    if (it.current()->contains(wid))
      area = it.current();
  }
  if (!area) return;

  QWidgetList wids = area->widgetList();
  if (wids.count() < 2)
    return;

  QSplitter* s = splitter(area);
  QPtrList<QtxWorkstackArea> areaList;
  areas(s, areaList);

  QPtrList<QSplitter> splitList;
  splitters(s, splitList);

  QSplitter* trg = 0;
  if (areaList.count() + splitList.count() < 2 || s->orientation() == o)
    trg = s;

  if (!trg) trg = wrapSplitter(area);
  if (!trg) return;

  trg->setOrientation(o);

  QtxWorkstackArea* newArea = createArea(0);
  insertWidget(newArea, trg, area);

  switch (type) {
  case SPLIT_STAY:
    {
      QWidgetListIt itr (wids);
      for (; itr.current(); ++itr)
      {
        QWidget* wid_i = itr.current();
        if (wid_i != wid) {
          area->removeWidget(wid_i);
          newArea->insertWidget(wid_i);
        }
      }
    }
    break;
  case SPLIT_AT:
    {
      QWidgetListIt itr (wids);
      for (; itr.current() && itr.current() != wid; ++itr) {
      }
      for (; itr.current(); ++itr) {
        area->removeWidget(itr.current());
        newArea->insertWidget(itr.current());
      }
    }
    break;
  case SPLIT_MOVE:
    area->removeWidget(wid);
    newArea->insertWidget(wid);
    break;
  }

  distributeSpace(trg);
}

/*!
 \brief Put given widget on top of its workarea
 \param wid - widget, belonging to this workstack
*/
/*
void QtxWorkstack::OnTop (QWidget* wid)
{
  if ( !wid )
    return;

  // find area of the given widget
  QtxWorkstackArea* area = 0;
  QPtrList<QtxWorkstackArea> allAreas;
  areas( mySplit, allAreas, true );
  for ( QPtrListIterator<QtxWorkstackArea> it( allAreas ); it.current() && !area; ++it )
  {
    if ( it.current()->contains( wid ) )
      area = it.current();
  }

  if ( area )
    area->setActiveWidget( wid );
}
*/

/*!
 \brief Move widget(s) from source workarea into target workarea
        or just reorder widgets inside one workarea.
 \param wid1 - widget from target workarea
 \param wid2 - widget from source workarea
 \param all  - if this parameter is TRUE, all widgets from source workarea will
               be moved into the target one, else only the \a wid2 will be moved

 Move \a wid2 in target workarea. Put it right after \a wid1.
 If value of boolean argument is TRUE, all widgets from source workarea
 will be moved together with \a wid2, source workarea will be deleted.
 If \a wid1 and \a wid2 belongs to one workarea, simple reordering will take place.
*/
void QtxWorkstack::Attract ( QWidget* wid1, QWidget* wid2, const bool all )
{
  if ( !wid1 || !wid2 )
    return;

  // find area of the widgets
  QtxWorkstackArea *area1 = NULL, *area2 = NULL;
  QPtrList<QtxWorkstackArea> allAreas;
  areas(mySplit, allAreas, true);
  QPtrListIterator<QtxWorkstackArea> it (allAreas);
  for (; it.current() && !(area1 && area2); ++it) {
    if (it.current()->contains(wid1))
      area1 = it.current();
    if (it.current()->contains(wid2))
      area2 = it.current();
  }
  if (!area1 || !area2) return;

  QWidget* curWid = area1->activeWidget();
  if (!curWid) return;

  if (area1 == area2) {
    if (all) {
      // Set wid1 at first position, wid2 at second
      area1->insertWidget(wid1);
      area1->insertWidget(wid2, 1);
    } else {
      // Set wid2 right after wid1
      area1->removeWidget(wid2);
      int wid1_ind = 0;
      QWidgetList wids1 = area1->widgetList();
      QWidgetListIt itr1 (wids1);
      for (; itr1.current() && itr1.current() != wid1; ++itr1, ++wid1_ind);
      area1->insertWidget(wid2, wid1_ind + 1);
    }
  } else {
    int wid1_ind = 0;
    QWidgetList wids1 = area1->widgetList();
    QWidgetListIt itr1 (wids1);
    for (; itr1.current() && itr1.current() != wid1; ++itr1, ++wid1_ind);

    if (all) {
      // Set wid2 right after wid1, other widgets from area2 right after wid2
      QWidgetList wids2 = area2->widgetList();
      QWidgetListIt itr2 (wids2);
      for (int ind = wid1_ind + 1; itr2.current(); ++itr2, ++ind)
      {
        area2->removeWidget(itr2.current());
        if (itr2.current() == wid2) {
          area1->insertWidget(itr2.current(), wid1_ind + 1);
        } else {
          area1->insertWidget(itr2.current(), ind);
        }
      }
    } else {
      // Set wid2 right after wid1
      area2->removeWidget(wid2);
      area1->insertWidget(wid2, wid1_ind + 1);
    }
  }

  area1->setActiveWidget( curWid );
}

static void setSizes (QIntList& szList, const int item_ind,
                      const int new_near, const int new_this, const int new_farr)
{
  // set size to all items before an item # <item_ind>
  int cur_pos = 0;
  QIntList::iterator its = szList.begin();
  for (; its != szList.end() && cur_pos < item_ind; ++its, ++cur_pos) {
    *its = new_near;
  }
  if (its == szList.end()) return;
  // set size to item # <item_ind>
  *its = new_this;
  ++its;
  // set size to all items after an item # <item_ind>
  for (; its != szList.end(); ++its) {
    *its = new_farr;
  }
}

/*!
* \brief Set position of the widget relatively its splitter.
* \param wid - widget to set position of
* \param pos - position relatively splitter. Value in range [0..1].
*
* Orientation of positioning will correspond to the splitter orientation.
*/
void QtxWorkstack::SetRelativePositionInSplitter( QWidget* wid, const double position )
{
  if ( position < 0.0 || 1.0 < position)
    return;

  if ( !wid )
    return;

  // find area of the given widget
  QtxWorkstackArea* area = NULL;
  QPtrList<QtxWorkstackArea> allAreas;
  areas(mySplit, allAreas, true);
  for ( QPtrListIterator<QtxWorkstackArea> it( allAreas );
       it.current() && !area;
       ++it )
  {
    if (it.current()->contains(wid))
      area = it.current();
  }

  if ( !area )
    return;

  QSplitter* split = splitter( area );
  if ( !split )
    return;

  // find index of the area in its splitter
  int item_ind = -1;
  bool isFound = false;
  const QObjectList* was = split->children();
  for (QObjectListIt ito (*was); ito.current() && !isFound; ++ito, ++item_ind)
  {
    if (ito.current() == area)
      isFound = true;
  }
  if (!isFound || item_ind == 0)
    return;

  QIntList szList = split->sizes();
  int splitter_size = (split->orientation() == Horizontal ?
                       split->width() : split->height());
  int nb = szList.count();

  int new_prev = int(splitter_size * position / item_ind);
  int new_next  = int(splitter_size * (1.0 - position) / (nb - item_ind));
  setSizes (szList, item_ind, new_prev, new_next, new_next);
  split->setSizes(szList);
}

/*!
* \brief Set position of the widget relatively the entire workstack.
* \param wid - widget to set position of
* \param o   - orientation of positioning (Qt::Horizontal or Qt::Vertical).
*              If o = Qt::Horizontal, horizontal position of \a wid will be changed.
*              If o = Qt::Vertical, vertical position of \a wid will be changed.
* \param pos - position relatively workstack. Value in range [0..1].
*/
void QtxWorkstack::SetRelativePosition( QWidget* wid, const Qt::Orientation o,
                                        const double position )
{
  if ( position < 0.0 || 1.0 < position )
    return;

  if ( !wid )
    return;

  int splitter_size = o == Horizontal ? mySplit->width() : mySplit->height();
  int need_pos = int( position * splitter_size );
  int splitter_pos = 0;

  if ( setPosition( wid, mySplit, o, need_pos, splitter_pos ) != 0 )
  {
    // impossible to set required position
  }
}

/*!
* \brief Sets the action's accelerator key to accel. 
* \param id - the key of the action in the actions map.
* \param accel - action's accelerator key.
*/
void QtxWorkstack::setAccel( const int id, const int accel )
{
  if ( !myActionsMap.contains( id ) )
    return;

  myActionsMap[id]->setAccel( accel );
}

/*!
* \brief Returns the action's accelerator key.
* \param id - the key of the action in the actions map.
* \retval int  - action's accelerator key.
*/
int QtxWorkstack::accel( const int id ) const
{
  int res = 0;
  if ( myActionsMap.contains( id ) )
    res = myActionsMap[id]->accel();
  return res;
}

static int positionSimple (QIntList& szList, const int nb, const int splitter_size,
                           const int item_ind, const int item_rel_pos,
                           const int need_pos, const int splitter_pos)
{
  if (item_ind == 0) { // cannot move in this splitter
    return (need_pos - splitter_pos);
  }

  int delta = 0;
  int new_prev = 0;
  int new_this = szList[item_ind];
  int new_next = 0;

  bool isToCheck = false;

  if (need_pos < splitter_pos) {
    // Set size of all previous workareas to zero <--
    if (item_ind == nb - 1) {
      // item iz last in the splitter, it will occupy all the splitter
      new_this = splitter_size;
    } else {
      // recompute size of next items in splitter
      new_next = (splitter_size - new_this) / (nb - item_ind - 1);
    }
    delta = need_pos - splitter_pos;

  } else if (need_pos > (splitter_pos + splitter_size)) {
    // Set size of all next workareas to zero -->
    // recompute size of previous items in splitter
    new_this = 0;
    new_prev = (splitter_size - new_this) / item_ind;
    delta = need_pos - (splitter_pos + splitter_size - new_this);

  } else { // required position lays inside this splitter
    // Move workarea inside splitter into required position <->
    int new_item_rel_pos = need_pos - splitter_pos;
    new_prev = new_item_rel_pos / item_ind;
    if (need_pos < (splitter_pos + item_rel_pos)) {
      // Make previous workareas smaller, next - bigger
      // No problem to keep old size of the widget
    } else {
      // Make previous workareas bigger, next - smaller
      if (new_this > splitter_size - new_item_rel_pos) {
        new_this = splitter_size - new_item_rel_pos;
      }
      // jfa to do: in this case fixed size of next widgets could prevent right resizing
      isToCheck = true;
    }
    if (item_ind == nb - 1) {
      new_this = splitter_size - new_item_rel_pos;
    } else {
      new_next = (splitter_size - new_item_rel_pos - new_this) / (nb - item_ind - 1);
    }
    delta = 0;
  }

  setSizes (szList, item_ind, new_prev, new_this, new_next);
  return delta;
}

/*!
* \brief Set position of given widget.
* \param wid          - widget to be moved
* \param split        - currently processed splitter (goes from more common
*                       to more particular splitter in recursion calls)
* \param o            - orientation of positioning
* \param need_pos     - required position of the given widget in pixels
*                       (from top/left side of workstack area)
* \param splitter_pos - position of the splitter \a split
*                       (from top/left side of workstack area)
* \retval int - returns difference between a required and a distinguished position.
* 
* Internal method. Recursively calls itself.
* Is called from <VAR>SetRelativePosition</VAR> public method.
*/
int QtxWorkstack::setPosition( QWidget* wid, QSplitter* split, const Qt::Orientation o,
                               const int need_pos, const int splitter_pos )
{
  if ( !wid || !split )
    return need_pos - splitter_pos;

  // Find corresponding sub-splitter.
  // Find also index of appropriate item in current splitter.
  int cur_ind = 0, item_ind = 0;
  bool isBottom = false, isFound = false;
  QSplitter* sub_split = NULL;
  const QObjectList* objs = split->children();
  if ( objs )
  {
    for (QObjectListIt it (*objs); it.current() && !isFound; ++it)
    {
      if (it.current()->inherits( "QtxWorkstackArea")) {
        if (((QtxWorkstackArea*)it.current())->contains(wid)) {
          item_ind = cur_ind;
          isBottom = true;
          isFound = true;
        }
        cur_ind++;
      } else if (it.current()->inherits("QSplitter")) {
        QPtrList<QtxWorkstackArea> areaList;
        areas((QSplitter*)it.current(), areaList, true);
        for (QPtrListIterator<QtxWorkstackArea> ita (areaList);
             ita.current() && !isFound;
             ++ita)
        {
          if (ita.current()->contains(wid)) {
            item_ind = cur_ind;
            isFound = true;
            sub_split = (QSplitter*)it.current();
          }
        }
        cur_ind++;
      }
    }
  }
  if (!isFound)
    return (need_pos - splitter_pos);

  if (split->orientation() == o) {
    // Find coordinates of near and far sides of the appropriate item relatively current splitter
    int splitter_size = (o == Horizontal ? split->width() : split->height());
    QIntList szList = split->sizes();
    int nb = szList.count();
    int item_rel_pos = 0; // position of near side of item relatively this splitter
    for (int i = 0; i < item_ind; i++) {
      item_rel_pos += szList[i];
    }
    int item_size = szList[item_ind]; // size of item
    int item_pos = splitter_pos + item_rel_pos;

    // Resize splitter items to complete the conditions
    if (isBottom) {
      // I. Bottom of splitters stack reached

      int delta = positionSimple(szList, nb, splitter_size, item_ind, item_rel_pos, need_pos, splitter_pos);
      split->setSizes(szList);
      // Recompute delta, as some windows can reject given size
      int new_item_rel_pos = 0;
      QIntList szList1 = split->sizes();
      for (int i = 0; i < item_ind; i++) {
        new_item_rel_pos += szList1[i];
      }
      delta = need_pos - (splitter_pos + new_item_rel_pos);
      return delta;

    } else {
      // II. Bottom of splitters stack is not yet reached

      if (item_ind == 0) { // cannot move in this splitter
        // Process in sub-splitter
        return setPosition(wid, sub_split, o, need_pos, splitter_pos);
      }

      int new_prev = 0;
      int new_this = szList[item_ind];
      int new_next = 0;

      if (need_pos < splitter_pos) {
        // Set size of all previous workareas to zero <--
        if (item_ind == nb - 1) {
          new_this = splitter_size;
        } else {
          new_next = (splitter_size - new_this) / (nb - item_ind - 1);
        }
        setSizes (szList, item_ind, new_prev, new_this, new_next);
        split->setSizes(szList);
        // Recompute splitter_pos, as some windows can reject given size
        int new_item_rel_pos = 0;
        QIntList szList1 = split->sizes();
        for (int i = 0; i < item_ind; i++) {
          new_item_rel_pos += szList1[i];
        }
        // Process in sub-splitter
        return setPosition(wid, sub_split, o, need_pos, splitter_pos + new_item_rel_pos);
      } else if (need_pos > (splitter_pos + splitter_size)) {
        // Set size of all next workareas to zero -->
        new_prev = (splitter_size - new_this) / item_ind;
        setSizes (szList, item_ind, new_prev, new_this, new_next);
        split->setSizes(szList);
        // Recompute splitter_pos, as some windows can reject given size
        int new_item_rel_pos = 0;
        QIntList szList1 = split->sizes();
        for (int i = 0; i < item_ind; i++) {
          new_item_rel_pos += szList1[i];
        }
        // Process in sub-splitter
        return setPosition(wid, sub_split, o, need_pos, splitter_pos + new_item_rel_pos);
      } else {
        // Set appropriate size of all previous/next items <->
        int new_item_rel_pos = item_rel_pos;
        if (need_pos < item_pos || (item_pos + item_size) < need_pos) {
          // Move item inside splitter into required position <->
          int new_this = szList[item_ind];
          int new_next = 0;
          new_item_rel_pos = need_pos - splitter_pos;
          if ((item_pos + item_size) < need_pos) {
            //new_item_rel_pos = need_pos - (item_pos + item_size);
            new_item_rel_pos = item_rel_pos + (need_pos - (item_pos + item_size));
          }
          int new_prev = new_item_rel_pos / item_ind;
          if (need_pos < (splitter_pos + item_rel_pos)) {
            // Make previous workareas smaller, next - bigger
            // No problem to keep old size of the widget
          } else {
            // Make previous workareas bigger, next - smaller
            if (new_this > splitter_size - new_item_rel_pos) {
              new_this = splitter_size - new_item_rel_pos;
            }
          }
          if (item_ind == nb - 1) {
            new_this = splitter_size - new_item_rel_pos;
          } else {
            new_next = (splitter_size - new_item_rel_pos - new_this) / (nb - item_ind - 1);
          }
          setSizes (szList, item_ind, new_prev, new_this, new_next);
          split->setSizes(szList);
          // Recompute new_item_rel_pos, as some windows can reject given size
          new_item_rel_pos = 0;
          QIntList szList1 = split->sizes();
          for (int i = 0; i < item_ind; i++) {
            new_item_rel_pos += szList1[i];
          }
        } else {
          // Do nothing
        }
        // Process in sub-splitter
        int add_pos = setPosition(wid, sub_split, o, need_pos, splitter_pos + new_item_rel_pos);
        if (add_pos == 0)
          return 0;

        // this can be if corresponding workarea is first in sub-splitter
        // or sub-splitter has another orientation

        // Resize ones again to reach precize position <->
        int need_pos_1 = splitter_pos + new_item_rel_pos + add_pos;

        // Move workarea inside splitter into required position <->
        int delta_1 = positionSimple(szList, nb, splitter_size, item_ind,
                                     new_item_rel_pos, need_pos_1, splitter_pos);
        split->setSizes(szList);
        // Recompute new_item_rel_pos, as some windows can reject given size
        new_item_rel_pos = 0;
        QIntList szList1 = split->sizes();
        for (int i = 0; i < item_ind; i++) {
          new_item_rel_pos += szList1[i];
        }
        delta_1 = need_pos_1 - (splitter_pos + new_item_rel_pos);
        return delta_1;
      }
    }
  } else {
    return setPosition(wid, sub_split, o, need_pos, splitter_pos);
  }

  return 0;
}

/*!
  Redistributes space among widgets equally
*/
void QtxWorkstack::distributeSpace( QSplitter* split ) const
{
  if ( !split )
    return;

  QIntList szList = split->sizes();
  int size = ( split->orientation() == Horizontal ?
               split->width() : split->height() ) / szList.count();
  for ( QIntList::iterator it = szList.begin(); it != szList.end(); ++it )
    *it = size;
  split->setSizes( szList );
}

/*!
  Splits widgets vertically
*/
void QtxWorkstack::splitVertical()
{
  split( Qt::Horizontal );
}

/*!
  Splits widgets horizontally
*/
void QtxWorkstack::splitHorizontal()
{
  split( Qt::Vertical );
}

/*!
  SLOT: called if action "Rename" is activated, changes caption of widget
*/
void QtxWorkstack::onRename()
{
  if ( !myWorkWin )
    return;

  bool ok = false;
  QString newName = QInputDialog::getText( tr( "Rename" ), tr( "Enter new name:" ), QLineEdit::Normal,
                                           myWorkWin->caption(), &ok, topLevelWidget() );
  if ( ok && !newName.isEmpty() )
    myWorkWin->setCaption( newName );
}

/*!
  Wraps area into new splitter
  \return new splitter
*/
QSplitter* QtxWorkstack::wrapSplitter( QtxWorkstackArea* area )
{
  if ( !area )
    return 0;

  QSplitter* pSplit = splitter( area );
  if ( !pSplit )
    return 0;

  bool upd = pSplit->isUpdatesEnabled();
  pSplit->setUpdatesEnabled( false );

  QIntList szList = pSplit->sizes();

  QSplitter* wrap = new QSplitter( 0 );
#if defined QT_VERSION && QT_VERSION >= 0x30200
  wrap->setChildrenCollapsible( false );
#endif
  insertWidget( wrap, pSplit, area );
  area->reparent( wrap, QPoint( 0, 0 ), true );

  pSplit->setSizes( szList );

  pSplit->setUpdatesEnabled( upd );

  return wrap;
}

/*!
  Reparenst and adds widget
  \param wid - widget
  \param pWid - parent widget
  \param after - after widget
*/
void QtxWorkstack::insertWidget( QWidget* wid, QWidget* pWid, QWidget* after )
{
  if ( !wid || !pWid )
    return;

  QWidgetList moveList;
  const QObjectList* lst = pWid->children();
  if ( lst )
  {
    bool found = false;
    for ( QObjectListIt it( *lst ); it.current(); ++it )
    {
      if ( found && ( it.current()->inherits( "QSplitter" ) ||
                      it.current()->inherits( "QtxWorkstackArea" ) ) )
        moveList.append( (QWidget*)it.current() );
      if ( it.current() == after )
        found = true;
    }
  }

  QMap<QWidget*, bool> map;
  for ( QWidgetListIt it( moveList ); it.current(); ++it )
  {
    map.insert( it.current(), it.current()->isVisibleTo( it.current()->parentWidget() ) );
    it.current()->reparent( 0, QPoint( 0, 0 ), false );
  }

  wid->reparent( pWid, QPoint( 0, 0 ), true );

  for ( QWidgetListIt itr( moveList ); itr.current(); ++itr )
    itr.current()->reparent( pWid, QPoint( 0, 0 ), map.contains( itr.current() ) ? map[itr.current()] : false );
}

/*!
* \brief Closes the active window.
*/
void QtxWorkstack::onCloseWindow()
{
  if ( myWorkWin )
    myWorkWin->close();
  else if( activeWindow() )
    activeWindow()->close();
}

/*!
  SLOT: called on area is destroyed
  Sets focus to neighbour area
*/
void QtxWorkstack::onDestroyed( QObject* obj )
{
  QtxWorkstackArea* area = (QtxWorkstackArea*)obj;

  if ( area == myArea )
    myArea = 0;

  if ( !myArea )
  {
    QtxWorkstackArea* cur = neighbourArea( area );
    if ( cur )
      cur->setFocus();
  }

  QApplication::postEvent( this, new QCustomEvent( QEvent::User ) );
}

/*!
  SLOT: called on window activating
*/
void QtxWorkstack::onWindowActivated( QWidget* wid )
{
  const QObject* obj = sender();
  if ( !obj->inherits( "QtxWorkstackArea" ) )
    return;

  setActiveArea( (QtxWorkstackArea*)obj );
}

/*!
  SLOT: called on window deactivating
*/
void QtxWorkstack::onDeactivated( QtxWorkstackArea* area )
{
  if ( myArea != area )
    return;

  QPtrList<QtxWorkstackArea> lst;
  areas( mySplit, lst, true );

  int idx = lst.find( area );
  if ( idx == -1 )
    return;

  myWin = 0;
  myArea = 0;

  QtxWorkstackArea* newArea = neighbourArea( area );
  if ( newArea && newArea->activeWidget() )
    newArea->activeWidget()->setFocus();

  QApplication::postEvent( this, new QCustomEvent( QEvent::User ) );
}

/*!
  Creates and shows popup menu for area
  \param w - area
  \param p - popup position
*/
void QtxWorkstack::onContextMenuRequested( QWidget* w, QPoint p )
{
  QtxWorkstackArea* anArea = ::qt_cast<QtxWorkstackArea*>( (QObject*)sender() );
  if ( !anArea )
    anArea = activeArea();

  if ( !anArea )
    return;

  QWidgetList lst = anArea->widgetList();
  if ( lst.isEmpty() )
    return;

  myWorkWin = w;
  myWorkArea = anArea;

  QPopupMenu* pm = new QPopupMenu();
  
  if ( lst.count() > 1 )
  {
    myActionsMap[SplitVertical]->addTo( pm );
    myActionsMap[SplitHorizontal]->addTo( pm );
    pm->insertSeparator();
  }

  if ( w )
  {
    myActionsMap[Close]->addTo( pm );
    myActionsMap[Rename]->addTo( pm );
  }

  Qtx::simplifySeparators( pm );

  if ( pm->count() )
    pm->exec( p );

  delete pm;

  myWorkWin = 0;
  myWorkArea = 0;
}

/*!
  Custom child event handler, inserts widget to active or current area
*/
void QtxWorkstack::childEvent( QChildEvent* e )
{
  if ( e->inserted() && e->child()->isWidgetType() )
  {
	  QWidget* w = (QWidget*)e->child();
	  if ( w && w != mySplit )
    {
      targetArea()->insertWidget( w );
      return;
    }
  }
  QWidget::childEvent( e );
}

/*!
  Handler of custom events
*/
void QtxWorkstack::customEvent( QCustomEvent* e )
{
  updateState();
}

/*!
  \return splitter corresponding to area
  \param area
*/
QSplitter* QtxWorkstack::splitter( QtxWorkstackArea* area ) const
{
  if ( !area )
    return 0;

  QSplitter* split = 0;

  QWidget* wid = area->parentWidget();
  if ( wid && wid->inherits( "QSplitter" ) )
    split = (QSplitter*)wid;

  return split;
}

/*!
  Fills list with children splitters
  \param split - parent splitter
  \param splitList - list to be filled with
  \param rec - recursive search of children
*/
void QtxWorkstack::splitters( QSplitter* split, QPtrList<QSplitter>& splitList, const bool rec ) const
{
  if ( !split )
    return;

  const QObjectList* objs = split->children();
  if ( objs )
  {
    for ( QObjectListIt it( *objs ); it.current(); ++it )
    {
      if ( rec )
        splitters( (QSplitter*)it.current(), splitList, rec );
      if ( it.current()->inherits( "QSplitter" ) )
        splitList.append( (QSplitter*)it.current() );
    }
  }
}

/*!
  Fills list with children areas
  \param split - parent splitter
  \param areaList - list to be filled with
  \param rec - recursive search of children
*/
void QtxWorkstack::areas( QSplitter* split, QPtrList<QtxWorkstackArea>& areaList, const bool rec ) const
{
  if ( !split )
    return;

  const QObjectList* objs = split->children();
  if ( objs )
  {
    for ( QObjectListIt it( *objs ); it.current(); ++it )
    {
      if ( it.current()->inherits( "QtxWorkstackArea" ) )
        areaList.append( (QtxWorkstackArea*)it.current() );
      else if ( rec && it.current()->inherits( "QSplitter" ) )
        areas( (QSplitter*)it.current(), areaList, rec );
    }
  }
}

/*!
  \return active area
*/
QtxWorkstackArea* QtxWorkstack::activeArea() const
{
  return myArea;
}

/*!
  \return active area or current area or create new area of there is no one
*/
QtxWorkstackArea* QtxWorkstack::targetArea()
{
  QtxWorkstackArea* area = activeArea();
  if ( !area )
    area = currentArea();
  if ( !area )
  {
    QPtrList<QtxWorkstackArea> lst;
    areas( mySplit, lst );
    if ( !lst.isEmpty() )
      area = lst.first();
  }

  if ( !area )
    area = createArea( mySplit );

  return area;
}

/*!
  \return current area (that has focus)
*/
QtxWorkstackArea* QtxWorkstack::currentArea() const
{
  QtxWorkstackArea* area = 0;
  QWidget* wid = focusWidget();
  while ( wid && !area )
  {
    if ( wid->inherits( "QtxWorkstackArea" ) )
      area = (QtxWorkstackArea*)wid;
    wid = wid->parentWidget();
  }

  return area;
}

/*!
  Creates new area
*/
QtxWorkstackArea* QtxWorkstack::createArea( QWidget* parent ) const
{
  QtxWorkstackArea* area = new QtxWorkstackArea( parent );

  connect( area, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );
  connect( area, SIGNAL( activated( QWidget* ) ), this, SLOT( onWindowActivated( QWidget* ) ) );
  connect( area, SIGNAL( contextMenuRequested( QWidget*, QPoint ) ),
	   this, SLOT( onContextMenuRequested( QWidget*, QPoint ) ) );
  connect( area, SIGNAL( deactivated( QtxWorkstackArea* ) ), this, SLOT( onDeactivated( QtxWorkstackArea* ) ) );

  return area;
}

/*!
  Sets area as active
  \param area
*/
void QtxWorkstack::setActiveArea( QtxWorkstackArea* area )
{
  QWidget* oldCur = myWin;

  QtxWorkstackArea* oldArea = myArea;

  myArea = area;

  if ( myArea != oldArea )
  {
    if ( oldArea )
      oldArea->updateActiveState();
    if ( myArea )
      myArea->updateActiveState();
  }

  if ( myArea )
    myWin = myArea->activeWidget();

  if ( myWin && oldCur != myWin )
    emit windowActivated( myWin );
}

/*!
  \return neighbour area
  \param area - area to search neighbour
*/
QtxWorkstackArea* QtxWorkstack::neighbourArea( QtxWorkstackArea* area ) const
{
  QPtrList<QtxWorkstackArea> lst;
  areas( mySplit, lst, true );
  int pos = lst.find( area );
  if ( pos < 0 )
    return 0;

  QtxWorkstackArea* na = 0;
  for ( int i = pos - 1; i >= 0 && !na; i-- )
  {
    if ( !lst.at( i )->isEmpty() )
      na = lst.at( i );
  }

  for ( int j = pos + 1; j < (int)lst.count() && !na; j++ )
  {
    if ( !lst.at( j )->isEmpty() )
        na = lst.at( j );
  }
  return na;
}

/*!
  \return area covering point
  \param p - point
*/
QtxWorkstackArea* QtxWorkstack::areaAt( const QPoint& p ) const
{
  QtxWorkstackArea* area = 0;
  QPtrList<QtxWorkstackArea> lst;
  areas( mySplit, lst, true );
  for ( QPtrListIterator<QtxWorkstackArea> it( lst ); it.current() && !area; ++it )
  {
    QtxWorkstackArea* cur = it.current();
    QRect r = cur->geometry();
    if ( cur->parentWidget() )
      r = QRect( cur->parentWidget()->mapToGlobal( r.topLeft() ), r.size() );
    if ( r.contains( p ) )
      area = cur;
  }
  return area;
}

/*!
  Update
*/
void QtxWorkstack::updateState()
{
  updateState( mySplit );
}

/*!
  Update splitters
*/
void QtxWorkstack::updateState( QSplitter* split )
{
  QPtrList<QSplitter> recList;
  splitters( split, recList, false );
  for ( QPtrListIterator<QSplitter> itr( recList ); itr.current(); ++itr )
    updateState( itr.current() );

  QPtrList<QSplitter> splitList;
  splitters( split, splitList, false );

  QPtrList<QtxWorkstackArea> areaList;
  areas( split, areaList, false );

  bool vis = false;
  for ( QPtrListIterator<QtxWorkstackArea> it( areaList ); it.current(); ++it )
  {
    if ( it.current()->isEmpty() )
      it.current()->hide();
    else
    {
      it.current()->show();
      vis = true;
    }
  }

  if ( split == mySplit )
    return;

  for ( QPtrListIterator<QSplitter> iter( splitList ); iter.current() && !vis; ++iter )
    vis = iter.current()->isVisibleTo( iter.current()->parentWidget() );

  if ( areaList.isEmpty() && splitList.isEmpty() )
    delete split;
  else if ( vis )
    split->show();
  else
    split->hide();
}

/*!
  Gets splitter info for debug
  \param split - splitter
  \param info - string to be filled with info
*/
void QtxWorkstack::splitterInfo( QSplitter* split, QString& info ) const
{
  if ( !split )
    return;

  const QObjectList* objs = split->children();
  if ( objs )
  {
    // make up a sizes string: integer values are separated by ':' char
    QValueList<int> sizes = split->sizes();
    QString sizesStr;
    for ( QValueList<int>::Iterator sIt = sizes.begin(); sIt != sizes.end(); ++sIt ) {
      if ( *sIt > 1 ) // size 1 pixel usually means empty Workstack area, which will NOT be re-created,
	sizesStr += QString( ":%1" ).arg( *sIt );  // so we don't need to store its size
    }
    if ( !sizesStr.isEmpty() ) // cut the first ':'
      sizesStr = sizesStr.right( sizesStr.length()-1 );    

    // count all QSplitter-s and QtxWorkstackArea-s
    //    int nChilds( 0 );
    //    QObjectListIt it( *objs );
    //    for ( ; it.current(); ++it )
    //    {
    //      if ( it.current()->inherits( "QSplitter" ) ||
    //           it.current()->inherits( "QtxWorkstackArea" ) )
    //	nChilds++;
    //    }
      
    info += QString( "(splitter orientation=%1 sizes=%3 " ).arg( split->orientation() ).arg( sizesStr );
    
    for ( QObjectListIt it( *objs ); it.current(); ++it )
    {
      if ( it.current()->inherits( "QSplitter" ) )
	splitterInfo( (QSplitter*)it.current(), info );
      else if ( it.current()->inherits( "QtxWorkstackArea" ) ) {
	QtxWorkstackArea* area = (QtxWorkstackArea*)it.current();
	if ( area->isEmpty() )
	  continue;
	info += QString( "(views active='%1'" ).arg( area->activeWidget()->name() );
	QWidgetList views = area->widgetList();
	for ( QWidgetListIt wIt( views ); wIt.current(); ++wIt )
	  info += QString( " '%1'" ).arg( wIt.current()->name() );
	info += ')';
      }
    }
  }
  info += ')';
}


//Cuts starting '(' symbol and ending '(' symbol
void cutBrackets( QString& parameters )
{
  QChar c1 = parameters[0];
  QChar c2 = parameters[int(parameters.length()-1)];
  if ( !parameters.isEmpty() && c1 == '(' && c2 == ')' )
    parameters = parameters.mid( 1, parameters.length()-2 );
}

/*
  for strings like "(splitter orientation=0 children=2 sizes=332:478" returns values of
  parameters.  For example, getValue( example, "children" ) returns "2"
  getValue( example, "sizes" ) returns "332:478"
*/
QString getValue( const QString& str, const QString& valName )
{
  int i = str.find( valName );
  if ( i != -1 ) {
    int equal_i = str.find( '=', i );
    if ( equal_i != -1 ) {
      int space_i = str.find( ' ', ++equal_i );
      if ( space_i != -1 )
	return str.mid( equal_i, space_i-equal_i );
    }
  }
  return QString( "" );
}

/*
  checks format of splitter parameters string
*/
bool checkFormat( const QString& parameters )
{
  QString params( parameters );
  // 1. begins and ends with brackets
  QChar c1 = params[0];
  QChar c2 = params[int(params.length()-1)];
  bool ok = ( c1 == '(' && c2 == ')' );
  if ( !ok ) return ok;
  ::cutBrackets( params );
  // 2. has splitter word
  ok = ( params.left( 8 ) == "splitter" );
  if ( !ok ) return ok;
  // 3. has children?  = '(' is found
  int i = params.find( '(' );
  ok = i != -1;
  if ( !ok ) return ok;
  params = params.left( i ); // cut all children, they will be checked later
  // 4. has orientation word and correct value
  ::getValue( params, "orientation" ).toInt( &ok );
  if ( !ok ) return ok;
  // 5. has sizes word and values
  ok = ! ::getValue( params, "sizes" ).isEmpty();
  if ( !ok ) return ok;
  // 6. check children -> number of '(' == number of ')' in original string
  ok = ( parameters.contains( '(' ) == parameters.contains( ')' ) );
  return ok;
}

/*
  Returns children of splitter in a list.  Children are separated by '(' and ')' symbols
*/
QStringList getChildren( const QString& str )
{
  QStringList lst;
  if ( !str.startsWith( "(" ) )
    return lst;
  
  int i = 1,
  nOpen = 1, // count brackets: '(' increments nOpen, ')' decrements
  start = 0;
  while ( i < (int)str.length() )
  {
    if ( str[i] == '(' )
    {
      nOpen++;
      if ( nOpen == 1 )
	start = i;
    }
    else if ( str[i] == ')' )
    {
      nOpen--;
      if ( nOpen == 0 ) 
	lst.append( str.mid( start, i-start+1 ) );
    }
    i++;
  }

  return lst;
}

// for a string like "views active='AnotherView' 'GLView' 'AnotherView'"
// getViewName( example, 0 ) returns "GLView", 
// getViewName( example, 1 ) -> "AnotherView", etc.
QString getViewName( const QString& str, int i )
{
  QRegExp exp( "\\s'\\w+'" );
  int start = 0; // start index of view name in the string
  int num = 0 ; // index of found match
  while ( ( start = exp.search( str, start ) ) != -1 && num < i ) {
    start += exp.matchedLength();
    num ++;
  }
  if ( start != -1 )      // +2 and -3 avoid starting space and starting and ending ' symbols
    return str.mid( start+2, exp.matchedLength()-3 );

  return QString( "" );
}

// returns widget with given name
QWidget* getView( const QWidget* parent, const QString& aName )
{
  QWidget* view = 0;
  QObjectList *l = parent->topLevelWidget()->queryList( "QWidget", aName, false, true );
  if ( !l->isEmpty() )
    view = ::qt_cast<QWidget*>( l->first() );
  delete l;
  return view;
}

/*!
  Installs a splitter described by given parameters string
*/
void QtxWorkstack::setSplitter( QSplitter* splitter, const QString& parameters, QMap< QSplitter*, QValueList<int> >& sMap )
{
  if ( !::checkFormat( parameters ) ) {
    printf( "\nInvalid format of workstack parameters.  Positions of viewers can not be restored.\n" );
    return;
  }

  QString params( parameters );
  ::cutBrackets( params );

  // get splitter sizes and store it in the map for future setting
  QValueList<int> sizes;
  QStringList sizesLst = QStringList::split( ':', ::getValue( params, "sizes" ) );
  QStringList::Iterator it;
  for ( it = sizesLst.begin(); it != sizesLst.end(); ++it )
    sizes.append( (*it).toInt() );
  sMap[ splitter ] = sizes;

  // set orientation of splitter
  int orient = ::getValue( params, "orientation" ).toInt();
  splitter->setOrientation( (Qt::Orientation)orient );

  // get children
  QString options = params.left( params.find( '(' ) );
  QString childrenStr = params.right( params.length()-options.length() );
  QStringList children = ::getChildren( childrenStr );

  // debug output..
  //  printf (" splitter orient=%d, sizes_count=%d, children=%d\n", orient, sizes.count(), children.count() ); 
  //  for ( QStringList::Iterator tit = children.begin(); tit != children.end(); ++tit ) 
  //    printf ("   |-> child = [%s]\n", (*tit).latin1() );

  for ( QStringList::Iterator it = children.begin(); it != children.end(); ++it ) {
    if ( (*it).startsWith( "(splitter" ) ) {
      QSplitter* newSplitter = new QSplitter( splitter );
      setSplitter( newSplitter, *it, sMap );
    }
    else if ( (*it).startsWith( "(views" ) ) {
      QtxWorkstackArea* newArea = createArea( splitter );
      QString activeViewName = ::getValue( *it, "active" );
      QWidget* activeView( 0 );
      activeViewName = activeViewName.mid( 1, activeViewName.length()-2 ); // chop off ' symbols
      int i = 0;
      QString viewName = ::getViewName( *it, i );
      while ( !viewName.isEmpty() ) {
	if ( QWidget* view = ::getView( splitter, viewName ) ) {
	  newArea->insertWidget( view );
	  if ( activeViewName == view->name() )
	    activeView = view;
	}
	viewName = ::getViewName( *it, ++i );
      }
      if ( activeView )
      	newArea->setActiveWidget( activeView );
    }
  }
}

/*!
  Restore workstack's configuration stored in 'parameters' string
*/
QtxWorkstack& QtxWorkstack::operator<<( const QString& parameters )
{
  // clear the main splitter - remove all child splitters and empty areas from it
  QPtrList<QSplitter> splitList;
  QPtrList<QtxWorkstackArea> areaList;
  splitters( mySplit, splitList, false );
  areas( mySplit, areaList, false );
  for ( QPtrListIterator<QSplitter> iter( splitList ); iter.current(); ++iter )
    delete iter.current();
  for ( QPtrListIterator<QtxWorkstackArea> it( areaList ); it.current(); ++it ) 
    if ( it.current()->isEmpty() )
      delete it.current();

  // restore splitter recursively
  QMap< QSplitter*, QValueList<int> > sMap;
  setSplitter( mySplit, parameters, sMap );  

  // now mySplit may contains empty area (where all views were located before restoring)
  // in order setSize to work correctly we have to exclude this area
  areaList.clear();
  areas( mySplit, areaList, false );
  for ( QPtrListIterator<QtxWorkstackArea> delIt( areaList ); delIt.current(); ++delIt ) 
    if ( delIt.current()->isEmpty() )
      delete delIt.current();

  qApp->processEvents();

  // restore splitters' sizes (map of sizes is filled in setSplitters)
  for ( QMap< QSplitter*, QValueList<int> >::Iterator itm = sMap.begin(); itm != sMap.end(); ++itm )
    itm.key()->setSizes( itm.data() );

  return (*this);
}

/*!
  Example of string produced by operator>> :
  "(splitter orientation=0 sizes=186:624 (views active='OCCViewer_0_0' 'OCCViewer_0_0')
/ (views active='VTKViewer_0_0' 'VTKViewer_0_0'))"
*/
QtxWorkstack& QtxWorkstack::operator>>( QString& outParameters )
{
  splitterInfo( mySplit, outParameters );
  return (*this);
}


/*!
  Constructor
*/
QtxWorkstackArea::QtxWorkstackArea( QWidget* parent )
: QWidget( parent )
{
  QVBoxLayout* base = new QVBoxLayout( this );

  QHBox* top = new QHBox( this );
  base->addWidget( top );

  myBar = new QtxWorkstackTabBar( top );

  QPushButton* close = new QPushButton( top );
  close->setPixmap( style().stylePixmap( QStyle::SP_TitleBarCloseButton ) );
  close->setAutoDefault( true );
  close->setFlat( true );
  myClose = close;

  top->setStretchFactor( myBar, 1 );

  myStack = new QWidgetStack( this );

  base->addWidget( myStack, 1 );

  connect( myClose, SIGNAL( clicked() ), this, SLOT( onClose() ) );
  connect( myBar, SIGNAL( selected( int ) ), this, SLOT( onSelected( int ) ) );
  connect( myBar, SIGNAL( dragActiveTab() ), this, SLOT( onDragActiveTab() ) );
  connect( myBar, SIGNAL( contextMenuRequested( QPoint ) ), this, SLOT( onContextMenuRequested( QPoint ) ) );

  updateState();

  updateActiveState();

  qApp->installEventFilter( this );
}

/*!
  Destructor
*/
QtxWorkstackArea::~QtxWorkstackArea()
{
  qApp->removeEventFilter( this );
}

/*!
  \return true if area is empty
*/
bool QtxWorkstackArea::isEmpty() const
{
  bool res = false;
  for ( WidgetInfoMap::ConstIterator it = myInfo.begin(); it != myInfo.end() && !res; ++it )
    res = it.data().vis;
  return !res;
}

/*!
  Adds widget to area
  \param wid - widget
  \param idx - index
*/
void QtxWorkstackArea::insertWidget( QWidget* wid, const int idx )
{
  if ( !wid )
    return;

  int pos = myList.find( wid );
  if ( pos != -1 && ( pos == idx || ( idx < 0 && pos == (int)myList.count() - 1 ) ) )
    return;

  myList.removeRef( wid );
  pos = idx < 0 ? myList.count() : idx;
  myList.insert( QMIN( pos, (int)myList.count() ), wid );
  if ( !myInfo.contains( wid ) )
  {
    QtxWorkstackChild* child = new QtxWorkstackChild( wid, myStack );
    myChild.insert( wid, child );
    myInfo.insert( wid, WidgetInfo() );
    myInfo[wid].id = generateId();
    myInfo[wid].vis = wid->isVisibleTo( wid->parentWidget() );

    connect( child, SIGNAL( destroyed( QObject* ) ), this, SLOT( onChildDestroyed( QObject* ) ) );
    connect( wid, SIGNAL( destroyed() ), this, SLOT( onWidgetDestroyed() ) );
    connect( child, SIGNAL( shown( QtxWorkstackChild* ) ), this, SLOT( onChildShown( QtxWorkstackChild* ) ) );
    connect( child, SIGNAL( hided( QtxWorkstackChild* ) ), this, SLOT( onChildHided( QtxWorkstackChild* ) ) );
    connect( child, SIGNAL( activated( QtxWorkstackChild* ) ), this, SLOT( onChildActivated( QtxWorkstackChild* ) ) );
    connect( child, SIGNAL( captionChanged( QtxWorkstackChild* ) ), this, SLOT( onChildCaptionChanged( QtxWorkstackChild* ) ) );
  }

  updateState();

  setWidgetActive( wid );
  wid->setFocus();
}

/*!
  Creates and shows popup menu for area
  \param p - popup position
*/
void QtxWorkstackArea::onContextMenuRequested( QPoint p )
{
  const QtxWorkstackTabBar* bar = ::qt_cast<const QtxWorkstackTabBar*>( sender() );
  if ( !bar )
    return;

  QWidget* wid = 0;
  QTab* tab = myBar->tabAt( tabAt( p ) );
  if ( tab )
    wid = widget( tab->identifier() );

  emit contextMenuRequested( wid, p );
}

/*!
  SLOT: called when widget added to area is destroyed, removes widget from area
*/
void QtxWorkstackArea::onWidgetDestroyed()
{
  if ( sender() )
    removeWidget( (QWidget*)sender(), false );
}

/*!
  Removes widget from area
  \param wid - widget
  \param del - auto deleting
*/
void QtxWorkstackArea::removeWidget( QWidget* wid, const bool del )
{
  if ( !myList.contains( wid ) )
    return;

  if ( myBar->tab( widgetId( wid ) ) )
    myBar->removeTab( myBar->tab( widgetId( wid ) ) );
  myStack->removeWidget( child( wid ) );

  myList.remove( wid );
  myInfo.remove( wid );
  myChild.remove( wid );

  if( del )
  {
    delete child( wid );
    if( myList.isEmpty() )
      delete this;
    else
      updateState();
  }
  else
    updateState();
}

/*!
  \return list of visible widgets
*/
QWidgetList QtxWorkstackArea::widgetList() const
{
  QWidgetList lst;
  for ( QWidgetListIt it( myList ); it.current(); ++it )
  {
    if ( widgetVisibility( it.current() ) )
      lst.append( it.current() );
  }
  return lst;
}

/*!
  \return active widget
*/
QWidget* QtxWorkstackArea::activeWidget() const
{
  return widget( myBar->currentTab() );
}

/*!
  Sets widget as active
  \param wid - widget
*/
void QtxWorkstackArea::setActiveWidget( QWidget* wid )
{
  myBar->setCurrentTab( widgetId( wid ) );
}

/*!
  \return true if area contains widget
  \param wid - widget
*/
bool QtxWorkstackArea::contains( QWidget* wid ) const
{
  return myList.contains( wid );
}

/*!
  Shows area
*/
void QtxWorkstackArea::show()
{
  QMap<QWidget*, bool> map;
  for ( QWidgetListIt it( myList ); it.current(); ++it )
  {
    map.insert( it.current(), isBlocked( it.current() ) );
    setBlocked( it.current(), true );
  }

  QWidget::show();

  for ( QWidgetListIt itr( myList ); itr.current(); ++itr )
    setBlocked( itr.current(), map.contains( itr.current() ) ? map[itr.current()] : false );
}

/*!
  Hides area
*/
void QtxWorkstackArea::hide()
{
  QMap<QWidget*, bool> map;
  for ( QWidgetListIt it( myList ); it.current(); ++it )
  {
    map.insert( it.current(), isBlocked( it.current() ) );
    setBlocked( it.current(), true );
  }

  QWidget::hide();

  for ( QWidgetListIt itr( myList ); itr.current(); ++itr )
    setBlocked( itr.current(), map.contains( itr.current() ) ? map[itr.current()] : false );
}

/*!
  \return true if area is active
*/
bool QtxWorkstackArea::isActive() const
{
  QtxWorkstack* ws = workstack();
  if ( !ws )
    return false;

  return ws->activeArea() == this;
}

/*!
  Update active state of tab bar
*/
void QtxWorkstackArea::updateActiveState()
{
  myBar->setActive( isActive() );
}

/*!
  \return corresponding workstack
*/
QtxWorkstack* QtxWorkstackArea::workstack() const
{
  QtxWorkstack* ws = 0;
  QWidget* wid = parentWidget();
  while ( wid && !ws )
  {
    if ( wid->inherits( "QtxWorkstack" ) )
      ws = (QtxWorkstack*)wid;
    wid = wid->parentWidget();
  }
  return ws;
}

/*!
  Custom event filter
*/
bool QtxWorkstackArea::eventFilter( QObject* o, QEvent* e )
{
  if ( o->isWidgetType() )
  {
    QWidget* wid = (QWidget*)o;
    if ( e->type() == QEvent::FocusIn || e->type() == QEvent::MouseButtonPress )
    {
      bool ok = false;
      while ( !ok && wid && wid != myClose )
      {
        ok = wid == this;
        wid = wid->parentWidget();
      }
      if ( ok )
        QApplication::postEvent( this, new QCustomEvent( (QEvent::Type)( e->type() == QEvent::FocusIn ? ActivateWidget : FocusWidget ) ) );
    }
  }
  return false;
}

/*!
  \return rectangle of area in order to draw drop rectangle on area
*/
QRect QtxWorkstackArea::floatRect() const
{
  QRect r = myStack->geometry();
  return QRect( mapToGlobal( r.topLeft() ), mapToGlobal( r.bottomRight() ) );
}

/*!
  \return rectangle of tab in order to draw drop rectangle on tab
  \param idx - tab index
*/
QRect QtxWorkstackArea::floatTab( const int idx ) const
{
  return myBar->tabRect( idx );
}

/*!
  \return tab covering point 
  \param p - point
*/
int QtxWorkstackArea::tabAt( const QPoint& p ) const
{
  int idx = -1;
  for ( int i = 0; i < myBar->count() && idx == -1; i++ )
  {
    QRect r = myBar->tabRect( i );
    if ( r.isValid() && r.contains( p ) )
      idx = i;
  }
  return idx;
}

/*!
  Event handler for custom events
*/
void QtxWorkstackArea::customEvent( QCustomEvent* e )
{
  switch ( e->type() )
  {
  case ActivateWidget:
    emit activated( activeWidget() );
    break;
  case FocusWidget:
    if ( activeWidget() )
    {
      if ( !activeWidget()->focusWidget() )
        activeWidget()->setFocus();
      else {
        if ( activeWidget()->focusWidget()->hasFocus()) {
          QFocusEvent in(QEvent::FocusIn);
	  QApplication::sendEvent(this, &in);
	}
        else
          activeWidget()->focusWidget()->setFocus();
      }
    }
    break;
  case RemoveWidget:
    removeWidget( (QWidget*)e->data() );
    break;
  }
}

/*!
  Custom focus in event handler
*/
void QtxWorkstackArea::focusInEvent( QFocusEvent* e )
{
  QWidget::focusInEvent( e );

  emit activated( activeWidget() );
}

/*!
  Custom mouse press event handler
*/
void QtxWorkstackArea::mousePressEvent( QMouseEvent* e )
{
  QWidget::mousePressEvent( e );

  emit activated( activeWidget() );
}

/*!
  SLOT: called if button close is pressed
*/
void QtxWorkstackArea::onClose()
{
  QWidget* wid = activeWidget();
  if ( wid )
    wid->close();
}

/*!
  SLOT: called if tab page is selected
*/
void QtxWorkstackArea::onSelected( int id )
{
  updateCurrent();

  emit activated( activeWidget() );
}

/*!
  SLOT: called if active tab page is dragged
*/
void QtxWorkstackArea::onDragActiveTab()
{
  QtxWorkstackChild* c = child( activeWidget() );
  if ( !c )
    return;

  new QtxWorkstackDrag( workstack(), c );
}

/*!
  SLOT: called on child is destroyed, removes from area
*/
void QtxWorkstackArea::onChildDestroyed( QObject* obj )
{
  QtxWorkstackChild* child = (QtxWorkstackChild*)obj;
  myStack->removeWidget( child );

  QWidget* wid = 0;
  for ( ChildMap::ConstIterator it = myChild.begin(); it != myChild.end() && !wid; ++it )
  {
    if ( it.data() == child )
      wid = it.key();
  }

  myChild.remove( wid );

  QApplication::postEvent( this, new QCustomEvent( (QEvent::Type)RemoveWidget, wid ) );
}

/*!
  SLOT: called on child is shown
*/
void QtxWorkstackArea::onChildShown( QtxWorkstackChild* c )
{
  setWidgetShown( c->widget(), true );
}

/*!
  SLOT: called on child is hidden
*/
void QtxWorkstackArea::onChildHided( QtxWorkstackChild* c )
{
  setWidgetShown( c->widget(), false );
}

/*!
  SLOT: called on child is activated
*/
void QtxWorkstackArea::onChildActivated( QtxWorkstackChild* c )
{
  setWidgetActive( c->widget() );
}

/*!
  SLOT: called on child caption is changed
*/
void QtxWorkstackArea::onChildCaptionChanged( QtxWorkstackChild* c )
{
  updateTab( c->widget() );
}

/*!
  Raises widget when active tab is changed
*/
void QtxWorkstackArea::updateCurrent()
{
  QMap<QWidget*, bool> map;
  for ( QWidgetListIt it( myList ); it.current(); ++it )
  {
    map.insert( it.current(), isBlocked( it.current() ) );
    setBlocked( it.current(), true );
  }

  myStack->raiseWidget( myBar->currentTab() );

  for ( QWidgetListIt itr( myList ); itr.current(); ++itr )
    setBlocked( itr.current(), map.contains( itr.current() ) ? map[itr.current()] : false );
}

/*!
  Updates tab
  \param wid - tab widget
*/
void QtxWorkstackArea::updateTab( QWidget* wid )
{
  QTab* tab = myBar->tab( widgetId( wid ) );
  if ( !tab )
    return;

  QIconSet icoSet;
  if ( wid->icon() )
  {
    QPixmap pix = *wid->icon();
    pix.convertFromImage( pix.convertToImage().smoothScale( pix.width(), 16, QImage::ScaleMin ) );
    icoSet = QIconSet( pix );
  }

  tab->setIconSet( icoSet );
  tab->setText( wid->caption() );
}

/*!
  \return widget
  \param id - widget id
*/
QWidget* QtxWorkstackArea::widget( const int id ) const
{
  QWidget* wid = 0;
  for ( WidgetInfoMap::ConstIterator it = myInfo.begin(); it != myInfo.end() && !wid; ++it )
  {
    if ( it.data().id == id )
      wid = it.key();
  }
  return wid;
}

/*!
  \return widget id
  \param wid - widget
*/
int QtxWorkstackArea::widgetId( QWidget* wid ) const
{
  int id = -1;
  if ( myInfo.contains( wid ) )
    id = myInfo[wid].id;
  return id;
}

/*!
  \return true if widget is visible
  \param wid - widget
*/
bool QtxWorkstackArea::widgetVisibility( QWidget* wid ) const
{
  bool res = false;
  if ( myInfo.contains( wid ) )
    res = myInfo[wid].vis;
  return res;
}

/*!
  Sets widget as active
  \param wid - widget
*/
void QtxWorkstackArea::setWidgetActive( QWidget* wid )
{
  int id = widgetId( wid );
  if ( id < 0 )
    return;

  myBar->setCurrentTab( id );
}

/*!
  Shows/hides widget
  \param wid - widget
  \param on - new shown state
*/
void QtxWorkstackArea::setWidgetShown( QWidget* wid, const bool on )
{
  if ( isBlocked( wid ) || !myInfo.contains( wid ) || myInfo[wid].vis == on )
    return;

  myInfo[wid].vis = on;
  updateState();
}

/*!
  Update
*/
void QtxWorkstackArea::updateState()
{
  bool updBar = myBar->isUpdatesEnabled();
  bool updStk = myStack->isUpdatesEnabled();
  myBar->setUpdatesEnabled( false );
  myStack->setUpdatesEnabled( false );

  bool block = myBar->signalsBlocked();
  myBar->blockSignals( true );

  QWidget* prev = activeWidget();

  int idx = 0;
  for ( QWidgetListIt it( myList ); it.current(); ++it )
  {
    QWidget* wid = it.current();
    int id = widgetId( wid );

    if ( id < 0 )
      continue;

    bool vis = widgetVisibility( wid );

    if ( myBar->tab( id ) && ( !vis || myBar->indexOf( id ) != idx ) )
      myBar->removeTab( myBar->tab( id ) );

    if ( !myBar->tab( id ) && vis )
    {
      QTab* tab = new QTab( wid->caption() );
      myBar->insertTab( tab, idx );
      tab->setIdentifier( id );
    }

    updateTab( wid );

    bool block = isBlocked( wid );
    setBlocked( wid, true );

    QtxWorkstackChild* cont = child( wid );

    if ( !vis )
      myStack->removeWidget( cont );
    else if ( !myStack->widget( id ) )
      myStack->addWidget( cont, id );

    if ( vis )
      idx++;

    setBlocked( wid, block );
  }

  int curId = widgetId( prev );
  if ( !myBar->tab( curId ) )
  {
    QWidget* wid = 0;
    int pos = myList.find( prev );
    for ( int i = pos - 1; i >= 0 && !wid; i-- )
    {
      if ( widgetVisibility( myList.at( i ) ) )
        wid = myList.at( i );
    }

    for ( int j = pos + 1; j < (int)myList.count() && !wid; j++ )
    {
      if ( widgetVisibility( myList.at( j ) ) )
        wid = myList.at( j );
    }

    if ( wid )
      curId = widgetId( wid );
  }

  myBar->setCurrentTab( curId );

  myBar->blockSignals( block );

  updateCurrent();

  myBar->setUpdatesEnabled( updBar );
  myStack->setUpdatesEnabled( updStk );
  if ( updBar )
    myBar->update();
  if ( updStk )
    myStack->update();

  QResizeEvent re( myBar->size(), myBar->size() );
  QApplication::sendEvent( myBar, &re );

  if ( isEmpty() )
  {
    hide();
    emit deactivated( this );
  }
  else
  {
    show();
    if ( prev != activeWidget() )
      emit activated( activeWidget() );
  }
}

/*!
  \return first unshared widget id
*/
int QtxWorkstackArea::generateId() const
{
  QMap<int, int> map;

  for ( WidgetInfoMap::ConstIterator it = myInfo.begin(); it != myInfo.end(); ++it )
    map.insert( it.data().id, 0 );

  int id = 0;
  while ( map.contains( id ) )
    id++;

  return id;
}

/*!
  \return true if widget is blocked
  \param wid - widget
*/
bool QtxWorkstackArea::isBlocked( QWidget* wid ) const
{
  return myBlock.contains( wid );
}

/*!
  Blocks widget
  \param wid - widget
  \param on - new blocked state
*/
void QtxWorkstackArea::setBlocked( QWidget* wid, const bool on )
{
  if ( on )
    myBlock.insert( wid, 0 );
  else
    myBlock.remove( wid );
}

/*!
  \return child corresponding to widget
  \param wid - widget
*/
QtxWorkstackChild* QtxWorkstackArea::child( QWidget* wid ) const
{
  QtxWorkstackChild* res = 0;
  if ( myChild.contains( wid ) )
    res = myChild[wid];
  return res;
}

/*!
  Constructor
*/
QtxWorkstackChild::QtxWorkstackChild( QWidget* wid, QWidget* parent )
: QHBox( parent ),
myWidget( wid )
{
  myWidget->reparent( this, QPoint( 0, 0 ), myWidget->isVisibleTo( myWidget->parentWidget() ) );
  myWidget->installEventFilter( this );

  connect( myWidget, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );
}

/*!
  Destructor
*/
QtxWorkstackChild::~QtxWorkstackChild()
{
  qApp->removeEventFilter( this );

  if ( !widget() )
    return;

  widget()->removeEventFilter( this );
  widget()->reparent( 0, QPoint( 0, 0 ), false );
  disconnect( widget(), SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );
}

/*!
  \return corresponding widget
*/
QWidget* QtxWorkstackChild::widget() const
{
  return myWidget;
}

/*!
  Custom event filter
*/
bool QtxWorkstackChild::eventFilter( QObject* o, QEvent* e )
{
  if ( o->isWidgetType() )
  {
    if ( e->type() == QEvent::CaptionChange || e->type() == QEvent::IconChange )
      emit captionChanged( this );

    if ( !e->spontaneous() && ( e->type() == QEvent::Show || e->type() == QEvent::ShowToParent ) )
      emit shown( this );

    if ( !e->spontaneous() && ( e->type() == QEvent::Hide || e->type() == QEvent::HideToParent ) )
      emit hided( this );

    if ( e->type() == QEvent::FocusIn )
      emit activated( this );
  }
  return QHBox::eventFilter( o, e );
}

/*!
  SLOT: called on object is destroyed
*/
void QtxWorkstackChild::onDestroyed( QObject* obj )
{
  if ( obj != widget() )
    return;

  myWidget = 0;
  deleteLater();
}

/*!
  Custom child event handler
*/
void QtxWorkstackChild::childEvent( QChildEvent* e )
{
  if ( e->type() == QEvent::ChildRemoved && e->child() == widget() )
  {
    myWidget = 0;
    deleteLater();
  }
  QHBox::childEvent( e );
}

/*!
  Constructor
*/
QtxWorkstackTabBar::QtxWorkstackTabBar( QWidget* parent )
: QTabBar( parent ),
myId( -1 )
{
}

/*!
  Destructor
*/
QtxWorkstackTabBar::~QtxWorkstackTabBar()
{
}

/*!
  Sets tab bar as active or inactive
  \param on - new active state
*/
void QtxWorkstackTabBar::setActive( const bool on )
{
  QFont aFont = font();
  aFont.setUnderline( on );
  QColorGroup aColGrp;
  QPalette aPal = palette();
  if ( !on ) {
    aPal.setColor( QColorGroup::HighlightedText, aColGrp.foreground() );
    aPal.setColor( QColorGroup::Highlight, colorGroup().dark().light( DARK_COLOR_LIGHT ) );
    setPalette( aPal );
  }
  else {
    aPal.setColor( QColorGroup::HighlightedText, aColGrp.highlightedText() );
    aPal.setColor( QColorGroup::Highlight, aColGrp.highlight() );
    unsetPalette();
  }
  setFont( aFont );

  update();
}

/*!
  \return tab rectangle
  \param idx - tab index
*/
QRect QtxWorkstackTabBar::tabRect( const int idx ) const
{
  QRect r;
  QTab* t = tabAt( idx );
  if ( t )
  {
    r = t->rect();
    r.setLeft( QMAX( r.left(), 0 ) );

    int x1 = tabAt( 0 )->rect().left();
    int x2 = tabAt( count() - 1 )->rect().right();

    int bw = 0;
    if ( QABS( x2 - x1 ) > width() )
#if defined QT_VERSION && QT_VERSION >= 0x30300
      bw = 2 * style().pixelMetric( QStyle::PM_TabBarScrollButtonWidth, this );
#else
      bw = 2 * 16;
#endif

    int limit = width() - bw;
    r.setRight( QMIN( r.right(), limit ) );

    r = QRect( mapToGlobal( r.topLeft() ), r.size() );
  }
  return r;
}

/*!
  Custom mouse move event handler
*/
void QtxWorkstackTabBar::mouseMoveEvent( QMouseEvent* e )
{
  if ( myId != -1 && !tab( myId )->rect().contains( e->pos() ) )
  {
    myId = -1;
    emit dragActiveTab();
  }

  QTabBar::mouseMoveEvent( e );
}

/*!
  Custom mouse press event handler
*/
void QtxWorkstackTabBar::mousePressEvent( QMouseEvent* e )
{
  QTabBar::mousePressEvent( e );

  if ( e->button() == LeftButton )
    myId = currentTab();
}

/*!
  Custom mouse release event handler
*/
void QtxWorkstackTabBar::mouseReleaseEvent( QMouseEvent* e )
{
  QTabBar::mouseReleaseEvent( e );

  myId = -1;

  if ( e->button() == RightButton )
    emit contextMenuRequested( e->globalPos() );
}

/*!
  Custom context menu event handler
*/
void QtxWorkstackTabBar::contextMenuEvent( QContextMenuEvent* e )
{
  if ( e->reason() != QContextMenuEvent::Mouse )
    emit contextMenuRequested( e->globalPos() );
}

/*!
  Draws label of tab bar
*/
void QtxWorkstackTabBar::paintLabel( QPainter* p, const QRect& br, QTab* t, bool has_focus ) const
{
  if ( currentTab() != t->identifier() )
  {
    QFont fnt = p->font();
    fnt.setUnderline( false );
    p->setFont( fnt );
  }
  QTabBar::paintLabel( p, br, t, has_focus );
}

/*!
  Constructor
*/
QtxWorkstackDrag::QtxWorkstackDrag( QtxWorkstack* ws, QtxWorkstackChild* child )
: QObject( 0 ),
myWS( ws ),
myTab( -1 ),
myArea( 0 ),
myPainter( 0 ),
myChild( child )
{
  qApp->installEventFilter( this );
}

/*!
  Destructor
*/
QtxWorkstackDrag::~QtxWorkstackDrag()
{
  qApp->removeEventFilter( this );

  endDrawRect();
}

/*!
  Custom event filter
*/
bool QtxWorkstackDrag::eventFilter( QObject*, QEvent* e )
{
  switch ( e->type() )
  {
  case QEvent::MouseMove:
    updateTarget( ((QMouseEvent*)e)->globalPos() );
    break;
  case QEvent::MouseButtonRelease:
    drawRect();
    endDrawRect();
    dropWidget();
    deleteLater();
    break;
  default:
    return false;
  }
  return true;
}

/*!
  Updates internal field with widget-target for dropping
  \param p - current point of dragging
*/
void QtxWorkstackDrag::updateTarget( const QPoint& p )
{
  int tab = -1;
  QtxWorkstackArea* area = detectTarget( p, tab );
  setTarget( area, tab );
}

/*!
  \return target area for dropping by point
  \param p - current point of dragging
  \param tab - index of tab to dropping
*/
QtxWorkstackArea* QtxWorkstackDrag::detectTarget( const QPoint& p, int& tab ) const
{
  if ( p.isNull() )
    return 0;

  QtxWorkstackArea* area = myWS->areaAt( p );
  if ( area )
    tab = area->tabAt( p );
  return area;
}

/*!
  Changes target area for dropping
  \param area - new target area
  \param tab - tab index
*/
void QtxWorkstackDrag::setTarget( QtxWorkstackArea* area, const int tab )
{
  if ( !area || ( myArea == area && tab == myTab ) )
    return;

  startDrawRect();

  if ( myArea )
    drawRect();

  myTab = tab;
  myArea = area;

  if ( myArea )
    drawRect();
}

/*!
  Called on widget drop, inserts dropped widget to area
*/
void QtxWorkstackDrag::dropWidget()
{
  if ( myArea )
    myArea->insertWidget( myChild->widget(), myTab );
}

/*!
  Draws float rect
*/
void QtxWorkstackDrag::drawRect()
{
  if ( !myPainter || !myArea )
    return;

  QRect r = myArea->floatRect();
  int m = myPainter->pen().width();

  r.setTop( r.top() + m + 2 );
  r.setLeft( r.left() + m + 2 );
  r.setRight( r.right() - m - 2 );
  r.setBottom( r.bottom() - m - 2 );

  myPainter->drawRect( r );

  QRect tr = myArea->floatTab( myTab );
  tr.setTop( tr.top() + m );
  tr.setLeft( tr.left() + m );
  tr.setRight( tr.right() - m );
  tr.setBottom( tr.bottom() - m );

  myPainter->drawRect( tr );
}

/*!
  Deletes internal painter
*/
void QtxWorkstackDrag::endDrawRect()
{
  delete myPainter;
  myPainter = 0;
}

/*!
  Initialize internal painter
*/
void QtxWorkstackDrag::startDrawRect()
{
  if ( myPainter )
    return;

  int scr = QApplication::desktop()->screenNumber( (QWidget*)this );
  QWidget* paint_on = QApplication::desktop()->screen( scr );

  myPainter = new QPainter( paint_on, true );
  myPainter->setPen( QPen( gray, 3 ) );
  myPainter->setRasterOp( XorROP );
}
