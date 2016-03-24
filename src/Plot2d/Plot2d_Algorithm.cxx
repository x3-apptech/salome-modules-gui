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
//  File   : Plot2d_Algorithm.cxx

#include "Plot2d_Algorithm.h"

/*!
  Constructor
*/
Plot2d_Algorithm::Plot2d_Algorithm(QObject *parent) :
  QObject(parent),
  myDataChanged(false)
{
}

/*!
  Destructor
*/
Plot2d_Algorithm::~Plot2d_Algorithm()
{
}

/*!
  Sets input data values
*/
void Plot2d_Algorithm::setInput(AlgoPlot2dInputData theInuptData)
{
  clear();
  myInuptData = theInuptData;
  myDataChanged = true;
}

/*!
  Gets result data values
*/
AlgoPlot2dOutputData Plot2d_Algorithm::getOutput()
{
  return myResultData;
}

/*!
  Check the data values changed
*/
bool Plot2d_Algorithm::isDataChanged()
{
  return myDataChanged;
}

void Plot2d_Algorithm::clear() {
  myDataChanged=false;
  myInuptData.clear();
  myResultData.clear();
}
