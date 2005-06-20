// SUIT_ViewWindow.cxx: implementation of the SUIT_ViewWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "SUIT_ViewWindow.h"
#include "SUIT_Desktop.h"
#include "SUIT_Application.h"
#include "SUIT_Study.h"
#include "SUIT_ViewManager.h"
#include "SUIT_Tools.h"
#include "SUIT_MessageBox.h"
#include "qhbox.h"
#include "qpopupmenu.h"
#include "qapplication.h"

// Dump view custom event
const int DUMP_EVENT = QEvent::User + 123;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SUIT_ViewWindow::SUIT_ViewWindow(SUIT_Desktop* theDesktop)
: QMainWindow( theDesktop, "SUIT_ViewWindow", Qt::WDestructiveClose )
{
  myDesktop = theDesktop;
}

SUIT_ViewWindow::~SUIT_ViewWindow()
{
}

//***************************************************************
void SUIT_ViewWindow::closeEvent(QCloseEvent* theEvent)
{
  QMainWindow::closeEvent( theEvent );
  emit closing( this );
}

//****************************************************************
void SUIT_ViewWindow::contextMenuEvent ( QContextMenuEvent * e )
{
  if ( e->reason() != QContextMenuEvent::Mouse )
    emit contextMenuRequested( e );
}

//****************************************************************
void SUIT_ViewWindow::onDumpView()
{
  qApp->postEvent( this, new QPaintEvent( QRect( 0, 0, width(), height() ), TRUE ) );
  qApp->postEvent( this, new QCustomEvent( DUMP_EVENT ) );
}

//****************************************************************
bool SUIT_ViewWindow::event( QEvent* e )
{
  if ( e->type() == DUMP_EVENT ) {
    bool bOk = false;
    if ( myManager && myManager->study() && myManager->study()->application() ) {
      // first create an image (this is small trick to avoid dialog box overlapping)
      QImage img = dumpView();
      if ( !img.isNull() ) {
        // get file name
        QString fileName = myManager->study()->application()->getFileName( false, QString::null, tr( "TLT_IMAGE_FILES" ), tr( "TLT_DUMP_VIEW" ), 0 );
        if ( fileName ) {
          QString fmt = SUIT_Tools::extension( fileName ).upper();
          if ( fmt.isEmpty() ) fmt = QString( "BMP" ); // default format
	  if ( fmt == "JPG" )  fmt = "JPEG";
          QApplication::setOverrideCursor( Qt::waitCursor );
	  bOk = img.save( fileName, fmt.latin1() );
	  QApplication::restoreOverrideCursor();
        }
	else {
          bOk = true; // cancelled
        }
      }
    }
    if ( !bOk ) {
      SUIT_MessageBox::error1( this, tr( "ERROR" ), tr( "ERR_CANT_DUMP_VIEW" ), tr( "BUT_OK" ) );
    }
    return TRUE;
  }
  return QMainWindow::event( e );
}

//****************************************************************
