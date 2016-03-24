// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

//  File   : SALOME_PYQT_Module.h
//  Author : Roman NIKOLAEV, Open CASCADE S.A.S. (roman.nikolaev@opencascade.com)
//  Date   : 13/04/2009
//
#ifndef SALOME_PYQT_PYINTERP_H
#define SALOME_PYQT_PYINTERP_H

#include "SALOME_PYQT_GUILight.h"

#include "PyInterp_Interp.h" // this include must be first !!!

class SALOME_PYQT_LIGHT_EXPORT SALOME_PYQT_PyInterp : public PyInterp_Interp
{
 public:
  SALOME_PYQT_PyInterp();
  ~SALOME_PYQT_PyInterp();
  
  virtual int run(const char *command);
  
 protected:
  virtual void initPython();
  virtual bool initContext();
};

#endif // SALOME_PYQT_PYINTERP_H
