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
// File:      LightApp_Module.cxx
// Created:   6/20/2005 16:30:56 AM
// Author:    OCC team
// Copyright (C) CEA 2005

#include "LightApp_Module.h"

#include "CAM_Application.h"

#include "LightApp_Application.h"
#include "LightApp_DataModel.h"
#include "LightApp_DataObject.h"
#include "LightApp_Study.h"
#include "LightApp_Preferences.h"
#include "LightApp_Selection.h"
#include "LightApp_Operation.h"
#include "LightApp_SwitchOp.h"
#include "LightApp_UpdateFlags.h"
#include "LightApp_ShowHideOp.h"

#include "SUIT_Operation.h"
#include <SUIT_Study.h>
#include <SUIT_DataObject.h>
#include <SUIT_ResourceMgr.h>

#ifndef DISABLE_VTKVIEWER
#ifndef DISABLE_SALOMEOBJECT
  #include <SVTK_ViewWindow.h>
  #include <SVTK_ViewModel.h>
#else
  #include <VTKViewer_ViewWindow.h>
#endif
  #include <VTKViewer_ViewModel.h>
#endif
#ifndef DISABLE_OCCVIEWER
  #include <OCCViewer_ViewWindow.h>
  #include <OCCViewer_ViewPort3d.h>
#ifndef DISABLE_SALOMEOBJECT
  #include <SOCC_ViewModel.h>
#else
  #include <OCCViewer_ViewModel.h>
#endif
#endif
#ifndef DISABLE_GLVIEWER
  #include <GLViewer_ViewFrame.h>
  #include <GLViewer_ViewPort.h>
#endif
#ifndef DISABLE_PLOT2DVIEWER
  #include <Plot2d_ViewWindow.h>
  #include <Plot2d_ViewFrame.h>
#ifndef DISABLE_SALOMEOBJECT
  #include <SPlot2d_ViewModel.h>
#else
  #include <Plot2d_ViewModel.h>
#endif
#endif

#include <OB_Browser.h>

#include <QtxPopupMgr.h>

#include <qvariant.h>
#include <qstring.h>
#include <qstringlist.h>

/*!Constructor.*/
LightApp_Module::LightApp_Module( const QString& name )
: CAM_Module( name ),
  myPopupMgr( 0 ),
  mySwitchOp( 0 ),
  myDisplay( -1 ),
  myErase( -1 ),
  myDisplayOnly( -1 ),
  myEraseAll( -1 )
{
}

/*!Destructor.*/
LightApp_Module::~LightApp_Module()
{
  if ( mySwitchOp )
    delete mySwitchOp;
}

/*!Initialize module.*/
void LightApp_Module::initialize( CAM_Application* app )
{
  CAM_Module::initialize( app );

  SUIT_ResourceMgr* resMgr = app ? app->resourceMgr() : 0;
  if ( resMgr )
    resMgr->raiseTranslators( name() );
}

/*!NOT IMPLEMENTED*/
void LightApp_Module::windows( QMap<int, int>& ) const
{
}

/*!NOT IMPLEMENTED*/
void LightApp_Module::viewManagers( QStringList& ) const
{
}

/*!Context menu popup.*/
void LightApp_Module::contextMenuPopup( const QString& client, QPopupMenu* menu, QString& /*title*/ )
{
  LightApp_Selection* sel = createSelection();
  sel->init( client, getApp()->selectionMgr() );
  popupMgr()->updatePopup( menu, sel );
  delete sel;
}

/*!Update object browser.
 * For updating model or whole object browser use update() method can be used.
*/
void LightApp_Module::updateObjBrowser( bool theIsUpdateDataModel, 
					SUIT_DataObject* theDataObject )
{
  bool upd = getApp()->objectBrowser()->isAutoUpdate();
  getApp()->objectBrowser()->setAutoUpdate( false );

  SUIT_DataObject* aDataObject = theDataObject;
  if( theIsUpdateDataModel ){
    if( CAM_DataModel* aDataModel = dataModel() ){
      if ( LightApp_DataModel* aModel = dynamic_cast<LightApp_DataModel*>( aDataModel ) ) {
	SUIT_DataObject* aParent = NULL;
	if(theDataObject && theDataObject != aDataModel->root())
	  aParent = theDataObject->parent();

	LightApp_DataObject* anObject = dynamic_cast<LightApp_DataObject*>(theDataObject);
	LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>(getApp()->activeStudy());
        aModel->update( anObject, aStudy );

	if(aParent && aParent->childPos(anObject) < 0)
	  aDataObject = dynamic_cast<LightApp_DataObject*>(aParent);
      }
    }
  }
  getApp()->objectBrowser()->setAutoUpdate( upd );
  getApp()->objectBrowser()->updateTree( 0, false /*aDataObject*/ );
}

