//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 

#ifndef SVTK_ACTOR_H
#define SVTK_ACTOR_H

#include <TColStd_IndexedMapOfInteger.hxx>

class vtkRenderer;
class vtkShrinkFilter;
class vtkDataSetMapper;
class vtkUnstructuredGrid;

#include "SVTK.h"
#include "SALOME_Actor.h"

class SVTK_EXPORT SVTK_Actor : public SALOME_Actor
{
public:
  virtual ~SVTK_Actor();

  static SVTK_Actor* New();

  vtkTypeMacro(SVTK_Actor,SALOME_Actor);

  void  SetShrinkFactor(float value);
  virtual void SetShrink(); 
  virtual void UnShrink(); 

  void MapCells(SALOME_Actor* theMapActor, 
		const TColStd_IndexedMapOfInteger& theMapIndex);

  void MapPoints(SALOME_Actor* theMapActor, 
		 const TColStd_IndexedMapOfInteger& theMapIndex);

  void MapEdge(SALOME_Actor* theMapActor, 
	       const TColStd_IndexedMapOfInteger& theMapIndex);

 protected:
  vtkUnstructuredGrid* myUnstructuredGrid;
  vtkDataSetMapper* myMapper;

  vtkRenderer* myRenderer;

  vtkShrinkFilter* myShrinkFilter;
  bool myIsShrinkable;
  bool myIsShrunk;

  SVTK_Actor();
};

#endif
