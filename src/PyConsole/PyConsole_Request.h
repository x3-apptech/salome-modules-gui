// Copyright (C) 2007-2015  CEA/DEN, EDF R&D, OPEN CASCADE
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
// Author : Adrien Bruneton (CEA/DEN)
// Created on: 3 avr. 2013


#ifndef PYCONSOLE_REQUEST_H_
#define PYCONSOLE_REQUEST_H_

#include "PyInterp_Request.h"

#include <vector>
#include <QString>
#include <QEvent>

class PyInterp_Interp;

/*!
  \class ExecCommand
  \brief Python command execution request.
  \internal
*/
class ExecCommand : public PyInterp_LockRequest
{
public:
  /*!
    \brief Constructor.

    Creates new python command execution request.
    \param theInterp   python interpreter
    \param theCommand  python command
    \param theListener widget to get the notification messages
    \param sync        if True the request is processed synchronously
  */
  ExecCommand( PyInterp_Interp*        theInterp,
               const QString&          theCommand,
               QObject*                theListener,
               bool                    theSync = false );

protected:
  /*!
    \brief Execute the python command in the interpreter and
           get its execution status.
  */
  virtual void execute();

  /*!
    \brief Create and return a notification event.
    \return new notification event
  */
  virtual QEvent* createEvent();

private:
  QString myCommand;   //!< Python command
  int     myState;     //!< Python command execution status
};

class CompletionCommand : public PyInterp_LockRequest
{
public:
  CompletionCommand( PyInterp_Interp*     theInterp,
		     const QString&       theInput,
		     const QString&       theStartMatch,
		     QObject*             theListener,
		     bool                 theSync = false );


protected:
  /** List of separators identifying the last parsable token for completion */
  static const std::vector<QString> SEPARATORS;

  /** String to be passed to the dir() command */
  QString _dirArg;
  /** Begining of the command (as typed by the user) */
  QString _startMatch;
  /** was the completion command successful */
  bool _tabSuccess;

  virtual void execute();
  virtual QEvent* createEvent();
};

#endif /* PYCONSOLE_REQUEST_H_ */
