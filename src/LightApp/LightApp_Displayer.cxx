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

#include "LightApp_Displayer.h"
#include "LightApp_Application.h"
#include "LightApp_Module.h"
#include "LightApp_Study.h"

#include <CAM_Study.h>

#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ViewModel.h>
#include <SUIT_ViewWindow.h>

#include <QStringList>
#include <QString>
#ifndef DISABLE_SALOMEOBJECT
  #include "SALOME_InteractiveObject.hxx"
#endif

/*!
  Default constructor
*/
LightApp_Displayer::LightApp_Displayer()
{
}

/*!
  Destructor
*/
LightApp_Displayer::~LightApp_Displayer()
{
}

/*!
  Displays object in view
  \param entry - object entry
  \param updateViewer - is it necessary to update viewer
  \param theViewFrame - view
*/
void LightApp_Displayer::Display( const QString& entry, const bool updateViewer,
                                  SALOME_View* theViewFrame )
{
  QStringList aList;
  aList.append( entry );
  Display( aList, updateViewer, theViewFrame );
}

/*!
  Displays object in view
  \param list - object entries
  \param updateViewer - is it necessary to update viewer
  \param theViewFrame - view
*/
void LightApp_Displayer::Display( const QStringList& list, const bool updateViewer,
                                  SALOME_View* theViewFrame )
{
  SALOME_View* vf = theViewFrame ? theViewFrame : GetActiveView();
  QStringList::const_iterator it = list.constBegin();
  for ( ; it != list.constEnd(); ++it)
  {
    SALOME_Prs* prs = buildPresentation( *it, vf );
    if ( prs )
    {

      if ( vf )
      {
	myLastEntry = *it;
        vf->BeforeDisplay( this, prs );
        vf->Display( this, prs );
        vf->AfterDisplay( this, prs );

        if ( updateViewer )
          vf->Repaint();
      }
      delete prs;  // delete presentation because displayer is its owner
      setVisibilityState(*it, Qtx::ShownState);
    }
  }
}

