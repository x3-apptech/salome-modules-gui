// File:      GLViewer_ToolTip.xx
// Created:   March, 2005
// Author:    OCC team
// Copyright (C) CEA 2005

//#include "GLViewerAfx.h"
#include "GLViewer_Context.h"
#include "GLViewer_ToolTip.h"
#include "GLViewer_Viewer2d.h"
#include "GLViewer_ViewPort2d.h"

#include <qlabel.h>
#include <qtimer.h>
#include <qbitmap.h>
#include <qapplication.h>
#include <qtooltip.h>

/***************************************************************************
**  Class:   GLViewer_ToolTip
**  Descr:   ToolTip of GLViewer_Objects
**  Module:  GLViewer
**  Created: UI team, 25.03.05
****************************************************************************/

//--------------------------------------------------------------------------
//Function: GLViewer_ToolTip()
//Description: constructor
//--------------------------------------------------------------------------
GLViewer_ObjectTip::GLViewer_ObjectTip( GLViewer_ViewPort2d* theParent )
:QObject(),
 myText(),
 myPoint( -1, -1 )
{
  mypViewPort = theParent;
  //mypLabel = NULL;
  mypLabel = new QLabel( "Test", NULL, "ObjectTipText",
	     WStyle_StaysOnTop | WStyle_Customize | WStyle_NoBorder | WStyle_Tool | WX11BypassWM );
	mypLabel->setMargin( 1 );
	mypLabel->setAutoMask( FALSE );
  mypLabel->setFrameStyle( QFrame::Plain | QFrame::Box );
  mypLabel->setLineWidth( 1 );
  mypLabel->setAlignment( AlignAuto | AlignTop );
  mypLabel->setIndent( 0 );
  mypLabel->polish();
  
  //mypLabel->setPalette( QToolTip::palette() );

  mypTimer = new QTimer( this );

  connect( mypTimer, SIGNAL( timeout() ), this, SLOT( showTip() ) );
}

//--------------------------------------------------------------------------
//Function: GLViewer_ToolTip()
//Description: destructor
//--------------------------------------------------------------------------
GLViewer_ObjectTip::~GLViewer_ObjectTip()
{ 
//  delete mypRect;
  if( mypLabel )
    delete mypLabel;

  //if( mypPoint )
  //  delete mypPoint;

  //if( mypTimer )
  //  delete mypTimer;
}


//--------------------------------------------------------------------------
//Function: GLViewer_ToolTip()
//Description: destructor
//--------------------------------------------------------------------------
bool GLViewer_ObjectTip::maybeTip( const QPoint &p )
{


  GLViewer_Context* aContext = ((GLViewer_Viewer2d*)mypViewPort->getViewFrame()->getViewer())->getGLContext();

  /*if( !aContext->currentObjectIsChanged() )
    return false;
  else
    return true;
  if( myPoint.x() == -1 && myPoint.y() == -1 || aContext->currentObjectIsChanged())
  {
    myPoint = p;
  }
  else/if( abs(myPoint.y() - p.y()) < 16 )
  {
    return;
  }
  else // > 16
  {
    myPoint = p;
  }
*/  
  GLViewer_Object* anObj = aContext->getCurrentObject();
  if( anObj )
  {
    setText( anObj->getName() );
    return true;
  }

  return false;
  /*if( anObj )
  {
    //GLViewer_Rect* aRect = anObj->getRect();
    //QRect aWinRect = mypViewPort->GLV2win( *aRect );
    tip( QRect( p.x(), p.y(), 1, 1 ), anObj->getName() );
    //QFontMetrics aFM( font() );    
     //showTip( aWinRect, anObj->getName(), QRect( 0, 0, aFM.width( anObj->getName() + "  " ), aFM.height()*1.5 ) );
    //tip( aWinRect, anObj->getName(), aWinRect( aFM.width( anObj->getName() + "  " ), aFM.height()*1.5 )  );
  }
//  else
//    clear();
    
  //tip( QRect( 0, 0, mypViewPort->getGLWidget()->width(),mypViewPort->getGLWidget()->height() ) , "test Tool tip" );
  */
}

bool GLViewer_ObjectTip::eventFilter( QObject* theObj, QEvent* e )
{
  hideTipAndSleep();
  switch( e->type() )
  {
    /*case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
	    // input - turn off tool tip mode
	    hideTipAndSleep();
	    break;*/
    case QEvent::MouseMove:
      {
        //hideTipAndSleep();
        /*if( mypTimer->isActive() )
        {
          mypTimer->Stop();
          wakeUp();
        }*/
        QWidget* aWidget = (QWidget*) theObj;
        if( aWidget == mypViewPort->getGLWidget() )
        {
          wakeup();
          QMouseEvent* m = (QMouseEvent *)e;
          //if( !mypPoint )
          //  mypPoint = new QPoint();

          myPoint.setX( m->x() );
          myPoint.setY( m->y() );
        }
      }
  }
  return false;
}


void GLViewer_ObjectTip::hideTipAndSleep()
{
  //if( mypPoint )
  //  delete mypPoint;
  myPoint.setX(-1);
  myPoint.setY(-1);

  if( mypLabel )
  {
    mypLabel->hide();
    //delete mypLabel;
  }
  mypTimer->stop();
}

void GLViewer_ObjectTip::showTip()
{
  if( maybeTip( myPoint ) )
  {
    
    mypLabel->setText( myText );
    mypLabel->adjustSize( );
    
    QPoint pos = mypViewPort->getGLWidget()->mapToGlobal( myPoint );
    
    //mypLabel->show();
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

void GLViewer_ObjectTip::wakeup( int theTime )
{
  if( mypTimer->isActive() )
    mypTimer->stop();
  mypTimer->start( theTime );
}