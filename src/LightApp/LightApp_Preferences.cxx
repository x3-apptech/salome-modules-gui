// File:      LightApp_Preferences.cxx
// Author:    Sergey TELKOV

#include "LightApp_Preferences.h"

#include <QtxListResourceEdit.h>

#include <qlayout.h>

/*!
  Constructor.Initialize by resource manager and parent QWidget.
*/
LightApp_Preferences::LightApp_Preferences( QtxResourceMgr* resMgr, QWidget* parent )
: QtxListResourceEdit( resMgr, parent )
{
}

/*!
  Destructor.
*/
LightApp_Preferences::~LightApp_Preferences()
{
}

/*!
  Adds preference.
*/
int LightApp_Preferences::addPreference( const QString& label, const int pId, const int type,
                                         const QString& section, const QString& param )
{
  return addItem( label, pId, type, section, param );
}

/*!
  Adds preference.
*/
int LightApp_Preferences::addPreference( const QString& mod, const QString& label, const int pId,
                                         const int type, const QString& section, const QString& param )
{
  int id = addItem( label, pId, type, section, param );
  if ( id != -1 && !mod.isEmpty() )
    myPrefMod.insert( id, mod );
  return id;
}

/*
  Checks: is preferences has module with name \a mod.
*/
bool LightApp_Preferences::hasModule( const QString& mod ) const
{
  bool res = false;
  for ( PrefModuleMap::ConstIterator it = myPrefMod.begin(); it != myPrefMod.end() && !res; ++it )
    res = it.data() == mod;
  return res;
}

/*!Do nothing.*/
void LightApp_Preferences::onHelp()
{
}

/*!Store preferences on apply.*/
void LightApp_Preferences::onApply()
{
  store();
}

/*!Emit preference changed.*/
void LightApp_Preferences::changedResources( const QMap<Item*, QString>& map )
{
  for ( QMap<Item*, QString>::ConstIterator it = map.begin(); it != map.end(); ++it )
  {
    QString sec, param;
    it.key()->resource( sec, param );
    QString mod = module( it.key()->id() );
    emit preferenceChanged( mod, sec, param );
  }
}

/*!Gets module name by \a id, if exist.*/
QString LightApp_Preferences::module( const int id ) const
{
  QString mod;
  if ( myPrefMod.contains( id ) )
    mod = myPrefMod[id];
  return mod;
}
