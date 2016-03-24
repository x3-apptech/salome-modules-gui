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

//  KERNEL SALOME_Event : Define event posting mechanism
//  File   : SALOME_Event.cxx
//  Author : Sergey ANIKIN
//
#include "SALOME_Event.h"

#include <QSemaphore>
#include <QApplication>

// asv 21.02.05 : introducing multi-platform approach of thread comparison
// - on Unix using pthread_t type for storing ThreadId
// - on Win32 using integer type for storing ThreadId
// NOT using integer ThreadId on both Unix and Win32 because (from documentation):
// "...Do not allow your program to rely on the internal structure or size of the pthread_t..."

#ifdef WIN32
#include <windows.h>
static DWORD myThread;
#else
#include <pthread.h>
static pthread_t myThread;
#endif

/*!
  \class InitEvent
  \brief Helper event class responsible for initializing SALOME_Event
  mechanism by the main thread ID
 */
class InitEvent : public SALOME_Event
{
public:
  InitEvent();
  virtual      ~InitEvent();
  virtual void Execute();
};

/*!
  \brief Constructor, initializes the event mechanism by the current thread ID.
  It is asssumed to be the main thread ID, so be careful!
*/
InitEvent::InitEvent()
{
  GetSessionThread();
}

/*!
  \brief Destructor, does nothing.
*/
InitEvent::~InitEvent()
{
}

/*!
  \brief Nothing to be executed for this kind of event.
*/
void InitEvent::Execute()
{
}

// NOTE: Here the SALOME event mechanism is initalized by the 
// current thread ID that is always assumed to be the main thread ID.
// This should be revised as soon as the application library is no longer
// linked against the Event library (i.e. this static object is not created or created 
// outside the main thread).
static InitEvent myInitEvent;

/*!
  \class SALOME_CustomEvent
  \brief Generic event class for user-defined events
  
  This class contains a generic void* data member that may be used
  for transferring event-specific data to the receiver.

  \warning The internal data is not destroyed by the class destructor.
*/

/*!
  \brief Constructor.
  \param type event type
*/
SALOME_CustomEvent::SALOME_CustomEvent( int type )
: QEvent( (QEvent::Type)type ), d( 0 )
{
}

/*!
  \brief Constructor.
  \param type event type
  \param data custom data
*/
SALOME_CustomEvent::SALOME_CustomEvent( QEvent::Type type, void* data )
: QEvent( type ), d( data )
{
}

/*!
  \brief Get custom data.
  \return pointer to the internal data
*/
void* SALOME_CustomEvent::data() const
{
  return d;
}

/*!
  \brief Set custom data.
  \param data pointer to the internal data
*/
void SALOME_CustomEvent::setData( void* data )
{
  d = data;
}

/*!
  \class SALOME_Event
  \brief The class which encapsulates data and functionality required for 
         posting component-specific events to perform arbitrary operations 
         in the main GUI thread. 

  SALOME_Event objects can be posted by any thread belonging to the GUI process.
  
  It is necessary to derive a custom event class from SALOME_Event and 
  re-implement virtual Execute() method. This method should actually perform 
  the desirable operation. To pass all the required data to Execute() and 
  store a return value, arbitrary data fields can be added to the custom 
  event class. There is no need to protect such fields with a mutex, for only
  one thread working with a SALOME_Event object is active at any moment.
  
  Usage:
  - Create SALOME_Event. Components can derive their own event class from 
  SALOME_Event in order to pass custom data to the event handler.
  - Call process() method to post the event. After process() execution
  it is possible to examine fields of your custom event object.
  - Perform delete operator on the event to wake up the desktop (you can also 
  set <autoRelease>  parameter to \c true to automatically wake up desktop after 
  process().
  
  The method processed() is used by the desktop to signal that event processing 
  has been completed.

  To make all this work, it is necessary to call static method GetSessionThread()
  during the application initialization, i.e. from main() function.
  It is important to call this method from the primary application thread.

  Caveats: 
  - there are no.
*/

//! Total number of semaphore resources
const int NumberOfResources = 2;

/*!
  \brief Initialize event mechanism.

  This function sets up the main application thread. It should be called
  during the application initialization, i.e. main() function.
*/
void SALOME_Event::GetSessionThread(){
#ifdef WIN32
  myThread = ::GetCurrentThreadId();
#else
  myThread = pthread_self();
#endif
}

/*!
  \brief Check if the processing is in the main application thread.
  \return \c true if this method is called from the main application thread
*/
bool SALOME_Event::IsSessionThread(){
  bool aResult = false;
#ifdef WIN32
  aResult = myThread == ::GetCurrentThreadId();
#else
  aResult = myThread == pthread_self();
#endif
  return aResult;
}

/*!
  \brief Constructor.
*/
SALOME_Event::SALOME_Event(){
  // Prepare the semaphore 
  mySemaphore = new QSemaphore( NumberOfResources );
  mySemaphore->acquire( NumberOfResources );
}

/*!
  \brief Destructor.
*/
SALOME_Event::~SALOME_Event(){
  if ( mySemaphore->available() < NumberOfResources )
    mySemaphore->release( NumberOfResources - mySemaphore->available() );
  delete mySemaphore;
}

/*!
  \brief This method should be called by the main GUI thread
  in order to execute the code specific for this event and finally
  to inform the calling thread that the event 
  has been processed waking it up with help of the semaphore .
 */
void SALOME_Event::ExecutePostedEvent()
{
  // Diagnose incorrect usage of SALOME_Event API
  if ( !IsSessionThread() ){
    qWarning( "SALOME_Event::ExecutePostedEvent() is called from a secondary thread that might mean an error in application logic!" );
  }
  // Actual execution specific for particular kind of event
  Execute();
  // Signal the calling thread that the event has been processed
  processed();
}

/*!
  \brief Post the event and wait for its completion.
  process() should be called from a secondary thread only. 
  \sa processed()
*/
void SALOME_Event::process()
{
  // Diagnose incorrect usage of SALOME_Event API
  if ( IsSessionThread() ){
    qWarning( "SALOME_Event::process() is called from the main GUI thread that might mean an error in application logic!" );
  }

  QApplication::postEvent( qApp, new SALOME_CustomEvent( SALOME_EVENT, (void*)this ) );
  mySemaphore->acquire( 1 );
}

/*!
  \brief Use this method to signal that this event has been processed.
*/
void SALOME_Event::processed()
{
  mySemaphore->release( 1 );
}

/*!
  \fn virtual void SALOME_Event::Execute();
  \brief This method should be redefined in the successor classes
         to do real work.
*/
  
/*!
  \class TMemFunEvent
  \brief Template class for event which calls the function
  without arguments and returning result.
*/

/*!
  \class TVoidMemFunEvent
  \brief Template class for event which calls the function
  without arguments and without return value.
*/

/*!
  \class TMemFun1ArgEvent
  \brief Template class for event which calls the function
  with one argument and returning result.
*/

/*!
  \class TVoidMemFun1ArgEvent
  \brief Template class for event which calls the function
  with one argument and without return value.
*/

/*!
  \class TMemFun2ArgEvent
  \brief Template class for event which calls the function
  with two arguments and returning result.
*/

/*!
  \class TVoidMemFun2ArgEvent
  \brief Template class for event which calls the function
  with two arguments and without return value.
*/

/*!
  \fn ProcessEvent
  \brief Template function for processing events with return value.
*/

/*!
  \fn ProcessVoidEvent
  \brief Template function for processing events without return value.
*/
