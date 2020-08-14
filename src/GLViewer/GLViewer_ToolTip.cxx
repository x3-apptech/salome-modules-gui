// Copyright (C) 2007-2020  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "GLViewer_Context.h"
#include "GLViewer_ToolTip.h"
#include "GLViewer_Viewer2d.h"
#include "GLViewer_ViewPort2d.h"
#include "GLViewer_ViewFrame.h"

#include <QLabel>
#include <QTimer>
#include <QBitmap>
#include <QApplication>
#include <QToolTip>
#include <QMouseEvent>

/*!
  constructor
*/
GLViewer_ObjectTip::GLViewer_ObjectTip( GLViewer_ViewPort2d* theParent )
:QObject(),
 myPoint( -1, -1 )
{
  mypViewPort = theParent;
  mypLabel = new QLabel( "Test", NULL, 
                         Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool | Qt::X11BypassWindowManagerHint );
  mypLabel->setObjectName("ObjectTipText");
  mypLabel->setMargin( 1 );
  mypLabel->setFrameStyle( QFrame::Plain | QFrame::Box );
  mypLabel->setLineWidth( 1 );
  mypLabel->setAlignment( Qt::AlignAbsolute | Qt::AlignTop );
  mypLabel->setIndent( 0 );
  mypLabel->ensurePolished();
  
  mypTimer = new QTimer( this );

  connect( mypTimer, SIGNAL( timeout() ), this, SLOT( showTip() ) );
}

/*!
  destructor
*/
GLViewer_ObjectTip::~GLViewer_ObjectTip()
{ 
  if( mypLabel )
    delete mypLabel;
}


/*!
  It is called when there is a possibility that a tool tip should be shown
  \param p - position of tooltip
*/
bool GLViewer_ObjectTip::maybeTip( const QPoint& /*p*/ )
{
  GLViewer_Context* aContext = ((GLViewer_Viewer2d*)mypViewPort->getViewFrame()->getViewer())->getGLContext();
  GLViewer_Object* anObj = aContext->getCurrentObject();
  if( anObj )
  {
    setText( anObj->getName() );
    return true;
  }

  return false;
}

/*!
  Custom event filter
*/
bool GLViewer_ObjectTip::eventFilter( QObject* theObj, QEvent* e )
{
  hideTipAndSleep();
  switch( e->type() )
  {
    case QEvent::MouseMove:
    {
      QWidget* aWidget = (QWidget*) theObj;
      if( aWidget == mypViewPort->getGLWidget() )
      {
        wakeup();
        QMouseEvent* m = (QMouseEvent *)e;

        myPoint.setX( m->x() );
        myPoint.setY( m->y() );
      }
    }
    default:
      break;
  }
  return false;
}

/*!
  Hides tooltip and stops timer
*/
void GLViewer_ObjectTip::hideTipAndSleep()
{
  myPoint.setX(-1);
  myPoint.setY(-1);

  if( mypLabel )
  {
    mypLabel->hide();
  }
  mypTimer->stop();
}

/*!
  Shows tooltip
*/
void GLViewer_ObjectTip::showTip()
{
  if( maybeTip( myPoint ) )
  {
    
    mypLabel->setText( myText );
    mypLabel->adjustSize( );
    
    QPoint pos = mypViewPort->getGLWidget()->mapToGlobal( myPoint );
    
    int cur_height = 24;
    QCursor* aCursor = QApplication::overrideCursor();
    if( aCursor )
    {
      const QBitmap* aBitmap = aCursor->bitmap();
      if( aBitmap )
        cur_height = aBitmap->height();
    }
    mypLabel->setGeometry( pos.x(), pos.y() + cur_height, mypLabel->width(), mypLabel->height() );
    mypLabel->setPalette( QToolTip::palette() );

    mypLabel->show();

  }
}

/*!
  Restarts timer
*/
void GLViewer_ObjectTip::wakeup( int theTime )
{
  if( mypTimer->isActive() )
    mypTimer->stop();
  mypTimer->start( theTime );
}
