// SUIT_ViewWindow.cxx: implementation of the SUIT_ViewWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "SUIT_ViewWindow.h"
#include "SUIT_Desktop.h"
#include "qhbox.h"
#include "qpopupmenu.h"
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
