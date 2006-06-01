// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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

#ifndef LIGHTAPP_PREFERENCES_H
#define LIGHTAPP_PREFERENCES_H

#include <LightApp.h>

#include <QtxDialog.h>
#include <QtxListResourceEdit.h>

#include <qmap.h>

class QtxResourceMgr;

/*!
  \class LightApp_Preferences
  Custom preference container. Assign each preference category with module.
  Emits signal on preference changing
*/
class LIGHTAPP_EXPORT LightApp_Preferences : public QtxListResourceEdit
{
  Q_OBJECT

public:
  LightApp_Preferences( QtxResourceMgr*, QWidget* = 0 );
  virtual ~LightApp_Preferences();

  int                  addPreference( const QString& label, const int pId = -1, const int = -1,
                                      const QString& section = QString::null, const QString& param = QString::null );
  int                  addPreference( const QString& modName, const QString& label, const int pId = -1, const int = -1,
                                      const QString& section = QString::null, const QString& param = QString::null );

  bool                 hasModule( const QString& ) const;

signals:
  void                 preferenceChanged( QString&, QString&, QString& );

private slots:
  void                 onHelp();
  void                 onApply();
  virtual void         changedResources( const QMap<Item*, QString>& );

private:
  QString              module( const int ) const;

private:
  typedef QMap<int, QString> PrefModuleMap;

private:
  PrefModuleMap        myPrefMod;
};

#endif
