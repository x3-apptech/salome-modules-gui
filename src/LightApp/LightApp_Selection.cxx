//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "LightApp_Selection.h"
#include "LightApp_SelectionMgr.h"
#include "LightApp_DataOwner.h"
#include "LightApp_Study.h"
#include "LightApp_Application.h"
#include "LightApp_Displayer.h"

#include "CAM_Module.h"

#include "SUIT_Session.h"
#include "SUIT_ViewWindow.h"
#include "SUIT_ViewManager.h"
#include "SUIT_Desktop.h"
#include "SUIT_Selector.h"

/*!
  Constructor
*/
LightApp_Selection::LightApp_Selection()
: myStudy( 0 )
{
}

/*!
  Destructor.
*/
LightApp_Selection::~LightApp_Selection()
{
}

/*!
  Initialization.
*/
void LightApp_Selection::init( const QString& client, LightApp_SelectionMgr* mgr)
{
  myPopupClient = client;
  
  if( mgr )
  {
    if( mgr->application() )
      myStudy = dynamic_cast<LightApp_Study*>( mgr->application()->activeStudy() );
    if( !myStudy )
      return;

    //1) to take owners from current popup client
    SUIT_DataOwnerPtrList sel( true ), cur_sel( true );
    mgr->selected( sel, client );

    //2) to take such owners from other popup clients that it's entry is different with every entry from current list
    QList<SUIT_Selector*> aSelectors;
    mgr->selectors( aSelectors );
    QListIterator<SUIT_Selector*> it( aSelectors );
    while ( it.hasNext() )
    {
      SUIT_Selector* selector = it.next();
      if( selector->type() != client && selector->isEnabled() )
      {
	//mgr->selected( cur_sel, selector->type() );
        selector->selected( cur_sel );
	SUIT_DataOwnerPtrList::const_iterator aLIt = cur_sel.begin(), aLLast = cur_sel.end();
	for( ; aLIt!=aLLast; aLIt++ )
	  sel.append( *aLIt ); //check entry and don't append if such entry is in list already
      }
    }

    //3) to analyse owner and fill internal data structures
    SUIT_DataOwnerPtrList::const_iterator anIt = sel.begin(), aLast = sel.end();
    QMap<QString,int> entries;
    QString entry;
    int num=0;
    for( ; anIt!=aLast; anIt++ )
    {
      LightApp_DataOwner* sowner = dynamic_cast<LightApp_DataOwner*>( (*anIt ).get() );
      if( sowner )
      {
        entry = referencedToEntry( sowner->entry() );
	if( entries.contains( entry ) )
	  continue;

	entries.insert( entry, 0 );
        myEntries.insert( num, entry );
	myIsReferences.insert( num, sowner->entry() != entry );
        processOwner( sowner );
	num++;
      }
    }
  }
}

QString LightApp_Selection::referencedToEntry( const QString& entry ) const
{
  return myStudy->referencedToEntry( entry );
}

/*!
  Gets count of entries.
*/
int LightApp_Selection::count() const
{
  return myEntries.count();
}

/*!
  Gets QVariant();
*/
QVariant LightApp_Selection::parameter( const int ind, const QString& p ) const
{
  LightApp_Application* app = dynamic_cast<LightApp_Application*>( myStudy ? myStudy->application() : 0 );
  if( !( ind>=0 && ind<count() ) || !app )
    return QVariant();

  if( p=="isVisible" )
  {
    QString mod_name = app->moduleTitle( parameter( ind, "component" ).toString() );
    LightApp_Displayer* d = LightApp_Displayer::FindDisplayer( mod_name, false );
    // false in last parameter means that now we doesn't load module, if it isn't loaded

    bool vis = false;
    if( d )
      vis = d->IsDisplayed( myEntries[ ind ] );
    else
    {
      LightApp_Displayer local_d;
      vis = local_d.IsDisplayed( myEntries[ ind ] );
    }
    return QVariant( vis );
  }

  else if( p=="component" )
  {
    return myStudy->componentDataType( myEntries[ ind ] );
  }
  
  else if( p=="isComponent" )
  {
    return QVariant( myStudy->isComponent( myEntries[ ind ] ) );
  }

  else if( p=="isReference" )
    return QVariant( isReference( ind ) );

  else if( p=="displayer" )
    return parameter( ind, "component" );

  else if( p=="canBeDisplayed" )
  {
    QString mod_name = app->moduleTitle( parameter( ind, "component" ).toString() );
    LightApp_Displayer* d = LightApp_Displayer::FindDisplayer( mod_name, false );
    // false in last parameter means that now we doesn't load module, if it isn't loaded

    if ( d )
      return d->canBeDisplayed( myEntries[ ind ] );
    else if ( myEntries[ ind ].startsWith( QObject::tr( "SAVE_POINT_DEF_NAME" ) ) ) // object is a Save Point object
      return false;

    return true;
    //now if displayer is null, it means, that according module isn't loaded, so that we allow to all display/erase
    //operations under object
  }

  return QVariant();
}

/*!
  Gets global parameters. client, isActiveView, activeView etc.
*/
QVariant LightApp_Selection::parameter( const QString& p ) const
{
  if      ( p == "client" )        return QVariant( myPopupClient );
  else if ( p == "activeModule" )
  {
    LightApp_Application* app = dynamic_cast<LightApp_Application*>( myStudy->application() );
    QString mod_name = app ? QString( app->activeModule()->name() ) : QString();
    //cout << "activeModule : " << mod_name.latin1() << endl;
    if( !mod_name.isEmpty() )
      return mod_name;
    else
      return QVariant();
  }
  else if ( p == "isActiveView" )  return QVariant( (bool)activeVW() );
  else if ( p == "activeView" )    return QVariant( activeViewType() );
  else                             return QtxPopupSelection::parameter( p );
}

/*!
  Do nothing. To be redefined by successors
*/
void LightApp_Selection::processOwner( const LightApp_DataOwner* )
{
}

/*!
  Gets entry with index \a index.
*/
QString LightApp_Selection::entry( const int index ) const
{
  if ( index >= 0 && index < count() )
    return myEntries[ index ];
  return QString();
}

/*!
  Returns true if i-th selected object was reference to object with entry( i )
*/
bool LightApp_Selection::isReference( const int index ) const
{
  if( index >= 0 && index < count() )
    return myIsReferences[ index ];
  else
    return false;
}

/*!
  Gets type of active view manager.
*/
QString LightApp_Selection::activeViewType() const
{
  SUIT_ViewWindow* win = activeVW();
  if ( win ) {
    SUIT_ViewManager* vm = win->getViewManager();
    if ( vm )
      return vm->getType();
  }
  return QString();
}

/*!
  Gets active view window.
*/
SUIT_ViewWindow* LightApp_Selection::activeVW() const
{
  SUIT_Session* session = SUIT_Session::session();
  if ( session ) {
    SUIT_Application* app = session->activeApplication();
    if ( app ) {
      SUIT_Desktop* desk = app->desktop();
      if ( desk ) 
	return desk->activeWindow();
    }
  }
  return 0;
}
