// Copyright (C) 2007-2021  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "QtxMsgHandler.h"

/*!
  \brief Custom Qt messages handler.
  
  To install message handler use qInstallMessageHandler() function:

  \code
  #include <QtxMsgHandler.h>
  qInstallMessageHandler(QtxMsgHandler);
  \code

  To process Qt message implement a callback class by inheriting from
  QtxMsgHandlerCallback and override its qtMessage() function.

  \sa QtxMsgHandlerCallback
*/
void QtxMsgHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
  foreach(QtxMsgHandlerCallback* callback, QtxMsgHandlerCallback::callbacks)
  {
    callback->qtMessage( type, context, message );
  }
}

/*!
  \class QtxMsgHandlerCallback
  \brief A callback object to handle Qt messages.

  The QtxMsgHandlerCallback class works in conjunction with QtxMsgHandler()
  function which is a message handler itself. 

  Implement your own callback class by inheriting from QtxMsgHandlerCallback
  and override its qtMessage() function. Default implementation does nothing.

  \sa QtxMsgHandler()
*/

QList<QtxMsgHandlerCallback*> QtxMsgHandlerCallback::callbacks;

/*!
  \brief Create new callback instance and activate it \a on is \c true.
  \param on Automatically activate callback on creation. Defaults to \c true.
  \sa activate()
*/
QtxMsgHandlerCallback::QtxMsgHandlerCallback(bool on)
{
  if ( on )
    activate();
}

/*!
  \brief Deactivate and destroy callback instance.
  \sa deactivate()
*/
QtxMsgHandlerCallback::~QtxMsgHandlerCallback()
{
  deactivate();
}

/*!
  \brief Activate this callback instance in the message handler.
  \sa deactivate()
*/
void QtxMsgHandlerCallback::activate()
{
  if ( !callbacks.contains( this ) )
    callbacks.push_back( this );
}

/*!
  \brief Deactivate this callback instance from the message handler.
  \sa activate()
*/
void QtxMsgHandlerCallback::deactivate()
{
  if ( callbacks.contains( this ) )
    callbacks.removeAll( this );
}

/*!
  \brief This function is called when a new Qt message is reported.

  Override this method in your custom callback class to handle Qt
  messages.

  Default implementation does nothing.

  \param type Qt message type.
  \param context Message context.
  \param message Message text.
*/
void QtxMsgHandlerCallback::qtMessage(QtMsgType /*type*/,
				      const QMessageLogContext& /*context*/,
				      const QString& /*message*/)
{
}
