//  SALOME VTKViewer : build VTK viewer into Salome desktop
//
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
//
//
//
//  File   : SVTK_RenderWindowInteractor.h
//  Author : Nicolas REJNERI
//  Module : SALOME
//  $Header$

#ifndef SVTK_RenderWindowInteractor_h
#define SVTK_RenderWindowInteractor_h

#include "SVTK.h"

#include "SVTK_Selection.h"
#include "SALOME_InteractiveObject.hxx"

// QT Includes
#include <qobject.h>
#include <qtimer.h>

// VTK Includes
#include <vtkVersion.h>
#include <vtkRenderWindowInteractor.h>

// Open CASCADE Includes
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TColStd_MapIteratorOfMapOfInteger.hxx>

class vtkPicker;
class vtkCellPicker;
class vtkPointPicker;
class vtkActorCollection;

class SALOME_Actor;
class SVTK_Actor;

class SVTK_ViewWindow;
class SVTK_RenderWindow;
class SVTK_InteractorStyle;

// ------------------------------------------------------------
// :TRICKY: Fri Apr 21 22:19:27 2000 Pagey
// The Signal/Slot mechanism used by Qt requires that QObject 
// appear as the first class when using multiple inheritance. 
// Hence the order of the two classes QObject and vtkRenderWindowInteractor
// matters here. Be careful not to change it by accident. 
// ------------------------------------------------------------
class SVTK_EXPORT SVTK_RenderWindowInteractor: public QObject, 
  public vtkRenderWindowInteractor
{
  Q_OBJECT ;   
  friend class SVTK_ViewWindow;
public:

  static SVTK_RenderWindowInteractor *New() ; 

  vtkTypeMacro(SVTK_RenderWindowInteractor,vtkRenderWindowInteractor);

  // Description:
  // Initializes the event handlers without an XtAppContext.  This is
  // good for when you don`t have a user interface, but you still
  // want to have mouse interaction.
  virtual void Initialize();

  virtual void SetInteractorStyle(vtkInteractorObserver *);
  SVTK_InteractorStyle* GetSInteractorStyle(){ return myInteractorStyle;}

  // Description:
  // This will start up the X event loop and never return. If you
  // call this method it will loop processing X events until the
  // application is exited.
  virtual void Start();
  
  // Description:
  // Event loop notification member for Window size change
  virtual void UpdateSize(int x,int y);

  // Description:
  // Timer methods must be overridden by platform dependent subclasses.
  // flag is passed to indicate if this is first timer set or an update
  // as Win32 uses repeating timers, whereas X uses One shot more timer
  // if flag==VTKXI_TIMER_FIRST Win32 and X should createtimer
  // otherwise Win32 should exit and X should perform AddTimeOut()
  virtual int CreateTimer(int ) ; 
  virtual int DestroyTimer() ; 
  
  /* Selection Management */
  bool highlightCell(const TColStd_IndexedMapOfInteger& MapIndex, 
		     SALOME_Actor* theMapActor, 
		     bool hilight, 
		     bool update = true );
  bool highlightEdge(const TColStd_IndexedMapOfInteger& MapIndex, 
		     SALOME_Actor* theMapActor, 
		     bool hilight, 
		     bool update = true );
  bool highlightPoint(const TColStd_IndexedMapOfInteger& MapIndex, 
		      SALOME_Actor* theMapActor, 
		      bool hilight, 
		      bool update = true );
  bool highlight(const Handle(SALOME_InteractiveObject)& IObject, 
		 bool hiligth, 
		 bool immediatly = true );
  void unHighlightSubSelection();
  bool unHighlightAll();

  bool isInViewer( const Handle(SALOME_InteractiveObject)& IObject);
  bool isVisible( const Handle(SALOME_InteractiveObject)& IObject);
  void rename(const Handle(SALOME_InteractiveObject)& IObject, QString newName);

  void SetSelectionMode(Selection_Mode mode);
  void SetSelectionProp(const double& theRed = 1, 
			const double& theGreen = 1,
			const double& theBlue = 0, 
			const int& theWidth = 5);
  void SetSelectionTolerance(const double& theTolNodes = 0.025, 
			     const double& theTolCell = 0.001);

  // Displaymode management
  int GetDisplayMode();
  void SetDisplayMode(int);

  // Switch representation wireframe/shading
  void SetDisplayMode(const Handle(SALOME_InteractiveObject)& IObject, int theMode);

  // Change all actors to wireframe or surface
  void ChangeRepresentationToWireframe();
  void ChangeRepresentationToSurface();

  // Change to wireframe or surface a list of vtkactor
  void ChangeRepresentationToWireframe(vtkActorCollection* ListofActors);
  void ChangeRepresentationToSurface(vtkActorCollection* ListofActors);

  // Erase Display functions
  void EraseAll();
  void DisplayAll();
  void RemoveAll( const bool immediatly );
  void Erase(const Handle(SALOME_InteractiveObject)& IObject, 
	     bool immediatly=true);
  void Remove(const Handle(SALOME_InteractiveObject)& IObject, 
	      bool immediatly=true);
  void Display(const Handle(SALOME_InteractiveObject)& IObject, 
	       bool immediatly=true);

  void Display( SALOME_Actor* SActor, 
		bool immediatly = true );
  void Erase( SALOME_Actor* SActor, 
	      bool immediatly = true );
  void Remove( SALOME_Actor* SActor, 
	       bool updateViewer = true );

  // Transparency
  void SetTransparency(const Handle(SALOME_InteractiveObject)& IObject,
		       float trans);
  float GetTransparency(const Handle(SALOME_InteractiveObject)& IObject);

  // Color
  void SetColor(const Handle(SALOME_InteractiveObject)& IObject,
		QColor thecolor);
  QColor GetColor(const Handle(SALOME_InteractiveObject)& IObject);

  void Update();

  vtkRenderer* GetRenderer();

  void setGUIWindow(QWidget* theWindow);

  void setViewWindow(SVTK_ViewWindow* theViewWindow);
  
  void setCellData(const int& theIndex, 
		   SALOME_Actor* theMapActor,
		   SVTK_Actor* theActor);
  void setEdgeData(const int& theCellIndex, 
		   SALOME_Actor* theMapActor,
		   const int& theEdgeIndex, 
		   SVTK_Actor* theActor ); //NB
  void setPointData(const int& theIndex, 
		    SALOME_Actor* theMapActor,
		    SVTK_Actor* theActor);

  typedef void (*TUpdateActor)(const TColStd_IndexedMapOfInteger& theMapIndex,
			       SALOME_Actor* theMapActor, 
			       SVTK_Actor* theActor);
 protected:

  SVTK_RenderWindowInteractor();
  ~SVTK_RenderWindowInteractor();

  SVTK_InteractorStyle* myInteractorStyle;

  bool highlight(const TColStd_IndexedMapOfInteger& theMapIndex, 
		 SALOME_Actor* theMapActor, 
		 SVTK_Actor* theActor,
		 TUpdateActor theFun, 
		 bool hilight, 
		 bool update);
  void setActorData(const TColStd_IndexedMapOfInteger& theMapIndex,
		    SALOME_Actor* theMapActor,
		    SVTK_Actor *theActor,
		    TUpdateActor theFun);
  
  // Timer used during various mouse events to figure 
  // out mouse movements. 
  QTimer *mTimer ;

  int myDisplayMode;

  //NRI: Selection mode
  SVTK_Actor* myPointActor;
  SVTK_Actor* myEdgeActor;
  SVTK_Actor* myCellActor;
  void MoveInternalActors();

  vtkPicker* myBasicPicker;
  vtkCellPicker* myCellPicker;
  vtkPointPicker* myPointPicker;
  
  // User for switching to stereo mode.
  int PositionBeforeStereo[2];

 public slots:
  void MouseMove(const QMouseEvent *event) ;
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
  // Not all of these slots are needed in VTK_MAJOR_VERSION=3,
  // but moc does not understand "#if VTK_MAJOR_VERSION". Hence, 
  // we have to include all of these for the time being. Once,
  // this bug in MOC is fixed, we can separate these. 
  void TimerFunc() ;

 signals:
  void RenderWindowModified() ;
  void contextMenuRequested( QContextMenuEvent *e );

 private:
  SVTK_ViewWindow* myViewWindow;  
  QWidget* myGUIWindow;  
  double myTolNodes;
  double myTolItems;
};


#endif
