// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "CAF_Application.h"

#include "CAF_Tools.h"
#include "CAF_Study.h"

#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>

#include <QtxAction.h>
#include <QtxListAction.h>

#include <QMap>
#include <QStringList>

#include <Resource_Manager.hxx>
#include <TColStd_SequenceOfExtendedString.hxx>

/*!
  \brief Create new instance of CAF_Application.
  \return new instance of CAF_Application class
*/
extern "C" CAF_EXPORT SUIT_Application* createApplication()
{
  return new CAF_Application();
}

/*!
  \class CAF_Application
  \brief OCC OCAF-based application.

  Defines application configuration and behaviour for application using 
  standard OCC OCAF data model. Allows using OCC OCAF serives
  (for example, undo/redo mechanizm).
*/

/*!
  \brief Default constructor.
*/
CAF_Application::CAF_Application()
: STD_Application()
{
}

/*!
  \brief Constructor.
  \param app OCAF application
*/
CAF_Application::CAF_Application( const Handle( TDocStd_Application )& app )
: STD_Application(),
  myStdApp( app )
{
}

/*!
  \brief Destructor.
*/
CAF_Application::~CAF_Application()
{
}

/*!
  \brief Get application name.
  \return application name
*/
QString CAF_Application::applicationName() const
{
  return QString( "CAFApplication" );
}

/*!
  \brief Get OCAF application.
  \return handle to OCAF application object
*/
Handle( TDocStd_Application ) CAF_Application::stdApp() const
{
  return myStdApp;
}

/*!
  \brief Get file extension filter.

  The file extension filter is used in Open/Save dialog boxes.

  \return file filters for open/save document dialog box
*/
QString CAF_Application::getFileFilter() const
{
  if ( stdApp().IsNull() )
    return QString();

  TColStd_SequenceOfExtendedString formats;
  stdApp()->Formats( formats );

  QStringList allWC;
  QMap<QString, QStringList> wildCards;
  Handle(Resource_Manager) resMgr = new Resource_Manager( stdApp()->ResourcesName() );
  for ( int i = 1; i <= formats.Length(); i++ )
  {
    QString extension;
    QString extResStr = CAF_Tools::toQString( formats.Value( i ) ) + QString( ".FileExtension" );
    if ( resMgr->Find( extResStr.toLatin1().data() ) )
      extension = QString( resMgr->Value( extResStr.toLatin1().data() ) );

    QString descr;
    QString descrResStr = CAF_Tools::toQString( formats.Value( i ) ) + QString( ".Description" );
    if ( resMgr->Find( (char*)descrResStr.toLatin1().data() ) )
      descr = QString( resMgr->Value( (char*)descrResStr.toLatin1().data() ) );

    if ( !descr.isEmpty() && !extension.isEmpty() )
    {
      if ( !wildCards.contains( descr ) )
        wildCards.insert( descr, QStringList() );
      wildCards[descr].append( QString( "*.%1" ).arg( extension ) );
      allWC.append( QString( "*.%1" ).arg( extension ) );
    }
  }

  if ( wildCards.isEmpty() )
    return QString();

  QStringList filters;
  for ( QMap<QString, QStringList>::ConstIterator it = wildCards.begin(); it != wildCards.end(); ++it )
    filters.append( QString( "%1 (%2)" ).arg( it.key() ).arg( it.value().join( "; " ) ) );

  if ( wildCards.count() > 1 )
    filters.prepend( QString( "%1 (%2)" ).arg( tr( "INF_ALL_DOCUMENTS_FILTER" ) ).arg( allWC.join( "; " ) ) );

  if ( !filters.isEmpty() )
    filters.append( tr( "INF_ALL_FILTER" ) );

  return filters.join( ";;" );
}

