// Copyright (C) 2017-2019  CEA/DEN, EDF R&D
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
// Author : Anthony GEAY (EDF R&D)

#ifndef PVVIEWER_INITSINGLETON_H
#define PVVIEWER_INITSINGLETON_H

#include "PVViewer.h"

class QMainWindow;

/**
 * This class deals with initialization of SALOME_Session to make it a PV based application.
 * The initialization must be done only once.
 * It allows multi initializator ParaView visu modules other than PARAVIS.
 */
class PVVIEWER_EXPORT PVViewer_InitSingleton
{
public:
  static void Init(QMainWindow*);
private:
  static bool IS_INIT;
};

#endif // PVVIEWER_INITSINGLETON_H
