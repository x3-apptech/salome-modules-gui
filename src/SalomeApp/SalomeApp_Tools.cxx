#include "SalomeApp_Tools.h"

#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>

#include <utilities.h>

Quantity_Color SalomeApp_Tools::color( const QColor& c )
{
	Quantity_Color aColor;
	if ( c.isValid() )
		aColor = Quantity_Color( c.red() / 255., c.green() / 255., c.blue() / 255., Quantity_TOC_RGB );
	return aColor;
}

QColor SalomeApp_Tools::color( const Quantity_Color& c )
{
	return QColor( (int)( c.Red() * 255 ), (int)( c.Green() * 255 ), (int)( c.Blue() * 255 ) );
}

QString SalomeApp_Tools::ExceptionToString( const SALOME::SALOME_Exception& S_ex )
{
  QString message;
  
  switch ( S_ex.details.type )
  {
  case SALOME::COMM:
  case SALOME::INTERNAL_ERROR:
    {
	    message = QString( S_ex.details.text );
	    QString source( S_ex.details.sourceFile );
	    QString line;
	    line.setNum( S_ex.details.lineNumber );
	    message = message + " \n" + source + " : " + line;
      break;
    }
  case SALOME::BAD_PARAM:
    {
	    message = QString( S_ex.details.text );
#ifdef _DEBUG_
	    QString source( S_ex.details.sourceFile );
	    QString line;
	    line.setNum( S_ex.details.lineNumber );
	    message = message + " \n" + source + " : " + line;
#endif
	    break;
    }
  default:
    {
	    message = QString( "SALOME CORBA Exception Type invalid" );
	    QString source( S_ex.details.sourceFile );
	    QString line;
	    line.setNum( S_ex.details.lineNumber );
	    message = message + " \n" + source + " : " + line;
	    break;
    }
  }
  return message;
}

void SalomeApp_Tools::QtCatchCorbaException( const SALOME::SALOME_Exception& S_ex )
{
  QString message = ExceptionToString( S_ex );

  QString title;
  bool error = true;
  switch ( S_ex.details.type )
  {
  case SALOME::COMM:
  case SALOME::INTERNAL_ERROR:
    title = QObject::tr( "Engine Error" );
    break;
  case SALOME::BAD_PARAM:
    error = false;
    title = QObject::tr( "Engine Warning" );
	  break;
  default:
    title = QObject::tr( "Internal SALOME Error" );
    break;
  }

  if ( error )
    SUIT_MessageBox::error1( SUIT_Session::session()->activeApplication()->desktop(),
	                           title, message, QObject::tr( "OK" ) );
  else
    SUIT_MessageBox::warn1( SUIT_Session::session()->activeApplication()->desktop(),
	                          title, message, QObject::tr( "OK" ) );

}
