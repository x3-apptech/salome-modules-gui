
#include "LightApp_Displayer.h"
#include "LightApp_Application.h"

#include <SALOME_InteractiveObject.hxx>

#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ViewModel.h>
#include <SUIT_ViewWindow.h>

#include <qstring.h>

LightApp_Displayer::LightApp_Displayer()
{
}

LightApp_Displayer::~LightApp_Displayer()
{
}

void LightApp_Displayer::Display( const QString& entry, const bool updateViewer, SALOME_View* theViewFrame )
{
  SALOME_View* vf = theViewFrame ? theViewFrame : GetActiveView();
  if ( vf )
  {
    SALOME_Prs* prs = buildPresentation( entry, vf );

    if ( prs )
    {
      vf->BeforeDisplay( this );
      vf->Display( prs );
      vf->AfterDisplay( this );

      if ( updateViewer )
        vf->Repaint();

      delete prs;  // delete presentation because displayer is its owner
    }
  }
}

void LightApp_Displayer::Redisplay( const QString& entry, const bool updateViewer )
{
  // Remove the object permanently (<forced> == true)
  SUIT_Session* ses = SUIT_Session::session();
  SUIT_Application* app = ses->activeApplication();
  if ( app )
  {
    SUIT_Desktop* desk = app->desktop();
    QPtrList<SUIT_ViewWindow> wnds = desk->windows();
    SUIT_ViewWindow* wnd;
    for ( wnd = wnds.first(); wnd; wnd = wnds.next() )
    {
      SUIT_ViewManager* vman = wnd->getViewManager();
      if( !vman )
        continue;

      SUIT_ViewModel* vmodel = vman->getViewModel();
      if( !vmodel )
	continue;
	
      SALOME_View* view = dynamic_cast<SALOME_View*>(vmodel);
      if( view && ( IsDisplayed( entry, view ) || view == GetActiveView() ) )
      {
	Erase( entry, true, false, view );
	Display( entry, updateViewer, view );
      }
    }
  }
}

void LightApp_Displayer::Erase( const QString& entry, const bool forced,
                                const bool updateViewer, SALOME_View* theViewFrame )
{
  SALOME_View* vf = theViewFrame ? theViewFrame : GetActiveView();

  if ( vf ) {
    SALOME_Prs* prs = vf->CreatePrs( entry.latin1() );
    if ( prs ) {
      vf->Erase( prs, forced );
      if ( updateViewer )
	vf->Repaint();
      delete prs;  // delete presentation because displayer is its owner
    }
  }
}

void LightApp_Displayer::EraseAll( const bool forced, const bool updateViewer, SALOME_View* theViewFrame ) const
{
  SALOME_View* vf = theViewFrame ? theViewFrame : GetActiveView();

  if ( vf ) {
    vf->EraseAll( forced );
    if ( updateViewer )
      vf->Repaint();
  }
}

bool LightApp_Displayer::IsDisplayed( const QString& entry, SALOME_View* theViewFrame ) const
{
  SALOME_View* vf = theViewFrame ? theViewFrame : GetActiveView();
  if( vf )
  {
    Handle( SALOME_InteractiveObject ) temp = new SALOME_InteractiveObject();
    temp->setEntry( entry.latin1() );
    return vf->isVisible( temp );
  }
  else
    return false;
}

void LightApp_Displayer::UpdateViewer() const
{
  SALOME_View* vf = GetActiveView();
  if ( vf )
    vf->Repaint();
}

SALOME_Prs* LightApp_Displayer::buildPresentation( const QString& entry, SALOME_View* theViewFrame )
{
  SALOME_Prs* prs = 0;

  SALOME_View* vf = theViewFrame ? theViewFrame : GetActiveView();

  if ( vf )
    prs = vf->CreatePrs( entry.latin1() );

  return prs;
}

SALOME_View* LightApp_Displayer::GetActiveView()
{
  SUIT_Session* session = SUIT_Session::session();
  if (  SUIT_Application* app = session->activeApplication() ) {
    if ( LightApp_Application* sApp = dynamic_cast<LightApp_Application*>( app ) ) {
      if( SUIT_ViewManager* vman = sApp->activeViewManager() ) {
	if ( SUIT_ViewModel* vmod = vman->getViewModel() )
	  return dynamic_cast<SALOME_View*>( vmod );
      }
    }
  }
  return 0;
}
