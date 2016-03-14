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

#ifndef VTKVIEWER_RENDERWINDOWINTERACTOR_H
#define VTKVIEWER_RENDERWINDOWINTERACTOR_H

#include "VTKViewer.h"
#include "VTKViewer_Actor.h"

#include <QObject>

class QTimer;
class QMouseEvent;
class QKeyEvent;
class QContextMenuEvent;

// Open CASCADE Includes
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_MapIteratorOfMapOfInteger.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>

class vtkPicker;
class vtkCellPicker;
class vtkPointPicker;
class vtkActorCollection;

class VTKViewer_Actor;
class VTKViewer_ViewWindow;
class VTKViewer_RenderWindow;
class VTKViewer_InteractorStyle;

#include "VTKViewer_Algorithm.h"

#include <vtkActor.h>
#include <vtkVersion.h>
#include <vtkRenderWindowInteractor.h>

class VTKVIEWER_EXPORT VTKViewer_RenderWindowInteractor : public QObject, public vtkRenderWindowInteractor
{
  Q_OBJECT

public:
  static VTKViewer_RenderWindowInteractor *New();

  vtkTypeMacro(VTKViewer_RenderWindowInteractor,vtkRenderWindowInteractor);

  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void Initialize();

  virtual void               SetInteractorStyle(vtkInteractorObserver *);
  /*!Return interactor style pointer.*/
  VTKViewer_InteractorStyle* GetInteractorStyle() const
  {
    return myInteractorStyle;
  }

  virtual void Start();
  
  virtual void Enable();
  virtual void Disable();

  virtual void UpdateSize(int x,int y);

  /** @name Timer options*/
  //@{
  virtual int CreateTimer(int ) ;
  virtual int DestroyTimer() ; 
  //@}
  
  /*! Description:\n
   * This function is called on 'q','e' keypress if exitmethod is not\n
   * specified and should be overidden by platform dependent subclasses\n
   * to provide a termination procedure if one is required.
   */
  virtual void TerminateApp(void) { /* empty */ }
  
  // Description:
  // These methods correspond to the the Exit, User and Pick
  // callbacks. They allow for the Style to invoke them.
  //virtual void ExitCallback();
  //virtual void UserCallback();
  //virtual void StartPickCallback();
  //virtual void EndPickCallback();
  
  /** @name Selection Management */
  //@{
  bool highlightCell(const TColStd_IndexedMapOfInteger& MapIndex,
                     VTKViewer_Actor* theMapActor,
                     bool hilight,
                     bool update = true );
  bool highlightEdge(const TColStd_IndexedMapOfInteger& MapIndex,
                     VTKViewer_Actor* theMapActor,
                     bool hilight,
                     bool update = true );
  bool highlightPoint(const TColStd_IndexedMapOfInteger& MapIndex,
                      VTKViewer_Actor* theMapActor,
                      bool hilight,
                      bool update = true );

  void unHighlightSubSelection();
  bool unHighlightAll();

  //void SetSelectionMode(Selection_Mode mode);
  void SetSelectionProp(const double& theRed = 1, const double& theGreen = 1,
                        const double& theBlue = 0, const int& theWidth = 5);
  void SetSelectionTolerance(const double& theTolNodes = 0.025, const double& theTolCell = 0.001);
  //@}

  /** @name Displaymode management*/
  //@{
  int GetDisplayMode();
  void SetDisplayMode(int);
  //@}

  /** @name Change all actors to wireframe or surface*/
  //@{
  void ChangeRepresentationToWireframe();
  void ChangeRepresentationToSurface();
  void ChangeRepresentationToSurfaceWithEdges();
  //@}

  /** @name Change to wireframe or surface a list of vtkactor*/
  //@{
  void ChangeRepresentationToWireframe(vtkActorCollection* ListofActors);
  void ChangeRepresentationToSurface(vtkActorCollection* ListofActors);
  void ChangeRepresentationToSurfaceWithEdges(vtkActorCollection* ListofActors);
  //@}

  /** @name Erase Display functions*/
  //@{
  void EraseAll();
  void DisplayAll();
  void RemoveAll( const bool immediatly );

  void Display( VTKViewer_Actor* SActor, bool immediatly = true );
  void Erase( VTKViewer_Actor* SActor, bool immediatly = true );
  void Remove( VTKViewer_Actor* SActor, bool updateViewer = true );
  //@}

  void Update();

  vtkRenderer* GetRenderer();

  void setViewWindow( VTKViewer_ViewWindow* theViewWnd );

  void setCellData(const int& theIndex,
                   VTKViewer_Actor* theMapActor,
                   VTKViewer_Actor* theActor) {}
  void setEdgeData(const int& theCellIndex,
                   VTKViewer_Actor* theMapActor,
                   const int& theEdgeIndex,
                   VTKViewer_Actor* theActor ) {} //NB
  void setPointData(const int& theIndex,
                    VTKViewer_Actor* theMapActor,
                    VTKViewer_Actor* theActor) {}

  typedef void (*TUpdateActor)(const TColStd_IndexedMapOfInteger& theMapIndex,
                               VTKViewer_Actor* theMapActor,
                               VTKViewer_Actor* theActor);
 protected:

  VTKViewer_RenderWindowInteractor();
  ~VTKViewer_RenderWindowInteractor();

  VTKViewer_InteractorStyle* myInteractorStyle;

  bool highlight(const TColStd_IndexedMapOfInteger& theMapIndex,
                 VTKViewer_Actor* theMapActor, VTKViewer_Actor* theActor,
                 TUpdateActor theFun, bool hilight, bool update);
  void setActorData(const TColStd_IndexedMapOfInteger& theMapIndex,
                    VTKViewer_Actor* theMapActor,
                    VTKViewer_Actor *theActor,
                    TUpdateActor theFun);

  /*! Timer used during various mouse events to figure 
   * out mouse movements.
   */
  QTimer *mTimer ;

  int myDisplayMode;

  //NRI: Selection mode
  VTKViewer_Actor* myPointActor;
  VTKViewer_Actor* myEdgeActor;
  VTKViewer_Actor* myCellActor;
  void MoveInternalActors();

  vtkPicker* myBasicPicker;
  vtkCellPicker* myCellPicker;
  vtkPointPicker* myPointPicker;
  
  /*! User for switching to stereo mode.*/
  int PositionBeforeStereo[2];

 public slots:
  void MouseMove(QMouseEvent *event) ;
  void LeftButtonPressed(const QMouseEvent *event) ;
  void LeftButtonReleased(const QMouseEvent *event) ;
  void MiddleButtonPressed(const QMouseEvent *event) ;
  void MiddleButtonReleased(const QMouseEvent *event) ;
  void RightButtonPressed(const QMouseEvent *event) ;
  void RightButtonReleased(const QMouseEvent *event) ;
  void ButtonPressed(const QMouseEvent *event) ;
  void ButtonReleased(const QMouseEvent *event) ;
  void KeyPressed(QKeyEvent *event) ;

  private slots:
    void TimerFunc() ;

signals:
  void RenderWindowModified() ;
  void contextMenuRequested( QContextMenuEvent *e );

private:
  friend class VTKViewer_ViewWindow;

  VTKViewer_ViewWindow* myViewWnd;
  /** Selection node tolerance.*/
  double       myTolNodes;
  /** Selection cell tolerance.*/
  double       myTolItems;
};

#endif
