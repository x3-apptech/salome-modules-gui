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

//  SALOME VTKViewer : build VTK viewer into Salome desktop
//  File   : 
//  Author : 

#ifndef SVTK_RenderWindowInteractor_h
#define SVTK_RenderWindowInteractor_h

#include "SVTK.h"
#include "SVTK_Selection.h"

#include <vtkSmartPointer.h>
#include <QWidget>

// undefining min and max because CASCADE's defines them and
// it clashes with std::min(), std::max()
#undef min
#undef max

#include <stack>

class vtkGenericRenderWindowInteractor;
class vtkInteractorStyle;
class vtkCallbackCommand;
class vtkRenderWindow;
class vtkRenderer;
class vtkObject;

class SVTK_Selector;
class SVTK_Renderer;

#ifdef WIN32
#pragma warning ( disable:4251 )
#endif

/*!
  \class QVTK_RenderWindowInteractor
  Implements Qt based vtkRenderWindowInteractor.
  The class inherits #QWidget class in order to be possible process Qt events.
  It invokes corresponding VTK events through usage of its device - a #vtkGenericRenderWindowInteractor.
  Also, it creates, initialize and holds vtkRenderWindow instance.
*/
class SVTK_EXPORT QVTK_RenderWindowInteractor: public QWidget
{
  Q_OBJECT;

 public:
  QVTK_RenderWindowInteractor(QWidget* theParent, 
                              const char* theName);

  ~QVTK_RenderWindowInteractor();

  //! To initialize by #vtkGenericRenderWindowInteractor instance
  virtual
  void
  Initialize(vtkGenericRenderWindowInteractor* theDevice);

  vtkGenericRenderWindowInteractor* 
  GetDevice();

  vtkRenderWindow*
  getRenderWindow();

  //! Just to simplify usage of its device (#vtkGenericRenderWindowInteractor)
  virtual
  void
  InvokeEvent(unsigned long theEvent, void* theCallData);

  //! Get paint engine for the scene
  virtual QPaintEngine* paintEngine() const;

 public slots:
   //! Need for initial contents display on Win32
  virtual void show();

  //! To implement final initialization, just before the widget is displayed
  virtual void polish();

  //! To adjust widget and vtkRenderWindow size
  virtual void resize(int w, int h);

 protected:
  virtual void paintEvent( QPaintEvent* );
  virtual void resizeEvent( QResizeEvent* );

  virtual void mouseMoveEvent( QMouseEvent* );
  virtual void mousePressEvent( QMouseEvent* );
  virtual void mouseReleaseEvent( QMouseEvent* );
  virtual void mouseDoubleClickEvent( QMouseEvent* );
  virtual void wheelEvent( QWheelEvent* );
  virtual void keyPressEvent( QKeyEvent* );
  virtual void keyReleaseEvent( QKeyEvent* );
  virtual void enterEvent( QEvent * );
  virtual void leaveEvent( QEvent * );

  virtual void contextMenuEvent( QContextMenuEvent * e );

  // reimplemented from QWidget in order to set window - receiver
  // of space mouse events. 
  virtual void focusInEvent( QFocusEvent* );
  virtual void focusOutEvent( QFocusEvent* );

  //! To handle native events (from such devices as SpaceMouse)
// TODO (QT5 PORTING) Below is a temporary solution, to allow compiling with Qt 5
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#ifdef WIN32
  virtual bool winEvent( MSG*, long* );
#else
  virtual bool x11Event( XEvent *e );
#endif
#else
  virtual bool nativeEvent( const QByteArray&, void*, long* );
#endif
  vtkSmartPointer<vtkRenderWindow> myRenderWindow;
  vtkSmartPointer<vtkGenericRenderWindowInteractor> myDevice;
};


//! Extends QVTK_RenderWindowInteractor functionality.
/*!
  \class SVTK_RenderWindowInteractor
  Implements such features as 
  support of selection, 
  run-time interactor style management,
  one render window per one renderer collaboration and
  SUIT_ViewWindow events invocation.
*/
class SVTK_EXPORT SVTK_RenderWindowInteractor: public QVTK_RenderWindowInteractor
{
  Q_OBJECT;

 public:
  SVTK_RenderWindowInteractor(QWidget* theParent, 
                              const char* theName);

  ~SVTK_RenderWindowInteractor();

  //! To initialize properly the class
  virtual
  void
  Initialize(vtkGenericRenderWindowInteractor* theDevice,
             SVTK_Renderer* theRenderer,
             SVTK_Selector* theSelector);

  //----------------------------------------------------------------------------
  //! To get corresponding SVTK_Renderer instance
  SVTK_Renderer* 
  GetRenderer();

  //! To get corresponding SVTK_Renderer device (just to simplify collobaration with SVTK_Renderer)
  vtkRenderer* 
  getRenderer();

  //----------------------------------------------------------------------------
  //! To get current interactor style
  vtkInteractorStyle* 
  GetInteractorStyle();

  //! To change current interactor style by pushing the new one into the container
  void
  PushInteractorStyle(vtkInteractorStyle* theStyle);

  //! To restore previous interactor style
  void
  PopInteractorStyle();

  //----------------------------------------------------------------------------
  //! To get corresponding SVTK_Selector
  SVTK_Selector* 
  GetSelector();

  //! To get current selection mode (just to simplify collobaration with SVTK_Selector)
  Selection_Mode 
  SelectionMode() const;

  //! To change selection mode (just to simplify collobaration with SVTK_Selector)
  void 
  SetSelectionMode(Selection_Mode theMode);

 public:
  //! To transform vtkCommand::EndPickEvent to Qt selectionChanged signal
  void
  onEmitSelectionChanged();

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
  virtual void mouseMoveEvent( QMouseEvent* );
  virtual void mousePressEvent( QMouseEvent* );
  virtual void mouseReleaseEvent( QMouseEvent* );
  virtual void mouseDoubleClickEvent( QMouseEvent* );
  virtual void wheelEvent( QWheelEvent* );
  virtual void keyPressEvent( QKeyEvent* );
  virtual void keyReleaseEvent( QKeyEvent* );

  void
  SetRenderer(SVTK_Renderer *theRenderer);

  void
  SetSelector(SVTK_Selector* theSelector);

  void
  InitInteractorStyle(vtkInteractorStyle* theStyle);

  //----------------------------------------------------------------
  // Main process VTK event method
  static
  void
  ProcessEvents(vtkObject* theObject, 
                unsigned long theEvent,
                void* theClientData, 
                void* theCallData);

  // Used to process VTK events
  vtkSmartPointer<vtkCallbackCommand> myEventCallbackCommand;

  // Priority at which events are processed
  float myPriority;

  //----------------------------------------------------------------
  vtkSmartPointer<SVTK_Selector> mySelector;

  vtkSmartPointer<SVTK_Renderer> myRenderer;

  typedef vtkSmartPointer<vtkInteractorStyle> PInteractorStyle;
  typedef std::stack<PInteractorStyle> TInteractorStyles;
  TInteractorStyles myInteractorStyles;
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
