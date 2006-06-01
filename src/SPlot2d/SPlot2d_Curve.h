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
//  File   : SPlot2d_Curve.h
//  Author : Sergey RUIN
//  Module : SUIT

#ifndef SPlot2d_Curve_h
#define SPlot2d_Curve_h

#include "SPlot2d.h"  

#include "Plot2d_Curve.h"

#include <qstring.h>
#include <qcolor.h>
#include <qmemarray.h>
#ifndef _Handle_SALOME_InteractiveObject_HeaderFile
#include <Handle_SALOME_InteractiveObject.hxx>
#endif
#include "SALOME_InteractiveObject.hxx"

class SPLOT2D_EXPORT SPlot2d_Curve : public Plot2d_Curve
{
public:

  SPlot2d_Curve();
  virtual ~SPlot2d_Curve();
  SPlot2d_Curve( const SPlot2d_Curve& curve );
  SPlot2d_Curve& operator= ( const SPlot2d_Curve& curve );

  virtual QString                          getTableTitle() const;

  virtual bool                             hasIO() const;
  virtual Handle(SALOME_InteractiveObject) getIO() const;
  virtual void                             setIO( const Handle(SALOME_InteractiveObject)& );

  virtual bool                             hasTableIO() const;
  virtual Handle(SALOME_InteractiveObject) getTableIO() const;
  virtual void                             setTableIO( const Handle(SALOME_InteractiveObject)& );
  
private:
  Handle(SALOME_InteractiveObject) myIO;
  Handle(SALOME_InteractiveObject) myTableIO;
};

#endif // SPlot2d_Curve_h


