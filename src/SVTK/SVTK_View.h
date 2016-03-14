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

#ifndef SVTK_VIEW_H
#define SVTK_VIEW_H

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

#include "SVTK.h"

#include <SALOME_InteractiveObject.hxx>
#include <SALOME_ListIO.hxx>

#include <QObject>

class vtkActorCollection;
class vtkRenderer;
class vtkProperty;

class SVTK_ViewWindow;
class SVTK_Renderer;

class SALOME_Actor;

class QMouseEvent;
class QWheelEvent;
class QKeyEvent;
class QContextMenuEvent;
class QColor;


//! Main purpose of the class is to provide a way to customize #SVTK_ViewWindow.
/*!
  This class is initialized by #SVTK_ViewWindow and just pass Qt signals from 
  corresponding #SVTK_RenderWindowInteractor of the #SVTK_ViewWindow.
  Its main purpose is to provide a simple and flexible way to customize the #SVTK_ViewWindow.
  So, in your own viewer it is possible to derive new #SVTK_ViewWindow and 
  use existing functionality without any modifications.
 */
class SVTK_EXPORT SVTK_SignalHandler : public QObject
{
  Q_OBJECT;

public:
  SVTK_SignalHandler(SVTK_ViewWindow* theMainWindow);

  virtual
  ~SVTK_SignalHandler();

  //! Get reference to its #SVTK_ViewWindow
  SVTK_ViewWindow*
  GetMainWindow();

  //----------------------------------------------------------------------------
  //! Redirect the request to #SVTK_ViewWindow::Repaint (just for flexibility)
  void
  Repaint(bool theUpdateTrihedron = true);

  //----------------------------------------------------------------------------
  //! Redirect the request to #SVTK_ViewWindow::GetRenderer (just for flexibility)
  SVTK_Renderer* 
  GetRenderer();

  //! Redirect the request to #SVTK_ViewWindow::getRenderer (just for flexibility)
  vtkRenderer* 
  getRenderer();

  //----------------------------------------------------------------------------
 public slots:
  void onSelectionChanged();

 public:
 signals:
  void MouseMove( QMouseEvent* );
  void MouseButtonPressed( QMouseEvent* );
  void MouseButtonReleased( QMouseEvent* );
  void MouseDoubleClicked( QMouseEvent* );
  void ButtonPressed(const QMouseEvent *event);
  void ButtonReleased(const QMouseEvent *event);
  void WheelMoved( QWheelEvent* );
  void KeyPressed( QKeyEvent* );
  void KeyReleased( QKeyEvent* );
  void contextMenuRequested( QContextMenuEvent *e );
  void selectionChanged();

 protected:
  SVTK_ViewWindow* myMainWindow;
};


//! This class is introduced just for compatibility with old code.
/*!
  This class contains frequantly used functionality in old code.
  Now, you are supposed to extend existing functionality through
  writing corresponding functors (see SVTK_Functor.h for example).
 */
class SVTK_EXPORT SVTK_View : public SVTK_SignalHandler
{
  Q_OBJECT;

public:
  SVTK_View(SVTK_ViewWindow* theMainWindow);

  virtual
  ~SVTK_View();
  
  /*  interactive object management */
  //! To highlight a VTK presentation with the same #SALOME_InteractiveObject
  void
  highlight(const Handle(SALOME_InteractiveObject)& IObject, 
            bool highlight, 
            bool immediatly = true);

  //! To unhighlight all VTK presentations
  void
  unHighlightAll();

  //! Try to find a SALOME_InteractiveObject in the view that corresponds to the entry
  Handle(SALOME_InteractiveObject) 
  FindIObject(const char* Entry);

  //! Check, if the viewer contains a presentatin with defined #SALOME_InteractiveObject
  bool
  isInViewer(const Handle(SALOME_InteractiveObject)& IObject);

  //! Check, if a presentatin with defined #SALOME_InteractiveObject is visible
  bool
  isVisible(const Handle(SALOME_InteractiveObject)& IObject);

  void
  rename(const Handle(SALOME_InteractiveObject)& IObject, 
         const QString& newName);
  
  //----------------------------------------------------------------------------
  // Displaymode management
  //! Get current display mode (obsolete)
  int 
  GetDisplayMode();

