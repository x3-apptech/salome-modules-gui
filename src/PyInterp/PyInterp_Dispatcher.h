//  SALOME SALOMEGUI : implementation of desktop and GUI kernel
//
//  Copyright (C) 2005  CEA/DEN, EDF R&D
//
//
//
//  File   : PyInterp_Dispatcher.h
//  Author : Sergey Anikin, OCC
//  Module : SALOME
//  $Header$

#ifndef _PYINTERP_DISPATCHER_H_
#define _PYINTERP_DISPATCHER_H_

#include "PyInterp.h"

#include <qthread.h>
#include <qevent.h>

#include <list>

class QObject;

class PyInterp_base;
class PyInterp_Watcher;
class PyInterp_Dispatcher;

class PYINTERP_EXPORT PyInterp_Request
{
  friend class PyInterp_Dispatcher;

  PyInterp_Request();
  PyInterp_Request( const PyInterp_Request& );

protected:
  virtual         ~PyInterp_Request() {}; 
  // protected destructor - to control deletion of requests

public:
  PyInterp_Request( QObject* listener, bool sync = false )
    : myIsSync( sync ), myListener( listener ), myEvent( 0 ) {};

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

private:
  void            process();
  QObject*        getListener() const { return myListener; }
  void            setListener( QObject* );
  QEvent*         getEvent();
  void            postEvent();

private:
  bool            myIsSync;
  QObject*        myListener;
  QEvent*         myEvent;
  QMutex          myMutex;
};

class PYINTERP_EXPORT PyInterp_LockRequest : public PyInterp_Request
{
public:
  PyInterp_LockRequest( PyInterp_base* interp, QObject* listener = 0, bool sync = false )
    : PyInterp_Request( listener, sync ), myInterp( interp ) {}

protected:
  PyInterp_base*  getInterp() const { return myInterp; }

  virtual void    safeExecute();

private:
  PyInterp_base*  myInterp;
};

class PYINTERP_EXPORT PyInterp_Event : public QCustomEvent
{
  PyInterp_Event();
  PyInterp_Event( const PyInterp_Event& );

public:
  enum { NOTIFY = QEvent::User + 5000, OK, ERROR, INCOMPLETE, LAST };

  PyInterp_Event( int type, PyInterp_Request* request )
    : QCustomEvent( (QEvent::Type)type ), myRequest( request ) {}

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

  std::list<RequestPtr>       myQueue;
  QMutex                      myQueueMutex;
  PyInterp_Watcher*           myWatcher;

  static PyInterp_Dispatcher* myInstance;

  friend class PyInterp_Watcher;
};

#endif