/*!NOT IMPLEMENTED*/
void LightApp_Module::selectionChanged()
{
}

/*!Activate module.*/
bool LightApp_Module::activateModule( SUIT_Study* study )
{
  bool res = CAM_Module::activateModule( study );

  if ( res && application() && application()->resourceMgr() )
    application()->resourceMgr()->raiseTranslators( name() );

  connect( application(), SIGNAL( viewManagerAdded( SUIT_ViewManager* ) ),
           this, SLOT( onViewManagerAdded( SUIT_ViewManager* ) ) );
  connect( application(), SIGNAL( viewManagerRemoved( SUIT_ViewManager* ) ),
           this, SLOT( onViewManagerRemoved( SUIT_ViewManager* ) ) );

  if ( mySwitchOp == 0 )
    mySwitchOp = new LightApp_SwitchOp( this );

  return res;
}

/*!Deactivate module.*/
bool LightApp_Module::deactivateModule( SUIT_Study* study )
{
  delete mySwitchOp;
  mySwitchOp = 0;

  disconnect( application(), SIGNAL( viewManagerAdded( SUIT_ViewManager* ) ),
	      this, SLOT( onViewManagerAdded( SUIT_ViewManager* ) ) );
  disconnect( application(), SIGNAL( viewManagerRemoved( SUIT_ViewManager* ) ),
	      this, SLOT( onViewManagerRemoved( SUIT_ViewManager* ) ) );

  // abort all operations
  MapOfOperation::const_iterator anIt;
  for( anIt = myOperations.begin(); anIt != myOperations.end(); anIt++ ) {
    anIt.data()->abort();
  }

  return CAM_Module::activateModule( study );
}

/*!NOT IMPLEMENTED*/
void LightApp_Module::MenuItem()
{
}

/*!NOT IMPLEMENTED*/
void LightApp_Module::createPreferences()
{
}

/*!NOT IMPLEMENTED*/
void LightApp_Module::preferencesChanged( const QString&, const QString& )
{
}

/*!Gets application.*/
LightApp_Application* LightApp_Module::getApp() const
{
  return (LightApp_Application*)application();
}

/*!
 * \brief Update something in accordance with update flags
 * \param theFlags - update flags
*
* Update viewer or/and object browser etc. in accordance with update flags ( see
* LightApp_UpdateFlags enumeration ). Derived modules can redefine this method for their
* own purposes
*/
void LightApp_Module::update( const int theFlags )
{
  if ( theFlags & UF_Model )
  {
    if( CAM_DataModel* aDataModel = dataModel() )
      if( LightApp_DataModel* aModel = dynamic_cast<LightApp_DataModel*>( aDataModel ) )
        aModel->update( 0, dynamic_cast<LightApp_Study*>( getApp()->activeStudy() ) );
  }
  if ( theFlags & UF_ObjBrowser )
    getApp()->objectBrowser()->updateTree( 0 );
  if ( theFlags & UF_Controls )
    updateControls();
  if ( theFlags & UF_Viewer )
  {
    if ( SUIT_ViewManager* viewMgr = getApp()->activeViewManager() )
      if ( SUIT_ViewWindow* viewWnd = viewMgr->getActiveView() )
      {
#ifndef DISABLE_VTKVIEWER
#ifndef DISABLE_SALOMEOBJECT
        if ( viewWnd->inherits( "SVTK_ViewWindow" ) )
          ( (SVTK_ViewWindow*)viewWnd )->Repaint();
#else
        if ( viewWnd->inherits( "VTKViewer_ViewWindow" ) )
          ( (VTKViewer_ViewWindow*)viewWnd )->Repaint();
#endif
#endif
#ifndef DISABLE_OCCVIEWER
        if ( viewWnd->inherits( "OCCViewer_ViewWindow" ) )
          ( (OCCViewer_ViewWindow*)viewWnd )->getViewPort()->onUpdate();
#endif
#ifndef DISABLE_PLOT2DVIEWER
        if ( viewWnd->inherits( "Plot2d_ViewWindow" ) )
          ( (Plot2d_ViewWindow*)viewWnd )->getViewFrame()->Repaint();
#endif
#ifndef DISABLE_GLVIEWER
        if ( viewWnd->inherits( "GLViewer_ViewFrame" ) )
          ( (GLViewer_ViewFrame*)viewWnd )->getViewPort()->onUpdate();
#endif
      }
  }
}
/*!
 * \brief Updates controls
*
* Updates (i.e. disable/enable) controls states (menus, tool bars etc.). This method is
* called from update( UF_Controls ). You may redefine it in concrete module.
*/
void LightApp_Module::updateControls()
{
}

