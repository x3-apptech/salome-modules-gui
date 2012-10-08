// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

//  SVTK OBJECT : interactive object for SVTK visualization
//  File   : SVTK_DeviceActor.h
//  Author : 

#ifndef SVTK_DEVICE_ACTOR_H
#define SVTK_DEVICE_ACTOR_H

#include "SVTK.h"
#include "VTKViewer.h"
#include "VTKViewer_Actor.h"
#include "VTKViewer_MarkerDef.h"

#include <vector>

#include <vtkLODActor.h>
#include <vtkProperty.h>

class VTKViewer_Transform;
class VTKViewer_TransformFilter;
class VTKViewer_GeometryFilter;

class vtkCell;
class vtkDataSet;
class vtkShrinkFilter;
class vtkFeatureEdges;
class VTKViewer_DataSetMapper;
class vtkPassThroughFilter;

#ifdef WIN32
#pragma warning ( disable:4251 )
#endif

class SVTK_EXPORT SVTK_DeviceActor: public vtkLODActor
{
 public:
  vtkTypeMacro(SVTK_DeviceActor,vtkLODActor);

  static
  SVTK_DeviceActor* 
  New();

  //! Apply a view transformation
  virtual
  void
  SetTransform(VTKViewer_Transform* theTransform); 

  //! To insert some additional filters and then sets the given #vtkMapper
  virtual
  void
  SetMapper(vtkMapper* theMapper); 

  //! Allows to get initial #vtkDataSet
  virtual
  vtkDataSet* 
  GetInput(); 

  //! Allows to set initial #vtkDataSet
  virtual
  void
  SetInput(vtkDataSet* theDataSet); 

  /** @name For selection mapping purpose */
  //@{
  virtual
  int
  GetNodeObjId(int theVtkID);

  virtual
  vtkFloatingPointType* 
  GetNodeCoord(int theObjID);

  virtual
  int
  GetElemObjId(int theVtkID);

  virtual
  vtkCell* 
  GetElemCell(int theObjID);

  //! To provide VTK to Object and backward mapping
  virtual 
  void
  SetStoreMapping(bool theStoreMapping);
  //@}

  virtual 
  unsigned long int 
  GetMTime();

  /** @name For shrink mamnagement purpose */
  //@{
  vtkFloatingPointType
  GetShrinkFactor();

  virtual 
  void  
  SetShrinkFactor(vtkFloatingPointType value);

  virtual
  void
  SetShrinkable(bool theIsShrinkable);

  bool
  IsShrunkable();

  bool
  IsShrunk();

  virtual
  void
  SetShrink(); 

  virtual
  void
  UnShrink(); 
  //@}

  /** @name For feature edges management purpose */
  //@{
  virtual
  bool
  IsFeatureEdgesAllowed();

  virtual
  void
  SetFeatureEdgesAllowed(bool theIsFeatureEdgesAllowed);

  virtual
  bool
  IsFeatureEdgesEnabled();

  virtual
  void
  SetFeatureEdgesEnabled(bool theIsFeatureEdgesEnabled);

  virtual
  vtkFloatingPointType
  GetFeatureEdgesAngle();

  virtual
  void
  SetFeatureEdgesAngle(vtkFloatingPointType theAngle); 

  virtual
  void
  GetFeatureEdgesFlags(bool& theIsFeatureEdges,
                       bool& theIsBoundaryEdges,
                       bool& theIsManifoldEdges,
                       bool& theIsNonManifoldEdges);
  virtual
  void
  SetFeatureEdgesFlags(bool theIsFeatureEdges,
                       bool theIsBoundaryEdges,
                       bool theIsManifoldEdges,
                       bool theIsNonManifoldEdges);

  virtual
  bool
  GetFeatureEdgesColoring();

  virtual
  void
  SetFeatureEdgesColoring(bool theIsColoring);
  //@}

  /** @name For representation mamnagement purpose */
  virtual
  void 
    SetRepresentation(VTKViewer::Representation::Type theMode);

  VTKViewer::Representation::Type 
  GetRepresentation();

  virtual
  vtkFloatingPointType
  GetDefaultPointSize();

  virtual
  vtkFloatingPointType
  GetDefaultLineWidth();

  bool
  IsShaded();

  void
  SetShaded(bool theShaded);
  //@}

  /** @name For marker management purpose */
  void
  SetMarkerEnabled( bool );

  void
  SetMarkerStd( VTK::MarkerType, VTK::MarkerScale );

  void
  SetMarkerTexture( int, VTK::MarkerTexture );

  VTK::MarkerType
  GetMarkerType();

  VTK::MarkerScale
  GetMarkerScale();

  int
  GetMarkerTexture();
  //@}

  virtual
  void
  Render(vtkRenderer *, vtkMapper *);

  VTKViewer_DataSetMapper* GetDataSetMapper();

  //----------------------------------------------------------------------------
  //! Setting for displaying quadratic elements
  virtual void SetQuadraticArcMode(bool theFlag);
  virtual bool GetQuadraticArcMode();

  virtual void SetQuadraticArcAngle(vtkFloatingPointType theMaxAngle);
  virtual vtkFloatingPointType GetQuadraticArcAngle();

  virtual
  void
  SetCoincident3DAllowed(bool theIsFeatureEdgesAllowed);

  virtual
  bool 
  IsCoincident3DAllowed() const;
  
  void
  SetResolveCoincidentTopology(bool theIsResolve);
    


 protected:
  VTKViewer::Representation::Type myRepresentation;
  vtkProperty *myProperty;
  bool myIsShaded;

  //! To initialize internal pipeline
  void
  InitPipeLine(vtkMapper* theMapper); 

  VTKViewer_GeometryFilter *myGeomFilter;
  VTKViewer_TransformFilter *myTransformFilter;
  std::vector<vtkPassThroughFilter*> myPassFilter;
  vtkShrinkFilter* myShrinkFilter;
  vtkFeatureEdges* myFeatureEdges;
  VTKViewer_DataSetMapper* myMapper;

  bool myIsShrinkable;
  bool myIsShrunk;
  
  bool myIsFeatureEdgesAllowed;
  bool myIsFeatureEdgesEnabled;

  bool myIsResolveCoincidentTopology;
  vtkFloatingPointType myPolygonOffsetFactor;
  vtkFloatingPointType myPolygonOffsetUnits;

  void SetPolygonOffsetParameters(vtkFloatingPointType factor, 
                                  vtkFloatingPointType units);
  void GetPolygonOffsetParameters(vtkFloatingPointType& factor, 
                                  vtkFloatingPointType& units);

  SVTK_DeviceActor();
  ~SVTK_DeviceActor();

 private:
  SVTK_DeviceActor(const SVTK_DeviceActor&); // Not implemented
  void operator=(const SVTK_DeviceActor&); // Not implemented

};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif //SVTK_DEVICE_ACTOR_H
