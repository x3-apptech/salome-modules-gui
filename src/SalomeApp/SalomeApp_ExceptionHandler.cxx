#include "SalomeApp_ExceptionHandler.h"

#include <OSD.hxx>

#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx>

#include <stdexcept>
#include <exception>

#include <qstring.h>

SalomeApp_ExceptionHandler::SalomeApp_ExceptionHandler( const bool floatSignal )
: SUIT_ExceptionHandler()
{
  OSD::SetSignal( floatSignal );
}

bool SalomeApp_ExceptionHandler::handleSignals( QObject* o, QEvent* e )
{
  try {
    SUIT_ExceptionHandler::internalHandle( o, e );
  }
  catch( Standard_Failure )
  {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    throw std::runtime_error( aFail->GetMessageString() );
  }
  return true;
}

#ifdef try
#undef try
#endif

#ifdef catch
#undef catch
#endif

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

extern "C" SALOMEAPP_EXPORT SUIT_ExceptionHandler* getExceptionHandler()
{
  return new SalomeApp_ExceptionHandler( true );
}
