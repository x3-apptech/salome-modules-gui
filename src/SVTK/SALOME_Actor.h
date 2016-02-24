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

#ifndef SALOME_ACTOR_H
#define SALOME_ACTOR_H

#include "SVTK_SelectionEvent.h"
#include "SVTK_Selector.h"
#include "SVTK.h"

class SALOME_InteractiveObject;

// undefining min and max because CASCADE's defines them and
// it clashes with std::min(), std::max()
#undef min
#undef max

#include <VTKViewer_Actor.h>
#include <VTKViewer_MarkerDef.h>

#include <vtkSmartPointer.h>

class vtkAbstractPicker;
class vtkPointPicker;
class vtkCellPicker;
class vtkOutlineSource;
class vtkInteractorStyle;
class vtkRenderWindowInteractor;

class VTKViewer_FramedTextActor;

class SVTK_Actor;
class SVTK_AreaPicker;
class SVTK_InteractorStyle;

SVTK_EXPORT extern int SALOME_POINT_SIZE;
SVTK_EXPORT extern int SALOME_LINE_WIDTH;

#ifdef WIN32
#pragma warning ( disable:4251 )
#endif

//! The class is a basic one for all SALOME VTK presentation.
/*!
  It provide highlight and prehighlight capabilites,
  common way to publish and remove VTK presentation, 
  mapping of VTK and object IDs and so on.
 */
class SVTK_EXPORT SALOME_Actor : public VTKViewer_Actor 
{
 public:
  static SALOME_Actor* New();
  
  vtkTypeMacro(SALOME_Actor,VTKViewer_Actor);

  //----------------------------------------------------------------------------
  //! To check, if the #SALOME_Actor has a reference to #SALOME_InteractiveObject
  virtual
  Standard_Boolean 
  hasIO();

  //! Get correspoinding reference to #SALOME_InteractiveObject
  virtual 
  const Handle(SALOME_InteractiveObject)& 
  getIO(); 

  //! Set reference to #SALOME_InteractiveObject
  virtual
  void
  setIO(const Handle(SALOME_InteractiveObject)& theIO);

  //! Name the #SALOME_Actor
  virtual
  void
  setName(const char* theName);

  //----------------------------------------------------------------------------
  //! Apply view transformation
  virtual
  void
  SetTransform(VTKViewer_Transform* theTransform); 

  //! Apply additional position
  virtual
  void
  SetPosition(double _arg1, 
              double _arg2, 
              double _arg3);

  //! Apply additional position
  virtual
  void
  SetPosition(double _arg[3]);

  //----------------------------------------------------------------------------
  //! Visibility management
  virtual
  void
  SetVisibility( int );

  //! Gets know whether the actor should be displayed or not
  virtual
  bool
  ShouldBeDisplayed();

  //----------------------------------------------------------------------------
  //! To publish the actor an all its internal devices
  virtual
  void
  AddToRender(vtkRenderer* theRendere); 

  //! To remove the actor an all its internal devices
  virtual
  void
  RemoveFromRender(vtkRenderer* theRendere);

  //! Get reference on renderer where it is published
  vtkRenderer*
  GetRenderer();

  //----------------------------------------------------------------------------
  //! To set interactor in order to use #vtkInteractorObserver devices
  virtual
  void
  SetInteractor(vtkRenderWindowInteractor* theInteractor);

  //! Put a request to redraw the view 
  virtual
  void
  Update();

  //----------------------------------------------------------------------------
  //! Set selector in order to the actor at any time can restore current selection
  virtual
  void
  SetSelector(SVTK_Selector* theSelector);

  //! Just to update visibility of the highlight devices
  virtual
  void
  highlight(bool theHighlight);  

  //! To map current selection to VTK representation
  virtual
  void
  Highlight(bool theHighlight);  

  //----------------------------------------------------------------------------
  //! To process prehighlight (called from #SVTK_InteractorStyle)
  virtual
  bool
  PreHighlight(vtkInteractorStyle* theInteractorStyle, 
               SVTK_SelectionEvent* theSelectionEvent,
               bool theIsHighlight);

