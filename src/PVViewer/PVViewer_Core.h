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
// Author : Adrien Bruneton (CEA)

#ifndef SRC_PVVIEWER_PVVIEWER_CORE_H_
#define SRC_PVVIEWER_PVVIEWER_CORE_H_

#include "PVViewer.h"

#include <QString>

class PVServer_ServiceWrapper;
class PVViewer_Behaviors;
class LogWindow;
class QMainWindow;
class pqPVApplicationCore;

/**
 Pure static class gathering most of the interactions with ParaView's API and ParaView's
 start sequence.
 */
class PVVIEWER_EXPORT PVViewer_Core
{
public:
  static pqPVApplicationCore * GetPVApplication();


  //! Initialize ParaView if not yet done (once per session)
  static bool   ParaviewInitApp(QMainWindow* aDesktop, LogWindow * w);
  static void   ParaviewInitBehaviors(bool fullSetup=false, QMainWindow* aDesktop=0);
  static void   ParaviewLoadConfigurations(const QString & configPath, bool force=false);
  static void   ParaviewCleanup();

private:
  PVViewer_Core();
  virtual ~PVViewer_Core(){};

  static pqPVApplicationCore* MyCoreApp;
  static bool ConfigLoaded;
  static PVViewer_Behaviors * ParaviewBehaviors;
};

#endif /* SRC_PVVIEWER_PVVIEWER_CORE_H_ */
