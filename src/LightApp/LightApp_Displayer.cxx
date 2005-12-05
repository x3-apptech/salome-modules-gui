// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/
//

#include "LightApp_Displayer.h"
#include "LightApp_Application.h"
#include "LightApp_Module.h"

#include <CAM_Study.h>

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

bool LightApp_Displayer::canBeDisplayed( const QString&, const QString& ) const
{
  return true;
}

bool LightApp_Displayer::canBeDisplayed( const QString& entry ) const
{
  QString viewerType;
  SUIT_Session* session = SUIT_Session::session();
  if(  SUIT_Application* app = session->activeApplication() )
    if( LightApp_Application* sApp = dynamic_cast<LightApp_Application*>( app ) )
      if( SUIT_ViewManager* vman = sApp->activeViewManager() )
	if( SUIT_ViewModel* vmod = vman->getViewModel() )
	  viewerType = vmod->getType();
  return !viewerType.isNull() && canBeDisplayed( entry, viewerType );
}

LightApp_Displayer* LightApp_Displayer::FindDisplayer( const QString& mod_name, const bool load )
{
  SUIT_Session* session = SUIT_Session::session();
  SUIT_Application* sapp = session ? session->activeApplication() : 0;
  LightApp_Application* app = dynamic_cast<LightApp_Application*>( sapp );
  if( !app )
    return 0;

  LightApp_Module* m = dynamic_cast<LightApp_Module*>( app ? app->module( mod_name ) : 0 );
  if( !m && load )
  {
    m = dynamic_cast<LightApp_Module*>( app->loadModule( mod_name ) );
    if( m )
      app->addModule( m );
  }

  if( m )
  {
    m->connectToStudy( dynamic_cast<CAM_Study*>( app->activeStudy() ) );
    if( m!=app->activeModule() && load )
    {
      m->setMenuShown( false );
      m->setToolShown( false );
    }
  }
  return m ? m->displayer() : 0;
}
