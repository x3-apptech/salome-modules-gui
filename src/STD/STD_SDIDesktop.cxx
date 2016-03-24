// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#include "STD_SDIDesktop.h"

#include <SUIT_ViewWindow.h>

#include <QFrame>
#include <QVBoxLayout>

/*!Constructor. Create instance of QVBox*/
STD_SDIDesktop::STD_SDIDesktop()
: SUIT_Desktop()
{
  myMainWidget = new QFrame( this );
  myMainWidget->setFrameStyle( QFrame::Panel | QFrame::Sunken );
 
  QVBoxLayout* main = new QVBoxLayout( myMainWidget );
  main->setMargin( 0 );

  setCentralWidget( myMainWidget );
}

/*!Destructor.*/
STD_SDIDesktop::~STD_SDIDesktop()
{
}

/*!\retval SUIT_ViewWindow - return const active window.*/
SUIT_ViewWindow* STD_SDIDesktop::activeWindow() const
{
  const QObjectList& lst = myMainWidget->children();
  QList<SUIT_ViewWindow*> winList;
  for ( QObjectList::const_iterator it = lst.begin(); it != lst.end(); ++it )
  {
    SUIT_ViewWindow* vw = ::qobject_cast<SUIT_ViewWindow*>( *it );
    if ( vw )
      winList.append( vw );
  }

  SUIT_ViewWindow* win = 0;
  for ( QList<SUIT_ViewWindow*>::iterator itr = winList.begin(); itr != winList.end() && !win; ++itr )
  {
    if ( (*itr)->isActiveWindow() )
      win = *itr;
  }

  if ( !win && !winList.isEmpty() )
    win = winList.first();

  return win;
}

/*!\retval QPtrList<SUIT_ViewWindow> - return const active window list.*/
QList<SUIT_ViewWindow*> STD_SDIDesktop::windows() const
{
  QList<SUIT_ViewWindow*> winList;
  winList.append( activeWindow() );
  return winList;
}

/*! add new widget into desktop.*/
void STD_SDIDesktop::addWindow( QWidget* w )
{
  if ( !w || !centralWidget() || !centralWidget()->layout() )
    return;

  w->setParent( centralWidget() );
  centralWidget()->layout()->addWidget( w );
}
