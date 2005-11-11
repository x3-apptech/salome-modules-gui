#include "CAF_Application.h"

#include "CAF_Study.h"

#include "SUIT_Desktop.h"
#include "SUIT_Session.h"
#include "SUIT_ViewModel.h"
#include "SUIT_Operation.h"
#include "SUIT_MessageBox.h"
#include "SUIT_ResourceMgr.h"

#include "QtxListAction.h"

#include <qtoolbar.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qstatusbar.h>
#include <qapplication.h>

extern "C" CAF_EXPORT SUIT_Application* createApplication()
{
  return new CAF_Application();
}

CAF_Application::CAF_Application()
: STD_Application()
{
}

CAF_Application::~CAF_Application()
{
}

QString CAF_Application::applicationName() const
{
  return QString( "CAFApplication" );
}

void CAF_Application::createActions()
{
  STD_Application::createActions();

  SUIT_Desktop* desk = desktop();
  SUIT_ResourceMgr* resMgr = resourceMgr();

  QtxListAction* editUndo =
    new QtxListAction( tr( "TOT_APP_EDIT_UNDO" ), resMgr->loadPixmap( "CAF", tr( "ICON_APP_EDIT_UNDO" ) ),
			                 tr( "MEN_APP_EDIT_UNDO" ), CTRL+Key_Z, desk );
  registerAction( EditUndoId, editUndo );

  QtxListAction* editRedo =
    new QtxListAction( tr( "TOT_APP_EDIT_REDO" ), resMgr->loadPixmap( "CAF", tr( "ICON_APP_EDIT_REDO" ) ),
			                 tr( "MEN_APP_EDIT_REDO" ), CTRL+Key_Y, desk );
  registerAction( EditRedoId, editRedo );

  editUndo->setComment( tr( "INF_APP_UNDOACTIONS" ) );
  editRedo->setComment( tr( "INF_APP_REDOACTIONS" ) );

  connect( editUndo, SIGNAL( activated( int ) ), this, SLOT( onUndo( int ) ) );
  connect( editRedo, SIGNAL( activated( int ) ), this, SLOT( onRedo( int ) ) );


  int editMenu = createMenu( tr( "MEN_DESK_EDIT" ), -1, -1, 10 );

  createMenu( EditUndoId, editMenu, 0 );
  createMenu( EditRedoId, editMenu, 0 );
  createMenu( separator(), editMenu, -1, 0 );

  int stdTBar = createTool( tr( "INF_DESK_TOOLBAR_STANDARD" ) );

  createTool( separator(), stdTBar );
  createTool( EditUndoId, stdTBar );
  createTool( EditRedoId, stdTBar );
  createTool( separator(), stdTBar );
}

/*!
    Undo operation on the given document. [ virtual protected ]
*/
bool CAF_Application::undo( CAF_Study* doc )
{
  bool success = false;
  if ( doc )
  {
    if ( success = doc->undo() )
      doc->update();
  }
  return success;
}

/*!
    Redo operation on the given document. [ virtual protected ]
*/
bool CAF_Application::redo(CAF_Study* doc)
{
  bool success = false;
  if ( doc )
  {
    if ( success = doc->redo() )
      doc->update();
  }
  return success;
}

/*!
    Undo operation on the active document. [ virtual protected slot ]
*/
bool CAF_Application::onUndo( int numActions )
{
  bool ok = true;
  while ( numActions > 0 )
  {
	  CAF_Study* cafStudy = dynamic_cast<CAF_Study*>( activeStudy() );
		if ( cafStudy )
    {
	    if ( !undo( cafStudy ) )
		  {
			  ok = false;
				break;
			}
			numActions--;
		}
  }
  updateCommandsStatus();     /* enable/disable undo/redo */
  return ok;
}

/*!
    Redo operation on the active document. [ virtual protected slot ]
*/
bool CAF_Application::onRedo( int numActions )
{
  bool ok = true;
  while ( numActions > 0 )
  {
	  CAF_Study* cafStudy = dynamic_cast<CAF_Study*>( activeStudy() );
		if ( cafStudy )
    {
			if ( !redo( cafStudy ) )
			{
	      ok = false;
		    break;
			}
			numActions--;
		}
  }
  updateCommandsStatus();     /* enable/disable undo/redo */
  return ok;
}

/*!
  Enables / disables the actions according to the application state. [ virtual protected ]
*/
void CAF_Application::updateCommandsStatus()
{
	STD_Application::updateCommandsStatus();

  CAF_Study* cafStudy = 0;
  if ( activeStudy() && activeStudy()->inherits( "CAF_Study" ) )
    cafStudy = (CAF_Study*)activeStudy();

  QAction* undo = action( EditUndoId );
  if ( cafStudy && undo )
    undo->setProperty( "names", cafStudy->undoNames() );

  QAction* redo = action( EditRedoId );
  if ( cafStudy && redo )
    redo->setProperty( "names", cafStudy->redoNames() );

  if ( undo )
    undo->setEnabled( cafStudy && cafStudy->canUndo() );
  if ( redo )
    redo->setEnabled( cafStudy && cafStudy->canRedo() );
}

void CAF_Application::onHelpAbout()
{
  SUIT_MessageBox::info1( desktop(), tr( "About" ), tr( "ABOUT_INFO" ), "&OK" );
}

SUIT_Study* CAF_Application::createNewStudy()
{
  return new CAF_Study( this );
}
