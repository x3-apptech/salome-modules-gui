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

//  SALOME OBJECT : implementation of interactive object visualization for OCC and VTK viewers
//  File   : SALOME_Actor.h
//  Author : Nicolas REJNERI

#ifndef VTKVIEVER_ACTOR_H
#define VTKVIEVER_ACTOR_H

#include "VTKViewer.h"

#include <string>
#include <vector>

#include <vtkLODActor.h>
#include <vtkProperty.h>

class vtkCell;
class vtkPointPicker;
class vtkCellPicker;
class vtkDataSet;
class vtkCamera;
class vtkProperty;
class vtkRenderer;
class vtkPassThroughFilter;

class VTKViewer_Transform;
class VTKViewer_GeometryFilter;
class VTKViewer_TransformFilter;

extern int VTKViewer_POINT_SIZE;
extern int VTKViewer_LINE_WIDTH;

namespace VTKViewer
{
  namespace Representation
  {
    typedef int Type;
    const Type Points = VTK_POINTS;
    const Type Wireframe = VTK_WIREFRAME;
    const Type Surface = VTK_SURFACE;
    const Type Insideframe = Surface + 1;
    const Type SurfaceWithEdges = Insideframe + 1;
  }
}

#ifdef WIN32
#pragma warning ( disable:4251 )
#endif

/*! \class vtkLODActor
 * \brief For more information see <a href="http://www.vtk.org/">VTK documentation</a>
 */
class VTKVIEWER_EXPORT VTKViewer_Actor : public vtkLODActor 
{
 public:
  static VTKViewer_Actor* New();
  
  vtkTypeMacro(VTKViewer_Actor,vtkLODActor);

  //----------------------------------------------------------------------------
  //! Get its name
  virtual 
  const char* 
  getName();

  //! Name the #VTKViewer_Actor
  virtual
  void
  setName(const char* theName);

  //----------------------------------------------------------------------------
  //! Change opacity
  virtual
  void
  SetOpacity(double theOpacity);

  //! Get current opacity
  virtual
  double 
  GetOpacity();

  //! Change color
  virtual
  void
  SetColor(double r,
           double g,
           double b);

  //! Get current color
  virtual
  void
  GetColor(double& r,
           double& g,
           double& b);

  //! Change color
  virtual
  void
  SetColor(const double theRGB[3]);

  //! Change material
  virtual
  void
  SetMaterial(std::vector<vtkProperty*> theProps);

  //! Get current front material
  virtual
  vtkProperty* 
  GetFrontMaterial();

  //! Get current back material
  virtual
  vtkProperty* 
  GetBackMaterial();

  //----------------------------------------------------------------------------
  // For selection mapping purpose
  //! Maps VTK index of a node to corresponding object index
  virtual
  int 
  GetNodeObjId(int theVtkID);

  //! Get coordinates of a node for given object index
  virtual
  double*
  GetNodeCoord(int theObjID);

  //! Maps VTK index of a cell to corresponding object index
  virtual 
  int
  GetElemObjId(int theVtkID);

  //! Get corresponding #vtkCell for given object index
  virtual
  vtkCell* 
  GetElemCell(int theObjID);

  //----------------------------------------------------------------------------
  //! Get dimension of corresponding mesh element
  virtual
  int
  GetObjDimension( const int theObjId );

  //! To insert some additional filters and then sets the given #vtkMapper
  virtual
  void
  SetMapper(vtkMapper* theMapper); 

  //! Allows to get initial #vtkDataSet
  virtual
  vtkDataSet* 
  GetInput(); 

  //! Apply view transformation
  virtual
  void
  SetTransform(VTKViewer_Transform* theTransform); 

  //! To calculatate last modified time
  virtual
  vtkMTimeType
  GetMTime();

  //----------------------------------------------------------------------------
  //! Set representation (VTK_SURFACE, VTK_POINTS, VTK_WIREFRAME and so on)
  virtual
  void
  SetRepresentation(int theMode);

  //! Get current representation mode
  virtual
  int
  GetRepresentation();

  //! Get current display mode (obsolete)
  virtual
  int
  getDisplayMode();

  //! Set display mode (obsolete)
  virtual
  void
  setDisplayMode(int theMode);

