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
//  File   : SPlot2d_Curve.h
//  Author : Sergey RUIN, Open CASCADE S.A.S. (sergey.ruin@opencascade.com)

#ifndef SPlot2d_Curve_h
#define SPlot2d_Curve_h

#include "SPlot2d.h"  
#include "Plot2d_Curve.h"
#include "SALOME_InteractiveObject.hxx"

#include <QSet>

typedef QSet<QString> OwnerSet;

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
  
  virtual void                             addOwner(const QString& owner);
  virtual void                             removeOwner(const QString& owner);
  virtual OwnerSet                         getOwners() const;
  virtual void                             addOwners (OwnerSet& owners);
  
private:
  Handle(SALOME_InteractiveObject) myIO;
  Handle(SALOME_InteractiveObject) myTableIO;
  OwnerSet                         myOwners;
};

#endif // SPlot2d_Curve_h
