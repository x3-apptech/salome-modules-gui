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
// See http://www.salome-platform.org/
//
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

/*!\class SUIT_ViewWindow
 * Class provide view window.
 */

/*! Dump view custom event*/
const int DUMP_EVENT = QEvent::User + 123;

/*! Constructor.*/
SUIT_ViewWindow::SUIT_ViewWindow(SUIT_Desktop* theDesktop)
: QMainWindow( theDesktop, "SUIT_ViewWindow", Qt::WDestructiveClose )
{
  myDesktop = theDesktop;

  if ( myDesktop->icon() )
    setIcon( *myDesktop->icon() );
}

/*! Destructor.*/
SUIT_ViewWindow::~SUIT_ViewWindow()
{
}

/*! Close event \a theEvent.
*/
void SUIT_ViewWindow::closeEvent(QCloseEvent* theEvent)
{
  QMainWindow::closeEvent( theEvent );
  emit closing( this );
}

/*! Context menu requested for event \a e.
*/
void SUIT_ViewWindow::contextMenuEvent ( QContextMenuEvent * e )
{
  if ( e->reason() != QContextMenuEvent::Mouse )
    emit contextMenuRequested( e );
}

/*! Post events on dump view.
*/
void SUIT_ViewWindow::onDumpView()
{
  qApp->postEvent( this, new QPaintEvent( QRect( 0, 0, width(), height() ), TRUE ) );
  qApp->postEvent( this, new QCustomEvent( DUMP_EVENT ) );
}

/*! Reaction view window on event \a e.
*/
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
        if ( !fileName.isEmpty() ) {
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

/*! Called by SUIT_Accel::onActivated() when a key accelerator was activated and this window was active
*/
void SUIT_ViewWindow::onAccelAction( int _action )
{
  action( _action );
}

/*! action  handle standard action (zoom, pan) or custom action.  to be redefined in successors.
*/
void SUIT_ViewWindow::action( const int  )
{
}
