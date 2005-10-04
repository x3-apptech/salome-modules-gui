// File:      SalomeApp_Module.cxx
// Created:   10/25/2004 11:39:56 AM
// Author:    Sergey LITONIN
// Copyright (C) CEA 2004

#include "SalomeApp_Module.h"

#include "SalomeApp_Study.h"
#include "SalomeApp_DataModel.h"
#include "SalomeApp_Application.h"
#include "SalomeApp_Preferences.h"
#include "SalomeApp_UpdateFlags.h"
#include "SalomeApp_Operation.h"
#include "SalomeApp_SwitchOp.h"
#include "SalomeApp_ShowHideOp.h"

#include <OB_Browser.h>

#include <CAM_Study.h>

#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>

#include <QtxPopupMgr.h>

#include <SVTK_ViewWindow.h>
#include <OCCViewer_ViewWindow.h>
#include <OCCViewer_ViewPort3d.h>
#include <GLViewer_ViewFrame.h>
#include <GLViewer_ViewPort.h>
#include <Plot2d_ViewWindow.h>

/*!Constructor.*/
SalomeApp_Module::SalomeApp_Module( const QString& name )
: CAM_Module( name ),
myPopupMgr( 0 ),
mySwitchOp( 0 ),
myDisplay( -1 ),
myErase( -1 ),
myDisplayOnly( -1 )
{
}

/*!Destructor.*/
SalomeApp_Module::~SalomeApp_Module()
{
  if ( mySwitchOp )
    delete mySwitchOp;
}

/*!Initialize module.*/
void SalomeApp_Module::initialize( CAM_Application* app )
{
  CAM_Module::initialize( app );

  SUIT_ResourceMgr* resMgr = app ? app->resourceMgr() : 0;
  if ( resMgr )
    resMgr->raiseTranslators( name() );
}

/*!Activate module.*/
bool SalomeApp_Module::activateModule( SUIT_Study* study )
{
  bool res = CAM_Module::activateModule( study );

  if ( res && application() && application()->resourceMgr() )
    application()->resourceMgr()->raiseTranslators( name() );

  if ( mySwitchOp == 0 )
    mySwitchOp = new SalomeApp_SwitchOp( this );
    
  return res;
}

/*!Deactivate module.*/
bool SalomeApp_Module::deactivateModule( SUIT_Study* )
{
  delete mySwitchOp;
  mySwitchOp = 0;
  
  return true;
}

/*!NOT IMPLEMENTED*/
void SalomeApp_Module::selectionChanged()
{
}

/*!NOT IMPLEMENTED*/
void SalomeApp_Module::MenuItem()
{
}

/*!NOT IMPLEMENTED*/
void SalomeApp_Module::windows( QMap<int, int>& ) const
{
}

/*!NOT IMPLEMENTED*/
void SalomeApp_Module::viewManagers( QStringList& ) const
{
}

/*!NOT IMPLEMENTED*/
void SalomeApp_Module::createPreferences()
{
}

/*!NOT IMPLEMENTED*/
void SalomeApp_Module::preferencesChanged( const QString&, const QString& )
{
}

/*!Gets application.*/
SalomeApp_Application* SalomeApp_Module::getApp() const
{
  return (SalomeApp_Application*)application();
}

/*!NOT IMPLEMENTED*/
void SalomeApp_Module::onModelOpened()
{
}

/*!NOT IMPLEMENTED*/
void SalomeApp_Module::onModelSaved()
{
}

/*!NOT IMPLEMENTED*/
void SalomeApp_Module::onModelClosed()
{
}

/*!Gets popup manager.(create if not exist)*/
QtxPopupMgr* SalomeApp_Module::popupMgr()
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
			        0, d, false, this, SLOT( onShowHide() ) );
    myDisplay     = actionId( disp );
    myErase       = actionId( erase );
    myDisplayOnly = actionId( dispOnly );

    myPopupMgr->insert( disp, -1, 0 ); 
    myPopupMgr->insert( erase, -1, 0 );
    myPopupMgr->insert( dispOnly, -1, 0 );
    myPopupMgr->insert( separator(), -1, 0 );

    QString uniform = "( count( $component ) = 1 ) and ( component != activeModule ) and ( activeModule = '%1' )";
    uniform = uniform.arg( name() );
    myPopupMgr->setRule( disp, QString( "( not isVisible ) and " ) + uniform, true );
    myPopupMgr->setRule( erase, QString( "( isVisible ) and " ) + uniform, true );
    myPopupMgr->setRule( dispOnly, uniform, true );
  }
  return myPopupMgr;
}

/*!Gets preferences.*/
SalomeApp_Preferences* SalomeApp_Module::preferences() const
{
  SalomeApp_Preferences* pref = 0;
  if ( getApp() )
    pref = getApp()->preferences();
  return pref;
}

/*!Create new instance of data model and return it.*/
CAM_DataModel* SalomeApp_Module::createDataModel()
{
  return new SalomeApp_DataModel(this);
}

/*!Update object browser.*/
void SalomeApp_Module::updateObjBrowser( bool updateDataModel, SUIT_DataObject* root )
{
  if( updateDataModel )
    if( CAM_DataModel* aDataModel = dataModel() )
      if( SalomeApp_DataModel* aModel = dynamic_cast<SalomeApp_DataModel*>( aDataModel ) )
        aModel->update( 0, dynamic_cast<SalomeApp_Study*>( getApp()->activeStudy() ) );
  getApp()->objectBrowser()->updateTree( root );
}

