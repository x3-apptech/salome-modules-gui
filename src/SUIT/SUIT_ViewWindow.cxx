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
// SUIT_ViewWindow.cxx: implementation of the SUIT_ViewWindow class.
//

#include "SUIT_ViewWindow.h"
#include "SUIT_Desktop.h"
#include "SUIT_Application.h"
#include "SUIT_Study.h"
#include "SUIT_ViewManager.h"
#include "SUIT_Tools.h"
#include "SUIT_MessageBox.h"
#include <qhbox.h>
#include <qpopupmenu.h>
#include <qapplication.h>
#include <qimage.h>

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

/*!
  Sets new view manager for window
  \param theManager - new view manager
*/
void SUIT_ViewWindow::setViewManager( SUIT_ViewManager* theManager )
{
  myManager = theManager;
}

/*!
  \return view manager of window
*/
SUIT_ViewManager* SUIT_ViewWindow::getViewManager() const
{
  return myManager;
}

/*!
  \return QImage, containing all scene rendering in window
*/
QImage SUIT_ViewWindow::dumpView()
{
  return QImage();
}

/*!
  Saves image to file according to the format
  \param image - image
  \param fileName - name of file
  \param format - string contains name of format (for example, "BMP"(default) or "JPEG", "JPG")
*/
bool SUIT_ViewWindow::dumpViewToFormat( const QImage& img, const QString& fileName, const QString& format )
{
  if( img.isNull() )
    return false; 

  QString fmt = format;
  if( fmt.isEmpty() )
    fmt = QString( "BMP" ); // default format
  else if( fmt == "JPG" )
    fmt = "JPEG";

  QApplication::setOverrideCursor( Qt::waitCursor );
  bool res = img.save( fileName, fmt.latin1() );
  QApplication::restoreOverrideCursor();
  return res;
}

/*!
  Saves scene rendering in window to file
  \param fileName - name of file
  \param format - string contains name of format (for example, "BMP"(default) or "JPEG", "JPG")
*/
bool SUIT_ViewWindow::dumpViewToFormat( const QString& fileName, const QString& format )
{
  return dumpViewToFormat( dumpView(), fileName, format );
}

/*!
  Set or clear flag Qt::WDestructiveClose
*/
void SUIT_ViewWindow::setDestructiveClose( const bool on )
{
  if ( on )
    setWFlags( WDestructiveClose );
  else
    clearWFlags( WDestructiveClose );
}

/*! Close event \a theEvent.
*/
void SUIT_ViewWindow::closeEvent(QCloseEvent* theEvent)
{
//  QMainWindow::closeEvent( theEvent );
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

/*!
  \return filters for image files
*/
QString SUIT_ViewWindow::filter() const
{
  return tr( "TLT_IMAGE_FILES" );
}

/*! Reaction view window on event \a e.
*/
bool SUIT_ViewWindow::event( QEvent* e )
{
  if ( e->type() == DUMP_EVENT )
  {
    bool bOk = false;
    if ( myManager && myManager->study() && myManager->study()->application() )
    {
      QImage im = dumpView();

      // get file name
      SUIT_Application* app = myManager->study()->application();
      QString fileName = app->getFileName( false, QString::null, filter(), tr( "TLT_DUMP_VIEW" ), 0 );
      if( !fileName.isEmpty() )
      {
	QString fmt = SUIT_Tools::extension( fileName ).upper();
	bOk = dumpViewToFormat( im, fileName, fmt );
      }
      else
      {
	bOk = true; // cancelled
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
bool SUIT_ViewWindow::onAccelAction( int _action )
{
  return action( _action );
}

/*! action  handle standard action (zoom, pan) or custom action.  to be redefined in successors.
*/
bool SUIT_ViewWindow::action( const int  )
{
  return true;
}

/*!
  \return string containing visual parameters of window
*/
QString   SUIT_ViewWindow::getVisualParameters()
{
  return "empty";
}

/*!
  Sets visual parameters of window by its string representation
  \param parameters - string with visual parameters
*/ 
void SUIT_ViewWindow::setVisualParameters( const QString& parameters )
{
}
