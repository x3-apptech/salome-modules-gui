
#include "LightApp_Selection.h"
#include "LightApp_SelectionMgr.h"
#include "LightApp_DataOwner.h"
#include "LightApp_Study.h"
#include "LightApp_Application.h"
#include "LightApp_Displayer.h"

#include "SUIT_Session.h"
#include "SUIT_ViewWindow.h"

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
  Initializetion.
*/
void LightApp_Selection::init( const QString& client, LightApp_SelectionMgr* mgr)
{
  myPopupClient = client;
  
  if( mgr )
  {
    if ( mgr->application() )
      myStudy = dynamic_cast<LightApp_Study*>( mgr->application()->activeStudy() );
    if ( !myStudy )
      return;

    SUIT_DataOwnerPtrList sel;
    //asl: fix for PAL10471
    //mgr->selected( sel, client );
    mgr->selected( sel );
    SUIT_DataOwnerPtrList::const_iterator anIt = sel.begin(), aLast = sel.end();

    QString entry, curEntry;
    for( ; anIt!=aLast; anIt++ )
    {
      SUIT_DataOwner* owner = ( SUIT_DataOwner* )( (*anIt ).get() );
      LightApp_DataOwner* sowner = dynamic_cast<LightApp_DataOwner*>( owner );
      if( sowner ) {
        curEntry = sowner->entry();
        entry = myStudy->referencedToEntry( curEntry );
        myEntries.append( entry );
        if ( curEntry == entry )
	  myIsReferences.append( true );
        else
	  myIsReferences.append( false );
        processOwner( sowner );
      }
    }
  }
}

/*!
  Gets count of entries.
*/
int LightApp_Selection::count() const
{
  return myEntries.count();
}

/*!
  Gets QtxValue();
*/
QtxValue LightApp_Selection::param( const int ind, const QString& p ) const
{
  if( !( ind>=0 && ind<count() ) )
    return QtxValue();

  if( p=="isVisible" )
  {
    LightApp_Displayer d;
    bool vis = d.IsDisplayed( myEntries[ ind ] );
    return QtxValue( vis, 0 );
  }
  else if( p=="component" ) {
    return myStudy->componentDataType( myEntries[ ind ] );
  }
  else if( p=="isReference" )
    return QtxValue( isReference( ind ), false );

  return QtxValue();
}

/*!
  Gets global parameters. client, isActiveView, activeView etc.
*/
QtxValue LightApp_Selection::globalParam( const QString& p ) const
{
  if      ( p == "client" )        return QtxValue( myPopupClient );
  else if ( p == "activeModule" )
  {
    LightApp_Application* app = dynamic_cast<LightApp_Application*>( myStudy->application() );
    QString mod_name = app ? QString( app->activeModule()->name() ) : QString::null;
    //cout << "activeModule : " << mod_name.latin1() << endl;
    if( !mod_name.isEmpty() )
      return mod_name;
    else
      return QtxValue();
  }
  else if ( p == "isActiveView" )  return QtxValue( (bool)activeVW() );
  else if ( p == "activeView" )    return QtxValue( activeViewType() );
#ifndef WNT
  else                             return QtxPopupMgr::Selection::globalParam( p );
#else
  else                             return Selection::globalParam( p );
#endif
}

/*!
  Do nothing.
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
  return QString::null;
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
