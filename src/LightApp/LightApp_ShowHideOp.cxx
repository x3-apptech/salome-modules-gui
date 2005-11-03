
#include "LightApp_ShowHideOp.h"
#include "LightApp_Application.h"
#include "LightApp_DataOwner.h"
#include "LightApp_Module.h"
#include "LightApp_Displayer.h"
#include "CAM_Study.h"

#include "LightApp_SelectionMgr.h"
#include "LightApp_Selection.h"

#include <SALOME_ListIO.hxx>
#include <SALOME_ListIteratorOfListIO.hxx>

LightApp_ShowHideOp::LightApp_ShowHideOp( ActionType type )
: LightApp_Operation(),
  myActionType( type )
{
}

LightApp_ShowHideOp::~LightApp_ShowHideOp()
{
}

void LightApp_ShowHideOp::startOperation()
{
  LightApp_Application* app = dynamic_cast<LightApp_Application*>( application() );
  if( !app )
  {
    abort();
    return;
  }

  LightApp_SelectionMgr* mgr = app->selectionMgr();
  LightApp_Selection sel; sel.init( "", mgr );
  if( sel.count()==0 )
  {
    abort();
    return;
  }
  QString aStr =  sel.param( 0, "component" ).toString();
  QString mod_name = app->moduleTitle( aStr );//sel.param( 0, "component" ).toString() );
  LightApp_Module* m = dynamic_cast<LightApp_Module*>( app ? app->module( mod_name ) : 0 );
  if( !m )
  {
    m = dynamic_cast<LightApp_Module*>( app->loadModule( mod_name ) );
    app->addModule( m );
    m->connectToStudy( dynamic_cast<CAM_Study*>( app->activeStudy() ) );
    m->setMenuShown( false );
    m->setToolShown( false );
  }

  LightApp_Displayer* d = m ? m->displayer(): 0;
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

