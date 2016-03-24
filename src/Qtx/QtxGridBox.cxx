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

// File:      QtxGridBox.cxx
// Author:    Sergey TELKOV
//
#include "QtxGridBox.h"

#include <QGridLayout>
#include <QChildEvent>

/*!
  \class QtxGridBox::Space
  \internal
  \brief Represents a space in the grid box.
*/

class QtxGridBox::Space : public QWidget
{
public:
  Space( const int, QtxGridBox* );
  virtual ~Space();

  virtual QSize sizeHint() const;
  virtual QSize minimumSizeHint() const;

private:
  int           mySize;
  QtxGridBox*   myGrid;
};

/*!
  \brief Constructor.
  \param sz size
  \param gb parent grid box
*/
QtxGridBox::Space::Space( const int sz, QtxGridBox* gb )
: QWidget( gb ),
  mySize( sz ),
  myGrid( gb )
{
  setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
}

/*!
  \brief Destructor.
*/
QtxGridBox::Space::~Space()
{
}

/*!
  \brief Get recommended size for the widget.
  \return recommended size for the widget
*/
QSize QtxGridBox::Space::sizeHint() const
{
  return minimumSizeHint();
}

/*!
  \brief Get recommended minimum size for the widget.
  \return recommended minimum size for the widget
*/
QSize QtxGridBox::Space::minimumSizeHint() const
{
  QSize sz( 0, 0 );
  if ( myGrid && myGrid->orientation() == Qt::Horizontal )
    sz.setWidth( mySize );
  else
    sz.setHeight( mySize );
  return sz;
}

/*!
  \class QtxGridBox
  \brief A container widget with possibility to automatically layout
         child widgets.
*/

/*!
  \brief Constructor.
  \param parent parent widget
  \param m grid box margin
  \param s grid box spacing
*/
QtxGridBox::QtxGridBox( QWidget* parent, const int m, const int s )
: QWidget( parent ),
  myCols( 1 ),
  mySkip( false ),
  myOrient( Qt::Vertical ),
  myCol( 0 ),
  myRow( 0 )
{
  myLayout = new QGridLayout( this );
  myLayout->setMargin( m );
  myLayout->setSpacing( s );
}

/*!
  \brief Constructor.
  \param cols number of grid box columns or rows (depending on the orientation)
  \param o grid box orientation
  \param parent parent widget
  \param m grid box margin
  \param s grid box spacing
*/
QtxGridBox::QtxGridBox( const int cols, Qt::Orientation o, QWidget* parent, const int m, const int s )
: QWidget( parent ),
  myCols( cols ),
  mySkip( false ),
  myOrient( o ),
  myLayout( 0 ),
  myCol( 0 ),
  myRow( 0 )
{
  myLayout = new QGridLayout( this );
  myLayout->setMargin( m );
  myLayout->setSpacing( s );
}

/*!
  \brief Destructor.
*/
QtxGridBox::~QtxGridBox()
{
}

/*!
  \brief Get number of grid box columns/rows (depending on the orientation).
  \return number of columns (rows)
*/
int QtxGridBox::columns() const
{
  return myCols;
}

/*!
  \brief Get the grid box orientation.
  \return orientation
*/
Qt::Orientation QtxGridBox::orientation() const
{
  return myOrient;
}

/*!
  \brief Set number of grid box columns/rows (depending on the orientation).
  \param cols number of columns (rows)
*/
void QtxGridBox::setColumns( const int cols )
{
  setLayout( cols, orientation() );
}

/*!
  \brief Set the grid box orientation.
  \param o orientation
*/
void QtxGridBox::setOrientation( Qt::Orientation o )
{
  setLayout( columns(), o );
}

/*!
  \brief Initialize internal layout.
  \param cols number of columns (rows)
  \param o orientation
*/
void QtxGridBox::setLayout( const int cols, Qt::Orientation o )
{
  if ( myCols == cols && myOrient == o )
    return;

  myCols = cols;
  myOrient = o;

  arrangeWidgets();
}

