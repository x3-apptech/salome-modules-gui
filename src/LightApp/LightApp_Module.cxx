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

// File:      LightApp_Module.cxx
// Created:   6/20/2005 16:30:56 AM
// Author:    OCC team

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
#include "LightApp_SelectionMgr.h"

#include <SUIT_Study.h>
#include <SUIT_DataObject.h>
#include <SUIT_DataBrowser.h>
#include <SUIT_Operation.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_ShortcutMgr.h>
#include <SUIT_Desktop.h>
#include <SUIT_TreeModel.h>
#include <SUIT_Session.h>

#ifndef DISABLE_SALOMEOBJECT
#include <SALOME_ListIO.hxx>
#endif

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
#ifndef DISABLE_PVVIEWER
  #include <PVViewer_ViewManager.h>
  #include <PVViewer_ViewWindow.h>
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

#include <QtxPopupMgr.h>

#include <QVariant>
#include <QString>
#include <QStringList>
#include <QAction>

#include <iostream>

/*!Constructor.*/
LightApp_Module::LightApp_Module( const QString& name )
: CAM_Module( name ),
  myPopupMgr( 0 ),
  mySwitchOp( 0 ),
  myDisplay( -1 ),
  myErase( -1 ),
  myDisplayOnly( -1 ),
  myEraseAll( -1 ),
  myIsFirstActivate( true )
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
void LightApp_Module::contextMenuPopup( const QString& client, QMenu* menu, QString& /*title*/ )
{
  LightApp_Selection* sel = createSelection();
  sel->init( client, getApp()->selectionMgr() );

  popupMgr()->setSelection( sel );
  popupMgr()->setMenu( menu );
  popupMgr()->updateMenu();
}

/*!Update object browser.
 * For updating model or whole object browser use update() method can be used.
*/
void LightApp_Module::updateObjBrowser( bool theIsUpdateDataModel, 
                                        SUIT_DataObject* theDataObject )
{
  if (!getApp()->objectBrowser())
    return;
  bool upd = getApp()->objectBrowser()->autoUpdate();
  getApp()->objectBrowser()->setAutoUpdate( false );

  if( theIsUpdateDataModel ){
    if( CAM_DataModel* aDataModel = dataModel() ){
      if ( LightApp_DataModel* aModel = dynamic_cast<LightApp_DataModel*>( aDataModel ) ) {
        //SUIT_DataObject* aParent = NULL;
        //if(theDataObject && theDataObject != aDataModel->root())
        //  aParent = theDataObject->parent();

        LightApp_DataObject* anObject = dynamic_cast<LightApp_DataObject*>(theDataObject);
        LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>(getApp()->activeStudy());
        aModel->update( anObject, aStudy );
      }
    }
  }

  getApp()->objectBrowser()->setAutoUpdate( upd );
  getApp()->objectBrowser()->updateTree( 0, false );
}

/*!NOT IMPLEMENTED*/
void LightApp_Module::selectionChanged()
{
}

/*! \brief If return false, selection will be cleared at module activation
 */
bool LightApp_Module::isSelectionCompatible()
{
  // return true if selected objects belong to this module
  bool isCompatible = true;
#ifndef DISABLE_SALOMEOBJECT
  SALOME_ListIO selected;
  if ( LightApp_SelectionMgr *Sel = getApp()->selectionMgr() )
    Sel->selectedObjects( selected );

  LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>( getApp()->activeStudy() );
  LightApp_DataObject* aRoot = dynamic_cast<LightApp_DataObject*>( dataModel()->root() );
  if ( aStudy && aRoot ) {
    // my data type
    QString moduleDataType = aRoot->componentDataType();
    // check data type of selection
    SALOME_ListIteratorOfListIO It( selected );
    for ( ; isCompatible && It.More(); It.Next()) {
      Handle(SALOME_InteractiveObject) io = It.Value();
      isCompatible = ( aStudy->componentDataType( io->getEntry() ) == moduleDataType );
    }
  }
#endif
  return isCompatible;
}

