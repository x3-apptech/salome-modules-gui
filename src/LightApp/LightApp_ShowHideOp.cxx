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

#include "LightApp_ShowHideOp.h"
#include "LightApp_Application.h"
#include "LightApp_Module.h"
#include "LightApp_Study.h"
#include "LightApp_Displayer.h"

#include "LightApp_SelectionMgr.h"
#include "LightApp_Selection.h"

#include "SUIT_OverrideCursor.h"

#ifndef DISABLE_SALOMEOBJECT
  #include <SALOME_ListIO.hxx>
#endif

/*!
  Constructor
*/
LightApp_ShowHideOp::LightApp_ShowHideOp( ActionType type )
: LightApp_Operation(),
  myActionType( type )
{
}

/*!
  Destructor
*/
LightApp_ShowHideOp::~LightApp_ShowHideOp()
{
}

/*!
  Makes show/hide operation
*/
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
  LightApp_Module* mod = dynamic_cast<LightApp_Module*>( app->activeModule() );
  if( !mod )
    return;

  LightApp_Selection* sel = mod->createSelection();
  if( !sel )
    return;

  sel->init( "", mgr );
  if( sel->count()==0 && myActionType!=ERASE_ALL )
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
      LightApp_Displayer* disp = LightApp_Displayer::FindDisplayer( app->moduleTitle( *anIt ), false );
      if( disp )
        disp->EraseAll( false, false, 0 );
    }
    if( myActionType==ERASE_ALL )
    {
      // Temporary displayer just to update viewer!
      LightApp_Displayer ld;
      ld.UpdateViewer();
      commit();
      return;
    }
  }

  QString mod_name;
  if( sel->count()>0 )
  {
    QString aStr =  sel->parameter( 0, "displayer" ).toString();
    mod_name = app->moduleTitle( aStr );
  }
  else if( app->activeModule() )
    mod_name = app->moduleTitle( app->activeModule()->name() );

  LightApp_Displayer* d = LightApp_Displayer::FindDisplayer( mod_name, true );
  if( !d )
  {
    abort();
    return;
  }

  QStringList entries;

#ifndef DISABLE_SALOMEOBJECT
  SALOME_ListIO selObjs;
  mgr->selectedObjects( selObjs );
  SALOME_ListIteratorOfListIO anIt( selObjs );
  for( ; anIt.More(); anIt.Next() )
    if( !anIt.Value().IsNull() )
#else
  QStringList selObjs;
  mgr->selectedObjects( selObjs );
  QStringList::const_iterator anIt = selObjs.begin(), aLast = selObjs.end();
  for( ; ; anIt!=aLast )
#endif
    {
      QString entry = 
#ifndef DISABLE_SALOMEOBJECT
        anIt.Value()->getEntry();
#else
        *anIt;
#endif

      if( study->isComponent( entry ) )
        study->children( entry, entries );
      else
        entries.append( entry );
    }

  // be sure to use real object entries
  QStringList objEntries;
  QStringList::const_iterator it = entries.begin(), last = entries.end();
  for ( ; it!=last; ++it )
    objEntries.append( study->referencedToEntry( *it ) ); 
  
  if( myActionType==DISPLAY || myActionType==DISPLAY_ONLY ) {
    SUIT_OverrideCursor wc;
    d->Display( objEntries, false, 0 );
    mgr->setSelectedObjects(selObjs);
  }
  else if( myActionType==ERASE ) {
    d->Erase( objEntries, false, false, 0 );
  }
  
  d->UpdateViewer();
  commit();
}