/*!
  \brief Get "skip invisible widgets" flags.
  \return current flag state
*/
bool QtxGridBox::skipInvisible() const
{
  return mySkip;
}

/*!
  \brief Set "skip invisible widgets" flags.

  If this flag is set to \c false, invisible widgets
  are not taken into account when layouting widgets.

  \param on new flag state
*/
void QtxGridBox::setSkipInvisible( const bool on )
{
  if ( mySkip == on )
    return;

  mySkip = on;
  arrangeWidgets();
}

/*!
  \brief Add space (empty cell) to the grid box.
  \param sp requied space size
*/
void QtxGridBox::addSpace( const int sp )
{
  new Space( sp, this );
}

/*!
  \brief Get grid box's inside margin size.
  \return inside margin size
*/
int QtxGridBox::insideMargin() const
{
  return myLayout->margin();
}

/*!
  \brief Get grid box's inside spacing size.
  \return inside spacing size
*/
int QtxGridBox::insideSpacing() const
{
  return myLayout->spacing();
}

/*!
  \brief Set grid box's inside margin size.
  \param m new inside margin size
*/
void QtxGridBox::setInsideMargin( const int m )
{
  myLayout->setMargin( m );
}

/*!
  \brief Set grid box's inside spacing size.
  \param s new inside spacing size
*/
void QtxGridBox::setInsideSpacing( const int s )
{
  myLayout->setSpacing( s );
}

/*!
  \brief Custom event filter.
  \param o event receiver object.
  \param e event
  \return \c true if the event processing should be stopped
*/
bool QtxGridBox::eventFilter( QObject* o, QEvent* e )
{
  if ( skipInvisible() && ( e->type() == QEvent::Show || e->type() == QEvent::ShowToParent ||
                            e->type() == QEvent::Hide || e->type() == QEvent::HideToParent ) )
    arrangeWidgets();

  return QWidget::eventFilter( o, e );
}

/*!
  \brief Customize child event.
  \param e child event
*/
void QtxGridBox::childEvent( QChildEvent* e )
{
  if ( e->child()->isWidgetType() )
  {
    QWidget* wid = (QWidget*)e->child();
    if ( e->type() == QEvent::ChildAdded )
    {
      placeWidget( wid );
      wid->installEventFilter( this );
    }
    else if ( e->type() == QEvent::ChildRemoved )
      wid->removeEventFilter( this );
  }
  QWidget::childEvent( e );
}

/*!
  \brief Increment the grid box current cell.
*/
void QtxGridBox::skip()
{
  if ( orientation() == Qt::Horizontal )
  {
    myCol++;
    if ( myCol >= columns() )
    {
      myRow++;
      myCol = 0;
    }
  }
  else
  {
    myRow++;
    if ( myRow >= columns() )
    {
      myCol++;
      myRow = 0;
    }
  }
}

/*!
  \brief Arrange child widgets.
*/
void QtxGridBox::arrangeWidgets()
{
  myRow = myCol = 0;
  int m = myLayout ? myLayout->margin() : 0;
  int s = myLayout ? myLayout->spacing() : 0;
  delete myLayout;
  myLayout = new QGridLayout( this );
  myLayout->setMargin( m );
  myLayout->setSpacing( s );

  QObjectList list = children();
  for ( QObjectList::iterator it = list.begin(); it != list.end(); ++it )
  {
    if ( !(*it)->isWidgetType() )
      continue;

    QWidget* wid = (QWidget*)(*it);
    if ( !skipInvisible() || wid->isVisibleTo( this ) )
      placeWidget( wid );
  }
  updateGeometry();
}

/*!
  \brief Place new widget to the current grid box cell.
  \param wid widget being inserted
*/
void QtxGridBox::placeWidget( QWidget* wid )
{
  myLayout->addWidget( wid, myRow, myCol );

  skip();
}
