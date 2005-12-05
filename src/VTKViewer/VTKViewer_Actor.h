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
// See http://www.salome-platform.org/
//
#ifndef VTKVIEVER_ACTOR_H
#define VTKVIEVER_ACTOR_H

#include "VTKViewer.h"

#include <vtkLODActor.h>
#include <vtkProperty.h>
#include <vtkShrinkFilter.h>
#include <vtkDataSetMapper.h>
#include <vtkUnstructuredGrid.h>

#include <vector>

class vtkCell;
class vtkDataSet;
class vtkPolyData;

class VTKViewer_Transform;
class VTKViewer_GeometryFilter;
class VTKViewer_TransformFilter;
class VTKViewer_PassThroughFilter;

#define VTKViewer_POINT_SIZE 3
/*! \class vtkLODActor
 * \brief For more information see <a href="http://www.vtk.org/">VTK documentation</a>
 */
class VTKVIEWER_EXPORT VTKViewer_Actor : public vtkLODActor
{
public:
  /*!Create new instance of actor.*/
  static VTKViewer_Actor* New();

  vtkTypeMacro( VTKViewer_Actor, vtkLODActor );

  /*!Get name of the actor*/
  virtual const char* getName() { return myName.c_str(); }
  /*!Set name of the actor*/
  virtual void setName(const char* theName){ myName = theName;}

  //! To generate highlight automaticaly
  virtual bool hasHighlight() { return false; }
  //! Sets highlight.
  virtual void highlight(bool theHighlight) { myIsHighlighted = theHighlight; }
  //! Check highlight.
  virtual bool isHighlighted() { return myIsHighlighted; }
  
  virtual void SetOpacity(float theOpacity);
  virtual float GetOpacity();

  virtual void SetColor(float r,float g,float b);
  virtual void GetColor(float& r,float& g,float& b);
  void SetColor(const float theRGB[3]){ SetColor(theRGB[0],theRGB[1],theRGB[2]); }

  vtkSetObjectMacro(PreviewProperty,vtkProperty);

  virtual void SetPreSelected(bool thePreselect = false) { myIsPreselected = thePreselect;}

  //! Used to obtain all dependent actors
  virtual void GetChildActors(vtkActorCollection*) {};
  
  virtual void AddToRender(vtkRenderer* theRenderer); 
  virtual void RemoveFromRender(vtkRenderer* theRenderer);


  /** @name For selection mapping purpose */
  //@{
  virtual int GetNodeObjId(int theVtkID) { return theVtkID;}
  virtual float* GetNodeCoord(int theObjID);

  virtual int GetElemObjId(int theVtkID) { return theVtkID;}
  virtual vtkCell* GetElemCell(int theObjID);
  //@}

  virtual int GetObjDimension( const int theObjId );

  virtual void SetMapper(vtkMapper* theMapper); 
  virtual vtkDataSet* GetInput(); 

  virtual void SetTransform(VTKViewer_Transform* theTransform); 
  virtual unsigned long int GetMTime();

  virtual void SetRepresentation(int theMode);
  virtual int GetRepresentation();

  virtual int getDisplayMode();
  virtual void setDisplayMode(int theMode);

  /*! Infinitive means actor without size (point for example), \n
   * which is not taken into account in calculation of boundaries of the scene
   */
  void SetInfinitive(bool theIsInfinite) { myIsInfinite = theIsInfinite; }
  virtual bool IsInfinitive();
    
  void SetResolveCoincidentTopology(bool theIsResolve);
  void SetPolygonOffsetParameters(float factor, float units);
  void GetPolygonOffsetParameters(float& factor, float& units);

  virtual void Render(vtkRenderer *, vtkMapper *);

protected:
  /*!resolve coincedent topology flag*/
  bool myIsResolveCoincidentTopology;
  /*!polygon offset factor*/
  float myPolygonOffsetFactor;
  /*!polygon offset units*/
  float myPolygonOffsetUnits;

  /*!Actor name.*/
  std::string myName;

  /*!preview property*/
  vtkProperty *PreviewProperty;
  /*!preselected flag*/
  bool myIsPreselected;

  /*!opacity*/
  float myOpacity;
  /*!highlighted flag*/
  bool myIsHighlighted;
  /*!display mode*/
  int myDisplayMode;
  /*!infinite flag*/
  bool myIsInfinite;

  /*!store mapping flag*/
  bool myStoreMapping;
  /*!geometry filter*/
  VTKViewer_GeometryFilter *myGeomFilter;
  /*!transform filter*/
  VTKViewer_TransformFilter *myTransformFilter;
  /*!vector of passive filters(siz filters used)*/
  std::vector<VTKViewer_PassThroughFilter*> myPassFilter;

  /*!presentation mode*/
  int myRepresentation;
  /*!property*/
  vtkProperty *myProperty;

  //! Main method, which calculate output.
  void InitPipeLine(vtkMapper* theMapper); 

  VTKViewer_Actor();
  ~VTKViewer_Actor();
};

#endif
