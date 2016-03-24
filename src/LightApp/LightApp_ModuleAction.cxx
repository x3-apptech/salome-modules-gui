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

// File   : LightApp_ModuleAction.cxx
// Author : Sergey TELKOV, Vadim SANDLER
//
#include "LightApp_ModuleAction.h"

#include <QtxComboBox.h>
#include <QtxActionSet.h>
#include <QVBoxLayout>
#include <QApplication>
#include <QEvent>

/*!
  \class LightApp_ModuleAction::ActionSet
  \brief Internal class to represent list of modules buttons.
  \internal
*/

class LightApp_ModuleAction::ActionSet : public QtxActionSet
{
public:
  ActionSet( QObject* );
  QAction* moduleAction( const QString& ) const;
  int      moduleId( const QString& ) const;
  int      moduleId( QAction* ) const;
  void     setVisible( bool );
};

/*!
  \brief Constructor.
  \internal
  \param parent parent object
*/
LightApp_ModuleAction::ActionSet::ActionSet( QObject* parent )
: QtxActionSet( parent ) 
{
}

/*!
  \brief Get action corresponding to the specified module.
  \internal
  \param name module name
  \return module action or 0 if \a name is invalid
*/
QAction* LightApp_ModuleAction::ActionSet::moduleAction( const QString& name ) const
{
  QAction* a = 0;

  QList<QAction*> alist = actions();
  for ( QList<QAction*>::const_iterator it = alist.begin(); it != alist.end() && !a; ++it )
  {
    if ( (*it)->text() == name )
      a = *it;
  }

  return a;
}

/*!
  \brief Get module action identifier.
  \internal
  \param name module name
  \return module action ID or -1 if \a name is invalid
*/
int LightApp_ModuleAction::ActionSet::moduleId( const QString& name ) const
{
  int id = -1;

  QList<QAction*> alist = actions();
  for ( QList<QAction*>::const_iterator it = alist.begin(); it != alist.end() && id == -1; ++it )
  {
    if ( (*it)->text() == name )
      id = actionId( *it );
  }

  return id;
}

/*!
  \brief Get module action identifier.
  \internal
  \param a module action
  \return module action ID or -1 if \a a is null or invalid
*/
int LightApp_ModuleAction::ActionSet::moduleId( QAction* a ) const
{
  return actionId( a );
}

/*!
  \brief Show/hide modules actions.
  \internal
  \param on new visibility state
*/
void LightApp_ModuleAction::ActionSet::setVisible( bool on )
{
  QList<QAction*> alist = actions();
  for ( QList<QAction*>::const_iterator it = alist.begin(); it != alist.end(); ++it )
    (*it)->setVisible( on );

  QtxActionSet::setVisible( on );
}

/*!
  \class LightApp_ModuleAction::ComboAction
  \brief Internal class to represent combo box with the list of modules in the toolbar.
  \internal
*/

/*!
  \brief Constructor.
  \internal
  \param parent parent object
*/
LightApp_ModuleAction::ComboAction::ComboAction( QObject* parent )
: QtxAction( parent )
{
}

/*!
  \brief Get list of associated widgets.
  \internal
  \return list of created widgets (QtxComboBox)
*/
QList<QtxComboBox*> LightApp_ModuleAction::ComboAction::widgets() const
{
  QList<QtxComboBox*> lst;

  QList<QWidget*> wlist = createdWidgets();
  for ( QList<QWidget*>::const_iterator wit = wlist.begin(); wit != wlist.end(); ++wit )
    lst += (*wit)->findChildren<QtxComboBox*>();

  return lst;
}

/*!
  \brief Create combo box widget by request from the toolbar.
  \internal
  \param parent parent widget (should be QToolBar or its successor)
  \return new custom widget, containing combo box
*/
QWidget* LightApp_ModuleAction::ComboAction::createWidget( QWidget* parent )
{
  if ( !parent->inherits( "QToolBar" ) )
    return 0;

  QWidget* dumb = new QWidget( parent );
  QVBoxLayout* l = new QVBoxLayout( dumb );
  l->setSpacing( 0 ); l->setMargin( 0 );
  QtxComboBox* cb = new QtxComboBox( dumb );
  cb->setSizeAdjustPolicy( QComboBox::AdjustToContents );
  cb->setFocusPolicy( Qt::NoFocus );
  l->addWidget( cb );
  l->addSpacing( 3 );

  connect( cb, SIGNAL( activatedId( int ) ), this, SIGNAL( activatedId( int ) ) );

  return dumb;
}

/*!
  \fn void LightApp_ModuleAction::ComboAction::activatedId( int id );
  \internal
  \brief Emitted when the combo box item is activated
  \param item identifier
*/

/*!
  \class LightApp_ModuleAction::ActivateEvent
  \brief Internal class to represent custom event for transfer the activation item id.
  \internal
*/
class LightApp_ModuleAction::ActivateEvent : public QEvent
{
public:
  ActivateEvent( QEvent::Type type, int id ) : QEvent( type ), myId( id ) {};
  ~ActivateEvent() {};

  int     id() const { return myId; }

private:
  int     myId;
};

