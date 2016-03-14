// Copyright (C) 2015-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef PVSERVERSERVICEWRAPPER_H_
#define PVSERVERSERVICEWRAPPER_H_

#include "PVServerService.h"

#include <string>

/*!
 * Class facilitating the access to the PARAVIS engine without having to link
 * to it. Documentation of the method is found in the PARAVIS module (in the idl directory).
 */
class PVSERVERSERVICE_EXPORT PVServer_ServiceWrapper
{
  class Private;
public:
  //! Returns the unique instance of the engine.
  static PVServer_ServiceWrapper* GetInstance();

  bool GetGUIConnected();
  void SetGUIConnected(bool isConnected);
  std::string FindOrStartPVServer(int port);
  bool StopPVServer();

private:
  PVServer_ServiceWrapper();
  virtual ~PVServer_ServiceWrapper();

  static PVServer_ServiceWrapper* instance;
  Private* myData;
};

#endif /* PVSERVERSERVICEWRAPPER_H_ */