/*!
  Redisplays object in view
  \param entry - object entry
  \param updateViewer - is it necessary to update viewer
*/
void LightApp_Displayer::Redisplay( const QString& entry, const bool updateViewer )
{
  // Remove the object permanently (<forced> == true)
  SUIT_Session* ses = SUIT_Session::session();
  SUIT_Application* app = ses->activeApplication();
  if ( app )
  {
    SUIT_Desktop* desk = app->desktop();
    QListIterator<SUIT_ViewWindow*> itWnds( desk->windows() );
    while ( itWnds.hasNext() )
    {
      SUIT_ViewManager* vman = itWnds.next()->getViewManager();
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
void LightApp_Displayer::Erase( const QString& entry, const bool forced,
                                const bool updateViewer,
                                SALOME_View* theViewFrame )
{
  QStringList aList;
  aList.append( entry );
  Erase( aList, forced, updateViewer, theViewFrame );
}

/*!
  Erases object in view
  \param list - object entries
  \param forced - deletes object from viewer (otherwise it will be erased, but cached)
  \param updateViewer - is it necessary to update viewer
  \param theViewFrame - view
*/
void LightApp_Displayer::Erase( const QStringList& list, const bool forced,
                                const bool updateViewer,
                                SALOME_View* theViewFrame )
{
  SALOME_View* vf = theViewFrame ? theViewFrame : GetActiveView();

  if ( !vf )
    return;

  QStringList::const_iterator it = list.constBegin();
  for ( ; it != list.constEnd(); ++it)
  {
    SALOME_Prs* prs = vf->CreatePrs( (*it).toLatin1().data() );
    if ( prs ) {
      myLastEntry = *it;
      vf->BeforeErase( this, prs );
      vf->Erase( this, prs, forced );
      vf->AfterErase( this, prs );
      if ( updateViewer )
        vf->Repaint();
      delete prs;  // delete presentation because displayer is its owner
      setVisibilityState(*it,Qtx::HiddenState);
    }
  }
}

/*!
  Erases all objects in view
  \param forced - deletes objects from viewer
  \param updateViewer - is it necessary to update viewer
  \param theViewFrame - view
*/
void LightApp_Displayer::EraseAll( const bool forced, const bool updateViewer, SALOME_View* theViewFrame )
{
  SALOME_View* vf = theViewFrame ? theViewFrame : GetActiveView();

  if ( vf ) {
    vf->EraseAll( this, forced );
    if ( updateViewer )
      vf->Repaint();
  }

  LightApp_Application* app = dynamic_cast<LightApp_Application*>( SUIT_Session::session()->activeApplication() );
  LightApp_Study* study = app ? dynamic_cast<LightApp_Study*>( app->activeStudy() ) : 0;  
  if(study)
    study->setVisibilityStateForAll(Qtx::HiddenState);
}

/*!
  \return true if object is displayed in viewer
  \param entry - object entry
  \param theViewFrame - view
*/
bool LightApp_Displayer::IsDisplayed( const QString& entry, SALOME_View* theViewFrame ) const
{
  SALOME_View* vf = theViewFrame ? theViewFrame : GetActiveView();
  bool res = false;
  if( vf )
  {
#ifndef DISABLE_SALOMEOBJECT
    Handle( SALOME_InteractiveObject ) temp = new SALOME_InteractiveObject();
    temp->setEntry( entry.toLatin1() );
    res = vf->isVisible( temp );
#endif
  }
  return res;
}

/*!
  Updates active view
*/
void LightApp_Displayer::UpdateViewer() const
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
SALOME_Prs* LightApp_Displayer::buildPresentation( const QString& entry, SALOME_View* theViewFrame )
{
  SALOME_Prs* prs = 0;

  SALOME_View* vf = theViewFrame ? theViewFrame : GetActiveView();

  if ( vf )
    prs = vf->CreatePrs( entry.toLatin1() );

  return prs;
}

/*!
  \return active view
*/
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

/*!
  \return true, if object can be displayed in this type of viewer
  \param entry - object entry
  \param viewer_type - type of viewer
*/
bool LightApp_Displayer::canBeDisplayed( const QString& /*entry*/, const QString& /*viewer_type*/ ) const
{
  return true;
}

/*!
  \return true, if object can be displayed in any type of viewer
  \param entry - object entry
*/
bool LightApp_Displayer::canBeDisplayed( const QString& entry ) const
{
  QString viewerType;
  SUIT_Session* session = SUIT_Session::session();
  if(  SUIT_Application* app = session->activeApplication() )
    if( LightApp_Application* sApp = dynamic_cast<LightApp_Application*>( app ) )
      if( SUIT_ViewManager* vman = sApp->activeViewManager() )
        if( SUIT_ViewModel* vmod = vman->getViewModel() )
          viewerType = vmod->getType();
  return canBeDisplayed( entry, viewerType );
}

/*!
  \return displayer, corresponding to module
  \param mod_name - name of module
  \param load - is module has to be forced loaded
*/
LightApp_Displayer* LightApp_Displayer::FindDisplayer( const QString& mod_name, const bool load )
{
  SUIT_Session* session = SUIT_Session::session();
  SUIT_Application* sapp = session ? session->activeApplication() : 0;
  LightApp_Application* app = dynamic_cast<LightApp_Application*>( sapp );
  if( !app )
    return 0;

  LightApp_Module* m = dynamic_cast<LightApp_Module*>( app ? app->module( mod_name ) : 0 );
  bool wasLoaded = false;
  if( !m && load )
  {
    m = dynamic_cast<LightApp_Module*>( app->loadModule( mod_name, false ) );
	if( m ) {
      app->addModule( m );
	  wasLoaded = true;
	}
  }

  if( m )
  {
    m->connectToStudy( dynamic_cast<CAM_Study*>( app->activeStudy() ) );
	if( wasLoaded ) 
		m->updateModuleVisibilityState();
  }
  return m ? m->displayer() : 0;
}

/*!
  Find the active study and set the 'visibility state' property of the object
  \param mod_name - name of module
  \param load - is module has to be forced loaded
*/
void LightApp_Displayer::setVisibilityState( const QString& theEntry, Qtx::VisibilityState theState) const {
  LightApp_Application* app = dynamic_cast<LightApp_Application*>( SUIT_Session::session()->activeApplication() );
  LightApp_Study* study = app ? dynamic_cast<LightApp_Study*>( app->activeStudy() ) : 0;
  
  if(study)
    study->setVisibilityState( theEntry, theState);
}
