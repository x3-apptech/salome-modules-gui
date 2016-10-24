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

#ifndef VTKVIEWER_VIEWWINDOW_H
#define VTKVIEWER_VIEWWINDOW_H

#include "VTKViewer.h"

#include "SUIT_ViewWindow.h"

#include "QtxAction.h"

class vtkRenderer;
class SUIT_Desktop;
class VTKViewer_Viewer;
class VTKViewer_Trihedron;
class VTKViewer_Transform;
class VTKViewer_RenderWindow;
class VTKViewer_InteractorStyle;
class VTKViewer_RenderWindowInteractor;
class VTKViewer_Actor;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class VTKVIEWER_EXPORT VTKViewer_ViewWindow : public SUIT_ViewWindow
{
  Q_OBJECT

public:
  VTKViewer_ViewWindow( SUIT_Desktop*, VTKViewer_Viewer*,
                        VTKViewer_InteractorStyle* = 0,
                        VTKViewer_RenderWindowInteractor* = 0 );
  virtual ~VTKViewer_ViewWindow();
  
  /*!Gets tool bar.*/
  QToolBar*                         getToolBar() { return myToolBar; }
  
  void                              setBackgroundColor( const QColor& ); // obsolete
  QColor                            backgroundColor() const;             // obsolete
  void                              setBackground( const Qtx::BackgroundData& );
  Qtx::BackgroundData               background() const;

  /*!Gets renderer.*/
  vtkRenderer*                      getRenderer()     { return myRenderer;     }
  /*!Gets render window.*/
  VTKViewer_RenderWindow*           getRenderWindow() { return myRenderWindow; }
  /*!Gets render window interactor.*/
  VTKViewer_RenderWindowInteractor* getRWInteractor() { return myRWInteractor; }
  bool                              isTrihedronDisplayed();

  void Repaint( bool theUpdateTrihedron = true );
  void onAdjustTrihedron();
  void GetScale( double theScale[3] );
  void SetScale( double theScale[3] );
  void AddActor( VTKViewer_Actor*, bool update = false );
  void RemoveActor( VTKViewer_Actor*, bool update = false);

  virtual QString   getVisualParameters();
  virtual void      setVisualParameters( const QString& parameters );

public slots:
  void onFrontView();
  void onBackView();
  void onTopView();
  void onBottomView();
  void onLeftView();
  void onRightView();
  void onClockWiseView();
  void onAntiClockWiseView();
  void onResetView();
  void onFitAll();
  void onFitSelection();
  void activateZoom();
  void activateWindowFit();
  void activateRotation();
  void activatePanning();
  void activateGlobalPanning();
  void onTrihedronShow();

protected:
  QImage dumpView();

protected slots:
  void onKeyPressed(QKeyEvent* event);
  void onKeyReleased(QKeyEvent* event);
  void onMousePressed(QMouseEvent* event);
  void onMouseDoubleClicked(QMouseEvent* event);
  void onMouseReleased(QMouseEvent* event);
  void onMouseMoving(QMouseEvent* event);

private:
  void                              InsertActor( VTKViewer_Actor* theActor,
                                                 bool theMoveInternalActors = false );
  void                              MoveActor( VTKViewer_Actor* theActor );

private:
  enum { DumpId, FitAllId, FitRectId, FitSelectionId, ZoomId, PanId, GlobalPanId, RotationId,
         FrontId, BackId, TopId, BottomId, LeftId, RightId, ClockWiseId, AntiClockWiseId,
         ResetId, TrihedronShowId };
  typedef QMap<int, QtxAction*> ActionsMap;
  
  void    createActions();
  void    createToolBar();
  
  VTKViewer_Viewer*                 myModel;
 
  vtkRenderer*                      myRenderer;
  VTKViewer_RenderWindow*           myRenderWindow;
  VTKViewer_RenderWindowInteractor* myRWInteractor;
  
  VTKViewer_Trihedron*              myTrihedron;  
  VTKViewer_Transform*              myTransform;
  
  QToolBar*                         myToolBar;
  ActionsMap                        myActionsMap;  
  
  double                            myCurScale;
  Qtx::BackgroundData               myBackground;

  friend class VTKViewer_RenderWindowInteractor;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
