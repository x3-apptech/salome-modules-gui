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
// File   : SalomePyQt.sip
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

%If (ENABLE_PLOT2D)

%ExportedHeaderCode
#include <Plot2d_Curve.h>
%End

enum Axis {
  yLeft,
  yRight,
  xBottom,
  xTop,		
};	

enum ObjectType
{
  MainTitle,
  XTitle,
  YTitle,
  Y2Title,
  XAxis,
  YAxis,
  Y2Axis
};

class Plot2d_Curve
{
%TypeHeaderCode
#include <Plot2d_Curve.h>
%End

%ConvertToSubClassCode
    if ( dynamic_cast<Plot2d_Curve*>( sipCpp ) )
      sipClass = sipClass_Plot2d_Curve;
    else
      sipClass = NULL;
%End

public:
  Plot2d_Curve();
  virtual ~Plot2d_Curve();
  void setName( const QString& );
  void addPoint( double, double );
  void addPoints(SIP_PYLIST X, SIP_PYLIST Y);
%MethodCode
  int nx = PyList_Size(a0);
  int ny = PyList_Size(a1);
  
  if( nx != ny ) { 
    PyErr_Format(PyExc_TypeError,"The dimension of x and y should be the same. It is %d and %d currently.", nx, ny);
  }
  
  int i;
  PyObject *pX, *pY;

  for (i=0; i<nx; i++) {
    pX = PyList_GET_ITEM(a0,i);
    pY = PyList_GET_ITEM(a1,i);
    double aX = PyFloat_AsDouble(pX);
    double aY = PyFloat_AsDouble(pY);
    sipCpp->addPoint(aX, aY);
  }
%End
  void insertPoint( int, double, double );
  void deletePoint( int );
  void clearAllPoints();
  void setXAxis( Axis );
%MethodCode
  int ax = int(a0);
  sipCpp->setXAxis(QwtPlot::Axis(ax));
%End
  Axis getXAxis() const;
%MethodCode
  sipRes = Axis(sipCpp->getXAxis());
%End
  void setYAxis( Axis );
%MethodCode
  int ay = int(a0);
  sipCpp->setYAxis(QwtPlot::Axis(ay));
%End
  Axis getYAxis() const;
%MethodCode
  sipRes = Axis(sipCpp->getYAxis());
%End
};

%End
