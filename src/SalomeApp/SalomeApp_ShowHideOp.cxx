
#include "SalomeApp_ShowHideOp.h"
#include "SalomeApp_Application.h"
#include "SalomeApp_SelectionMgr.h"
#include "SalomeApp_Selection.h"
#include "SalomeApp_Module.h"
#include "SalomeApp_Displayer.h"
#include "CAM_Study.h"
#include <SALOME_ListIO.hxx>
#include <SALOME_ListIteratorOfListIO.hxx>

SalomeApp_ShowHideOp::SalomeApp_ShowHideOp( ActionType type )
: SalomeApp_Operation(),
  myActionType( type )
{
}

SalomeApp_ShowHideOp::~SalomeApp_ShowHideOp()
{
}

void SalomeApp_ShowHideOp::startOperation()
{
  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( application() );
  if( !app )
  {
    abort();
    return;
  }

  SalomeApp_SelectionMgr* mgr = app->selectionMgr();
  SalomeApp_Selection sel; sel.init( "", mgr );
  if( sel.count()==0 )
  {
    abort();
    return;
  }

  QString mod_name = app->moduleTitle( sel.param( 0, "component" ).toString() );
  SalomeApp_Module* m = dynamic_cast<SalomeApp_Module*>( app ? app->module( mod_name ) : 0 );
  if( !m )
  {
    m = dynamic_cast<SalomeApp_Module*>( app->loadModule( mod_name ) );
    app->addModule( m );
    m->connectToStudy( dynamic_cast<CAM_Study*>( app->activeStudy() ) );
    m->setMenuShown( false );
    m->setToolShown( false );
  }

  SalomeApp_Displayer* d = m ? m->displayer(): 0;
  if( !d )
  {
    abort();
    return;
  }

  if( myActionType==DISPLAY_ONLY )
    d->EraseAll( false, false, 0 );

  SALOME_ListIO selObjs;
  mgr->selectedObjects( selObjs );
  SALOME_ListIteratorOfListIO anIt( selObjs );
  for( ; anIt.More(); anIt.Next() )
  {
    if( anIt.Value().IsNull() )
      continue;

    if( myActionType==DISPLAY || myActionType==DISPLAY_ONLY )
      d->Display( anIt.Value()->getEntry(), false, 0 );
    else if( myActionType==ERASE )
      d->Erase( anIt.Value()->getEntry(), false, false, 0 );
  }
  d->UpdateViewer();
  commit();
}

