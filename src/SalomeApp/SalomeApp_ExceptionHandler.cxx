#include "SalomeApp_ExceptionHandler.h"

#include <OSD.hxx>

#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx>

#include <stdexcept>
#include <exception>

#include <qstring.h>

#include <CASCatch_CatchSignals.hxx>
#include <CASCatch_ErrorHandler.hxx>
#include <CASCatch_Failure.hxx> 


/*!Constructor. Initialize by \a floatSignal.*/
SalomeApp_ExceptionHandler::SalomeApp_ExceptionHandler( const bool floatSignal )
: SUIT_ExceptionHandler()
{
  OSD::SetSignal( floatSignal );
}

/*!Try to call SUIT_ExceptionHandler::internalHandle(o, e), catch if failure.*/
bool SalomeApp_ExceptionHandler::handleSignals( QObject* o, QEvent* e )
{

  CASCatch_CatchSignals aCatchSignals;
  aCatchSignals.Activate();
    
    
  CASCatch_TRY {   
    SUIT_ExceptionHandler::internalHandle( o, e );
  }
  CASCatch_CATCH(CASCatch_Failure) {
    aCatchSignals.Deactivate();
    Handle(CASCatch_Failure) aFail = CASCatch_Failure::Caught();          
    throw std::runtime_error( aFail->GetError() );
  }
  
  aCatchSignals.Deactivate();   
  return true;
}

#ifdef try
#undef try
#endif

#ifdef catch
#undef catch
#endif

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
  return new SalomeApp_ExceptionHandler( true );
}