  //----------------------------------------------------------------------------
  //! Set infinive flag
  /*!
    Infinitive means actor without size (point for example),
    which is not taken into account in calculation of boundaries of the scene
  */
  void
  SetInfinitive(bool theIsInfinite);

  //! Get infinive flag
  virtual
  bool
  IsInfinitive();
    
  //! To calcualte current bounding box
  virtual
  double* 
  GetBounds();

  //! To calcualte current bounding box
  void
  GetBounds(double bounds[6]);

  //----------------------------------------------------------------------------
  virtual
  bool
  IsSetCamera() const;

  virtual
  bool
  IsResizable() const;

  virtual
  void
  SetSize( const double );

  virtual
  void 
  SetCamera( vtkCamera* );

  //----------------------------------------------------------------------------
  //! Set ResolveCoincidentTopology flag
  void
  SetResolveCoincidentTopology(bool theIsResolve);

  //! Set ResolveCoincidentTopology parameters
  void
  SetPolygonOffsetParameters(double factor, 
                             double units);

  //! Get current ResolveCoincidentTopology parameters
  void
  GetPolygonOffsetParameters(double& factor, 
                             double& units);

  virtual
  void
  Render(vtkRenderer *, vtkMapper *);

  //----------------------------------------------------------------------------
  //! Get current shrink factor
  virtual
  double
  GetShrinkFactor();

  //! Is the actor is shrunkable
  virtual
  bool
  IsShrunkable();

  //! Is the actor is shrunk
  virtual
  bool
  IsShrunk();

  //! Insert shrink filter into pipeline
  virtual
  void
  SetShrink();

  //! Remove shrink filter from pipeline
  virtual
  void
  UnShrink();

  //----------------------------------------------------------------------------
  //! To publish the actor an all its internal devices
  virtual
  void
  AddToRender(vtkRenderer* theRendere); 

  //! To remove the actor an all its internal devices
  virtual
  void
  RemoveFromRender(vtkRenderer* theRendere);

  //! Used to obtain all dependent actors
  virtual
  void
  GetChildActors(vtkActorCollection*);

  //----------------------------------------------------------------------------
  //! Is notified on enabling/disabling selection
  virtual
  void
  EnableSelection( bool enable ) {}

  //! Ask, if the descendant of the VTKViewer_Actor will implement its own highlight or not
  virtual
  bool
  hasHighlight(); 

  //! Ask, if the VTKViewer_Actor is already highlighted
  virtual
  bool
  isHighlighted();

  //! Ask, if the VTKViewer_Actor is already preselected
  virtual
  bool
  isPreselected();

  //! Set preselection mode
  virtual
  void
  SetPreSelected(bool thePreselect = false);

  //----------------------------------------------------------------------------
  //! Just to update visibility of the highlight devices
  virtual
  void
  highlight(bool theHighlight);  

  void
  SetPreviewProperty(vtkProperty* theProperty);

  //----------------------------------------------------------------------------
  //! Setting for displaying quadratic elements
  virtual void SetQuadraticArcMode(bool theFlag);
  virtual bool GetQuadraticArcMode() const;

  virtual void   SetQuadraticArcAngle(double theMaxAngle);
  virtual double GetQuadraticArcAngle() const;

  //----------------------------------------------------------------------------
  //! Return pointer to the dataset, which used to calculation of the bounding box of the actor
  virtual vtkDataSet* GetHighlightedDataSet();

 protected:
  //----------------------------------------------------------------------------
  bool myIsResolveCoincidentTopology;
  double myPolygonOffsetFactor;
  double myPolygonOffsetUnits;

  std::string myName;

  double myOpacity;
  int myDisplayMode;
  bool myIsInfinite;

  bool myStoreMapping;
  VTKViewer_GeometryFilter *myGeomFilter;
  VTKViewer_TransformFilter *myTransformFilter;
  std::vector<vtkPassThroughFilter*> myPassFilter;

  int myRepresentation;
  vtkProperty *myProperty;

  void
  InitPipeLine(vtkMapper* theMapper); 

  VTKViewer_Actor();
  ~VTKViewer_Actor();

 protected:
  vtkProperty *PreviewProperty;
  bool myIsPreselected;
  bool myIsHighlighted;
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif // VTKVIEVER_ACTOR_H