/*!Activate module.*/
bool LightApp_Module::activateModule( SUIT_Study* study )
{
  bool res = CAM_Module::activateModule( study );

  if ( !isSelectionCompatible() )// PAL19290, PAL18352
    getApp()->selectionMgr()->clearSelected();

  if ( res && application() && application()->resourceMgr() )
    application()->resourceMgr()->raiseTranslators( name() );

  connect( application(), SIGNAL( viewManagerAdded( SUIT_ViewManager* ) ),
           this, SLOT( onViewManagerAdded( SUIT_ViewManager* ) ) );
  connect( application(), SIGNAL( viewManagerRemoved( SUIT_ViewManager* ) ),
           this, SLOT( onViewManagerRemoved( SUIT_ViewManager* ) ) );

  if ( mySwitchOp == 0 )
    mySwitchOp = new LightApp_SwitchOp( this );

  // Enable Display and Erase actions
  if ( action(myDisplay) )
    action(myDisplay)->setEnabled(true);
  if ( action(myErase) )
    action(myErase)->setEnabled(true);

  application()->shortcutMgr()->setSectionEnabled( moduleName() );

  /*  BUG 0020498 : The Entry column is always shown at module activation
      The registration of column is moved into LightApp_Application

  QString EntryCol = QObject::tr( "ENTRY_COLUMN" );
  LightApp_DataModel* m = dynamic_cast<LightApp_DataModel*>( dataModel() );
  if( m )
  {
    SUIT_AbstractModel* treeModel = dynamic_cast<SUIT_AbstractModel*>( getApp()->objectBrowser()->model() );
    m->registerColumn( getApp()->objectBrowser(), EntryCol, LightApp_DataObject::EntryId );
    treeModel->setAppropriate( EntryCol, Qtx::Toggled );
  }*/

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
    anIt.value()->abort();
  }

  // Disable Display and Erase action
  if ( action(myDisplay) )
    action(myDisplay)->setEnabled(false);
  if ( action(myErase) )
    action(myErase)->setEnabled(false);

  application()->shortcutMgr()->setSectionEnabled( moduleName(), false );
  
  /*  BUG 0020498 : The Entry column is always shown at module activation
  QString EntryCol = QObject::tr( "ENTRY_COLUMN" );
  LightApp_DataModel* m = dynamic_cast<LightApp_DataModel*>( dataModel() );
  if( m )
  {
    SUIT_AbstractModel* treeModel = dynamic_cast<SUIT_AbstractModel*>( getApp()->objectBrowser()->model() );

    treeModel->setAppropriate( EntryCol, Qtx::Shown );
    m->unregisterColumn( getApp()->objectBrowser(), EntryCol );
  }
  */
  return CAM_Module::deactivateModule( study );
}

/*! Redefined to reset internal flags valid for study instance */
void LightApp_Module::studyClosed( SUIT_Study* theStudy )
{
  CAM_Module::studyClosed( theStudy );
  
  myIsFirstActivate = true;
  
  LightApp_Application* app = dynamic_cast<LightApp_Application*>(application());
  if ( app ) {
    SUIT_DataBrowser* ob = app->objectBrowser();
    if ( ob && ob->model() )
      disconnect( ob->model(), SIGNAL( clicked( SUIT_DataObject*, int ) ),
		  this, SLOT( onObjectClicked( SUIT_DataObject*, int ) ) );
  }
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

/*!NOT IMPLEMENTED*/
void LightApp_Module::message( const QString& )
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
#ifndef DISABLE_PVVIEWER
//        if ( viewWnd->inherits( "PVViewer_ViewWindow" ) )
//          ( (PVViewer_ViewWindow*)viewWnd )->getViewPort()->onUpdate();
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
  return new LightApp_DataModel( this );
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
      *disp = createAction( -1, tr( "TOP_SHOW" ), p, tr( "MEN_SHOW" ), tr( "STB_SHOW" ),
                            0, d, false, this, SLOT( onShowHide() ), QString("General:Show object(s)") ),
      *erase = createAction( -1, tr( "TOP_HIDE" ), p, tr( "MEN_HIDE" ), tr( "STB_HIDE" ),
                             0, d, false, this, SLOT( onShowHide() ) , QString("General:Hide object(s)") ),
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

    QString oneAndNotActive = "( count( $component ) = 1 ) and ( not( activeModule in $component ) )";
    QString uniform = "true in $canBeDisplayed and %1 and ( activeModule = '%2' )";
    uniform = uniform.arg( oneAndNotActive ).arg( name() );
    myPopupMgr->setRule( disp, /*QString( "( not isVisible ) and " ) + */ uniform, QtxPopupMgr::VisibleRule );
    myPopupMgr->setRule( erase, /*QString( "( isVisible ) and " ) + */ uniform, QtxPopupMgr::VisibleRule );
    myPopupMgr->setRule( dispOnly, uniform, QtxPopupMgr::VisibleRule );

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
      myPopupMgr->setRule( eraseAll, QString( "client in %1" ).arg( strViewers ), QtxPopupMgr::VisibleRule );
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
    var = pref->itemProperty( prop, id );
  return var;
}