/*!
  \class LightApp_ModuleAction
  \brief An action, representing the list of modules to be inserted to the
  toolbar.

  This action is represented in the toolbar as combo box and a set of buttons 
  for each module. In addition to the modules items, the combo box contains 
  an item corresponding to the "neutral point" of the application 
  (when there is no active module).
  
  The action can be constructed with up to two parameters, defining the text
  and icon to be displayed for the "neutral point".

  Only one module can be active at the moment. It can be set programmatically 
  with setActiveModule() function. Use this method with empty string to turn
  to the "neutral point". To get active module, use activeModule() function.

  When user activates/deactivates any module, the signal moduleActivated() 
  is emitted.

  The action can be represented in the toolbar in different modes:
  * as combo box only (Qtx::ComboItem)
  * as set of modules buttons only (Qtx::Buttons)
  * as combo box followed by the set of modules buttons (Qtx::All)
  * as none (Qtx::None)
  By default, both combo box and buttons set are shown. Use method 
  setMode() to change this behavior.

  An action can be also added to the popup menu, but combo box is never shown
  in this case, only modules buttons.
*/

/*!
  \brief Constructor

  Creates an module action with "neutral point" item described by \a text.

  \param text "neutral point" item's text
  \param parent parent object
*/
LightApp_ModuleAction::LightApp_ModuleAction( const QString& text, QObject* parent )
: QtxAction( parent )
{
  setText( text );
  init();
}

/*!
  \brief Constructor

  Creates an module action with "neutral point" item described by \a text and \a ico.

  \param text "neutral point" item's text
  \param ico "neutral point" item's icon
  \param parent parent object
*/
LightApp_ModuleAction::LightApp_ModuleAction( const QString& text, const QIcon& ico, QObject* parent )
: QtxAction( parent )
{
  setText( text );
  setIcon( ico );
  init();
}

/*!
  \brief Destructor
*/
LightApp_ModuleAction::~LightApp_ModuleAction()
{
}

/*!
  \brief Get list of modules.
  \return modules names list
*/
QStringList LightApp_ModuleAction::modules() const
{
  QStringList lst;

  QList<QAction*> alist = mySet->actions();
  for ( QList<QAction*>::const_iterator it = alist.begin(); it != alist.end(); ++it )
    lst.append( (*it)->text() );

  return lst;
}

/*!
  \brief Get module icon.
  \param name module name
  \return module icon
  \sa setModuleIcon()
*/
QIcon LightApp_ModuleAction::moduleIcon( const QString& name ) const
{
  QAction* a = mySet->moduleAction( name );
  return a ? a->icon() : QIcon();
}

/*!
  \brief Set module icon.
  \param name module name
  \param ico new module icon
  \sa moduleIcon()
*/
void LightApp_ModuleAction::setModuleIcon( const QString& name, const QIcon& ico )
{
  QAction* a = mySet->moduleAction( name );
  if ( !a )
    return;

  a->setIcon( ico );
  update();
}

/*!
  \brief Add module into the list.
  \param name module name
  \param ico module icon
  \param idx position in the module list (if -1, the module is added to the end of list)
  \sa removeModule()
*/
void LightApp_ModuleAction::insertModule( const QString& name, const QIcon& ico,
                                          const int idx )
{
  QtxAction* a = new QtxAction( name, ico, name, 0, this, true );
  a->setStatusTip( tr( "ACTIVATE_MODULE_TOP" ).arg( name ) );

  mySet->insertAction( a, -1, idx );
  update();
}

/*!
  \brief Remove module from the list.
  \param name module name
  \sa insertModule()
*/
void LightApp_ModuleAction::removeModule( const QString& name )
{
  int id = mySet->moduleId( name );
  if ( id == -1 )
    return;

  mySet->removeAction( id );
  update();
}

/*!
  \brief Get active module.

  If there is no active module ("neutral point"), then the null string 
  is returned.

  \return active module name
  \sa setActiveModule()
*/
QString LightApp_ModuleAction::activeModule() const
{
  QAction* a = active();
  return a ? a->text() : QString();
}

/*!
  \brief Set active module.

  To turn to the "neutral point" (no active module), pass empty string.

  \param name new active module name
  \sa activeModule()
*/
void LightApp_ModuleAction::setActiveModule( const QString& name )
{
  if ( name == activeModule() )
    return;

  int id = mySet->moduleId( name );
  if ( name.isEmpty() || id != -1 )
    activate( id, false );
}

/*!
  \brief Set action display mode.

  Action can be represented in the toolbar as
  * combo box only (Qtx::ComboItem)
  * set of modules buttons only (Qtx::Buttons)
  * combo box followed by the set of modules buttons (Qtx::All)
  * none (Qtx::None)

  \param mode action display mode
  \sa mode()
*/
void LightApp_ModuleAction::setMode( const int mode )
{
  myMode = mode;
  update();
}

/*!
  \brief Get action display mode.
  \param mode action display mode
  \sa setMode()
*/
int LightApp_ModuleAction::mode() const
{
  return myMode;
}

