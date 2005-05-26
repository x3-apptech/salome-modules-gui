#include "VTKViewer_RenderWindow.h"

#include <qcolordialog.h>
#include <qpopupmenu.h>

#include <stdlib.h>
#include <math.h>

#include <vtkRenderWindowInteractor.h>
#include <vtkRendererCollection.h>
#include <vtkCamera.h>
#ifndef WNT
#include <vtkXOpenGLRenderWindow.h>
//#include <GL/gl.h>
//#include <GL/glu.h>
//#include <qgl.h>
#endif

#if QT_VERSION > 300
#include <qcursor.h>
#endif

//****************************************************************
VTKViewer_RenderWindow::VTKViewer_RenderWindow(QWidget* parent, const char* name) :
QWidget(parent, name, 
        Qt::WStyle_NoBorder | Qt::WDestructiveClose | 
        Qt::WResizeNoErase | Qt::WRepaintNoErase)
{
  myRW = vtkRenderWindow::New();
#ifndef WNT
  myRW->SetDisplayId((void*)x11Display());
#endif
  myRW->SetWindowId((void*)winId());
  myRW->DoubleBufferOn();
  setMouseTracking(true);
}

//****************************************************************
VTKViewer_RenderWindow::~VTKViewer_RenderWindow() 
{
  myRW->Delete();
}

//****************************************************************
void VTKViewer_RenderWindow::paintEvent(QPaintEvent* theEvent) 
{
  myRW->Render();
}

//****************************************************************
void VTKViewer_RenderWindow::resizeEvent(QResizeEvent* theEvent) 
{
  int aWidth = myRW->GetSize()[0], aHeight = myRW->GetSize()[1];
  if(vtkRenderWindowInteractor* aRWI = myRW->GetInteractor())
    aRWI->UpdateSize(width(), height());
  if(aWidth != width() || aHeight != height()){
    vtkRendererCollection * aRenderers = myRW->GetRenderers();
    aRenderers->InitTraversal();
    double aCoeff = 1.0;
    if(vtkRenderer *aRenderer = aRenderers->GetNextItem()){
      vtkCamera *aCamera = aRenderer->GetActiveCamera();
      double aScale = aCamera->GetParallelScale();
      if((aWidth - width())*(aHeight - height()) > 0)
        aCoeff = sqrt(double(aWidth)/double(width())*double(height())/double(aHeight));
      else
        aCoeff = double(aWidth)/double(width());
      aCamera->SetParallelScale(aScale*aCoeff);
    }
  }
}

//****************************************************************
void VTKViewer_RenderWindow::mouseMoveEvent(QMouseEvent* event) 
{
  emit MouseMove(event) ;
}

//****************************************************************
void VTKViewer_RenderWindow::mousePressEvent(QMouseEvent* event) 
{
  emit MouseButtonPressed( event );
}

//****************************************************************
void VTKViewer_RenderWindow::mouseReleaseEvent( QMouseEvent *event )
{
  emit MouseButtonReleased( event );
}

//****************************************************************
void VTKViewer_RenderWindow::mouseDoubleClickEvent( QMouseEvent* event )
{
  emit MouseDoubleClicked( event );
}

//****************************************************************
void VTKViewer_RenderWindow::keyPressEvent (QKeyEvent* event) 
{
  emit KeyPressed(event) ;
}

//****************************************************************
void VTKViewer_RenderWindow::keyReleaseEvent (QKeyEvent * event) 
{
  emit KeyReleased(event) ;
}

//****************************************************************
void VTKViewer_RenderWindow::wheelEvent(QWheelEvent* event)
{
  emit WheelMoved(event) ;
}

//****************************************************************
void VTKViewer_RenderWindow::onChangeBackgroundColor()
{
  //float red, green, blue;
  float backint[3];

  vtkRendererCollection * theRenderers = myRW->GetRenderers();
  theRenderers->InitTraversal();
  vtkRenderer * theRenderer = theRenderers->GetNextItem();
  theRenderer->GetBackground(backint);

  QColor selColor = QColorDialog::getColor ( QColor(int(backint[0]*255), int(backint[1]*255), int(backint[2]*255)), NULL );	
  if ( selColor.isValid() ) {
    theRenderer->SetBackground( selColor.red()/255., selColor.green()/255., selColor.blue()/255. ); 
    /* VSR : PAL5420 ---------------------------------------------------
    SUIT_CONFIG->addSetting( "VTKViewer:BackgroundColorRed",   selColor.red() );
    SUIT_CONFIG->addSetting( "VTKViewer:BackgroundColorGreen", selColor.green() );
    SUIT_CONFIG->addSetting( "VTKViewer:BackgroundColorBlue",  selColor.blue() );
    VSR : PAL5420 --------------------------------------------------- */
  }
}

//****************************************************************
void VTKViewer_RenderWindow::contextMenuEvent ( QContextMenuEvent * e )
{
  if ( e->reason() != QContextMenuEvent::Mouse )
    emit contextMenuRequested( e );
}
