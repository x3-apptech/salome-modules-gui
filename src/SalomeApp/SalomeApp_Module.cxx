// File:      SalomeApp_Module.cxx
// Created:   10/25/2004 11:39:56 AM
// Author:    Sergey LITONIN
// Copyright (C) CEA 2004

#include "SalomeApp_Module.h"

#include "SalomeApp_Study.h"
#include "SalomeApp_DataModel.h"
#include "SalomeApp_Application.h"
#include "SalomeApp_Preferences.h"

#include <OB_Browser.h>

#include <CAM_Study.h>

#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>

#include <QtxPopupMgr.h>

/*!Constructor.*/
SalomeApp_Module::SalomeApp_Module( const QString& name )
: CAM_Module( name ),
myPopupMgr( 0 )
{
}

/*!Destructor.*/
SalomeApp_Module::~SalomeApp_Module()
{
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
    
  return res;
}

/*!Deactivate module.*/
bool SalomeApp_Module::deactivateModule( SUIT_Study* )
{
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
    myPopupMgr = new QtxPopupMgr( 0, this );
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
