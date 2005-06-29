
#include "SalomeApp_Selection.h"

#include "SalomeApp_SelectionMgr.h"
#include "SalomeApp_DataOwner.h"
#include "SalomeApp_Study.h"
#include "SalomeApp_Application.h"

#include "SUIT_Session.h"

SalomeApp_Selection::SalomeApp_Selection()
: myStudy( 0 )
{
}

SalomeApp_Selection::~SalomeApp_Selection()
{
}

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

int SalomeApp_Selection::count() const
{
  return myEntries.count();
}

QtxValue SalomeApp_Selection::param( const int, const QString& p ) const
{
  if( p=="client" )
    return myPopupClient;
  else    
    return QtxValue();
}

QtxValue SalomeApp_Selection::globalParam( const QString& p ) const
{
  if( p=="client" )
    return myPopupClient;
  else if ( p=="activeView" )
    {
      QString aViewType = "";
      SUIT_ViewWindow* anActiveView = study()->application()->desktop()->activeWindow();
      if (anActiveView)
	aViewType = anActiveView->getViewManager()->getType();
      return QtxValue(aViewType);
    }
  else
    return QtxPopupMgr::Selection::globalParam( p );
}

void SalomeApp_Selection::processOwner( const SalomeApp_DataOwner* )
{
}

QString SalomeApp_Selection::entry( const int index ) const
{
  if ( index >= 0 && index < count() )
    return myEntries[ index ];
  return QString();
}
