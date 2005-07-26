
#include "SalomeApp_Selection.h"

#include "SalomeApp_SelectionMgr.h"
#include "SalomeApp_DataOwner.h"
#include "SalomeApp_Study.h"
#include "SalomeApp_Application.h"

#include "SUIT_Session.h"
#include "SUIT_ViewWindow.h"

/*!
  Constructor
*/
SalomeApp_Selection::SalomeApp_Selection()
: myStudy( 0 )
{
}

/*!
  Destructor.
*/
SalomeApp_Selection::~SalomeApp_Selection()
{
}

/*!
  Initializetion.
*/
void SalomeApp_Selection::init( const QString& client, SalomeApp_SelectionMgr* mgr)
{
  myPopupClient = client;
  
  if( mgr )
  {
    if ( mgr->application() )
      myStudy = dynamic_cast<SalomeApp_Study*>( mgr->application()->activeStudy() );

    SUIT_DataOwnerPtrList sel;
    mgr->selected( sel, client );
    SUIT_DataOwnerPtrList::const_iterator anIt = sel.begin(), aLast = sel.end();
    for( ; anIt!=aLast; anIt++ )
    {
      SUIT_DataOwner* owner = ( SUIT_DataOwner* )( (*anIt ).get() );
      SalomeApp_DataOwner* sowner = dynamic_cast<SalomeApp_DataOwner*>( owner );
      if( sowner ) {
	myEntries.append( sowner->entry() );
	processOwner( sowner );
      }
    }
  }
}

/*!
  Gets count of entries.
*/
int SalomeApp_Selection::count() const
{
  return myEntries.count();
}

/*!
  Gets QtxValue();
*/
QtxValue SalomeApp_Selection::param( const int, const QString& p ) const
{
  return QtxValue();
}

/*!
  Gets global parameters. client, isActiveView, activeView etc.
*/
QtxValue SalomeApp_Selection::globalParam( const QString& p ) const
{
  if      ( p == "client" )        return QtxValue( myPopupClient );
  else if ( p == "isActiveView" )  return QtxValue( (bool)activeVW() );
  else if ( p == "activeView" )    return QtxValue( activeViewType() );
  else                             return QtxPopupMgr::Selection::globalParam( p );
}

/*!
  Do nothing.
*/
void SalomeApp_Selection::processOwner( const SalomeApp_DataOwner* )
{
}

/*!
  Gets entry with index \a index.
*/
QString SalomeApp_Selection::entry( const int index ) const
{
  if ( index >= 0 && index < count() )
    return myEntries[ index ];
  return QString();
}

/*!
  Gets type of active view manager.
*/
QString SalomeApp_Selection::activeViewType() const
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
SUIT_ViewWindow* SalomeApp_Selection::activeVW() const
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
