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

// File:      QtxWorkspace.cxx
// Author:    Sergey TELKOV
//
#include "QtxWorkspace.h"

#include <QMdiSubWindow>

/*!
  \class QtxWorkspace
  \brief A workspace widget which can be used in the MDI application
         as top-level widget in the application main window.

  Provides methods to tile child windows in horizontal or vertical
  direction.
*/

/*!
  \brief Constructor.
  \param parent parent widget
*/
QtxWorkspace::QtxWorkspace( QWidget* parent )
: QMdiArea( parent )
{
}

/*!
  \brief Destructor.
*/
QtxWorkspace::~QtxWorkspace()
{
}

/*!
  \brief Tiles child windows vertically.
*/
void QtxWorkspace::tileVertical()
{
  QList<QMdiSubWindow *> winList = subWindowList();
  if ( winList.isEmpty() )
    return;
  
  int count = 0;
  for ( QList<QMdiSubWindow *>::const_iterator itr = winList.begin(); itr != winList.end(); ++itr )
    if ( !( (*itr)->windowState() & Qt::WindowMinimized ) )
      count++;

  if ( !count )
    return;

  if ( activeSubWindow() && ( activeSubWindow()->windowState() & Qt::WindowMaximized ) )
    activeSubWindow()->showNormal();

  int y = 0;
  int heightForEach = height() / count;
  for ( QList<QMdiSubWindow *>::iterator it = winList.begin(); it != winList.end(); ++it )
  {
    QWidget* win = *it;
    if ( win->windowState() & Qt::WindowMinimized )
      continue;
    
    if ( win->windowState() & Qt::WindowMaximized )
    {
      win->hide();
      win->showNormal();
    }
    
//    QApplication::sendPostedEvents( 0, QEvent::ShowNormal );

    int prefH = win->minimumHeight() + win->parentWidget()->baseSize().height();
    int actualH = qMax( heightForEach, prefH );

    win->parentWidget()->setGeometry( 0, y, width(), actualH );
    y += actualH;
  }
}

/*!
  \brief Tiles child windows horizontally.
*/
void QtxWorkspace::tileHorizontal()
{
  QList<QMdiSubWindow *> winList = subWindowList();
  if ( winList.isEmpty() )
    return;

  int count = 0;
  for ( QList<QMdiSubWindow *>::const_iterator itr = winList.begin(); itr != winList.end(); ++itr )
    if ( !( (*itr)->windowState() & Qt::WindowMinimized ) )
      count++;

  if ( !count )
    return;

  if ( activeSubWindow() && activeSubWindow()->windowState() & Qt::WindowMaximized )
    activeSubWindow()->showNormal();
  
  int x = 0;
  int widthForEach = width() / count;
  for ( QList<QMdiSubWindow *>::iterator it = winList.begin(); it != winList.end(); ++it )
  {
    QWidget* win = *it;
    if ( win->windowState() & Qt::WindowMinimized )
      continue;
    
    if ( win->windowState() & Qt::WindowMaximized )
    {
      win->hide();
      win->showNormal();
    }

//    QApplication::sendPostedEvents( 0, QEvent::ShowNormal );

    int prefW = win->minimumWidth();
    int actualW = qMax( widthForEach, prefW );
    
    win->parentWidget()->setGeometry( x, 0, actualW, height() );
    x += actualW;
  }
}

void QtxWorkspace::onSubWindowActivated( QMdiSubWindow* subWindow )
{
  QWidget* w = 0;
  if ( subWindow )
    w = subWindow->widget();
  emit windowActivated( w );
}
