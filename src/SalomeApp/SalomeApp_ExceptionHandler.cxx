// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "SalomeApp_ExceptionHandler.h"

#include "CASCatch.hxx"
#include "Basics_OCCTVersion.hxx"

#include <OSD.hxx>

#include <stdexcept>
#include <exception>

#include <QString>

#if OCC_VERSION_LARGE > 0x06010000
  #include <Standard_ErrorHandler.hxx>
  #include <Standard_Failure.hxx>
#else
  #include "CASCatch.hxx"
#endif

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
#if OCC_VERSION_LARGE > 0x06010000
  try {
    OCC_CATCH_SIGNALS;
#else
  CASCatch_TRY {
#endif
    SUIT_ExceptionHandler::internalHandle( o, e );
  }
#if OCC_VERSION_LARGE > 0x06010000
  catch(Standard_Failure) {
#else
  CASCatch_CATCH(Standard_Failure) {
#endif
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
#if defined(_DEBUG_) | defined(_DEBUG) //the Last for WNT default settings
  raiseFPE = true;
  char* envDisableFPE = getenv("DISABLE_FPE");
  if (envDisableFPE && atoi(envDisableFPE))
    raiseFPE = false;
#else
  raiseFPE = false;
#endif

  return new SalomeApp_ExceptionHandler( raiseFPE );
}
