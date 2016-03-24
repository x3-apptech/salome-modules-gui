// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
// File   : Plot2d_NormalizeAlgorithm.h

#ifndef PLOT2D_NORMALIZEALGORITHM_H
#define PLOT2D_NORMALIZEALGORITHM_H

#include "Plot2d.h"
#include "Plot2d_Algorithm.h"
#include <QList>

class Plot2d_Object;

class PLOT2D_EXPORT Plot2d_NormalizeAlgorithm : public Plot2d_Algorithm {
  Q_OBJECT
public:
  Plot2d_NormalizeAlgorithm(QObject *parent);
  ~Plot2d_NormalizeAlgorithm();

  enum NormalizationMode { NormalizeToMin, NormalizeToMax, NormalizeToMinMax, NormalizeNone };

  void               setNormalizationMode(NormalizationMode);
  NormalizationMode  getNormalizationMode() const;
  double             getKkoef(Plot2d_Object*);
  double             getBkoef(Plot2d_Object*);
  virtual void       execute();
  virtual void       clear();

private:  
  NormalizationMode  myNormalizationMode;
  QMap<Plot2d_Object*, double>      myBkoefs;
  QMap<Plot2d_Object*, double>      myKkoefs;
};

#endif //PLOT2D_NORMALIZEALGORITHM_H
