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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "SalomeApp_Displayer.h"
#include "SalomeApp_Application.h"

#include <SALOME_InteractiveObject.hxx>

#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ViewModel.h>
#include <SUIT_ViewWindow.h>

#include <qstring.h>

/*!
  Default constructor
*/
SalomeApp_Displayer::SalomeApp_Displayer()
{
}

/*!
  Destructor
*/
SalomeApp_Displayer::~SalomeApp_Displayer()
{
}

/*!
  Displays object in view
  \param entry - object entry
  \param updateViewer - is it necessary to update viewer
  \param theViewFrame - view
*/
void SalomeApp_Displayer::Display( const QString& entry, const bool updateViewer, SALOME_View* theViewFrame )
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

/*!
  Redisplays object in view
  \param entry - object entry
  \param updateViewer - is it necessary to update viewer
*/
void SalomeApp_Displayer::Redisplay( const QString& entry, const bool updateViewer )
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

/*!
  Erases object in view
  \param entry - object entry
  \param forced - deletes object from viewer (otherwise it will be erased, but cached)
  \param updateViewer - is it necessary to update viewer
  \param theViewFrame - view
*/
void SalomeApp_Displayer::Erase( const QString& entry, const bool forced,
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

/*!
  Erases all objects in view
  \param forced - deletes objects from viewer
  \param updateViewer - is it necessary to update viewer
  \param theViewFrame - view
*/
void SalomeApp_Displayer::EraseAll( const bool forced, const bool updateViewer, SALOME_View* theViewFrame ) const
{
  SALOME_View* vf = theViewFrame ? theViewFrame : GetActiveView();

  if ( vf ) {
    vf->EraseAll( forced );
    if ( updateViewer )
      vf->Repaint();
  }
}

/*!
  \return true if object is displayed in viewer
  \param entry - object entry
  \param theViewFrame - view
*/
bool SalomeApp_Displayer::IsDisplayed( const QString& entry, SALOME_View* theViewFrame ) const
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

/*!
  Updates active view
*/
void SalomeApp_Displayer::UpdateViewer() const
{
  SALOME_View* vf = GetActiveView();
  if ( vf )
    vf->Repaint();
}

/*!
  \return presentation of object, built with help of CreatePrs method
  \param entry - object entry
  \param theViewFrame - view
  \sa CreatePrs()
*/
SALOME_Prs* SalomeApp_Displayer::buildPresentation( const QString& entry, SALOME_View* theViewFrame )
{
  SALOME_Prs* prs = 0;

  SALOME_View* vf = theViewFrame ? theViewFrame : GetActiveView();

  if ( vf )
    prs = vf->CreatePrs( entry.latin1() );

  return prs;
}

/*!
  \return active view
*/
SALOME_View* SalomeApp_Displayer::GetActiveView()
{
  SUIT_Session* session = SUIT_Session::session();
  if (  SUIT_Application* app = session->activeApplication() ) {
    if ( SalomeApp_Application* sApp = dynamic_cast<SalomeApp_Application*>( app ) ) {
      if( SUIT_ViewManager* vman = sApp->activeViewManager() ) {
	if ( SUIT_ViewModel* vmod = vman->getViewModel() )
	  return dynamic_cast<SALOME_View*>( vmod );
      }
    }
  }
  return 0;
}
