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

#include "SalomeApp_VisualState.h"

#include "SalomeApp_Module.h"
#include "SalomeApp_Study.h"
#include "SalomeApp_Application.h"

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ViewWindow.h>
#include <QtxWorkstack.h>

#include <STD_TabDesktop.h>

#include <QList>
#include <QApplication>
#include <QMultiHash>

#include <SALOMEDSClient_ClientFactory.hxx>//?
#include <SALOMEDSClient_IParameters.hxx>//?

#include <vector>//?
#include <string>//?
#include <cstdio>

/*!
  Constructor.
*/
SalomeApp_VisualState::SalomeApp_VisualState( SalomeApp_Application* app )
  : QObject(),
    myApp( app )
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
  QMap<QString, int> viewersCounter;
  for ( QList<SUIT_ViewManager*>::const_iterator it = lst.begin(); it != lst.end(); ++it )
  {
    SUIT_ViewManager* aVM = *it;
    if ( !aVM )
      continue;

    int view_count = aVM->getViewsCount();
    QString vType = aVM->getType();
    if ( !view_count )
      continue; //No views is opened in the viewer

    if ( !viewersCounter.contains( vType ) )
      viewersCounter.insert( vType, 0 );

    int& viewerID = viewersCounter[vType];

    QVector<SUIT_ViewWindow*> views = aVM->getViews();
    for ( int i = 0; i < view_count; i++ )
    {
      QString vName = QString( "%1_%2_%3" ).arg( vType ).arg( viewerID ).arg( i );
      views[i]->setObjectName( vName );
    }
    viewerID++;
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

  // unlock study if it is locked
  bool aLocked = study->studyDS()->GetProperties()->IsLocked();
  if (aLocked) study->studyDS()->GetProperties()->SetLocked(false);

  int savePoint = 1;
  std::vector<int> savePoints = study->getSavePoints();
  //Calculate a new savePoint number = the last save point number + 1
  if ( savePoints.size() > 0)
    savePoint = savePoints[savePoints.size()-1] + 1;

  _PTR(AttributeParameter) ap = study->studyDS()->GetCommonParameters( study->getVisualComponentName().toLatin1().constData(), 
								       savePoint );
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
    ip->setProperty("AP_ACTIVE_VIEW", win->objectName().toStdString() );

  int viewerID = 0;
  SUIT_ViewManager* vm = 0;
  QListIterator<SUIT_ViewManager*> it( lst );
  while ( it.hasNext() ) {
    vm = it.next();
    if ( !vm ) continue;

    int view_count = vm->getViewsCount();
    if ( !view_count )
      continue; //No views is opened in the viewer

    std::string viewerEntry = QString( "%1_%2" ).arg( vm->getType() ).arg( ++viewerID ).toStdString();
    ip->append("AP_VIEWERS_LIST", viewerEntry);

    QVector<SUIT_ViewWindow*> views = vm->getViews();
    for(int i = 0; i<view_count; i++) {
      ip->append( viewerEntry, views[i]->windowTitle().toStdString() );
      ip->append( viewerEntry, views[i]->getVisualParameters().toStdString() );
    }
  }

  //Save information about split areas
  if ( myApp->desktop()->inherits( "STD_TabDesktop" ) )
  {
    QString workstackInfo;
    QtxWorkstack* workstack = ((STD_TabDesktop*)myApp->desktop())->workstack();
    QByteArray geomState = workstack->saveState( 0 );
    ip->setProperty( "AP_WORKSTACK_INFO", QString( geomState.toHex() ).toStdString() );
  }

  //Save a name of the active module
  if ( CAM_Module* activeModule = myApp->activeModule() )
    ip->setProperty( "AP_ACTIVE_MODULE", activeModule->moduleName().toStdString() );

  //Store visual parameters of the modules
  QList<CAM_Module*> mlist;
  myApp->modules( mlist );
  QListIterator<CAM_Module*> itM( mlist );
  CAM_Module* module = 0;
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  bool loadLight = aResMgr->booleanValue( "Study", "autoload_light_modules", true );

  while ( itM.hasNext() ) {
    module = itM.next();
    if ( !module ) continue;


    if ( LightApp_Module* lModule = dynamic_cast<LightApp_Module*>( module ) ) {
      if (loadLight)
        ip->append( "AP_MODULES_LIST", lModule->moduleName().toStdString() );
      if ( SalomeApp_Module* sModule = dynamic_cast<SalomeApp_Module*>( module ) )
        sModule->storeVisualParameters( savePoint );
    }
  }

  // set default name of new savePoint
  study->setNameOfSavePoint( savePoint, QObject::tr( "SAVE_POINT_DEF_NAME" ) + QString::number( savePoint ) );

  if (aLocked) study->studyDS()->GetProperties()->SetLocked(true);

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

  _PTR(AttributeParameter) ap = study->studyDS()->GetCommonParameters( study->getVisualComponentName().toLatin1().constData(),
								       savePoint );
  _PTR(IParameters) ip = ClientFactory::getIParameters(ap);

  qApp->installEventFilter( this );

  //Remove all already existent veiwers and their views
  //myApp->clearViewManagers();
  myApp->clearKnownViewManagers();

  //Restore the viewers and view windows
  int nbViewers = ip->nbValues( "AP_VIEWERS_LIST" );
  SUIT_ViewWindow* viewWin = 0;

  // parameters of view windows are stored in a map for restoring after restoring of the workstack
  QMap<SUIT_ViewWindow*, QString> viewersParameters;

  for ( int i = 0; i < nbViewers; i++ )
  {
    std::string viewerEntry = ip->getValue( "AP_VIEWERS_LIST", i );
    std::vector<std::string> veiewerParams = ip->parseValue(viewerEntry,'_');
    std::string type = veiewerParams[0];
    std::string viewerID = veiewerParams[1];
    SUIT_ViewManager* vm = myApp->newViewManager( type.c_str() );
    if ( !vm )
      continue; //Unknown viewer

    int nbViews = (ip->nbValues(viewerEntry))/2;

    //Create nbViews-1 view (-1 because 1 view is created by createViewManager)
    for ( int i = 1; i< nbViews; i++ )
    {
      SUIT_ViewWindow* aView = vm->createViewWindow();
      aView->show();
    }

    int viewCount = vm->getViewsCount();
    if ( viewCount != nbViews )
    {
      printf( "\nRestore visual state: Unknow error, Can't create a view!\n" );
      continue;
    }

    //Resize the views, set their captions and apply visual parameters.
    QVector<SUIT_ViewWindow*> views = vm->getViews();
    for ( int i = 0, j = 0; i<viewCount; i++, j+=2 )
    {
      viewWin = views[i];
      if ( !viewWin )
        continue;

      // wait untill the window is really shown.  This step fixes MANY bugs..
      //      while ( !vm->isVisible() )
      //        qApp->processEvents();

      viewWin->setWindowTitle( ip->getValue( viewerEntry, j ).c_str() );

      //      printf ( "VP for viewWin \"%s\": %s\n", viewerEntry.c_str(), ip->getValue(viewerEntry, j+1).c_str() );
      viewersParameters[ viewWin ] = ip->getValue( viewerEntry, j + 1 ).c_str();
      //viewWin->setVisualParameters(ip->getValue(viewerEntry, j+1).c_str());
    }
  }

  qApp->processEvents( QEventLoop::ExcludeUserInputEvents, 5000 );

  // restore modules' visual parameters
  std::vector<std::string> v = ip->getValues( "AP_MODULES_LIST" );
  for ( int i = 0; i < v.size(); i++ )
  {
    myApp->activateModule( v[i].c_str() );
    if ( SalomeApp_Module* module = dynamic_cast<SalomeApp_Module*>( myApp->activeModule() ) )
      module->restoreVisualParameters( savePoint );
  }

  // new view windows may have been created in  module->restoreVisualParameters() [GAUSS]
  // so here we store their visual parameters for later restoring..
  QList<SUIT_ViewManager*> lst;
  myApp->viewManagers( lst );
  for ( QList<SUIT_ViewManager*>::iterator itVM = lst.begin(); itVM != lst.end(); ++itVM )
  {
    SUIT_ViewManager* aVM = *itVM;
    if ( !aVM )
      continue;

    int view_count = aVM->getViewsCount();
    QVector<SUIT_ViewWindow*> views = aVM->getViews();
    for ( int i = 0; i < view_count; i++ )
    {
      if ( !viewersParameters.contains( views[i] ) )
      {
        viewersParameters[ views[i] ] = views[i]->getVisualParameters();
        //      printf ( "store VP for viewWin \"%s\": %s\n", views[i]->name(), views[i]->getVisualParameters().toLatin1().constData() );
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

  qApp->processEvents( QEventLoop::ExcludeUserInputEvents, 5000 );

  // restore workstack parameters.  should be done after module's restoreVisualParameters(), because
  // some modules can create their own viewers (like VISU creates GaussViewers)
  if ( myApp->desktop()->inherits( "STD_TabDesktop" ) )
  {
    QtxWorkstack* workstack = ((STD_TabDesktop*)myApp->desktop())->workstack();
    workstack->restoreState( QByteArray::fromHex( QByteArray( ip->getProperty( "AP_WORKSTACK_INFO" ).c_str() ) ), 0 );
  }

  // restore visual parameters of view windows.  it must be done AFTER restoring workstack.
  // also set active view
  std::string activeViewName = ip->getProperty("AP_ACTIVE_VIEW");
  QMap<SUIT_ViewWindow*, QString>::Iterator mapIt;
  for ( mapIt = viewersParameters.begin(); mapIt != viewersParameters.end(); ++mapIt ) {
    mapIt.key()->setVisualParameters( mapIt.value() );
    if ( activeViewName == mapIt.key()->objectName().toStdString() )
      mapIt.key()->setFocus();
  }

  qApp->removeEventFilter( this );

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
  //    views[i]->setFocus();
  //    }
  //  }
}

/*!
  Custom event filter
*/
bool SalomeApp_VisualState::eventFilter( QObject* o, QEvent* e )
{
  // eat keyboard and mouse events
  QEvent::Type aType = e->type();
  if ( aType == QEvent::MouseButtonDblClick ||
       aType == QEvent::MouseButtonPress ||
       aType == QEvent::MouseButtonRelease ||
       aType == QEvent::MouseMove ||
       aType == QEvent::KeyPress ||
       aType == QEvent::KeyRelease )
    return true;

  return QObject::eventFilter( o, e );
}
