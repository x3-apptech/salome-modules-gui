// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#include "SalomeApp_ExceptionHandler.h"
#include "CASCatch.hxx"

#include <OSD.hxx>

#include <stdexcept>
#include <exception>

#include <qstring.h>

/*!Constructor. Initialize by \a floatSignal.*/
SalomeApp_ExceptionHandler::SalomeApp_ExceptionHandler( const bool floatSignal )
: SUIT_ExceptionHandler()
{
  OSD::SetSignal( floatSignal );
}

/*!Try to call SUIT_ExceptionHandler::internalHandle(o, e), catch if failure.*/
bool SalomeApp_ExceptionHandler::handleSignals( QObject* o, QEvent* e )
{
  CASCatch_TRY {   
    SUIT_ExceptionHandler::internalHandle( o, e );
  }
  CASCatch_CATCH(Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();          
    throw Standard_Failure( aFail->GetMessageString() );
  }
  
  return true;
}

/*!Try to call handleSignals( o, e ), catch and show error message.*/
bool SalomeApp_ExceptionHandler::handle( QObject* o, QEvent* e )
{
  bool res = false;
  QString title( "Fatal error" );

  try {
    res = handleSignals( o, e );
  }
  catch( std::exception& ex )
  {
    showMessage( title, QString( ex.what() ) );
  }
  catch( Standard_Failure& e )
  {
    showMessage( title, QString( e.GetMessageString() ) );
  }
#ifndef WNT
  catch(...)
  {
    showMessage( title, "Unknown Exception" );
  }
#endif

  return res;
}

/*!Create new SUIT_ExceptionHandler*/
extern "C" SALOMEAPP_EXPORT SUIT_ExceptionHandler* getExceptionHandler()
{
  // MSV 2006-04-26: work around PAL12004 "VTK window => SIGFPE Arithmetic Exception Detected"
  // We disable FPE signal as it was in earlier versions of SALOME. It is enabled
  // only in debug mode if the environment variable DISABLE_FPE is not set to 1.
  bool raiseFPE;
#ifdef _DEBUG_
  raiseFPE = true;
  char* envDisableFPE = getenv("DISABLE_FPE");
  if (envDisableFPE && atoi(envDisableFPE))
    raiseFPE = false;
#else
  raiseFPE = false;
#endif
  return new SalomeApp_ExceptionHandler( raiseFPE );
}
