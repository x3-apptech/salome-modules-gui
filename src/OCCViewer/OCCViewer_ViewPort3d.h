// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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

#include "OCCViewer_ViewPort.h"

#include <qcolor.h>

#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

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
	Handle(V3d_View)        getView() const;
  Handle(V3d_View)		    setView( const Handle(V3d_View)& );
	Handle(V3d_Viewer)      getViewer() const;

  void setAnimationMode(bool theDegenerated);

	virtual void			setBackgroundColor( const QColor& color);
  virtual QColor	backgroundColor() const;

//   void         setActive( V3d_TypeOfView );
  virtual bool syncronize( const OCCViewer_ViewPort3d* );

  double getZSize() const;
  void   setZSize( double );

  virtual void onUpdate();

  // TRANSFORMATIONS
  virtual void	        reset();
  virtual void	        pan( int , int );
	virtual void	        setCenter( int , int );
	virtual void	        fitRect( const QRect& );
	virtual void	        zoom( int, int, int, int );
  virtual void	        fitAll( bool keepScale = false, bool withZ = true, bool upd = true );

	void	                startRotation( int, int, int, const gp_Pnt& );
	void	                rotate( int, int, int, const gp_Pnt& );
	void	                endRotation();

protected:
    // EVENTS
	virtual void			paintEvent( QPaintEvent* );
  virtual void      resizeEvent( QResizeEvent* );

private:
	Handle(V3d_View) activeView() const;
	Handle(V3d_View) inactiveView() const;
	bool             mapView( const Handle(V3d_View)& );
	bool	           setWindow( const Handle(V3d_View)& );
	bool					   mapped( const Handle(V3d_View)& ) const;

private:
  Handle(V3d_View)		myOrthoView;
  Handle(V3d_View)		myPerspView;
  Handle(V3d_View)		myActiveView;
	bool		myDegenerated;
  bool    myAnimate;
	double	myScale;
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