  //! To process highlight (called from #SVTK_InteractorStyle)
  virtual 
  bool
  Highlight(vtkInteractorStyle* theInteractorStyle, 
            SVTK_SelectionEvent* theSelectionEvent,
            bool theIsHighlight);

  //----------------------------------------------------------------------------
  //! To get flag of displaying of name actor
  virtual
  bool
  IsDisplayNameActor() const;

  //! To set flag of displaying of name actor
  virtual
  void
  SetIsDisplayNameActor(bool theIsDisplayNameActor);

  //! To set text of name actor
  virtual
  void
  SetNameActorText(const char* theText);

  //! To set offset of name actor
  virtual
  void
  SetNameActorOffset(double theOffset[2]);

  //! To get size of name actor
  virtual
  void
  GetNameActorSize(vtkRenderer* theRenderer, double theSize[2]) const;

  //! To update visibility of name actors
  virtual
  void
  UpdateNameActors();

  //----------------------------------------------------------------------------
  //! Set standard point marker
  virtual
  void
  SetMarkerStd( VTK::MarkerType, VTK::MarkerScale );

  //! Set custom point marker
  virtual
  void
  SetMarkerTexture( int, VTK::MarkerTexture );

  //! Get type of the point marker
  virtual
  VTK::MarkerType
  GetMarkerType();

  //! Get scale of the point marker
  virtual
  VTK::MarkerScale
  GetMarkerScale();

  //! Get texture identifier of the point marker
  virtual
  int
  GetMarkerTexture();

  //----------------------------------------------------------------------------
  //! To set up a picker for nodal selection (initialized by #SVTK_Renderer::AddActor)
  void
  SetPointPicker(vtkPointPicker* thePointPicker); 

  //! To set up a picker for cell selection (initialized by #SVTK_Renderer::AddActor)
  void
  SetCellPicker(vtkCellPicker* theCellPicker); 

  //! To set up a picker for point rectangle selection (initialized by #SVTK_Renderer::AddActor)
  void
  SetPointAreaPicker(SVTK_AreaPicker* theAreaPicker);

  //! To set up a picker for cell rectangle selection (initialized by #SVTK_Renderer::AddActor)
  void
  SetCellAreaPicker(SVTK_AreaPicker* theAreaPicker);

  //----------------------------------------------------------------------------
  //! To find a gravity center of object
  double* GetGravityCenter( int );

  //----------------------------------------------------------------------------
  //! To set up a prehighlight property (initialized by #SVTK_Renderer::AddActor)
  void
  SetPreHighlightProperty(vtkProperty* theProperty);

  //! To set up a highlight property (initialized by #SVTK_Renderer::AddActor)
  void
  SetHighlightProperty(vtkProperty* theProperty);

 protected:
  //----------------------------------------------------------------------------
  vtkRenderWindowInteractor* myInteractor;
  vtkRenderer* myRenderer;

  Handle(SALOME_InteractiveObject) myIO;

  SALOME_Actor();
  ~SALOME_Actor();

 protected:
  Selection_Mode mySelectionMode;
  vtkSmartPointer<SVTK_Selector> mySelector;

  //----------------------------------------------------------------------------
  // Highlight/ Prehighlight devices
  vtkSmartPointer<vtkPointPicker> myPointPicker;
  vtkSmartPointer<vtkCellPicker> myCellPicker;

  vtkSmartPointer<SVTK_AreaPicker> myPointAreaPicker;
  vtkSmartPointer<SVTK_AreaPicker> myCellAreaPicker;

  vtkSmartPointer<SVTK_Actor> myPreHighlightActor;
  vtkSmartPointer<SVTK_Actor> myHighlightActor;

  vtkSmartPointer<VTKViewer_Actor> myOutlineActor;
  vtkSmartPointer<vtkOutlineSource> myOutline;

  bool myIsDisplayNameActor;
  vtkSmartPointer<VTKViewer_FramedTextActor> myNameActor;
};

namespace SVTK
{
  class SVTK_EXPORT TPickLimiter
  {
    vtkAbstractPicker* myPicker;
  public:
    TPickLimiter(vtkAbstractPicker*, SALOME_Actor*);
    ~TPickLimiter();
  };
}

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif // SALOME_ACTOR_H
