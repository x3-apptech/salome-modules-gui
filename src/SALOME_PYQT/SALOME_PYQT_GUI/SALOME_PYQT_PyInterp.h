//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File   : SALOME_PYQT_PyInterp.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#ifndef SALOME_PYQT_PYINTERP_H
#define SALOME_PYQT_PYINTERP_H

#include "SALOME_PYQT_GUI.h"

#include <PyInterp_Interp.h> // this include must be first (see PyInterp_base.h)!

class SALOME_PYQT_EXPORT SALOME_PYQT_PyInterp : public PyInterp_Interp
{
public:
  SALOME_PYQT_PyInterp();
  ~SALOME_PYQT_PyInterp();

  int run( const char* );
  
protected:
  virtual bool initState();
  virtual bool initContext();  
};

#endif // SALOME_PYQT_PYINTERP_H