  //! Set current display mode
  void
  SetDisplayMode(int);

  //! Switch representation wireframe/shading
  void
  SetDisplayMode(const Handle(SALOME_InteractiveObject)& IObject, 
                 int theMode);

  //! Change all actors to wireframe
  void 
  ChangeRepresentationToWireframe();

  //! Change all actors to surface
  void
  ChangeRepresentationToSurface();

  //! Change all actors to surface with edges
  void
  ChangeRepresentationToSurfaceWithEdges();

  //! Change to wireframe a list of vtkactor
  void
  ChangeRepresentationToWireframe(vtkActorCollection* theListofActors);

  //! Change to surface a list of vtkactor
  void
  ChangeRepresentationToSurface(vtkActorCollection* theListofActors);

  //! Change to surface with edges a list of vtkactor
  void
  ChangeRepresentationToSurfaceWithEdges(vtkActorCollection* theListofActors);

  //! Change transparency
  void
  SetTransparency(const Handle(SALOME_InteractiveObject)& theIObject,
                  float trans);

  //! Get current transparency
  float 
  GetTransparency(const Handle(SALOME_InteractiveObject)& theIObject);

  //! Change color
  void
  SetColor(const Handle(SALOME_InteractiveObject)& theIObject,
           const QColor& theColor);

  //! Get current color
  QColor
  GetColor(const Handle(SALOME_InteractiveObject)& theIObject);

  //! Change material
  void
  SetMaterial(const Handle(SALOME_InteractiveObject)& theIObject,
	      vtkProperty* thePropF, vtkProperty* thePropB);

  //! Get current front material
  vtkProperty* 
  GetFrontMaterial(const Handle(SALOME_InteractiveObject)& theIObject);

  //! Get current back material
  vtkProperty* 
  GetBackMaterial(const Handle(SALOME_InteractiveObject)& theIObject);

  //----------------------------------------------------------------------------
  // Erase Display functions
  //! To erase all existing VTK presentations
  void
  EraseAll();

  //! To display all existing VTK presentations
  void
  DisplayAll();

  //! To remove from the view all existing VTK presentations
  void
  RemoveAll( const bool immediatly );

  //! To erase VTK presentation with defined #SALOME_InteractiveObject
  void
  Erase(const Handle(SALOME_InteractiveObject)& IObject, 
        bool immediatly = true);
  void
  Remove(const Handle(SALOME_InteractiveObject)& IObject, 
         bool immediatly = true);

  //! To display VTK presentation with defined #SALOME_InteractiveObject
  void
  Display(const Handle(SALOME_InteractiveObject)& IObject, 
          bool immediatly = true);

  //! To display VTK presentation with defined #SALOME_InteractiveObject and erase all anothers
  void
  DisplayOnly(const Handle(SALOME_InteractiveObject)& IObject);

  //! To display the VTK presentation
  void
  Display(SALOME_Actor* SActor, 
          bool immediatly = true);

  //! To erase the VTK presentation
  void
  Erase(SALOME_Actor* SActor, 
        bool immediatly = true);

  //! To remove the VTK presentation
  void
  Remove(SALOME_Actor* SActor, 
         bool updateViewer = true);

  //! Collect objects visible in viewer
  void
  GetVisible( SALOME_ListIO& theList );

  //----------------------------------------------------------------------------
  //! Redirect the request to #SVTK_Renderer::SetPreselectionProp
  void
  SetSelectionProp(const double& theRed = 1, 
                   const double& theGreen = 1,
                   const double& theBlue = 0, 
                   const int& theWidth = 5);

  //! Redirect the request to #SVTK_Renderer::SetPreselectionProp
  void
  SetPreselectionProp(const double& theRed = 0, 
                      const double& theGreen = 1,
                      const double& theBlue = 1, 
                      const int& theWidth = 5);

  //! Redirect the request to #SVTK_Renderer::SetPreselectionProp
  void
  SetSelectionTolerance(const double& theTolNodes = 0.025, 
                        const double& theTolCell = 0.001,
                        const double& theTolObjects = 0.025);

 protected:  
  int myDisplayMode;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
