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

// File:      QtxDockAction.cxx
// Author:    Sergey TELKOV
//
#include "QtxDockAction.h"

#include <QMenu>
#include <QToolBar>
#include <QDockWidget>
#include <QMainWindow>

/*!
  \class QtxDockAction
  \brief Dockable windows & toolbars list action.

  Implements the action which provides the popup menu with the list
  of toolbars and/or dockable windows list owned by the main window.
  This action can be used, for example, in the menu "Windows".
*/

/*!
  \brief Constructor.
  \param mw parent main window
*/
QtxDockAction::QtxDockAction( QMainWindow* mw )
: QtxAction( "Windows and Toolbars", "Windows and Toolbars", 0, mw ),
  myType( Both ),
  myMain( mw )
{
  initialize();
}

/*!
  \brief Constructor.
  \param text description (tooltip) text
  \param menuText menu text
  \param mw parent main window
*/
QtxDockAction::QtxDockAction( const QString& text, const QString& menuText, QMainWindow* mw )
: QtxAction( text, menuText, 0, mw ),
  myType( Both ),
  myMain( mw )
{
  initialize();
}

/*!
  \brief Constructor.
  \param text description (tooltip) text
  \param icon action icon
  \param menuText menu text
  \param mw parent main window
*/
QtxDockAction::QtxDockAction( const QString& text, const QIcon& icon, const QString& menuText, QMainWindow* mw )
: QtxAction( text, icon, menuText, 0, mw ),
  myType( Both ),
  myMain( mw )
{
  initialize();
}

/*!
  \brief Desctructor
  
  Does nothing currently.
*/
QtxDockAction::~QtxDockAction()
{
  // to avoid memory leak
  if( QMenu* aMenu = menu() )
  {
    delete aMenu;
    aMenu = 0;
  }
}

/*!
  \brief Update associated menu
*/
void QtxDockAction::update()
{
  updateMenu();
}

/*!
  \brief Get parent main window.
  \return main window pointer.
*/
QMainWindow* QtxDockAction::mainWindow() const
{
  return myMain;
}

/*!
  \brief Get dock action type.
  \return dock type (QtxDockAction::DockType)
*/
int QtxDockAction::dockType() const
{
  return myType;
}

/*!
  \brief Set dock action type.
  \param type dock type (QtxDockAction::DockType)
*/
void QtxDockAction::setDockType( const int type )
{
  myType = type;
}

/*!
  \brief Prepare popup menu with dock windows list when 
         parent popup menu is shown.
*/
void QtxDockAction::onAboutToShow()
{
  updateMenu();
  setVisible( menu() && !menu()->isEmpty() );
}

/*!
  \brief Get all toolbars owned by parent main window.
  \param lst returned list of all toolbars owned by main window
*/
void QtxDockAction::toolBars( QList<QToolBar*>& lst ) const
{
  lst.clear();

  QMainWindow* mw = mainWindow();
  if ( !mw )
    return;

  QList<QToolBar*> toolbars = mw->findChildren<QToolBar*>();
  for ( QList<QToolBar*>::iterator it = toolbars.begin(); it != toolbars.end(); ++it )
  {
    QToolBar* tb = *it;
    if ( tb->parentWidget() == mw )
      lst.append( tb );
  }
}

/*!
  \brief Get all dockable windows owned by parent main window.
  \param lst returned list of all dockable windows owned by main window
*/
void QtxDockAction::dockWidgets( QList<QDockWidget*>& lst ) const
{
  lst.clear();

  QMainWindow* mw = mainWindow();
  if ( !mw )
    return;

  QList<QDockWidget*> dockwidgets = mw->findChildren<QDockWidget*>();
  for ( QList<QDockWidget*>::iterator it = dockwidgets.begin(); it != dockwidgets.end(); ++it )
  {
    QDockWidget* dw = *it;
    if ( dw->parentWidget() == mw )
      lst.append( dw );
  }
}

/*
void QtxDockAction::updateInfo( QDockWindow* dw )
{
  QAction* a = action( dw );
  if ( !a )
    return;

  a->setText( dw->caption() );
  a->setMenuText( dw->caption() );

  if ( isToolBar( dw ) )
    a->setStatusTip( tr( "Toggles toolbar \"%1\" on/off" ).arg( dw->caption() ) );
  else
    a->setStatusTip( tr( "Toggles window \"%1\" on/off" ).arg( dw->caption() ) );

  const QPixmap* icon = dw->icon();
  if ( icon )
    a->setIconSet( *icon );
}
*/

/*!
  \brief Customize action adding to the widget operation.
  
  Called when the action is added to the widget.
  Reimplemented from QtxAction class.

  \param w widget this action is added to (menu or toolbar)
*/
void QtxDockAction::addedTo( QWidget* w )
{
  if ( w->inherits( "QMenu" ) )
    connect( w, SIGNAL( aboutToShow() ), this, SLOT( onAboutToShow() ) );
}

/*!
  \brief Customize action removing from the widget operation.
  
  Called when the action is removed from the widget.
  Reimplemented from QtxAction class.

  \param w widget this action is removed from to (menu or toolbar)
*/
void QtxDockAction::removedFrom( QWidget* w )
{
  if ( w->inherits( "QMenu" ) )
    disconnect( w, SIGNAL( aboutToShow() ), this, SLOT( onAboutToShow() ) );
}

/*!
  \brief Initialize the action.
*/
void QtxDockAction::initialize()
{
  setMenu( new QMenu( 0 ) );
}

/*!
  \brief Update action child popup menu.
*/
void QtxDockAction::updateMenu()
{
  QMenu* pm = menu();
  if ( !pm )
    return;

  pm->clear();

  if ( dockType() == DockWidget || dockType() == Both )
  {
    QList<QDockWidget*> dwList;
    dockWidgets( dwList );
    for ( QList<QDockWidget*>::iterator it = dwList.begin(); it != dwList.end(); ++it )
      pm->addAction( (*it)->toggleViewAction() );
  }

  pm->addSeparator();

  if ( dockType() == ToolBar || dockType() == Both )
  {
    QList<QToolBar*> tbList;
    toolBars( tbList );
    for ( QList<QToolBar*>::iterator it = tbList.begin(); it != tbList.end(); ++it )
      pm->addAction( (*it)->toggleViewAction() );
  }

  Qtx::simplifySeparators( pm );
}
