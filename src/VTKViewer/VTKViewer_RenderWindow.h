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

#ifndef VTKVIEWER_RENDERWINDOW_H
#define VTKVIEWER_RENDERWINDOW_H

#include "VTKViewer.h"

#include <vtkRenderWindow.h>

#include <QWidget>

class VTKVIEWER_EXPORT VTKViewer_RenderWindow : public QWidget 
{
  Q_OBJECT 

public:
  /* popup management */
  //void                     onCreatePopup();

//  const char *GetClassName() {return "VTKViewer_RenderWindow";};

public:
  VTKViewer_RenderWindow(QWidget *parent, const char *name);
  virtual ~VTKViewer_RenderWindow() ;
  
  /*!Get render window pointer.*/
  vtkRenderWindow* getRenderWindow() { return myRW; }

  virtual QPaintEngine* paintEngine() const;

 protected:
  virtual void mouseMoveEvent( QMouseEvent* );
  virtual void mousePressEvent( QMouseEvent* );
  virtual void mouseReleaseEvent( QMouseEvent* );
  virtual void mouseDoubleClickEvent( QMouseEvent* );
  virtual void wheelEvent( QWheelEvent* );
  virtual void keyPressEvent( QKeyEvent* );
  virtual void keyReleaseEvent( QKeyEvent* );
  virtual void paintEvent( QPaintEvent* );
  virtual void resizeEvent( QResizeEvent* );
  virtual void onChangeBackgroundColor();
  virtual void contextMenuEvent( QContextMenuEvent * e );

 signals:
  /*!On mouse move signal.*/
  void MouseMove( QMouseEvent* );
  /*!On mouse button pressed signal.*/
  void MouseButtonPressed( QMouseEvent* );
  /*!On mouse button released signal.*/
  void MouseButtonReleased( QMouseEvent* );
  /*!On mouse double click signal.*/
  void MouseDoubleClicked( QMouseEvent* );
  /*!On wheel moved signal.*/
  void WheelMoved( QWheelEvent* );
  /*!On left button pressed signal.*/
  void LeftButtonPressed(const QMouseEvent *event) ;
  /*!On left button released signal.*/
  void LeftButtonReleased(const QMouseEvent *event) ;
  /*!On middle button pressed signal.*/
  void MiddleButtonPressed(const QMouseEvent *event) ;
  /*!On middle button released signal.*/
  void MiddleButtonReleased(const QMouseEvent *event) ;
  /*!On right button pressed signal.*/
  void RightButtonPressed(const QMouseEvent *event) ;
  /*!On right button released signal.*/
  void RightButtonReleased(const QMouseEvent *event) ;
  
  /*!On button pressed signal.*/
  void ButtonPressed(const QMouseEvent *event);
  /*!On button released signal.*/
  void ButtonReleased(const QMouseEvent *event);
  /*!On key pressed signal.*/
  void KeyPressed( QKeyEvent* );
  /*!On key released signal.*/
  void KeyReleased( QKeyEvent* );
  /*!On content menu requested signal.*/
  void contextMenuRequested( QContextMenuEvent *e );

 protected:
  vtkRenderWindow* myRW;
};

#endif
