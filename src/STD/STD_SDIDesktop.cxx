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
#include "STD_SDIDesktop.h"

#include <SUIT_ViewWindow.h>

#include <qvbox.h>
#include <qmenubar.h>
#include <qobjectlist.h>

/*!Constructor. Create instance of QVBox*/
STD_SDIDesktop::STD_SDIDesktop()
: SUIT_Desktop()
{
  myMainWidget = new QVBox( this );
  myMainWidget->setFrameStyle( QFrame::Panel | QFrame::Sunken );

  setCentralWidget( myMainWidget );
}

/*!Destructor.*/
STD_SDIDesktop::~STD_SDIDesktop()
{
}

/*!\retval SUIT_ViewWindow - return const active window.*/
SUIT_ViewWindow* STD_SDIDesktop::activeWindow() const
{
  const QObjectList* children = myMainWidget->children();
  if ( !children )
    return 0;

  QPtrList<SUIT_ViewWindow> winList;
  for ( QObjectListIt it( *children ); it.current(); ++it )
  {
    if ( it.current()->inherits( "SUIT_ViewWindow" ) )
      winList.append( (SUIT_ViewWindow*)it.current() );
  }

  SUIT_ViewWindow* win = 0;
  for ( QPtrListIterator<SUIT_ViewWindow> itr( winList ); itr.current() && !win; ++itr )
  {
    if ( itr.current()->isActiveWindow() )
      win = itr.current();
  }

  if ( !win && !winList.isEmpty() )
    win = winList.getFirst();

  return win;
}

/*!\retval QPtrList<SUIT_ViewWindow> - return const active window list.*/
QPtrList<SUIT_ViewWindow> STD_SDIDesktop::windows() const
{
  QPtrList<SUIT_ViewWindow> winList;
      winList.append( activeWindow() );
  return winList;
}

/*!\retval QWidget - pointer to main window.*/
QWidget* STD_SDIDesktop::parentArea() const
{
  return myMainWidget;
}
