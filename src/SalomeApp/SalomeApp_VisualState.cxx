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
#include "SalomeApp_VisualState.h"

#include "SalomeApp_Module.h"
#include "SalomeApp_Study.h"
#include "SalomeApp_Application.h"

#include <SUIT_ResourceMgr.h>
#include <QtxWorkstack.h>

#include <STD_TabDesktop.h>

#include <qptrlist.h>
#include <qapplication.h>
#include <qdict.h>

#include <SALOMEDSClient_ClientFactory.hxx>
#include <SALOMEDSClient_IParameters.hxx>

#include <vector>
#include <string>

/*!
  Constructor.
*/
SalomeApp_VisualState::SalomeApp_VisualState( SalomeApp_Application* app )
  : myApp( app )
{
}  

/*!
  Destructor.
*/
SalomeApp_VisualState::~SalomeApp_VisualState()
{
}

/*!
  Sets names of all view windows in given list.  This is used
  in order to apply the same naming algorithm when saving and restoring
  view windows.  Names of view windows must be the same before saving
  workstack (splitters) information, and before its restoring! 
  Naming rule: ViewerType_IndexOfViewerOfThisType_IndexOfViewInThisViewer
               VTKViewer_0_0
               OCCViewer_0_0  OCCViewer_0_1  OCCViewer_0_2
               VTKViewer_1_0
*/
void nameViewWindows( const ViewManagerList& lst )
{
  QDict<int> viewersCounter; // map viewerType - to - index_of_this_viewer_type
  viewersCounter.setAutoDelete( true );
  for ( QPtrListIterator<SUIT_ViewManager> it(lst); it.current(); ++it) {
    int view_count = it.current()->getViewsCount();
    QString vType = it.current()->getType();
    if ( !view_count )
      continue; //No views is opened in the viewer
    
    int* viewerID = viewersCounter[ vType ];
    if ( !viewerID ) {
      viewerID = new int( 0 );
      viewersCounter.insert( vType, viewerID );
    }
    else
      ++(*viewerID);

    QPtrVector<SUIT_ViewWindow> views = it.current()->getViews();
    for ( int i = 0; i < view_count; i++ )  {
      QString vName = QString( "%1_%2_%3" ).arg( vType ).arg( *viewerID ).arg( i );
      views[i]->setName( vName );
    }
  }
}

/*!
  Stores the visual parameters of the viewers
*/
int SalomeApp_VisualState::storeState()
{
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( myApp->activeStudy() );
  if ( !study )
    return -1;

  int savePoint = 1;
  std::vector<int> savePoints = study->getSavePoints();
  //Calculate a new savePoint number = the last save point number + 1
  if ( savePoints.size() > 0) 
    savePoint = savePoints[savePoints.size()-1] + 1;

  _PTR(AttributeParameter) ap = study->studyDS()->GetCommonParameters( study->getVisualComponentName(), savePoint );
  _PTR(IParameters) ip = ClientFactory::getIParameters( ap );

  ViewManagerList lst;
  myApp->viewManagers( lst );

  // setting unique names for view windows in order to save this view inside 
  // workstack's structure (see below).  On restore the views with the same names will
  // be placed to the same place inside the workstack's splitters.
  nameViewWindows( lst );

  // store active window's name
  SUIT_ViewWindow* win = myApp->desktop()->activeWindow();  
  if ( win )
    ip->setProperty("AP_ACTIVE_VIEW", win->name() );

  int viewerID = 0;
  SUIT_ViewManager* vm = 0;
  for (QPtrListIterator<SUIT_ViewManager> it( lst ); it.current(); ++it ) {
    vm = it.current();
    int view_count = vm->getViewsCount();
    if ( !view_count ) 
      continue; //No views is opened in the viewer
      
    std::string viewerEntry = QString( "%1_%2" ).arg( vm->getType() ).arg( ++viewerID ).latin1();
    ip->append("AP_VIEWERS_LIST", viewerEntry);
    
    QPtrVector<SUIT_ViewWindow> views = vm->getViews();
    for(int i = 0; i<view_count; i++) {
      ip->append( viewerEntry, views[i]->caption().latin1() );
      ip->append( viewerEntry, views[i]->getVisualParameters().latin1() );
    }
  }

  //Save information about split areas
  if ( myApp->desktop()->inherits( "STD_TabDesktop" ) ) {
    QtxWorkstack* workstack = ((STD_TabDesktop*)myApp->desktop())->workstack();
    QString workstackInfo;
    (*workstack) >> workstackInfo;
    ip->setProperty( "AP_WORKSTACK_INFO", workstackInfo.latin1() );
  }
  
  //Save a name of the active module
  if ( CAM_Module* activeModule = myApp->activeModule() ) 
    ip->setProperty( "AP_ACTIVE_MODULE", activeModule->moduleName().latin1() );

  //Store visual parameters of the modules
  QPtrList<CAM_Module> mlist; 
  myApp->modules( mlist );
  CAM_Module* module = 0;
  for ( module = mlist.first(); module; module = mlist.next() ) {
    if ( SalomeApp_Module* sModule = dynamic_cast<SalomeApp_Module*>( module ) ) {
      ip->append( "AP_MODULES_LIST", sModule->moduleName().latin1() );
      sModule->storeVisualParameters( savePoint );
    }
  }

  // set default name of new savePoint
  study->setNameOfSavePoint( savePoint, QObject::tr( "SAVE_POINT_DEF_NAME" ) + QString::number( savePoint ) );
  
  return savePoint;
}

