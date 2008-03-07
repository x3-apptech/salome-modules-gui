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
// File:      QtxAction.cxx
// Author:    Sergey TELKOV

#include "QtxAction.h"

#include <qpopupmenu.h>
#include <qmenubar.h>

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
  if ( w->inherits( "QMenuBar" ) ) {
    // --- Add action to the QMenuBar ---
    // n.b. currently for the actions inserted to the menu bar 
    // the following properties are not supported:
    // * tooltips
    // * what's this info
    // * toggle mode
    QMenuBar* mb = (QMenuBar*)w;
    if ( myMenuIds.find( w ) != myMenuIds.end() )
      return false;                        // already added
    if ( name() == "qt_separator_action" ) // separator
      myMenuIds[ w ] = mb->insertSeparator();
    else if ( iconSet().isNull() )         // has no icon
      myMenuIds[ w ] = mb->insertItem( menuText(), this, SIGNAL( activated() ), accel() );
    else                                   // has icon
      myMenuIds[ w ] = mb->insertItem( iconSet(), menuText(), this, SIGNAL( activated() ), accel() );
    mb->setItemEnabled( myMenuIds[ w ], isEnabled() );
    mb->setItemVisible( myMenuIds[ w ], isVisible() );
    return true;
  }
  return QAction::addTo( w );
}

/*!
	Name: addTo [virtual public]
	Desc: Adds this action to widget. If widget is QPopupMenu given index will
		  be used for menu item inserting. Returns true if the action was added
		  successfully and false otherwise.
*/

bool QtxAction::addTo( QWidget* w, const int index )
{
  if ( !addTo( w ) )
    return false;

  if ( w->inherits( "QPopupMenu" ) ) {
    // --- Add action to the QPopupMenu ---
    QPopupMenu* popup = (QPopupMenu*)w;
    if ( index >= 0 && index < (int)popup->count() - 1 ) {
      int id = popup->idAt( popup->count() - 1 );
      if ( id != -1 ) {
	QMenuItem* item = popup->findItem( id );
	if ( item && item->isSeparator() ) {
	  popup->removeItem( id );
	  popup->insertSeparator( index );
	}
	else {
	  QPopupMenu* p = item ? item->popup() : 0;
	  int accel = popup->accel( id );
	  bool isOn = popup->isItemEnabled( id );
	  bool isVisible = popup->isItemVisible( id );
	  bool isChecked = popup->isItemChecked( id );
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
	  popup->setItemVisible( id, isVisible );
	  popup->setItemChecked( id, isChecked );
	  if ( !whatsThis().isEmpty() )
	    popup->setWhatsThis( id, whatsThis() );
	  if ( !p )
	    popup->connectItem( id, this, SLOT( internalActivation() ) );
	}
      }
    }
  }
  else if ( w->inherits( "QMenuBar" ) ) {
    // --- Add action to the QMenuBar ---
    QMenuBar* mb = (QMenuBar*)w;
    if ( index >= 0 && index < (int)mb->count() - 1 ) {
      int id = mb->idAt( mb->count() - 1 );
      if ( id != -1 ) {
	QMenuItem* item = mb->findItem( id );
	if ( item && item->isSeparator() ) {
	  mb->removeItem( id );
	  mb->insertSeparator( index );
	}
	else {
	  QPopupMenu* p = item ? item->popup() : 0;
	  int accel = mb->accel( id );
	  bool isOn = mb->isItemEnabled( id );
	  bool isVisible = mb->isItemVisible( id );
	  QString text = mb->text( id );
	  QIconSet icon;
	  if ( mb->iconSet( id ) )
	    icon = *mb->iconSet( id );
	  mb->removeItem( id );
	  int pos;
	  if ( icon.isNull() )
	    if ( p )
	      pos = mb->indexOf( mb->insertItem( text, p, id, index ) );
	    else
	      pos = mb->indexOf( mb->insertItem( text, id, index ) );
	  else
	    if ( p )
	      pos = mb->indexOf( mb->insertItem( icon, text, p, id, index ) );
	    else
	      pos = mb->indexOf( mb->insertItem( icon, text, p, id, index ) );
	  mb->setId( pos, id );
	  mb->setAccel( accel, id );
	  mb->setItemEnabled( id, isOn );
	  mb->setItemVisible( id, isVisible );
	  if ( !p )
	    mb->connectItem( id, this, SIGNAL( activated() ) );
	}
      }
    }
  }
  return true;
}

/*!
	Name: removeFrom [virtual public]
	Desc: Removes this action from widget. Returns true if the action was removed
		  successfully and false otherwise.
*/

bool QtxAction::removeFrom( QWidget* w )
{
  // check if widget is QMenuBar
  if ( w->inherits( "QMenuBar" ) ) {
    QMenuBar* mb = (QMenuBar*)w;
    if ( myMenuIds.find( w ) == myMenuIds.end() )
      return false;  // not yet added
    mb->removeItem( myMenuIds[ w ] );
    myMenuIds.remove( w );
    return true;
  }
  return QAction::removeFrom( w );
}

/*!
	Name: setPopup [virtual public]
	Desc: Set or unset the sub popup menu for item with specified id in the given popup.
*/

void QtxAction::setPopup( QWidget* w, const int id, QPopupMenu* subPopup ) const
{
  if ( !w )
    return;

  QMenuData* pmd = 0;

  if ( w->inherits( "QPopupMenu" ) )
    pmd = ::qt_cast<QPopupMenu*>( w );
  else if ( w->inherits( "QMenuBar" ) )
    pmd = ::qt_cast<QMenuBar*>( w );

  if ( !pmd )
    return;  // bad widget

  QMenuData* md = 0;
  QMenuItem* item = pmd->findItem( id, &md );
  if ( !item || md != pmd )
    return;  // item is not found

  QPopupMenu* oldPopup = item->popup();
  if ( oldPopup == subPopup )
    return;  // popup is not changed

  // get properties
  int accel = pmd->accel( id );
  bool isOn = pmd->isItemEnabled( id );
  bool isVisible = pmd->isItemVisible( id );
  int pos = pmd->indexOf( id );
  QString text = pmd->text( id );
  QIconSet icon;
  if ( pmd->iconSet( id ) )
    icon = *pmd->iconSet( id );

  // remove previous item
  pmd->removeItem( id );

  // add new item
  if ( w->inherits( "QPopupMenu" ) ) {
    // --- QPopupMenu ---
    QPopupMenu* popup = (QPopupMenu*)w;
    if ( icon.isNull() )
      pos = popup->indexOf( subPopup ? popup->insertItem( text, subPopup, id, pos ) :
			               popup->insertItem( text, id, pos ) );
    else
      pos = popup->indexOf( subPopup ? popup->insertItem( icon, text, subPopup, id, pos ) : 
			               popup->insertItem( icon, text, id, pos ) );
  }
  else {
    // --- QMenuBar ---
    QMenuBar* mb = (QMenuBar*)w;
    if ( icon.isNull() )
      pos = mb->indexOf( subPopup ? mb->insertItem( text, subPopup, id, pos ) : 
 			            mb->insertItem( text, id, pos ) );
    else
      pos = mb->indexOf( subPopup ? mb->insertItem( icon, text, subPopup, id, pos ) : 
 			            mb->insertItem( icon, text, id, pos ) );
  }

  // restore properties
  pmd->setId( pos, id ); // for sure (if id < 0)
  pmd->setAccel( accel, id );
  pmd->setItemEnabled( id, isOn );
  pmd->setItemVisible( id, isVisible );

  // delete old popup
  delete oldPopup;
}

