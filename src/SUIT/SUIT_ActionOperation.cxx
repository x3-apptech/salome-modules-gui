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

#include "SUIT_ActionOperation.h"

#include "SUIT_Application.h"

#include <QtxAction.h>

/*!
  Constructor.
*/
SUIT_ActionOperation::SUIT_ActionOperation( SUIT_Application* app )
: SUIT_Operation( app ),
myAction( 0 )
{
}

/*!
  Destructor.
*/
SUIT_ActionOperation::~SUIT_ActionOperation()
{
}

/*!
  Gets action.
*/
QtxAction* SUIT_ActionOperation::action() const
{
  return myAction;
}

/*!Set action.
 * Create new instance of QtxAction and set.
 */
void SUIT_ActionOperation::setAction( const QString& text, const QIcon& icon,
                                      const QString& menuText, QKeySequence accel,
                                      QObject* parent, bool toggle )
{
  setAction( new QtxAction( text, icon, menuText, accel, parent, toggle ) );
}

/*!Set action.
 * Create new instance of QtxAction and set.
 */
void SUIT_ActionOperation::setAction( const QString& text, const QString& menuText,
                                      QKeySequence accel, QObject* parent, bool toggle )
{
  setAction( new QtxAction( text, menuText, accel, parent, toggle ) );
}

/*!Set action.
 */
void SUIT_ActionOperation::setAction( QtxAction* a )
{
  if ( myAction == a )
    return;

  delete myAction;
  myAction = a;

  myAction->setEnabled( application()->activeStudy() );
  connect( myAction, SIGNAL( triggered() ), SLOT( start() ) );
}

/*! Add action to widget \a wid.
 *\retval true - successful, false - not successful.
 */
bool SUIT_ActionOperation::addTo( QWidget* wid )
{
  if ( !action() )
    return false;

  wid->addAction( action() );
  return true;
}

/*! Set status tip for action.
*/
void SUIT_ActionOperation::setStatusTip( const QString& tip )
{
  if ( action() )
    action()->setStatusTip( tip );
}
