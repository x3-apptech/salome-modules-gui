#ifndef OCCVIEWER_VIEWPORT_H
#define OCCVIEWER_VIEWPORT_H

#include "OCCViewer.h"

#include "QtxAction.h"

#include <qlist.h>
#include <qcolor.h>
#include <qwidget.h>

#include <Aspect_Window.hxx>

class QRect;
class QCursor;
class QPainter;
class OCCViewer_ViewSketcher;
class OCCViewer_ViewTransformer;

class OCCVIEWER_EXPORT OCCViewer_ViewPort : public QWidget
{
	Q_OBJECT

  friend class OCCViewer_ViewSketcher;

public:
  OCCViewer_ViewPort( QWidget* parent );
	virtual ~OCCViewer_ViewPort();

public:
	void	                         setSketchingEnabled( bool );
  bool                           isSketchingEnabled() const;
  void	                         setTransformEnabled( bool );
  bool                           isTransformEnabled() const;

  virtual QColor	                 backgroundColor() const;
  virtual void                     setBackgroundColor( const QColor& );

  void	                         redrawPainters();

  virtual void                     onUpdate();

protected:
//	enum ViewType { Type2D, Type3D };
	void			                 selectVisualId();

// EVENTS
	virtual void                   paintEvent( QPaintEvent *);
	virtual void	                 mouseMoveEvent( QMouseEvent *);
	virtual void	                 mouseReleaseEvent( QMouseEvent *);
	virtual void	                 mousePressEvent( QMouseEvent *);
	virtual void	                 mouseDoubleClickEvent( QMouseEvent *);
  virtual void                     keyPressEvent( QKeyEvent *);
  virtual void                     keyReleaseEvent( QKeyEvent *);

// TO BE REDEFINED
  virtual void	                 reset() = 0;
  virtual void	                 pan( int, int ) = 0;
	virtual void	                 setCenter( int, int ) = 0;
	virtual void	                 fitRect( const QRect& ) = 0;
  virtual void	                 zoom( int, int, int, int ) = 0;
  virtual void	                 fitAll( bool keepScale = false, bool withZ = true ) = 0;

// POPUP
//  void                             onCreatePopup( QPopupMenu* );
//	void                             onDestroyPopup( QPopupMenu* );

protected slots:
  virtual void	                 onChangeBgColor();

signals:
  void			                 vpKeyEvent( QKeyEvent* );
  void			                 vpMouseEvent( QMouseEvent* );
	void			             vpDrawExternal( QPainter* );
  void                           vpChangeBGColor( QColor );

private:
	void	                         initialize();
	void	                         cleanup();

protected:
  Handle(Aspect_Window)         myWindow;
  bool			                    myEnableSketching;
  bool			                    myEnableTransform;
  bool			                    myPaintersRedrawing;	/* set to draw externally */
  QPtrList<QtxAction>           myPopupActions;

private:
	static int		                 nCounter;				/* objects counter */
};

#endif
