#ifndef VTKVIEWER_RENDERWINDOW_H
#define VTKVIEWER_RENDERWINDOW_H

#include "VTKViewer.h"

#include <vtkRenderWindow.h>

#include <qwidget.h>

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

  vtkRenderWindow* getRenderWindow() { return myRW; }

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
  void MouseMove( QMouseEvent* );
  void MouseButtonPressed( QMouseEvent* );
  void MouseButtonReleased( QMouseEvent* );
  void MouseDoubleClicked( QMouseEvent* );
  void WheelMoved( QWheelEvent* );
  void LeftButtonPressed(const QMouseEvent *event) ;
  void LeftButtonReleased(const QMouseEvent *event) ;
  void MiddleButtonPressed(const QMouseEvent *event) ;
  void MiddleButtonReleased(const QMouseEvent *event) ;
  void RightButtonPressed(const QMouseEvent *event) ;
  void RightButtonReleased(const QMouseEvent *event) ;
  void ButtonPressed(const QMouseEvent *event);
  void ButtonReleased(const QMouseEvent *event);
  void KeyPressed( QKeyEvent* );
  void KeyReleased( QKeyEvent* );
  void contextMenuRequested( QContextMenuEvent *e );

 protected:
  vtkRenderWindow* myRW;
};

#endif
