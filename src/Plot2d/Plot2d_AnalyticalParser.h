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
//  File   : Plot2d_AnalyticalParser.h
//  Author : Roman NIKOLAEV, Open CASCADE S.A.S. (roman.nikolaev@opencascade.com)

#ifndef PLOT2D_ANALYTICAL_Parser_H
#define PLOT2D_ANALYTICAL_Parser_H
#include <Python.h>

#include "Plot2d.h"

class PLOT2D_EXPORT Plot2d_AnalyticalParser {
public: 
  ~Plot2d_AnalyticalParser();


  static Plot2d_AnalyticalParser*  parser();
  int                            calculate( const QString&, const double,
					    const double, const int, 
					    double**, double**);
  
private:
  Plot2d_AnalyticalParser();
  void initScript();

private:
  static Plot2d_AnalyticalParser*  myParser;    //!< instance of the Parser
  PyObject*                      myMainMod;   //!< main python module
  PyObject*                      myMainDict;  //!< main python dictionary
  static QString                 myScript;    //!< python script       
};

#endif //PLOT2D_ANALYTICAL_Parser_H
