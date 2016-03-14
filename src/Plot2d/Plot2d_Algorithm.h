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
// File   : Plot2d_Algorithm.h

#ifndef PLOT2D_ALGORITHM_H
#define PLOT2D_ALGORITHM_H

#include "Plot2d.h"

#include <QPair>
#include <QList>
#include <QMap>

class Plot2d_Object;

typedef QList< QPair<double,double> >  AlgoPlot2dItem;
typedef QList<Plot2d_Object*> AlgoPlot2dInputData;
typedef QMap<Plot2d_Object*,AlgoPlot2dItem> AlgoPlot2dOutputData;

class PLOT2D_EXPORT Plot2d_Algorithm : public QObject {
  Q_OBJECT
public:
  Plot2d_Algorithm(QObject *parent = 0);
  ~Plot2d_Algorithm();

  void setInput(AlgoPlot2dInputData);
  AlgoPlot2dOutputData getOutput();
  virtual void execute() = 0;
  virtual void clear();

  bool isDataChanged();

protected:
  AlgoPlot2dInputData  myInuptData;
  AlgoPlot2dOutputData myResultData;
  bool                 myDataChanged;
};

#endif //PLOT2D_ALGORITHM_H