/*!Set property of preferences.*/
void LightApp_Module::setPreferenceProperty( const int id, const QString& prop, const QVariant& var )
{
  LightApp_Preferences* pref = preferences();
  if ( pref )
    pref->setItemProperty( prop, var, id );
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
  if( myOperations.contains( id ) && reusableOperation( id ) )
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
      if( anIt.value()==op )
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

/*!
  \brief Returns instance of operation by its id; if there is no operation
  corresponding to this id, null pointer is returned
  \param id - operation id 
  \return operation instance
*/
LightApp_Operation* LightApp_Module::operation( const int id ) const
{
  return myOperations.contains( id ) ? myOperations[id] : 0;
}

/*!
  virtual method called to manage the same operations
*/
bool LightApp_Module::reusableOperation( const int id )
{
 return true;
} 

/*!
  virtual method
  \return true if module can copy the current selection
*/
bool LightApp_Module::canCopy() const
{
  return false;
}

/*!
  virtual method
  \return true if module can paste previously copied data
*/
bool LightApp_Module::canPaste() const
{
  return false;
}

/*!
  virtual method
  \brief Copies the current selection into clipboard
*/
void LightApp_Module::copy()
{
}

/*!
  virtual method
  \brief Pastes the current data in the clipboard
*/
void LightApp_Module::paste()
{
}

/*!
  virtual method
  \return true if module allows dragging the given object
*/
bool LightApp_Module::isDraggable( const SUIT_DataObject* /*what*/ ) const
{
  return false;
}

/*!
  virtual method
  \return true if module allows dropping one or more objects (currently selected) on the object \c where
*/
bool LightApp_Module::isDropAccepted( const SUIT_DataObject* /*where*/ ) const
{
  return false;
}

/*!
  virtual method
  Complete drag-n-drop operation by processing objects \a what being dragged, dropped to the line \a row
  within the object \a where. The drop action being performed is specified by \a action.
*/
void LightApp_Module::dropObjects( const DataObjectList& /*what*/, SUIT_DataObject* /*where*/,
                                   const int /*row*/, Qt::DropAction /*action*/ )
{
}

/*!
  \brief Return \c true if object can be renamed
*/
bool LightApp_Module::renameAllowed( const QString& /*entry*/ ) const
{
  return false;
}

/*!
  Rename object by entry.
  \param entry entry of the object
  \param name new name of the object
  \brief Return \c true if rename operation finished successfully, \c false otherwise.
*/
bool LightApp_Module::renameObject( const QString& /*entry*/, const QString& /*name*/ )
{
  return false;
}

/*!
  Update visibility state for data objects
*/
void LightApp_Module::updateModuleVisibilityState()
{
  // update visibility state of objects
  LightApp_Application* app = dynamic_cast<LightApp_Application*>(SUIT_Session::session()->activeApplication());
  if ( !app ) return;
  
  SUIT_DataBrowser* ob = app->objectBrowser();
  if ( !ob || !ob->model() ) return;

  if ( !myIsFirstActivate )
    return;

  myIsFirstActivate = false;

  // connect to click on item
  connect( ob->model(), SIGNAL( clicked( SUIT_DataObject*, int ) ),
           this, SLOT( onObjectClicked( SUIT_DataObject*, int ) ), Qt::UniqueConnection );
  // connect to click on item
  connect( ob, SIGNAL( destroyed( QObject* ) ),
           this, SLOT( onOBDestroyed() ), Qt::UniqueConnection );

  SUIT_DataObject* rootObj = ob->root();
  if ( !rootObj ) return;
  
  DataObjectList listObj = rootObj->children( true );
  
  SUIT_ViewModel* vmod = 0;
  if ( SUIT_ViewManager* vman = app->activeViewManager() )
    vmod = vman->getViewModel();
  app->updateVisibilityState( listObj, vmod );
}

/*!
 * \brief Virtual public slot
 *
 * This method is called after the object inserted into data view to update their visibility state
 * This is default implementation
 */
void LightApp_Module::onObjectClicked( SUIT_DataObject* theObject, int theColumn )
{
  if ( !isActiveModule() ) return;

  // change visibility of object
  if ( !theObject || theColumn != SUIT_DataObject::VisibilityId ) return;

  LightApp_Study* study = dynamic_cast<LightApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
  if ( !study ) return;

  LightApp_DataObject* lo = dynamic_cast<LightApp_DataObject*>( theObject );
  if ( !lo ) return;
  
  // detect action index (from LightApp level)
  int id = -1;
  
  if ( study->visibilityState( lo->entry() ) == Qtx::ShownState )
    id = myErase;
  else if ( study->visibilityState( lo->entry() ) == Qtx::HiddenState )
    id = myDisplay;
  
  if ( id != -1 )
    startOperation( id );
}

void LightApp_Module::onOBDestroyed()
{
  myIsFirstActivate = true;
}