/*!Create new instance of data model and return it.*/
CAM_DataModel* LightApp_Module::createDataModel()
{
  return new LightApp_DataModel(this);
}

/*!Create and return instance of LightApp_Selection.*/
LightApp_Selection* LightApp_Module::createSelection() const
{
  return new LightApp_Selection();
}

/*!NOT IMPLEMENTED*/
void LightApp_Module::onModelOpened()
{
}

/*!NOT IMPLEMENTED*/
void LightApp_Module::onModelSaved()
{
}

/*!NOT IMPLEMENTED*/
void LightApp_Module::onModelClosed()
{
}

/*!Gets popup manager.(create if not exist)*/
QtxPopupMgr* LightApp_Module::popupMgr()
{
  if ( !myPopupMgr )
  {
    myPopupMgr = new QtxPopupMgr( 0, this );

    QPixmap p;
    SUIT_Desktop* d = application()->desktop();
    
    QAction 
      *disp = createAction( -1, tr( "TOP_DISPLAY" ), p, tr( "MEN_DISPLAY" ), tr( "STB_DISPLAY" ),
			    0, d, false, this, SLOT( onShowHide() ) ),
      *erase = createAction( -1, tr( "TOP_ERASE" ), p, tr( "MEN_ERASE" ), tr( "STB_ERASE" ),
			     0, d, false, this, SLOT( onShowHide() ) ),
      *dispOnly = createAction( -1, tr( "TOP_DISPLAY_ONLY" ), p, tr( "MEN_DISPLAY_ONLY" ), tr( "STB_DISPLAY_ONLY" ),
			        0, d, false, this, SLOT( onShowHide() ) ),
      *eraseAll = createAction( -1, tr( "TOP_ERASE_ALL" ), p, tr( "MEN_ERASE_ALL" ), tr( "STB_ERASE_ALL" ),
			        0, d, false, this, SLOT( onShowHide() ) );
    myDisplay     = actionId( disp );
    myErase       = actionId( erase );
    myDisplayOnly = actionId( dispOnly );
    myEraseAll    = actionId( eraseAll );

    myPopupMgr->insert( disp, -1, 0 ); 
    myPopupMgr->insert( erase, -1, 0 );
    myPopupMgr->insert( dispOnly, -1, 0 );
    myPopupMgr->insert( eraseAll, -1, 0 );
    myPopupMgr->insert( separator(), -1, 0 );

    QString oneAndNotActive = "( count( $component ) = 1 ) and ( component != activeModule )";
    QString uniform = "true in $canBeDisplayed and %1 and ( activeModule = '%2' )";
    uniform = uniform.arg( oneAndNotActive ).arg( name() );
    myPopupMgr->setRule( disp, /*QString( "( not isVisible ) and " ) + */ uniform, true );
    myPopupMgr->setRule( erase, /*QString( "( isVisible ) and " ) + */ uniform, true );
    myPopupMgr->setRule( dispOnly, uniform, true );

    QStringList viewers;

#ifndef DISABLE_OCCVIEWER
#ifndef DISABLE_SALOMEOBJECT
    viewers.append( SOCC_Viewer::Type() );
#else
    viewers.append( OCCViewer_Viewer::Type() );
#endif
#endif
#ifndef DISABLE_VTKVIEWER
#ifndef DISABLE_SALOMEOBJECT
    viewers.append( SVTK_Viewer::Type() );
#else
    viewers.append( VTKViewer_Viewer::Type() );
#endif
#endif
#ifndef DISABLE_PLOT2DVIEWER
#ifndef DISABLE_SALOMEOBJECT
    viewers.append( SPlot2d_Viewer::Type() );
#else
    viewers.append( Plot2d_Viewer::Type() );
#endif
#endif

    if( !viewers.isEmpty() )
    {
      QString strViewers = "{ ", temp = "'%1' ";
      QStringList::const_iterator anIt = viewers.begin(), aLast = viewers.end();
      for( ; anIt!=aLast; anIt++ )
        strViewers+=temp.arg( *anIt );
      strViewers+="}";
      myPopupMgr->setRule( eraseAll, QString( "client in %1" ).arg( strViewers ), true );
    }
  }
  return myPopupMgr;
}

/*!Gets preferences.*/
LightApp_Preferences* LightApp_Module::preferences() const
{
  LightApp_Preferences* pref = 0;
  if ( getApp() )
    pref = getApp()->preferences();
  return pref;
}

/*!Add preference to preferences.*/
int LightApp_Module::addPreference( const QString& label )
{
  LightApp_Preferences* pref = preferences();
  if ( !pref )
    return -1;

  int catId = pref->addPreference( moduleName(), -1 );
  if ( catId == -1 )
    return -1;

  return pref->addPreference( label, catId );
}

