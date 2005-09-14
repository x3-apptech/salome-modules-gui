// File:      SalomeApp_Module.cxx
// Created:   10/25/2004 11:39:56 AM
// Author:    Sergey LITONIN
// Copyright (C) CEA 2004

#include "SalomeApp_Module.h"

#include "SalomeApp_Study.h"
#include "SalomeApp_DataModel.h"
#include "SalomeApp_Application.h"

#include "LightApp_Preferences.h"
#include "SalomeApp_UpdateFlags.h"
#include "SalomeApp_Operation.h"
#include "SalomeApp_SwitchOp.h"

#include "SUIT_Operation.h"
#include "CAM_DataModel.h"

#include "OB_Browser.h"

#include <qstring.h>
#include <qmap.h>

#include <SVTK_ViewWindow.h>
#include <OCCViewer_ViewWindow.h>
#include <OCCViewer_ViewPort3d.h>
#include <GLViewer_ViewFrame.h>
#include <GLViewer_ViewPort.h>
#include <Plot2d_ViewWindow.h>

/*!Constructor.*/
SalomeApp_Module::SalomeApp_Module( const QString& name )
: LightApp_Module( name ),
mySwitchOp( 0 )
{
}

/*!Destructor.*/
SalomeApp_Module::~SalomeApp_Module()
{
  if ( mySwitchOp )
    delete mySwitchOp;
}

/*!Gets application.*/
SalomeApp_Application* SalomeApp_Module::getApp() const
{
  return (SalomeApp_Application*)application();
}

/*!Activate module.*/
bool SalomeApp_Module::activateModule( SUIT_Study* study )
{
  bool res = LightApp_Module::activateModule( study );

  if ( mySwitchOp == 0 )
    mySwitchOp = new SalomeApp_SwitchOp( this );

  return res;
}

/*!Deactivate module.*/
bool SalomeApp_Module::deactivateModule( SUIT_Study* study )
{
  bool res = LightApp_Module::deactivateModule( study );

  delete mySwitchOp;
  mySwitchOp = 0;

  return res;
}

/*!Create new instance of data model and return it.*/
CAM_DataModel* SalomeApp_Module::createDataModel()
{
  return new SalomeApp_DataModel(this);
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
      if( SalomeApp_DataModel* aModel = dynamic_cast<SalomeApp_DataModel*>( aDataModel ) ) {
        SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( getApp()->activeStudy() );
        if (aStudy)
          aModel->update( 0, aStudy );
      }
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
SalomeApp_Operation* SalomeApp_Module::createOperation( const int /*id*/ ) const
{
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
