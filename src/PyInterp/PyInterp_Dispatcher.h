//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  File   : PyInterp_Dispatcher.h
//  Author : Sergey Anikin, OCC
//  Module : SALOME
//
#ifndef PYINTERP_DISPATCHER_H
#define PYINTERP_DISPATCHER_H

#include "PyInterp.h"   // !!! WARNING !!! THIS INCLUDE MUST BE THE VERY FIRST !!!

#include <QMutex>
#include <QThread>
#include <QEvent>
#include <QQueue>

class QObject;

class PyInterp_Interp;
class PyInterp_Watcher;
class PyInterp_Dispatcher;
class PyInterp_ExecuteEvent;

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

  virtual QEvent* createEvent() const;
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
    : PyInterp_Request( listener, sync ), myInterp( interp ) {}

protected:
  PyInterp_Interp*  getInterp() const { return myInterp; }

  virtual void      safeExecute();

private:
  PyInterp_Interp*  myInterp;
};

class PYINTERP_EXPORT PyInterp_Event : public QEvent
{
  PyInterp_Event();
  PyInterp_Event( const PyInterp_Event& );

public:
  //Execution state
  enum { ES_NOTIFY = QEvent::User + 5000, ES_OK, ES_ERROR, ES_INCOMPLETE, ES_LAST };

  PyInterp_Event( int type, PyInterp_Request* request )
    : QEvent( (QEvent::Type)type ), myRequest( request ) {}

  virtual ~PyInterp_Event();

  PyInterp_Request* GetRequest() const { return myRequest; }
  operator PyInterp_Request*() const { return myRequest; }

private:
  PyInterp_Request* myRequest;
};

class PYINTERP_EXPORT PyInterp_Dispatcher : protected QThread
{
  PyInterp_Dispatcher(); // private constructor

public:
  static PyInterp_Dispatcher* Get();

  virtual                     ~PyInterp_Dispatcher();

  bool                        IsBusy() const;  
  void                        Exec( PyInterp_Request* );

private:
  virtual void                run();
  void                        processRequest( PyInterp_Request* );
  void                        objectDestroyed( const QObject* );

private:
  typedef PyInterp_Request*   RequestPtr;

  QQueue<RequestPtr>          myQueue;
  QMutex                      myQueueMutex;
  PyInterp_Watcher*           myWatcher;

  static PyInterp_Dispatcher* myInstance;

  friend class PyInterp_Watcher;
};

#endif // PYINTERP_DISPATCHER_H
