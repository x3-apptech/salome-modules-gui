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
//  Author : Sergey Anikin (OCC), Adrien Bruneton (CEA/DEN)
//  Module : GUI

#ifndef PYINTERP_REQUEST_H
#define PYINTERP_REQUEST_H

#include "PyInterp.h"
#include "PyInterp_Event.h"

#include <QMutex>
#include <QObject>

class PyInterp_Interp;
class PyInterp_Watcher;
class PyInterp_Dispatcher;
class PyInterp_ExecuteEvent;
class PyConsole_Editor;

class PYINTERP_EXPORT PyInterp_Request
{
  friend class PyInterp_Dispatcher;
  friend class PyInterp_ExecuteEvent;

  PyInterp_Request();
  PyInterp_Request( const PyInterp_Request& );

protected:
  virtual ~PyInterp_Request() {};
  // protected destructor - to control deletion of requests

public:
  PyInterp_Request( QObject* listener, bool sync = false )
    : myIsSync( sync ), myListener( listener ) {};

  static void     Destroy( PyInterp_Request* );
  // Deletes a request

  bool            IsSync() const { return myIsSync; }
  // Returns true if this request should be processed synchronously,
  // without putting it to a queue

protected:
  virtual void    safeExecute();

  virtual void    execute() = 0;
  // Should be redefined in successors, contains actual request code

  virtual QEvent* createEvent();
  // This method can be overridden to customize notification event creation

  virtual void    processEvent( QObject* );

  QObject*        listener() const { return myListener; }
  void            setListener( QObject* );

private:
  void            process();

private:
  QMutex          myMutex;
  bool            myIsSync;
  QObject*        myListener;
};

class PYINTERP_EXPORT PyInterp_LockRequest : public PyInterp_Request
{
public:

  PyInterp_LockRequest( PyInterp_Interp* interp, QObject* listener = 0, bool sync = false )
    : PyInterp_Request( listener, sync ), myInterp( interp )
  {}

protected:
  PyInterp_Interp*  getInterp() const { return myInterp; }

  virtual void      safeExecute();

private:
  PyInterp_Interp*  myInterp;
};

#endif // PYINTERP_REQUEST_H
