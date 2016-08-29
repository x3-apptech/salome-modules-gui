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

#ifndef OCCVIEWER_VIEWPORT3D_H
#define OCCVIEWER_VIEWPORT3D_H

#include <Basics_OCCTVersion.hxx>

#include "OCCViewer_ViewPort.h"
#include "Qtx.h"

#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

class QColor;
class QString;
class QRect;

#ifdef WIN32
#pragma warning ( disable:4251 )
#endif

class OCCVIEWER_EXPORT OCCViewer_ViewPort3d: public OCCViewer_ViewPort
{
  Q_OBJECT

  friend class OCCViewer_ViewTransformer;

public:
  OCCViewer_ViewPort3d( QWidget*, const Handle(V3d_Viewer)&, V3d_TypeOfView = V3d_ORTHOGRAPHIC );
  virtual ~OCCViewer_ViewPort3d();

public:
  Handle(V3d_View)      getView() const;
  Handle(V3d_View)      setView( const Handle(V3d_View)& );
  Handle(V3d_Viewer)    getViewer() const;

  virtual void          setBackgroundColor( const QColor& color);    // obsolete
  virtual QColor        backgroundColor() const;                     // obsolete
  void                  setBackground( const Qtx::BackgroundData& color);
  Qtx::BackgroundData   background() const;

  virtual int           getBgImgHeight(){return myBgImgHeight; };
  virtual int           getBgImgWidth() {return myBgImgWidth;  };

//   void         setActive( V3d_TypeOfView );
  virtual bool          syncronize( const OCCViewer_ViewPort3d* );

  double                getZSize() const;
  void                  setZSize( double );

  void                  getAxialScale( double&, double&, double& );

  virtual void          onUpdate();

  // TRANSFORMATIONS
  virtual void          reset();
  virtual void          pan( int , int );
  virtual void          setCenter( int , int );
  virtual void          fitRect( const QRect& );
  virtual void          startZoomAtPoint( int, int );
  virtual void          zoom( int, int, int, int );
  virtual void          fitAll( bool keepScale = false, bool withZ = true, bool upd = true );
  virtual void          rotateXY( double );
  virtual void          setAxialScale( double, double, double );

  virtual void          startRotation( int, int, int, const gp_Pnt& );
  virtual void          rotate( int, int, int, const gp_Pnt& );
  virtual void          endRotation();
  bool                  isBusy() {return myBusy;} // check that View Port is fully initialized

  void                  setAdvancedZoomingEnabled( const bool theState ) { myIsAdvancedZoomingEnabled = theState; }
  bool                  isAdvancedZoomingEnabled() const { return myIsAdvancedZoomingEnabled; }

  void                  showStaticTrihedron( bool );

  void                  setDefaultCursor( Qt::CursorShape theCursorShape );
  QCursor*              getDefaultCursor() const;

signals:
  void                  vpChangeBackground( const Qtx::BackgroundData& );
  void                  vpClosed(OCCViewer_ViewPort3d*);
  void                  vpMapped(OCCViewer_ViewPort3d*);

public slots:
  virtual bool          synchronize( OCCViewer_ViewPort* );

private slots:
  void                  repaintViewAfterMove();

protected:
    // EVENTS
  virtual void          paintEvent( QPaintEvent* );
  virtual void          resizeEvent( QResizeEvent* );

  // initialization
  virtual void          attachWindow( const Handle(V3d_View)&, const Handle(Aspect_Window)& );

private:
  Handle(V3d_View)      activeView() const;
  Handle(V3d_View)      inactiveView() const;
  bool                  mapView( const Handle(V3d_View)& );
  bool                  setWindow( const Handle(V3d_View)& );
  bool                  mapped( const Handle(V3d_View)& ) const;
  void                  updateBackground();
  void                  setDefaultParams();
  
private:
  Handle(V3d_View)      myActiveView;
#if OCC_VERSION_LARGE <= 0x06070100
  Handle(V3d_View)      myOrthoView;
  Handle(V3d_View)      myPerspView;
#endif
  bool                  myBusy;
  double                myScale;
  bool                  myIsAdvancedZoomingEnabled;
  Qtx::BackgroundData   myBackground;
  int                   myBgImgHeight;
  int                   myBgImgWidth;
  QCursor*              myCursor;
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
