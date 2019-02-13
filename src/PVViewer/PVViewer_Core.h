// Copyright (C) 2014-2019  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef PVVIEWER_CORE_H
#define PVVIEWER_CORE_H

#include "PVViewer.h"

#include <QString>

class PVServer_ServiceWrapper;
class PVViewer_Behaviors;
class QMainWindow;
class pqPVApplicationCore;

/**
 Pure static class gathering most of the interactions with ParaView's API and ParaView's
 start sequence.
 */
class PVVIEWER_EXPORT PVViewer_Core
{
public:
  static pqPVApplicationCore* GetPVApplication();

  //! Initialize ParaView if not yet done (once per session)
  static bool ParaviewInitApp(QMainWindow*);
  static void ParaviewInitBehaviors(bool = false, QMainWindow* = 0);
  static void ParaviewLoadConfigurations(const QString&, bool = false);
  static void ParaviewCleanup();

private:
  PVViewer_Core();
  virtual ~PVViewer_Core(){};

  static pqPVApplicationCore* MyCoreApp;
  static bool ConfigLoaded;
  static PVViewer_Behaviors* ParaviewBehaviors;
};

#endif // PVVIEWER_CORE_H
