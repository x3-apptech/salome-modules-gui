// File:      SalomeApp_Preferences.cxx
// Author:    Sergey TELKOV

#include "SalomeApp_Preferences.h"

#include <QtxListResourceEdit.h>

#include <qlayout.h>

/*!
  Constructor.Initialize by resource manager and parent QWidget.
*/
SalomeApp_Preferences::SalomeApp_Preferences( QtxResourceMgr* resMgr, QWidget* parent )
: QtxListResourceEdit( resMgr, parent )
{
}

/*!
  Destructor.
*/
SalomeApp_Preferences::~SalomeApp_Preferences()
{
}

/*!
  Adds preference.
*/
int SalomeApp_Preferences::addPreference( const QString& label, const int pId, const int type,
					  const QString& section, const QString& param )
{
  return addItem( label, pId, type, section, param );
}

/*!
  Adds preference.
*/
int SalomeApp_Preferences::addPreference( const QString& mod, const QString& label, const int pId,
					  const int type, const QString& section, const QString& param )
{
  int id = addItem( label, pId, type, section, param );
  if ( id != -1 && !mod.isEmpty() )
    myPrefMod.insert( id, mod );
  return id;
}

/*!
  Checks: is preferences has module with name \a mod.
*/
bool SalomeApp_Preferences::hasModule( const QString& mod ) const
{
  bool res = false;
  for ( PrefModuleMap::ConstIterator it = myPrefMod.begin(); it != myPrefMod.end() && !res; ++it )
    res = it.data() == mod;
  return res;
}

/*!Do nothing.*/
void SalomeApp_Preferences::onHelp()
{
}

/*!Store preferences on apply.*/
void SalomeApp_Preferences::onApply()
{
  store();
}

/*!Emit preference changed.*/
void SalomeApp_Preferences::changedResources( const QMap<Item*, QString>& map )
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
QString SalomeApp_Preferences::module( const int id ) const
{
  QString mod;
  if ( myPrefMod.contains( id ) )
    mod = myPrefMod[id];
  return mod;
}