/*!
  \brief Called when the action is added to the widget.
  \param w widget (not used)
*/
void LightApp_ModuleAction::addedTo( QWidget* w )
{
  if ( w->inherits( "QToolBar" ) )
    w->insertAction( this, myCombo );
  w->insertAction( this, mySet );
  update();
}

/*!
  \brief Remove action from widget.
  \param w widget (menu or toolbar)
  \return \c true if the action is removed successfully and \c false otherwise.
  \sa addTo()
*/
void LightApp_ModuleAction::removedFrom( QWidget* w )
{
  if ( w->inherits( "QToolBar" ) )
    w->removeAction( myCombo );
  w->removeAction( mySet );
}

/*!
  \brief Perform delayed activation with specified id.
  \param e custom event
  \return \c true if the event was processed successfully and \c false otherwise.
*/
bool LightApp_ModuleAction::event( QEvent* e )
{
  if ( e->type() == QEvent::MaxUser ) {
    activate( ((ActivateEvent*)e)->id(), false );
    return true;
  }
  return QtxAction::event( e );
}

/*!
  \fn void LightApp_ModuleAction::moduleActivated( const QString& name );
  \brief Emitted when the module is activated
  \param name module name (empty string for neutral point)
*/

/*!
  \brief Initialize an action,
  \internal
*/
void LightApp_ModuleAction::init()
{
  setVisible( false );

  myMode = All;
  myCombo = new ComboAction( this );
  mySet = new ActionSet( this );

  connect( this,    SIGNAL( changed() ),          this, SLOT( onChanged() ) );
  connect( mySet,   SIGNAL( triggered( int ) ),   this, SLOT( onTriggered( int ) ) );
  connect( myCombo, SIGNAL( activatedId( int ) ), this, SLOT( onComboActivated( int ) ) );
}

/*!
  \brief Update an action.
  \internal
*/
void LightApp_ModuleAction::update()
{
  QList<QtxComboBox*> lst = myCombo->widgets();
  for ( QList<QtxComboBox*>::const_iterator it = lst.begin(); it != lst.end(); ++it )
    update( *it );

  myCombo->setVisible( myMode & ComboItem );
  mySet->setVisible( myMode & Buttons );
}

/*!
  \brief Update combo box.
  \internal
  \param cb combo box
*/
void LightApp_ModuleAction::update( QtxComboBox* cb )
{
  if ( !cb )
    return;

  bool blocked = cb->blockSignals( true );
  int curId = mySet->moduleId( active() );
  QList<QAction*> alist = mySet->actions();
  cb->clear();
  
  cb->addItem( icon(), text() );
  cb->setId( 0, -1 );

  for ( QList<QAction*>::const_iterator it = alist.begin(); it != alist.end(); ++it )
  {
    QAction* a = *it;
    int id = mySet->moduleId( a );
    cb->addItem( a->icon(), a->text() );
    cb->setId( cb->count() - 1, id );
  }

  cb->setCurrentId( curId );
  cb->blockSignals( blocked );
}

/*!
  \brief Get an action corresponding to the active module.
  \internal
  \return active module action or 0 if there is no active module
*/
QAction* LightApp_ModuleAction::active() const
{
  QAction* a = 0;

  QList<QAction*> alist = mySet->actions();
  for ( QList<QAction*>::const_iterator it = alist.begin(); it != alist.end() && !a; ++it )
  {
    if ( (*it)->isChecked() )
      a = *it;
  }

  return a;
}

/*!
  \brief Activate a module item.
  \internal
  \param id module identifier
  \param fromAction \c true if function is called from the module button
*/
void LightApp_ModuleAction::activate( int id, bool fromAction )
{
  bool checked = false;

  QList<QAction*> alist = mySet->actions();
  for ( QList<QAction*>::const_iterator it = alist.begin(); it != alist.end(); ++it )
  {
    if ( mySet->moduleId( *it ) != id ) {
      (*it)->setChecked( false );
    }
    else {
      if ( !fromAction )
        (*it)->setChecked( true );
      checked = (*it)->isChecked();
    }
  }

  QList<QtxComboBox*> widgets = myCombo->widgets();
  for ( QList<QtxComboBox*>::const_iterator wit = widgets.begin(); wit != widgets.end(); ++wit )
  {
    QtxComboBox* cb = *wit;
    bool blocked = cb->signalsBlocked();
    cb->blockSignals( true );
    cb->setCurrentId( checked ? id : -1 );
    cb->blockSignals( blocked );
  }

  emit moduleActivated( activeModule() );
}

/*!
  \brief Called when module button is triggered.
  \internal
  \param id module identifier
*/
void LightApp_ModuleAction::onTriggered( int id )
{
  activate( id );
}

/*!
  \brief Called when action state is changed.
  \internal
  
  This slot is used to prevent making the parent action visible.
*/
void LightApp_ModuleAction::onChanged()
{
  if ( !isVisible() )
    return;

  bool block = signalsBlocked();
  blockSignals( true );
  setVisible( false );
  blockSignals( block );
}

/*!
  \brief Called when combo box item is activated.
  \param id module identifier
*/
void LightApp_ModuleAction::onComboActivated( int id )
{
  QApplication::postEvent( this, new ActivateEvent( QEvent::MaxUser, id ) );
} 
