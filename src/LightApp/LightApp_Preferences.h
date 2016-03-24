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

// File:      LightApp_Preferences.h
// Author:    Sergey TELKOV
//
#ifndef LIGHTAPP_PREFERENCES_H
#define LIGHTAPP_PREFERENCES_H

#include "LightApp.h"
#include <SUIT_PreferenceMgr.h>

#include <QMap>

class QtxResourceMgr;

/*!
  \class LightApp_Preferences
  Custom preference container. Assign each preference category with module.
  Emits signal on preference changing
*/
class LIGHTAPP_EXPORT LightApp_Preferences : public SUIT_PreferenceMgr
{
  Q_OBJECT

public:
  LightApp_Preferences( QtxResourceMgr*, QWidget* = 0 );
  virtual ~LightApp_Preferences();

  int                  addPreference( const QString& label,const int pId = -1, const int = Auto,
                                      const QString& section = QString(), const QString& param = QString() );
  int                  addPreference( const QString& modName, const QString& label, const int pId = -1, const int = Auto,
                                      const QString& section = QString(), const QString& param = QString() );

  bool                 hasModule( const QString& ) const;

  void                 activateItem( const QString& ) const;

protected:
  void                 changedResources( const ResourceMap& );

signals:
  void                 preferenceChanged( QString&, QString&, QString& );
  void                 resetToDefaults();
  void                 restartRequired();

private slots:
  void                 onHelp();
  void                 onApply();

private:
  QString              module( const int ) const;

private:
  typedef QMap<int, QString> PrefModuleMap;

private:
  PrefModuleMap        myPrefMod;
};

#endif
