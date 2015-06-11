// Copyright (C) 2010-2015  CEA/DEN, EDF R&D
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

#ifndef PVVIEWERENGINEWRAPPER_H_
#define PVVIEWERENGINEWRAPPER_H_

#include "PVViewer.h"

#include <string>

/*!
 * Class facilitating the access to the PARAVIS engine without having to link
 * to it. Documentation of the method is found in the PARAVIS module (in the idl directory).
 */
class PVVIEWER_EXPORT PVViewer_EngineWrapper
{
  class Private;
public:
  //! Returns the unique instance of the engine.
  static PVViewer_EngineWrapper* GetInstance();

  bool GetGUIConnected();
  void SetGUIConnected(bool isConnected);
  std::string FindOrStartPVServer(int port);
  void PutPythonTraceStringToEngine(const char*);

private:
  PVViewer_EngineWrapper();
  virtual ~PVViewer_EngineWrapper();

  static PVViewer_EngineWrapper* instance;
  Private* myData;
};

#endif /* PVVIEWERENGINEWRAPPER_H_ */
