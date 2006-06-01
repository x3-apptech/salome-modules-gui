//  KERNEL SALOME_Event : Define event posting mechanism
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : SALOME_Event.cxx
//  Author : Sergey ANIKIN
//  Module : KERNEL
//  $Header$

#include "SALOME_Event.hxx"

//#include "utilities.h"

#include <qsemaphore.h>
#include <qapplication.h>
#include <qthread.h>

// asv 21.02.05 : introducing multi-platform approach of thread comparison
// on Unix using pthread_t type for storing ThreadId
// on Win32 using integer type for storing ThreadId
// NOT using integer ThreadId on both Unix and Win32 because (from documentation):
// "...Do not allow your program to rely on the internal structure or size of the pthread_t..."

#ifdef WIN32
#include <windows.h>

static DWORD myThread;
#else
#include <qthread.h>
#include <pthread.h>

static pthread_t myThread;
#endif

/*!
  \return thread id
*/
void SALOME_Event::GetSessionThread(){
#ifdef WIN32
  myThread = ::GetCurrentThreadId();
#else
  myThread = pthread_self();
#endif
}

/*!
  \return true if it is session thread
*/
bool SALOME_Event::IsSessionThread(){
  bool aResult = false;
#ifdef WIN32
  aResult = myThread == ::GetCurrentThreadId();
#else
  aResult = myThread == pthread_self();
#endif
//  if(MYDEBUG) INFOS("IsSessionThread() - "<<aResult);
  return aResult;
}


/*!
  Constructor
*/
SALOME_Event::SALOME_Event(){
//  if(MYDEBUG) MESSAGE( "SALOME_Event::SALOME_Event(): this = "<<this );
  // Prepare the semaphore 
  mySemaphore = new QSemaphore( 2 );
  *mySemaphore += 2;
}

/*!
  Destructor
*/
SALOME_Event::~SALOME_Event(){
//  if(MYDEBUG) MESSAGE( "SALOME_Event::~SALOME_Event(): this = "<<this );
  if ( mySemaphore->available() < mySemaphore->total() )
    *mySemaphore -= mySemaphore->total() - mySemaphore->available();
  delete mySemaphore;
}

/*!
  Posts the event and optionally waits for its completion
*/
void SALOME_Event::process()
{
  QThread::postEvent( qApp, new QCustomEvent( SALOME_EVENT, (void*)this ) );
//  if(MYDEBUG) MESSAGE( "SALOME_Event::process(): this = "<<this<<", *mySemaphore += 1 " );
  *mySemaphore += 1;
//  if(MYDEBUG) MESSAGE( "SALOME_Event::process(): this = "<<this<<" - COMPLETED" );
}

/*!
  Signals that this event has been processed
*/
void SALOME_Event::processed()
{
//  if(MYDEBUG) MESSAGE( "SALOME_Event::processed(): this = "<<this );
  // process() takes control over mySemaphore after the next line is executed
  *mySemaphore -= 1;
}
