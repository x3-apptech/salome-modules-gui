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
#include "CAF_Application.h"

#include "CAF_Tools.h"
#include "CAF_Study.h"

#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_ViewModel.h>
#include <SUIT_Operation.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>

#include <QtxListAction.h>

#include <qtoolbar.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qstatusbar.h>
#include <qapplication.h>

#include <Resource_Manager.hxx>

#include <TColStd_SequenceOfExtendedString.hxx>

extern "C" CAF_EXPORT SUIT_Application* createApplication()
{
  return new CAF_Application();
}

/*!
  Default constructor
*/
CAF_Application::CAF_Application()
: STD_Application()
{
}

/*!
  Constructor with OCAF application
  \param app - OCAF application
*/
CAF_Application::CAF_Application( const Handle( TDocStd_Application )& app )
: STD_Application(),
myStdApp( app )
{
}

/*!
  Destructor
*/
CAF_Application::~CAF_Application()
{
}

/*!
  \return application name
*/
QString CAF_Application::applicationName() const
{
  return QString( "CAFApplication" );
}

/*!
  \return OCAF application
*/
Handle( TDocStd_Application ) CAF_Application::stdApp() const
{
  return myStdApp;
}

/*!
  \return file filters for open/save document
*/
QString CAF_Application::getFileFilter() const
{
  if ( stdApp().IsNull() )
    return QString::null;

  TColStd_SequenceOfExtendedString formats;
  stdApp()->Formats( formats );

  QStringList allWC;
  QMap<QString, QStringList> wildCards;
  Handle(Resource_Manager) resMgr = new Resource_Manager( stdApp()->ResourcesName() );
  for ( int i = 1; i <= formats.Length(); i++ )
  {
    QString extension;
    QString extResStr = CAF_Tools::toQString( formats.Value( i ) ) + QString( ".FileExtension" );
    if ( resMgr->Find( (char*)extResStr.latin1() ) )
      extension = QString( resMgr->Value( (char*)extResStr.latin1() ) );

    QString descr;
    QString descrResStr = CAF_Tools::toQString( formats.Value( i ) ) + QString( ".Description" );
    if ( resMgr->Find( (char*)descrResStr.latin1() ) )
      descr = QString( resMgr->Value( (char*)descrResStr.latin1() ) );

    if ( !descr.isEmpty() && !extension.isEmpty() )
    {
      if ( !wildCards.contains( descr ) )
        wildCards.insert( descr, QStringList() );
      wildCards[descr].append( QString( "*.%1" ).arg( extension ) );
      allWC.append( QString( "*.%1" ).arg( extension ) );
    }
  }

  if ( wildCards.isEmpty() )
    return QString::null;

  QStringList filters;
  for ( QMap<QString, QStringList>::ConstIterator it = wildCards.begin(); it != wildCards.end(); ++it )
    filters.append( QString( "%1 (%2)" ).arg( it.key() ).arg( it.data().join( "; " ) ) );

  if ( wildCards.count() > 1 )
    filters.prepend( QString( "%1 (%2)" ).arg( tr( "INF_ALL_DOCUMENTS_FILTER" ) ).arg( allWC.join( "; " ) ) );

  if ( !filters.isEmpty() )
    filters.append( tr( "INF_ALL_FILTER" ) );

  return filters.join( ";;" );
}

/*!
  Creates actions of application
*/
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

/*!
  SLOT: called by clicking on Help->About in main menu
*/
void CAF_Application::onHelpAbout()
{
  SUIT_MessageBox::info1( desktop(), tr( "About" ), tr( "ABOUT_INFO" ), "&OK" );
}

/*!
  Creates new study
*/
SUIT_Study* CAF_Application::createNewStudy()
{
  return new CAF_Study( this );
}

/*!
  Sets OCAF application
*/
void CAF_Application::setStdApp( const Handle(TDocStd_Application)& app )
{
  myStdApp = app;
}
