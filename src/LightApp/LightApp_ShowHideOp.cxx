
#include "LightApp_ShowHideOp.h"
#include "LightApp_Application.h"
#include "LightApp_DataOwner.h"
#include "LightApp_Module.h"
#include "LightApp_Study.h"
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

LightApp_Displayer* LightApp_ShowHideOp::displayer( const QString& mod_name ) const
{
  LightApp_Application* app = dynamic_cast<LightApp_Application*>( application() );
  LightApp_Module* m = dynamic_cast<LightApp_Module*>( app ? app->module( mod_name ) : 0 );
  if( !m )
  {
    m = dynamic_cast<LightApp_Module*>( app->loadModule( mod_name ) );
    if( m )
    {
      app->addModule( m );
      m->connectToStudy( dynamic_cast<CAM_Study*>( app->activeStudy() ) );
      m->setMenuShown( false );
      m->setToolShown( false );
    }
  }
  return m ? m->displayer() : 0;
}

void LightApp_ShowHideOp::startOperation()
{
  LightApp_Application* app = dynamic_cast<LightApp_Application*>( application() );
  LightApp_Study* study = app ? dynamic_cast<LightApp_Study*>( app->activeStudy() ) : 0;
  if( !app || !study )
  {
    abort();
    return;
  }

  LightApp_SelectionMgr* mgr = app->selectionMgr();
  LightApp_Selection sel; sel.init( "", mgr );
  if( sel.count()==0 && myActionType!=ERASE_ALL )
  {
    abort();
    return;
  }
  QString aStr =  sel.param( 0, "component" ).toString();
  QString mod_name = app->moduleTitle( aStr );//sel.param( 0, "component" ).toString() );
  LightApp_Displayer* d = displayer( mod_name );
  if( !d )
  {
    abort();
    return;
  }

  if( myActionType==DISPLAY_ONLY || myActionType==ERASE_ALL )
  {
    //ERASE ALL
    QStringList comps;
    study->components( comps );
    QStringList::const_iterator anIt = comps.begin(), aLast = comps.end();
    for( ; anIt!=aLast; anIt++ )
    {
      LightApp_Displayer* disp = displayer( app->moduleTitle( *anIt ) );
      if( disp )
	disp->EraseAll( false, false, 0 );
    }
    if( myActionType==ERASE_ALL )
    {
      d->UpdateViewer();
      commit();
      return;
    }
  }

  SALOME_ListIO selObjs;
  mgr->selectedObjects( selObjs );

  QStringList entries;
  SALOME_ListIteratorOfListIO anIt( selObjs );
  for( ; anIt.More(); anIt.Next() )
  {
    if( anIt.Value().IsNull() )
      continue;

    if( study->isComponent( anIt.Value()->getEntry() ) )
      study->children( anIt.Value()->getEntry(), entries );
    else
      entries.append( anIt.Value()->getEntry() );
  }

  for( QStringList::const_iterator it = entries.begin(), last = entries.end(); it!=last; it++ )
  {
    QString e = study->referencedToEntry( *it );
    if( myActionType==DISPLAY || myActionType==DISPLAY_ONLY )
      d->Display( e, false, 0 );
    else if( myActionType==ERASE )
      d->Erase( e, false, false, 0 );
  }
  d->UpdateViewer();
  commit();
}
