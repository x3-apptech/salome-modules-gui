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

#include "SUIT_ShortcutMgr.h"

#include "SUIT_Session.h"
#include "SUIT_ResourceMgr.h"

#include <QtxAction.h>

#include <QApplication>
#include <QActionEvent>

SUIT_ShortcutMgr* SUIT_ShortcutMgr::myShortcutMgr = NULL;

/*!
  \brief Constructor
*/
SUIT_ShortcutMgr::SUIT_ShortcutMgr()
: QObject()
{
  qApp->installEventFilter( this );
}

/*!
  \brief Destructor
*/
SUIT_ShortcutMgr::~SUIT_ShortcutMgr()
{
  qApp->removeEventFilter( this );
}

/*!
  \brief Create new instance of shortcut manager.
*/
void SUIT_ShortcutMgr::Init()
{
  if( myShortcutMgr==NULL )
    myShortcutMgr = new SUIT_ShortcutMgr();
}

/*!
  \brief Return shortcut manager. 
*/
SUIT_ShortcutMgr* SUIT_ShortcutMgr::getShortcutMgr()
{
  Init();
  
  return myShortcutMgr;
}

/*!
  \brief Custom event filter for qapplication .
  
  Redefined from QObject::eventFilter();
*/
bool SUIT_ShortcutMgr::eventFilter( QObject* o, QEvent* e )
{
  if ( e->type() == QEvent::ActionAdded ) {
    QActionEvent* anActionEvent = (QActionEvent*)e;
    if (anActionEvent) {
      QtxAction* anAction = qobject_cast<QtxAction*>( anActionEvent->action() );
      if ( anAction )
	processAction( anAction );
    }
  }

  return QObject::eventFilter( o, e );
}

/*!
  \brief Return key sequence for shortcut action name.
  \param actionName name of shortcut action in preferences
  \return key sequence defined in preferences or empty sequence
*/
QKeySequence SUIT_ShortcutMgr::getShortcutByActionName( const QString& actionName ) const
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  QString section = actionName.section( resMgr->sectionsToken(), 0, 0 );
  section.prepend( QString("shortcuts") + resMgr->sectionsToken() );
  QString parameter = actionName.section( resMgr->sectionsToken(), 1, 1 );

  QString shortcutValue;
  bool hasValue = resMgr->value( section, parameter, shortcutValue, false );

  if ( !hasValue )
    return QKeySequence();

  return QKeySequence::fromString( shortcutValue );
}

/*!
  \brief Set shortcut to the given action if the shortcut is defined.
  \param action action to process
 */
void SUIT_ShortcutMgr::processAction( QtxAction* action )
{
  QString shortcutActionName = action->shortcutActionName();
 
  if ( !shortcutActionName.isEmpty() ) {
    // Add action to the actions map
    if ( !myShortcutActions.contains( shortcutActionName, action ) ) {
      myShortcutActions.insert( shortcutActionName, action );
      connect( action, SIGNAL( destroyed( QObject* ) ), 
	       this, SLOT ( onActionDestroyed( QObject* ) ) );
    }

    QKeySequence keySeq = getShortcutByActionName( shortcutActionName );
    action->setShortcut( keySeq );
  }
}

/*!
  \brief Enable/disable a shortcuts section.

  Enables or disables actions which belong to the given shortcuts section.
  Only actions which have an active desktop as a parent widget 
  are taken into account.

  \param section shorcuts section
  \param on if \c true - action will be enabled, otherwise - disabled
*/
void SUIT_ShortcutMgr::setSectionEnabled( const QString& section, const bool on )
{
  QMap<QString, QtxAction*>::ConstIterator it;
  for ( it = myShortcutActions.constBegin(); it != myShortcutActions.constEnd(); ++it ) {
    QtxAction* action = it.value();
    QString shortcutActionName = action->shortcutActionName();
    QString actionSection = shortcutActionName.section( ":", 0, 0 );
    if ( actionSection == section ) {
      // Check if the action parent widget equals to the active desktop
      SUIT_Application* app = SUIT_Session::session()->activeApplication();
      if ( !app )
	return;
      if ( action->parentWidget() == (QWidget*)app->desktop() )
	action->setEnabled( on );
    }
  }
}

/*!
  \brief Update shortcuts from preferences.
*/
void SUIT_ShortcutMgr::updateShortcuts()
{
  QMap<QString, QtxAction*>::ConstIterator it;
  for ( it = myShortcutActions.constBegin(); it != myShortcutActions.constEnd(); ++it ) {
    QtxAction* action = it.value();
    QKeySequence keySeq = getShortcutByActionName( action->shortcutActionName() );
    action->setShortcut( keySeq );
  }
}

/*!
  \brief Called when the corresponding action is destroyed.
  
  Removes destroyed action from the actions list.

  \param obj action being destroyed
*/
void SUIT_ShortcutMgr::onActionDestroyed( QObject* obj )
{
  QtxAction* anAction = (QtxAction*)obj;
  
  if ( anAction )
    myShortcutActions.remove( anAction->shortcutActionName(), anAction );
}
