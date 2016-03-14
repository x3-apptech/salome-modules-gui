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

#include <QtCore/QSet>

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
  myContext = client;

  if ( mgr ) {
    if ( mgr->application() )
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
      if ( selector->type() != client && selector->isEnabled() ) {
        selector->selected( cur_sel );

        for ( SUIT_DataOwnerPtrList::const_iterator aLIt = cur_sel.begin(); aLIt != cur_sel.end(); ++aLIt )
          sel.append( *aLIt ); //check entry and don't append if such entry is in list already
      }
    }

    //3) to analyse owner and fill internal data structures

    int num = 0;
    QSet<QString> entries;
    myObjects.resize( sel.size() );
    myObjects.fill( ObjectInfo() );
    for ( SUIT_DataOwnerPtrList::const_iterator anIt = sel.begin(); anIt != sel.end(); anIt++ )
    {
      LightApp_DataOwner* sowner = dynamic_cast<LightApp_DataOwner*>( (*anIt ).get() );
      if ( sowner )
      {
        QString entry = referencedToEntry( sowner->entry() );
        if ( entries.contains( entry ) )
          continue;

        entries.insert( entry );

	setObjectInfo( num, OI_Entry, entry );
	setObjectInfo( num, OI_Reference, sowner->entry() != entry );

        if ( processOwner( sowner ) )
          num++;
        else
          entries.remove( entry );
      }
    }

    myObjects.resize( num );
    /*
    myContextParams.clear();
    myObjectsParams.resize( num );
    myObjectsParams.fill( ParameterMap() );
    */
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
  return myObjects.size();
}

/*!
  Gets global parameters.
*/
/*
QVariant LightApp_Selection::parameter( const QString& p ) const
{
  QVariant v;
  if ( myContextParams.contains( p ) )
    v = myContextParams[p];
  else
    v = contextParameter( p );
    if ( !v.isValid() )
      v = QtxPopupSelection::parameter( p );
    LightApp_Selection* that = (LightApp_Selection*)this;
    that->myContextParams.insert( p, v );
  }
  return v;
}
*/

/*!
  Gets the object parameter.
*/
 /*
QVariant LightApp_Selection::parameter( const int idx, const QString& p ) const
{
  QVariant v;
  if ( 0 <= idx && idx < myObjectsParams.size() ) {
    if ( myObjectsParams[idx].contains( p ) )
      v = myObjectsParams[idx][p];
    else {
      v = objectParameter( idx, p );
      LightApp_Selection* that = (LightApp_Selection*)this;
      that->myObjectsParams[idx].insert( p, v );
    }
  }
  return v;
}
 */
/*!
  Gets global parameters. client, isActiveView, activeView etc.
*/
  //QVariant LightApp_Selection::contextParameter( const QString& p ) const
QVariant LightApp_Selection::parameter( const QString& p ) const
{
  QVariant v;

  if ( p == "client" )
    v = myContext;
  else if ( p == "activeModule" ) {
    LightApp_Application* app = dynamic_cast<LightApp_Application*>( myStudy->application() );
    QString mod_name = app ? QString( app->activeModule()->name() ) : QString();
    if ( !mod_name.isEmpty() )
      v = mod_name;
  }
  else if ( p == "isActiveView" )
    v = activeVW() != 0;
  else if ( p == "activeView" )
    v = activeViewType();
  else
    v = QtxPopupSelection::parameter( p );

  return v;
}

/*!
  Gets the object parameter.
*/
//QVariant LightApp_Selection::objectParameter( const int idx, const QString& p ) const
QVariant LightApp_Selection::parameter( const int idx, const QString& p ) const
{
  LightApp_Application* app = 0;
  if ( myStudy )
    app = dynamic_cast<LightApp_Application*>( myStudy->application() );

  QVariant v;
  if ( app ) {
    QString e = entry( idx );
    if ( !e.isEmpty() ) {
      if ( p == "isVisible" ) {
	QString mod_name = app->moduleTitle( myStudy->componentDataType( e ) );
	LightApp_Displayer* d = LightApp_Displayer::FindDisplayer( mod_name, false );
	// false in last parameter means that now we doesn't load module, if it isn't loaded

	bool vis = false;
	if ( d )
	  vis = d->IsDisplayed( e );
	else
	  vis = LightApp_Displayer().IsDisplayed( e );
	v = vis;
      }
      else if ( p == "component" || p == "displayer" )
	v = myStudy->componentDataType( e );
      else if ( p == "isComponent" )
	v = myStudy->isComponent( e );
      else if ( p == "isReference" )
	v = isReference( idx );
      else if ( p == "canBeDisplayed" ) {
	QString mod_name = app->moduleTitle( myStudy->componentDataType( e ) );
	LightApp_Displayer* d = LightApp_Displayer::FindDisplayer( mod_name, false );
	// false in last parameter means that now we doesn't load module, if it isn't loaded

	if ( d )
	  v = d->canBeDisplayed( e );
	else if ( e.startsWith( QObject::tr( "SAVE_POINT_DEF_NAME" ) ) ) // object is a Save Point object
	  v = false;
	else
	  v = true;
	//now if displayer is null, it means, that according module isn't loaded, so that we allow to all display/erase
	//operations under object
      }
    }
  }

  return v;
}

/*!
  Perform additional processing of the selected item (to be redefined by successors if necessary).
  Returns \c true by default.
  Note: if this method returns \c false, the item will be removed from the items list and
  not taken into account when showing popup menu.

  \param owner a data owner being processed
  \return \c true if the owner should be collected and \c false otherwise
*/
bool LightApp_Selection::processOwner( const LightApp_DataOwner* /*owner*/ )
{
  return true;
}

/*!
  Gets entry with index \a index.
*/
QString LightApp_Selection::entry( const int index ) const
{
  QVariant v = objectInfo( index, OI_Entry );
  return v.canConvert( QVariant::String ) ? v.toString() : QString();
}

/*!
  Returns true if i-th selected object was reference to object with entry( i )
*/
bool LightApp_Selection::isReference( const int index ) const
{
  QVariant v = objectInfo( index, OI_Reference );
  return v.canConvert( QVariant::Bool ) ? v.toBool() : false;
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

/*!
  Gets specified information about object with index idx.
*/
QVariant LightApp_Selection::objectInfo( const int idx, const int inf ) const
{
  QVariant res;
  if ( 0 <= idx && idx < myObjects.size() ) {
    if ( myObjects[idx].contains( inf ) )
      res = myObjects[idx][inf];
  }
  return res;
}

/*!
  Sets specified information about object with index idx.
*/
void LightApp_Selection::setObjectInfo( const int idx, const int inf, const QVariant& val )
{
  if ( 0 <= idx && idx < myObjects.size() )
    myObjects[idx].insert( inf, val );
}
