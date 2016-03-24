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

// File:      QtxAction.cxx
// Author:    Sergey TELKOV
//
#include "QtxAction.h"

#include <QEvent>
#include <QPointer>
#include <QActionEvent>
#include <QApplication>

/*!
  \class QtxAction::ActionNotify
  \brief Notify event used to signalize about event adding/removing.
  \internal
*/

class QtxAction::ActionNotify : public QEvent
{
public:
  ActionNotify( bool add, QWidget* wid ) : QEvent( QEvent::User ), myAdd( add ), myWidget( wid ) {};
  ~ActionNotify() {};

  bool              isAdded() const { return myAdd; }
  QWidget*          widget() const { return myWidget; }

private:
  bool              myAdd;
  QPointer<QWidget> myWidget;
};

/*!
  \class QtxAction
  \brief Generic action class.

  The class QtxAction inherits QWidgetAction class and can be used 
  as base class when implementing any custom menu/toolbar actions.
  It is necessary to subclass from QtxAction and redefine virtual 
  callback methods addedTo(), removedFrom() (which are called automatically
  when the action is added to the widget and removed from it) to customize
  the action behavior.  
*/

/*!
  \brief Constructor.

  Creates an action owned by \a parent. 
  Parameter \a toggle can be used to make the action checkable.
  Parameter \a shortcutAction can be used to assign the shortcut from
  preferences. This parameter value corresponds to shortcut action identifier
  in shortcut preferences.

  \param parent parent object
  \param toggle if \c true the action will be a toggle action
  \param shortcutAction shortcut action identifier
*/
QtxAction::QtxAction( QObject* parent, bool toggle, const QString& shortcutAction )
: QWidgetAction( parent )
{
  setCheckable( toggle );
  setShortcutActionName(shortcutAction);

  QApplication::instance()->installEventFilter( this );
}

/*!
  \brief Constructor.

  Creates an action owned by \a parent. Parameters \a text,
  \a icon, \a menuText and \a accel specify the action's attributes.
  Parameter \a toggle can be used to make the action checkable.
  Parameter \a shortcutAction can be used to assign the shortcut from
  preferences. This parameter value corresponds to shortcut action identifier
  in shortcut preferences.

  \param text tooltip text
  \param icon iconset
  \param menuText menu text
  \param accel shortcut key sequence
  \param parent parent object
  \param toggle if \c true the action will be a toggle action
  \param shortcutAction shortcut action identifier
*/
QtxAction::QtxAction( const QString& text, const QIcon& icon, const QString& menuText, 
		      int accel, QObject* parent, bool toggle, const QString& shortcutAction )
: QWidgetAction( parent )
{
  setIcon( icon );
  setText( menuText );
  setToolTip( text );
  setShortcut( accel );
  setCheckable( toggle );
  setShortcutActionName(shortcutAction);

  QApplication::instance()->installEventFilter( this );
}

/*!
  \brief Constructor.

  Creates an action owned by \a parent. Parameters \a text,
  \a icon, \a menuText and \a accel specify the action's attributes.
  Parameter \a toggle can be used to make the action checkable.
  Parameter \a shortcutAction can be used to assign the shortcut from
  preferences. This parameter value corresponds to shortcut action identifier
  in shortcut preferences.

  \param text tooltip text
  \param icon iconset
  \param menuText menu text
  \param accel shortcut key sequence
  \param parent parent object
  \param toggle if \c true the action will be a toggle action
  \param shortcutAction shortcut action identifier
*/
QtxAction::QtxAction( const QString& text, const QIcon& icon, const QString& menuText, 
		      const QKeySequence& accel, QObject* parent, bool toggle, const QString& shortcutAction )
: QWidgetAction( parent )
{
  setIcon( icon );
  setText( menuText );
  setToolTip( text );
  setShortcut( accel );
  setCheckable( toggle );
  setShortcutActionName(shortcutAction);

  QApplication::instance()->installEventFilter( this );
}

