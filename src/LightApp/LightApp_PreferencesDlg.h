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

// File:      LightApp_PreferencesDlg.h
// Author:    Sergey TELKOV
//
#ifndef LIGHTAPP_PREFERENCESDLG_H
#define LIGHTAPP_PREFERENCESDLG_H

#include "LightApp.h"

#include <QtxDialog.h>

class LightApp_Preferences;

/*!
  \class LightApp_PreferencesDlg
  Dialog for preferences edition
*/
class LIGHTAPP_EXPORT LightApp_PreferencesDlg : public QtxDialog
{
  Q_OBJECT

public:
  LightApp_PreferencesDlg( LightApp_Preferences*, QWidget* = 0 );
  virtual ~LightApp_PreferencesDlg();

  virtual void          setVisible(bool visible);
  virtual void          accept();
  virtual void          reject();

  bool                  isSaved() { return mySaved; }
  void                  setSaved( bool saved ) { mySaved = saved; } 

signals:
  void                  defaultPressed();

private slots:
  void                  onHelp();
  void                  onApply();
  void                  onDefault();
  void                  onImportPref();
  void                  onRestartRequired();

private:
  LightApp_Preferences* myPrefs;
  bool                  mySaved;
};

#endif
