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
#ifndef SALOMEAPP_VISUALSTATE_H
#define SALOMEAPP_VISUALSTATE_H

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

#include "SalomeApp.h"

class SalomeApp_Application;

class SALOMEAPP_EXPORT SalomeApp_VisualState
{
public:
  SalomeApp_VisualState( SalomeApp_Application* );
  virtual ~SalomeApp_VisualState();

  virtual int            storeState();
  virtual void           restoreState( int savePoint );

protected:
  SalomeApp_Application* myApp;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif 