/*!
  Restores the visual parameters of the viewers
*/
void SalomeApp_VisualState::restoreState(int savePoint)
{
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( myApp->activeStudy() );
  if ( !study )
    return;

  _PTR(AttributeParameter) ap = study->studyDS()->GetCommonParameters( study->getVisualComponentName(), savePoint );
  _PTR(IParameters) ip = ClientFactory::getIParameters(ap);

  //Remove all already existent veiwers and their views
  ViewManagerList lst;
  myApp->viewManagers( lst );
  for ( QPtrListIterator<SUIT_ViewManager> it(lst); it.current(); ++it ) {
    myApp->removeViewManager( it.current() );
    qApp->processEvents();
  }
  //Restore the viewers and view windows
  int nbViewers = ip->nbValues( "AP_VIEWERS_LIST" );
  SUIT_ViewWindow* viewWin = 0;

  // parameters of view windows are stored in a map for restoring after restoring of the workstack
  QMap<SUIT_ViewWindow*, QString> viewersParameters;

  for ( int i = 0; i < nbViewers; i++ ) {
    std::string viewerEntry = ip->getValue( "AP_VIEWERS_LIST", i );
    std::vector<std::string> veiewerParams = ip->parseValue(viewerEntry,'_');
    std::string type = veiewerParams[0];
    std::string viewerID = veiewerParams[1];
    SUIT_ViewManager* vm = myApp->newViewManager( type.c_str() );
    if ( !vm ) 
      continue; //Unknown viewer
    
    int nbViews = (ip->nbValues(viewerEntry))/2;
    
    //Create nbViews-1 view (-1 because 1 view is created by createViewManager)
    for ( int i = 1; i< nbViews; i++ ) { 
      SUIT_ViewWindow* aView = vm->createViewWindow();
      aView->show();
    }

    int viewCount = vm->getViewsCount();
    if (viewCount != nbViews) {
      printf( "\nRestore visual state: Unknow error, Can't create a view!\n" );
      continue;
    }

    //Resize the views, set their captions and apply visual parameters.
    QPtrVector<SUIT_ViewWindow> views = vm->getViews();  
    for (int i = 0, j = 0; i<viewCount; i++, j+=2) {
      viewWin = views[i];
      if ( !viewWin ) 
	continue;

      // wait untill the window is really shown.  This step fixes MANY bugs..
      while ( !vm->isVisible() )
	qApp->processEvents();
      
      viewWin->setCaption(ip->getValue(viewerEntry, j).c_str());
      
      //      printf ( "VP for viewWin \"%s\": %s\n", viewerEntry.c_str(), ip->getValue(viewerEntry, j+1).c_str() );
      viewersParameters[ viewWin ] = ip->getValue(viewerEntry, j+1).c_str();
      //viewWin->setVisualParameters(ip->getValue(viewerEntry, j+1).c_str());
    }
  }

  // restore modules' visual parameters
  std::vector<std::string> v = ip->getValues("AP_MODULES_LIST");
  for ( int i = 0; i < v.size(); i++ ) {
    myApp->activateModule( v[i].c_str() );
    if ( SalomeApp_Module* module = dynamic_cast<SalomeApp_Module*>( myApp->activeModule() ) )
      module->restoreVisualParameters( savePoint );
  }

  // new view windows may have been created in  module->restoreVisualParameters() [GAUSS]
  // so here we store their visual parameters for later restoring..
  lst.clear();
  myApp->viewManagers(lst);
  QPtrListIterator<SUIT_ViewManager> it( lst );
  for ( ; it.current(); ++it ) {
    int view_count = it.current()->getViewsCount();
    QPtrVector<SUIT_ViewWindow> views = it.current()->getViews();
    for ( int i = 0; i < view_count; i++ ) {
      if ( !viewersParameters.contains( views[i] ) ) {
	viewersParameters[ views[i] ] = views[i]->getVisualParameters();
	//	printf ( "store VP for viewWin \"%s\": %s\n", views[i]->name(), views[i]->getVisualParameters().latin1() );
      }
    }
  }  

  // activate module that was active on save
  QString activeModuleName( ip->getProperty("AP_ACTIVE_MODULE" ).c_str() );
  if ( !activeModuleName.isEmpty() ) 
    myApp->activateModule( activeModuleName );

  // setting unique names for view windows in order to restore positions of view windows inside 
  // workstack's structure (see below).  During save the same naming algorithm was used, 
  // so the same views will get the same names.
  nameViewWindows( lst );

  // restore workstack parameters.  should be done after module's restoreVisualParameters(), because
  // some modules can create their own viewers (like VISU creates GaussViewers)
  if ( myApp->desktop()->inherits( "STD_TabDesktop" ) ) {
    QtxWorkstack* workstack = ((STD_TabDesktop*)myApp->desktop())->workstack();
    (*workstack) << ip->getProperty( "AP_WORKSTACK_INFO" ).c_str();
  }

  // restore visual parameters of view windows.  it must be done AFTER restoring workstack.
  // also set active view
  std::string activeViewName = ip->getProperty("AP_ACTIVE_VIEW");
  QMap<SUIT_ViewWindow*, QString>::Iterator mapIt;
  for ( mapIt = viewersParameters.begin(); mapIt != viewersParameters.end(); ++mapIt ) {
    mapIt.key()->setVisualParameters( mapIt.data() );
    if ( activeViewName == mapIt.key()->name() )
      mapIt.key()->setFocus();
  }
  
  //  for ( it.toFirst(); it.current(); ++it ) {
  //    int view_count = it.current()->getViewsCount();
  //    QPtrVector<SUIT_ViewWindow> views = it.current()->getViews();
  //    for ( int i = 0; i < view_count; i++ )
  //      views[i]->setVisualParameters( viewersParameters[ views[i]->name() ] );
  //  }

  // set focus to previously saved active view window
  //  std::string activeViewName = ip->getProperty("AP_ACTIVE_VIEW");
  //  for ( it.toFirst(); it.current(); ++it ) {
  //    int view_count = it.current()->getViewsCount();
  //    QPtrVector<SUIT_ViewWindow> views = it.current()->getViews();
  //    for ( int i = 0; i < view_count; i++ )  {
  //      if ( activeViewName == views[i]->name() )
  //	views[i]->setFocus();
  //    }
  //  }
}