/*!Context menu popup.*/
void SalomeApp_Module::contextMenuPopup( const QString& client, QPopupMenu* menu, QString& /*title*/ )
{
  SalomeApp_Selection* sel = createSelection();
  sel->init( client, getApp()->selectionMgr() );
  popupMgr()->updatePopup( menu, sel );
  delete sel;
}

/*!Create and return instance of SalomeApp_Selection.*/
SalomeApp_Selection* SalomeApp_Module::createSelection() const
{
  return new SalomeApp_Selection();
}

/*!Add preference to preferences.*/
int SalomeApp_Module::addPreference( const QString& label )
{
  SalomeApp_Preferences* pref = preferences();
  if ( !pref )
    return -1;

  int catId = pref->addPreference( moduleName(), -1 );
  if ( catId == -1 )
    return -1;
                             
  return pref->addPreference( label, catId );
}

/*!Add preference to preferences.*/
int SalomeApp_Module::addPreference( const QString& label, const int pId, const int type,
				     const QString& section, const QString& param )
{
  SalomeApp_Preferences* pref = preferences();
  if ( !pref )
    return -1;

  return pref->addPreference( moduleName(), label, pId, type, section, param );
}

/*!Gets property of preferences.*/
QVariant SalomeApp_Module::preferenceProperty( const int id, const QString& prop ) const
{
  QVariant var;
  SalomeApp_Preferences* pref = preferences();
  if ( pref )
    var = pref->itemProperty( id, prop );
  return var;
}


/*!Set property of preferences.*/
void SalomeApp_Module::setPreferenceProperty( const int id, const QString& prop, const QVariant& var )
{
  SalomeApp_Preferences* pref = preferences();
  if ( pref )
    pref->setItemProperty( id, prop, var );
}

/*!
 * \brief Update something in accordance with update flags
  * \param theFlags - update flags
*
* Update viewer or/and object browser etc. in accordance with update flags ( see
* SalomeApp_UpdateFlags enumeration ). Derived modules can redefine this method for their
* own purposes
*/
void SalomeApp_Module::update( const int theFlags )
{
  if ( theFlags & UF_Model )
  {
    if( CAM_DataModel* aDataModel = dataModel() )
      if( SalomeApp_DataModel* aModel = dynamic_cast<SalomeApp_DataModel*>( aDataModel ) )
        aModel->update( 0, dynamic_cast<SalomeApp_Study*>( getApp()->activeStudy() ) );
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
        if ( viewWnd->inherits( "SVTK_ViewWindow" ) )
          ( (SVTK_ViewWindow*)viewWnd )->Repaint();
        else if ( viewWnd->inherits( "OCCViewer_ViewWindow" ) )
          ( (OCCViewer_ViewWindow*)viewWnd )->getViewPort()->onUpdate();
        else if ( viewWnd->inherits( "Plot2d_ViewWindow" ) )
          ( (Plot2d_ViewWindow*)viewWnd )->getViewFrame()->Repaint();
        else if ( viewWnd->inherits( "GLViewer_ViewFrame" ) )
          ( (GLViewer_ViewFrame*)viewWnd )->getViewPort()->onUpdate();
      }
  }
}

/*!
 * \brief Updates controls
*
* Updates (i.e. disable/enable) controls states (menus, tool bars etc.). This method is
* called from update( UF_Controls ). You may redefine it in concrete module.
*/
void SalomeApp_Module::updateControls()
{
}

/*!
 * \brief Starts operation with given identifier
  * \param id - identifier of operation to be started
*
* Module stores operations in map. This method starts operation by id.
* If operation isn't in map, then it will be created by createOperation method
* and will be inserted to map
*/
void SalomeApp_Module::startOperation( const int id )
{
  SalomeApp_Operation* op = 0;
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
    op->start();
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
SalomeApp_Operation* SalomeApp_Module::createOperation( const int id ) const
{
  if( id==-1 )
    return 0;

  if( id==myDisplay )
    return new SalomeApp_ShowHideOp( SalomeApp_ShowHideOp::DISPLAY );
  else if( id==myErase )
    return new SalomeApp_ShowHideOp( SalomeApp_ShowHideOp::ERASE );
  else if( id==myDisplayOnly )
    return new SalomeApp_ShowHideOp( SalomeApp_ShowHideOp::DISPLAY_ONLY );
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
void SalomeApp_Module::onOperationStopped( SUIT_Operation* /*theOp*/ )
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
void SalomeApp_Module::onOperationDestroyed()
{
  const QObject* s = sender();
  if( s && s->inherits( "SalomeApp_Operation" ) )
  {
    const SalomeApp_Operation* op = ( SalomeApp_Operation* )s;
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

SalomeApp_Displayer* SalomeApp_Module::displayer()
{
  return 0;
}

void SalomeApp_Module::onShowHide()
{
  if( !sender()->inherits( "QAction" ) || !popupMgr() )
    return;

  QAction* act = ( QAction* )sender();
  int id = actionId( act );
  if( id!=-1 )
    startOperation( id );
}
