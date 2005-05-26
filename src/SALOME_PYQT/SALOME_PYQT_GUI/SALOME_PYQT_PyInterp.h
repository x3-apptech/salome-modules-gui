//=============================================================================
// File      : SALOME_PYQT_PyInterp.h
// Created   : 25/04/05
// Author    : Christian CAREMOLI, Paul RASCLE, EDF
// Project   : SALOME
// Copyright : 2003-2005 CEA/DEN, EDF R&D
// $Header   : $
//=============================================================================

#ifndef SALOME_PYQT_PYINTERP_H
#define SALOME_PYQT_PYINTERP_H

#include "SALOME_PYQT_GUI.h"

#include "PyInterp_base.h" // this include must be first (see PyInterp_base.h)!

class SALOME_PYQT_EXPORT SALOME_PYQT_PyInterp : public PyInterp_base
{
 public:
  SALOME_PYQT_PyInterp();
  ~SALOME_PYQT_PyInterp();

  int run(const char *command);

 protected:
  virtual bool initState();
  virtual bool initContext();  
};

#endif // SALOME_PYQT_PYINTERP_H
