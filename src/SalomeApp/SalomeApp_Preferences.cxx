// File:      SalomeApp_Preferences.cxx
// Author:    Sergey TELKOV

#include "SalomeApp_Preferences.h"

#include <QtxListResourceEdit.h>

#include <qlayout.h>

SalomeApp_Preferences::SalomeApp_Preferences( QtxResourceMgr* resMgr, QWidget* parent )
: QtxListResourceEdit( resMgr, parent )
{
}

SalomeApp_Preferences::~SalomeApp_Preferences()
{
}

int SalomeApp_Preferences::addPreference( const QString& label, const int pId, const int type,
					  const QString& section, const QString& param )
{
  return addItem( label, pId, type, section, param );
}

int SalomeApp_Preferences::addPreference( const QString& mod, const QString& label, const int pId,
					  const int type, const QString& section, const QString& param )
{
  int id = addItem( label, pId, type, section, param );
  if ( id != -1 && !mod.isEmpty() )
    myPrefMod.insert( id, mod );
  return id;
}

bool SalomeApp_Preferences::hasModule( const QString& mod ) const
{
  bool res = false;
  for ( PrefModuleMap::ConstIterator it = myPrefMod.begin(); it != myPrefMod.end() && !res; ++it )
    res = it.data() == mod;
  return res;
}

void SalomeApp_Preferences::onHelp()
{
}

void SalomeApp_Preferences::onApply()
{
  store();
}

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

QString SalomeApp_Preferences::module( const int id ) const
{
  QString mod;
  if ( myPrefMod.contains( id ) )
    mod = myPrefMod[id];
  return mod;
}
