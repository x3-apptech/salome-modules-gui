#ifndef OCCVIEWER_VIEWPORT3D_H
#define OCCVIEWER_VIEWPORT3D_H

#include "OCCViewer_ViewPort.h"

#include <qcolor.h>

#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

class QRect;

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
  virtual void	        fitAll( bool keepScale = false, bool withZ = true );

	void	                startRotation( int, int );
	void	                rotate( int, int );
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

#endif
