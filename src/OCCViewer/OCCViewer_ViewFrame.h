// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#ifndef OCCVIEWER_VIEWFRAME_H
#define OCCVIEWER_VIEWFRAME_H

#include "OCCViewer.h"
#include "OCCViewer_ViewWindow.h"

#include <gp_Pln.hxx>

class SUIT_Desktop;
class OCCViewer_Viewer;
class QGridLayout;

class OCCVIEWER_EXPORT OCCViewer_ViewFrame : public OCCViewer_ViewWindow  
{
  Q_OBJECT
public:
  enum { MAIN_VIEW, BOTTOM_RIGHT=MAIN_VIEW, BOTTOM_LEFT, TOP_LEFT, TOP_RIGHT };

  OCCViewer_ViewFrame(SUIT_Desktop* theDesktop, OCCViewer_Viewer* theModel);
  virtual ~OCCViewer_ViewFrame();

  bool event( QEvent* e );

  OCCViewer_ViewWindow*   getView( const int ) const;

  virtual OCCViewer_ViewPort3d* getViewPort() { return getView(MAIN_VIEW)->getViewPort(); }
  OCCViewer_ViewPort3d* getViewPort(int theView);

  virtual void performRestoring( const viewAspect& theAspect) { getView(MAIN_VIEW)->performRestoring(theAspect); }
  virtual void initLayout() {}

  virtual void updateEnabledDrawMode();
  virtual void setCuttingPlane( bool on, const double x = 0 , const double y = 0 , const double z = 0,
                                const double dx = 0, const double dy = 0, const double dz = 1);

  virtual void setCuttingPlane( bool on, const gp_Pln thePln );

  virtual bool isCuttingPlane() { return getView(MAIN_VIEW)->isCuttingPlane(); }

  virtual QString getVisualParameters();
  virtual void setVisualParameters( const QString& parameters );

  virtual void initSketchers() { getView(MAIN_VIEW)->initSketchers(); }
  virtual OCCViewer_ViewSketcher* getSketcher( const int i)  { return getView(MAIN_VIEW)->getSketcher(i); }
  virtual void activateSketching( int i ) { getView(MAIN_VIEW)->activateSketching(i); }

  virtual int  interactionStyle() const { return getView(MAIN_VIEW)->interactionStyle(); }
  virtual void setInteractionStyle( const int i );

  virtual int  zoomingStyle() const { return getView(MAIN_VIEW)->zoomingStyle(); }
  virtual void setZoomingStyle( const int );

  virtual void setViewManager( SUIT_ViewManager* );

  virtual bool eventFilter(QObject* watched, QEvent* e) { return SUIT_ViewWindow::eventFilter(watched, e); }

  virtual QColor backgroundColor() const;                 // obsolete
  virtual void   setBackgroundColor( const QColor& );     // obsolete

  virtual Qtx::BackgroundData background() const;
  virtual void                setBackground( const Qtx::BackgroundData& );

  virtual void   setDropDownButtons( bool );

  virtual SUIT_CameraProperties cameraProperties();

public slots:
  virtual void onFrontView() { getView(MAIN_VIEW)->onFrontView(); }
  virtual void onViewFitAll();
  virtual void onBackView() { getView(MAIN_VIEW)->onBackView(); }
  virtual void onTopView() { getView(MAIN_VIEW)->onTopView(); }
  virtual void onBottomView() { getView(MAIN_VIEW)->onBottomView(); }
  virtual void onLeftView() { getView(MAIN_VIEW)->onLeftView(); }
  virtual void onRightView() { getView(MAIN_VIEW)->onRightView(); }
  virtual void onClockWiseView() { getView(MAIN_VIEW)->onClockWiseView(); }
  virtual void onAntiClockWiseView() { getView(MAIN_VIEW)->onAntiClockWiseView(); }
  virtual void onResetView() { getView(MAIN_VIEW)->onResetView(); }
  virtual void onFitAll();
  virtual void activateZoom() {}
  virtual void activateWindowFit() {}
  virtual void activateRotation() {}
  virtual void activatePanning() {}
  virtual void activateGlobalPanning() {}
  virtual void onSetRotationPoint( bool on ) {}
  virtual void onAxialScale() {}
  virtual void onAmbientToogle() {}
  virtual void onMemorizeView() {}
  virtual void onRestoreView() {}
  virtual void onSwitchInteractionStyle( bool on ) {}

  virtual void activateSetRotationGravity() {}
  virtual void activateSetRotationSelected( double theX, double theY, double theZ ) {}
  virtual void activateStartPointSelection() {}
  virtual void updateGravityCoords() {}
   
  void onMaximizedView( OCCViewer_ViewWindow*, bool );

  virtual void onDumpView();

protected:
  virtual QImage dumpView();
  virtual bool   dumpViewToFormat( const QImage&, const QString&, const QString& );

private slots:
  void onContextMenuRequested(QContextMenuEvent*);

private:
  void connectViewSignals( OCCViewer_ViewWindow* theView );
  void updateWindowTitle( OCCViewer_ViewWindow* theView );

  QList<OCCViewer_ViewWindow*> myViews;
  QGridLayout* myLayout;

  OCCViewer_ViewWindow* myPopupRequestedView;
};

#endif
