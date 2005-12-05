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
// See http://www.salome-platform.org/
//
// File:      QtxAction.cxx
// Author:    Sergey TELKOV

#include "QtxAction.h"

#include <qpopupmenu.h>

/*!
	Name: QtxAction [public]
	Desc: Constructs an action with given parent and name. If toggle is true the
		  action will be a toggle action, otherwise it will be a command action.
*/

QtxAction::QtxAction( QObject* parent, const char* name, bool toggle )
: QAction( parent, name, toggle )
{
}

/*!
	Name: QtxAction [public]
	Desc: This constructor creates an action with the following properties: the
		  description text, the icon or iconset icon, the menu text and keyboard
		  accelerator. It is a child of given parent and named specified name.
		  If toggle is true the action will be a toggle action, otherwise it will
		  be a command action.
*/

QtxAction::QtxAction( const QString& text, const QIconSet& icon,
                      const QString& menuText, int accel,
                      QObject* parent, const char* name, bool toggle )
: QAction( text, icon, menuText, accel, parent, name, toggle )
{
}

/*!
	Name: QtxAction [public]
	Desc: This constructor creates an action with the following properties: the
		  description text, the menu text and keyboard accelerator. It is a child
		  of given parent and named specified name. If toggle is true the action
		  will be a toggle action, otherwise it will be a command action.
*/

QtxAction::QtxAction( const QString& text, const QString& menuText, int accel,
                      QObject* parent, const char* name, bool toggle )
: QAction( text, menuText, accel, parent, name, toggle )
{
}

/*!
	Name: ~QtxAction [virtual public]
	Desc: Destructor.
*/

QtxAction::~QtxAction()
{
}

/*!
	Name: addTo [virtual public]
	Desc: Adds this action to widget. Returns true if the action was added
		  successfully and false otherwise.
*/

bool QtxAction::addTo( QWidget* w )
{
  return QAction::addTo( w );
}

/*!
	Name: addTo [virtual public]
	Desc: Adds this action to widget. If widget is QPopupMenu given index will
		  be used for menu item inserting. Returns true if the action was added
		  successfully and false otherwise.
*/

bool QtxAction::addTo( QWidget* w, int index )
{
  if ( !addTo( w ) )
    return false;

  if ( w->inherits( "QPopupMenu" ) )
  {
    QPopupMenu* popup = (QPopupMenu*)w;
    if ( index < (int)popup->count() - 1 )
    {
      int id = popup->idAt( popup->count() - 1 );
      if ( id != -1 )
      {
			  QMenuItem* item = popup->findItem( id );
				if ( item && item->isSeparator() )
				{
					popup->removeItem( id );
          popup->insertSeparator( index );
				}
				else
				{
					QPopupMenu* p = item ? item->popup() : 0;
					int accel = popup->accel( id );
					bool isOn = popup->isItemEnabled( id );
					QString text = popup->text( id );
					QIconSet icon;
					if ( popup->iconSet( id ) )
						icon = *popup->iconSet( id );
					popup->removeItem( id );
					int pos;
					if ( icon.isNull() )
						if ( p )
							pos = popup->indexOf( popup->insertItem( text, p, id, index ) );
						else
							pos = popup->indexOf( popup->insertItem( text, id, index ) );
					else
						if ( p )
							pos = popup->indexOf( popup->insertItem( icon, text, p, id, index ) );
						else
							pos = popup->indexOf( popup->insertItem( icon, text, p, id, index ) );
					popup->setId( pos, id );
					popup->setAccel( accel, id );
					popup->setItemEnabled( id, isOn );
					if ( !p )
						popup->connectItem( id, this, SLOT( internalActivation() ) );
				}
      }
    }
  }

  return true;
}

/*!
	Name: setPopup [virtual public]
	Desc: Set or unset the sub popup menu for item with specified id in the given popup.
*/

void QtxAction::setPopup( QPopupMenu* popup, const int id, QPopupMenu* subPopup ) const
{
  if ( !popup )
    return;

  QMenuData* md = 0;
  const QMenuData* pmd = popup;
  QMenuItem* item = popup->findItem( id, &md );
  if ( !item || md != pmd )
    return;

  QPopupMenu* oldPopup = item->popup();
  if ( oldPopup == subPopup )
    return;

  int accel = popup->accel( id );
  bool isOn = popup->isItemEnabled( id );
  QString text = popup->text( id );
  QIconSet icon;
	if ( popup->iconSet( id ) )
    icon = *popup->iconSet( id );
  popup->removeItem( id );

  int pos;
  if ( icon.isNull() )
    pos = popup->indexOf( subPopup ? popup->insertItem( text, subPopup ) : popup->insertItem( text ) );
  else
    pos = popup->indexOf( subPopup ? popup->insertItem( icon, text, subPopup ) : popup->insertItem( icon, text ) );

  popup->setId( pos, id );
  popup->setAccel( accel, id );
  popup->setItemEnabled( id, isOn );

  delete oldPopup;
}
