// Copyright (C) 2014-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
// Author: Adrien Bruneton (CEA)

#ifndef PVViewer_VIEWMANAGER_H
#define PVViewer_VIEWMANAGER_H

#include "PVViewer.h"

#include <SUIT_ViewManager.h>

class SUIT_Desktop;
class SUIT_Study;
class SUIT_ViewWindow;
class LogWindow;
class PVServer_ServiceWrapper;
class QMainWindow;

class PVVIEWER_EXPORT PVViewer_ViewManager : public SUIT_ViewManager
{
  Q_OBJECT

public:
  PVViewer_ViewManager( SUIT_Study*, SUIT_Desktop*, LogWindow *);
  ~PVViewer_ViewManager() {}

  //! Get the CORBA engine wrapper.
  static PVServer_ServiceWrapper * GetService();

  //! Get PVViewer configuration path as stored by SALOME's resource manager:
  static QString GetPVConfigPath();

  //! Connect to the external PVServer, using the PARAVIS engine to launch it if it is not
  //! already up.
  static bool   ConnectToExternalPVServer(QMainWindow* aDesktop);

protected slots:
  void onWindowActivated(SUIT_ViewWindow*);

private:
  SUIT_Desktop * desktop;
};

#endif
