// File:      SalomeApp_Module.cxx
// Created:   10/25/2004 11:39:56 AM
// Author:    Sergey LITONIN
// Copyright (C) CEA 2004

#include "SalomeApp_Module.h"

#include "SalomeApp_DataModel.h"
#include "SalomeApp_Application.h"
#include "SalomeApp_Study.h"

#include <OB_Browser.h>

#include <CAM_Study.h>

#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>

#include <QtxPopupMgr.h>

SalomeApp_Module::SalomeApp_Module( const QString& name )
: CAM_Module( name ),
myPopupMgr( 0 )
{
}

SalomeApp_Module::~SalomeApp_Module()
{
}

void SalomeApp_Module::initialize( CAM_Application* app )
{
  CAM_Module::initialize( app );

  SUIT_ResourceMgr* resMgr = app ? app->resourceMgr() : 0;
  if ( resMgr )
    resMgr->raiseTranslators( name() );
}

void SalomeApp_Module::activateModule( SUIT_Study* study )
{
  CAM_Module::activateModule( study );

  if ( application() && application()->resourceMgr() )
    application()->resourceMgr()->raiseTranslators( name() );
}

void SalomeApp_Module::deactivateModule( SUIT_Study* )
{
}

void SalomeApp_Module::selectionChanged()
{
}

void SalomeApp_Module::MenuItem()
{
}

void SalomeApp_Module::windows( QMap<int, int>& ) const
{
}

void SalomeApp_Module::viewManagers( QStringList& ) const
{
}

SalomeApp_Application* SalomeApp_Module::getApp() const
{
  return (SalomeApp_Application*)application();
}

void SalomeApp_Module::onModelOpened()
{
}

void SalomeApp_Module::onModelSaved()
{
}

void SalomeApp_Module::onModelClosed()
{
}

QtxPopupMgr* SalomeApp_Module::popupMgr()
{
  if ( !myPopupMgr )
    myPopupMgr = new QtxPopupMgr( 0, this );
  return myPopupMgr;
}

CAM_DataModel* SalomeApp_Module::createDataModel()
{
  return new SalomeApp_DataModel(this);
}

void SalomeApp_Module::updateObjBrowser( bool updateDataModel, SUIT_DataObject* root )
{
  if( updateDataModel )
    if( CAM_DataModel* aDataModel = dataModel() )
      if( SalomeApp_DataModel* aModel = dynamic_cast<SalomeApp_DataModel*>( aDataModel ) )
        aModel->update( 0, dynamic_cast<SalomeApp_Study*>( getApp()->activeStudy() ) );
  getApp()->objectBrowser()->updateTree( root );
}

void SalomeApp_Module::contextMenuPopup( const QString& client, QPopupMenu* menu, QString& /*title*/ )
{
  SalomeApp_Selection* sel = createSelection();
  sel->init( client, getApp()->selectionMgr() );
  popupMgr()->updatePopup( menu, sel );
  delete sel;
}

SalomeApp_Selection* SalomeApp_Module::createSelection() const
{
  return new SalomeApp_Selection();
}
