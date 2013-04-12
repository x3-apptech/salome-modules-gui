
// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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
//  File   : PyInterp_Request.h
//  Author : Sergey Anikin, OCC, Adrien Bruneton (CEA/DEN)
//  Module : GUI

#ifndef PYINTERP_EVENT_H
#define PYINTERP_EVENT_H

#include "PyInterp.h"

#include <SALOME_Event.h>

#include <QEvent>

class PyInterp_Request;

class PyInterp_ExecuteEvent: public SALOME_Event
{
public:
  PyInterp_ExecuteEvent( PyInterp_Request* r )
    : myRequest( r ) {}

  virtual void Execute();

protected:
  PyInterp_Request* myRequest;
};

/**
 * Events thrown by the interpreter having executed a command and indicating
 * the return status.
 */
class PYINTERP_EXPORT PyInterp_Event : public QEvent
{
  PyInterp_Event();
  PyInterp_Event( const PyInterp_Event& );

public:
  //Execution state
  enum { ES_NOTIFY = QEvent::User + 5000, ES_OK, ES_ERROR, ES_INCOMPLETE,
         ES_TAB_COMPLETE_OK, ES_TAB_COMPLETE_ERR, ES_LAST };

  PyInterp_Event( int type, PyInterp_Request* request )
    : QEvent( (QEvent::Type)type ), myRequest( request ) {}

  virtual ~PyInterp_Event();

  PyInterp_Request* GetRequest() const { return myRequest; }
  operator PyInterp_Request*() const { return myRequest; }

private:
  PyInterp_Request* myRequest;
};

#endif // PYINTERP_EVENT_H