/*!Add preference to preferences.*/
int LightApp_Module::addPreference( const QString& label, const int pId, const int type,
                                    const QString& section, const QString& param )
{
  LightApp_Preferences* pref = preferences();
  if ( !pref )
    return -1;

  return pref->addPreference( moduleName(), label, pId, type, section, param );
}

/*!Gets property of preferences.*/
QVariant LightApp_Module::preferenceProperty( const int id, const QString& prop ) const
{
  QVariant var;
  LightApp_Preferences* pref = preferences();
  if ( pref )
    var = pref->itemProperty( id, prop );
  return var;
}

/*!Set property of preferences.*/
void LightApp_Module::setPreferenceProperty( const int id, const QString& prop, const QVariant& var )
{
  LightApp_Preferences* pref = preferences();
  if ( pref )
    pref->setItemProperty( id, prop, var );
}

/*!
 * \brief Starts operation with given identifier
  * \param id - identifier of operation to be started
*
* Module stores operations in map. This method starts operation by id.
* If operation isn't in map, then it will be created by createOperation method
* and will be inserted to map
*/
void LightApp_Module::startOperation( const int id )
{
  LightApp_Operation* op = 0;
  if( myOperations.contains( id ) )
    op = myOperations[ id ];
  else
  {
    op = createOperation( id );
    if( op )
    {
      myOperations.insert( id, op );
      op->setModule( this );
      connect( op, SIGNAL( stopped( SUIT_Operation* ) ), this, SLOT( onOperationStopped( SUIT_Operation* ) ) );
      connect( op, SIGNAL( destroyed() ), this, SLOT( onOperationDestroyed() ) );
    }
  }

  if( op )
  {
    // be sure that operation has correct study pointer
    op->setStudy( application() ? application()->activeStudy() : 0 );
    op->start();
  }
}

/*!
 * \brief Creates operation with given identifier
  * \param id - identifier of operation to be started
  * \return Pointer on created operation or NULL if operation is not created
*
* Creates operation with given id. You should not call this method, it will be called
* automatically from startOperation. You may redefine this method in concrete module to
* create operations. 
*/
LightApp_Operation* LightApp_Module::createOperation( const int id ) const
{
  if( id==-1 )
    return 0;

  if( id==myDisplay )
    return new LightApp_ShowHideOp( LightApp_ShowHideOp::DISPLAY );
  else if( id==myErase )
    return new LightApp_ShowHideOp( LightApp_ShowHideOp::ERASE );
  else if( id==myDisplayOnly )
    return new LightApp_ShowHideOp( LightApp_ShowHideOp::DISPLAY_ONLY );
  else if( id==myEraseAll )
    return new LightApp_ShowHideOp( LightApp_ShowHideOp::ERASE_ALL );
  else
    return 0;
}

/*!
 * \brief Virtual protected slot called when operation stopped
 * \param theOp - stopped operation
*
* Virtual protected slot called when operation stopped. Redefine this slot if you want to
* perform actions after stopping operation
*/
void LightApp_Module::onOperationStopped( SUIT_Operation* /*theOp*/ )
{
}

/*!
 * \brief Virtual protected slot called when operation destroyed
  * \param theOp - destroyed operation
*
* Virtual protected slot called when operation destroyed. Redefine this slot if you want to
* perform actions after destroying operation. Base implementation removes pointer on
* destroyed operation from the map of operations
*/
void LightApp_Module::onOperationDestroyed()
{
  const QObject* s = sender();
  if( s && s->inherits( "LightApp_Operation" ) )
  {
    const LightApp_Operation* op = ( LightApp_Operation* )s;
    MapOfOperation::const_iterator anIt = myOperations.begin(),
                                   aLast = myOperations.end();
    for( ; anIt!=aLast; anIt++ )
      if( anIt.data()==op )
      {
        myOperations.remove( anIt.key() );
        break;
      }
  }
}

/*!
  Must be redefined in order to use standard displayer mechanism
  \return displayer of module
*/
LightApp_Displayer* LightApp_Module::displayer()
{
  return 0;
}

/*!
  SLOT: called on activating of standard operations show/hide
*/
void LightApp_Module::onShowHide()
{
  if( !sender()->inherits( "QAction" ) || !popupMgr() )
    return;

  QAction* act = ( QAction* )sender();
  int id = actionId( act );
  if( id!=-1 )
    startOperation( id );
}

/*!
  virtual SLOT: called on view manager adding
*/
void LightApp_Module::onViewManagerAdded( SUIT_ViewManager* )
{
}

/*!
  virtual SLOT: called on view manager removing
*/
void LightApp_Module::onViewManagerRemoved( SUIT_ViewManager* )
{
}