/*!
  \brief Constructor.

  Creates an action owned by \a parent. Parameters \a text,
  \a menuText and \a accel specify the action's attributes.
  Parameter \a toggle can be used to make the action checkable.
  Parameter \a shortcutAction can be used to assign the shortcut from
  preferences. This parameter value corresponds to shortcut action identifier
  in shortcut preferences.

  \param text tooltip text
  \param menuText menu text
  \param accel shortcut key sequence
  \param parent parent object
  \param toggle if \c true the action is a toggle action
  \param shortcutAction shortcut action identifier
*/
QtxAction::QtxAction( const QString& text, const QString& menuText,
                      int accel, QObject* parent, bool toggle, const QString& shortcutAction )
: QWidgetAction( parent )
{
  setText( menuText );
  setToolTip( text );
  setShortcut( accel );
  setCheckable( toggle );
  setShortcutActionName(shortcutAction);
  
  QApplication::instance()->installEventFilter( this );
}

/*!
  \brief Constructor.

  Creates an action owned by \a parent. Parameters \a text,
  \a menuText and \a accel specify the action's attributes.
  Parameter \a toggle can be used to make the action checkable.
  Parameter \a shortcutAction can be used to assign the shortcut from
  preferences. This parameter value corresponds to shortcut action identifier
  in shortcut preferences.

  \param text tooltip text
  \param menuText menu text
  \param accel shortcut key sequence
  \param parent parent object
  \param toggle if \c true the action is a toggle action
  \param shortcutAction shortcut action identifier
*/
QtxAction::QtxAction( const QString& text, const QString& menuText,
                      const QKeySequence& accel, QObject* parent, bool toggle, const QString& shortcutAction )
: QWidgetAction( parent )
{
  setText( menuText );
  setToolTip( text );
  setShortcut( accel );
  setCheckable( toggle );
  setShortcutActionName(shortcutAction);
  
  QApplication::instance()->installEventFilter( this );
}

/*!
  \brief Destructor.
*/
QtxAction::~QtxAction()
{
}

/*!
  \brief Customize action events.
  
  Sends a notification event to the action when it is added to 
  the widget or removed from it in order to perform custom processing.

  \param o object
  \param e event
  \return \c true if further event processing should be stopped
  \sa customEvent(), addedTo(), removedFrom()
*/
bool QtxAction::eventFilter( QObject* o, QEvent* e )
{
  if ( o && o->isWidgetType() )
  {
    if ( e->type() == QEvent::ActionAdded && ((QActionEvent*)e)->action() == this )
      QApplication::postEvent( this, new ActionNotify( true, (QWidget*)o ) );
    if ( e->type() == QEvent::ActionRemoved && ((QActionEvent*)e)->action() == this )
      QApplication::postEvent( this, new ActionNotify( false, (QWidget*)o ) );
  }
  return QWidgetAction::eventFilter( o, e );
}

/*!
  \brief Called when the action is added to the widget.

  This method can be redefined in the subclasses to customize 
  the action behavior. Base implementation does nothing. 

  \param w widget (should be menu or toolbar)
  \sa removedFrom()
*/
void QtxAction::addedTo( QWidget* /*w*/ )
{
}

/*!
  \brief Called when the action is removed from the widget.

  This method can be redefined in the subclasses to customize
  the action behavior. Base implementation does nothing.

  \param w widget (should be menu or toolbar)
  \sa addedTo()
*/
void QtxAction::removedFrom( QWidget* /*w*/ )
{
}

/*!
  \brief Process notification events.
  
  Calls addedTo() method when the action is added to the widget
  and removedFrom() when it is removed from the widget
  in order to perform custom processing.

  \param e noification event
  \sa eventFilter(), addedTo(), removedFrom()
*/
void QtxAction::customEvent( QEvent* e )
{
  ActionNotify* ae = (ActionNotify*)e;
  if ( !ae->widget() )
    return;

  if ( ae->isAdded() )
    addedTo( ae->widget() );
  else
    removedFrom( ae->widget() );
}

/*!
  \brief Return shortcut action name for the action.
  
  \return shortcut action name
  \sa setShortcutActionName()
*/
QString QtxAction::shortcutActionName() const
{
  return myShortcutActionName;
}

/*!
  \brief Set shortcut action name to the action.

  Shortcut action name is used for shortcuts customization.

  \param shortcutAction shortcut action name
  \sa shortcutActionName()
*/
void QtxAction::setShortcutActionName( const QString& shortcutAction )
{
  myShortcutActionName = shortcutAction;
}