/*!
  \brief Create menu and toolbars actions.
*/
void CAF_Application::createActions()
{
  STD_Application::createActions();

  SUIT_Desktop* desk = desktop();
  SUIT_ResourceMgr* resMgr = resourceMgr();

  QtxListAction* editUndo = 
    new QtxListAction( tr( "TOT_APP_EDIT_UNDO" ), 
                       resMgr->loadPixmap( "STD", tr( "ICON_EDIT_UNDO" ) ),
                       tr( "MEN_APP_EDIT_UNDO" ), Qt::CTRL+Qt::Key_Z, desk );
  editUndo->setStatusTip( tr( "PRP_APP_EDIT_UNDO" ) );
  registerAction( EditUndoId, editUndo );

  QtxListAction* editRedo =
    new QtxListAction( tr( "TOT_APP_EDIT_REDO" ), 
                       resMgr->loadPixmap( "STD", tr( "ICON_EDIT_REDO" ) ),
                       tr( "MEN_APP_EDIT_REDO" ), Qt::CTRL+Qt::Key_Y, desk );
  editRedo->setStatusTip( tr( "PRP_APP_EDIT_REDO" ) );
  registerAction( EditRedoId, editRedo );

  editUndo->setComment( tr( "INF_APP_UNDOACTIONS" ) );
  editRedo->setComment( tr( "INF_APP_REDOACTIONS" ) );

  connect( editUndo, SIGNAL( triggered( int ) ), this, SLOT( onUndo( int ) ) );
  connect( editRedo, SIGNAL( triggered( int ) ), this, SLOT( onRedo( int ) ) );

  int editMenu = createMenu( tr( "MEN_DESK_EDIT" ), -1, -1, 10 );

  createMenu( EditUndoId, editMenu, 0 );
  createMenu( EditRedoId, editMenu, 0 );
  createMenu( separator(), editMenu, -1, 0 );

  int stdTBar = createTool( tr( "INF_DESK_TOOLBAR_STANDARD" ),   // title (language-dependant)
			    QString( "SalomeStandard" ) );       // name (language-independant)

  createTool( separator(), stdTBar );
  createTool( EditUndoId, stdTBar );
  createTool( EditRedoId, stdTBar );
  createTool( separator(), stdTBar );
}

/*!
  \brief Undo latest command operation for specified document.
  \param doc OCAF document
  \return \c true on success
*/
bool CAF_Application::undo( CAF_Study* doc )
{
  bool success = false;
  if ( doc )
  {
    success = doc->undo();
    if ( success )
      doc->update();
  }
  return success;
}

/*!
  \brief Redo latest command operation undo for specified document.
  \param doc OCAF document
  \return \c true on success
*/
bool CAF_Application::redo(CAF_Study* doc)
{
  bool success = false;
  if ( doc )
  {
    success = doc->redo();
    if ( success )
      doc->update();
  }
  return success;
}

/*!
  \brief Called when user activates "Undo" menu action.
  
  Undo operation on the active document.

  \param numActions undo depth (number of commands)
  \return \c true on success
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
  \brief Called when user activates "Redo" menu action.
  
  Redo latest undo commands on the active document.

  \param numActions redo depth (number of commands)
  \return \c true on success
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
  \brief Update actions state (Undo/Redo).
*/
void CAF_Application::updateCommandsStatus()
{
  STD_Application::updateCommandsStatus();

  CAF_Study* cafStudy = 0;
  if ( activeStudy() && activeStudy()->inherits( "CAF_Study" ) )
    cafStudy = (CAF_Study*)activeStudy();

  QtxListAction* undo = qobject_cast<QtxListAction*>( action( EditUndoId ) );
  if ( cafStudy && undo )
    undo->addNames( cafStudy->undoNames() );

  QtxListAction* redo = qobject_cast<QtxListAction*>( action( EditRedoId ) );
  if ( cafStudy && redo )
    redo->addNames( cafStudy->redoNames() );

  if ( undo )
    undo->setEnabled( cafStudy && cafStudy->canUndo() );
  if ( redo )
    redo->setEnabled( cafStudy && cafStudy->canRedo() );
}

/*!
  \brief Called when user activatees Help->About main menu command.
*/
void CAF_Application::onHelpAbout()
{
  SUIT_MessageBox::information( desktop(), tr( "About" ), tr( "ABOUT_INFO" ) );
}

/*!
  \brief Create new empty study.
  \return new study
*/
SUIT_Study* CAF_Application::createNewStudy()
{
  return new CAF_Study( this );
}

/*!
  \brief Set OCAF application.
  \param app new OCAF application
*/
void CAF_Application::setStdApp( const Handle(TDocStd_Application)& app )
{
  myStdApp = app;
}
