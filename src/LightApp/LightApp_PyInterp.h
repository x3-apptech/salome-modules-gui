// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

//  Author : Roman NIKOLAEV, Open CASCADE S.A.S. (roman.nikolaev@opencascade.com)
//  Date   : 22/06/2007
//
#ifndef _LIGHTAPP_PYINTERP_H_
#define _LIGHTAPP_PYINTERP_H_

#include <PyConsole_Interp.h> // this include must be first (see PyInterp_base.h)!

class LightApp_PyInterp : public PyConsole_Interp
{
public:
  LightApp_PyInterp();
  virtual ~LightApp_PyInterp();
  
protected:
  virtual void initPython();
  virtual bool initContext();
};

#endif //_LIGHTAPP_PYINTERP_H_
