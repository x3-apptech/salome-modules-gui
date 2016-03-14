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

// File:      LightApp_Preferences.cxx
// Author:    Sergey TELKOV

#include "LightApp_Preferences.h"

/*!
  Constructor.Initialize by resource manager and parent QWidget.
*/
LightApp_Preferences::LightApp_Preferences( QtxResourceMgr* resMgr, QWidget* parent )
: SUIT_PreferenceMgr( resMgr, parent )
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
  return addItem( label, pId, (SUIT_PreferenceMgr::PrefItemType)type, section, param );
}

/*!
  Adds preference.
*/
int LightApp_Preferences::addPreference( const QString& mod, const QString& label, const int pId,
                                         const int type, const QString& section, const QString& param )
{
  int id = addItem( label, pId, (SUIT_PreferenceMgr::PrefItemType)type, section, param );
  if ( id != -1 && !mod.isEmpty() )
    myPrefMod.insert( id, mod );
  return id;
}

/*!
  Checks: is preferences has module with name \a mod.
*/
bool LightApp_Preferences::hasModule( const QString& mod ) const
{
  bool res = false;
  for ( PrefModuleMap::ConstIterator it = myPrefMod.begin(); it != myPrefMod.end() && !res; ++it )
    res = it.value() == mod;
  return res;
}

void LightApp_Preferences::activateItem( const QString& mod ) const
{
  QtxPreferenceItem* item = findItem( mod, true );

  if ( !item )
    return;

  item->ensureVisible();
  item->activate();
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
void LightApp_Preferences::changedResources( const ResourceMap& map )
{
  bool toRestart = false;
  for ( ResourceMap::ConstIterator it = map.begin(); 
        it != map.end(); ++it )
  {
    QString sec, param;
    it.key()->resource( sec, param );
    QString mod = module( it.key()->id() );
    emit preferenceChanged( mod, sec, param );
    toRestart = toRestart || it.key()->isRestartRequired();
  }
  if ( toRestart ) {
    emit restartRequired();
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
