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

#include "SalomeApp_ExceptionHandler.h"
#include "Utils_CorbaException.hxx"

#include "CASCatch.hxx"

#include <OSD.hxx>

#include <stdexcept>
#include <exception>

#include <QString>

#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>


/*!Constructor. Initialize by \a floatSignal.*/
SalomeApp_ExceptionHandler::SalomeApp_ExceptionHandler( const bool floatSignal )
: SUIT_ExceptionHandler()
{
  // JFA 2006-09-28: PAL10867: suppress signal catching,
  // if environment variable DISABLE_SIGNALS_CATCHING is set to 1.
  // Commonly this is used with "noexcepthandler" option.
  char* envNoCatchSignals = getenv("NOT_INTERCEPT_SIGNALS");
  if (!envNoCatchSignals || !atoi(envNoCatchSignals))
  {
    OSD::SetSignal( floatSignal );
  }
}

/*!Try to call SUIT_ExceptionHandler::internalHandle(o, e), catch if failure.*/
bool SalomeApp_ExceptionHandler::handleSignals( QObject* o, QEvent* e )
{
  try {
    OCC_CATCH_SIGNALS;
    SUIT_ExceptionHandler::internalHandle( o, e );
  }
  catch(Standard_Failure) {
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
  catch( SALOME::SALOME_Exception& ex)
  {
    showMessage( title, QString( ex.details.text));
  }
#ifndef WIN32
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
  raiseFPE = false;

#if defined(_DEBUG_) | defined(_DEBUG) //the Last for WIN32 default settings
  char* envEnableFPE = getenv("ENABLE_FPE");
  if (envEnableFPE && atoi(envEnableFPE))
    raiseFPE = true;
#endif

  return new SalomeApp_ExceptionHandler( raiseFPE );
}
