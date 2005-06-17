#ifndef OCCVIEWER_VIEWMODEL_H
#define OCCVIEWER_VIEWMODEL_H

#include <qcursor.h>

#include "OCCViewer.h"

#include "SUIT_ViewModel.h"

#include <V3d_View.hxx>
#include <AIS_Trihedron.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_InteractiveContext.hxx>

class SUIT_ViewWindow;
class SUIT_Desktop;

struct viewAspect
{
public:
	double	  scale;
	double	centerX;
	double  centerY;
	double    projX;
	double    projY;
	double    projZ;
	double    twist;
	double      atX;
	double      atY;
	double      atZ;
	double     eyeX;
	double     eyeY;
	double     eyeZ;
	QString    name;
};

typedef QValueList<viewAspect> viewAspectList;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class OCCVIEWER_EXPORT OCCViewer_Viewer: public SUIT_ViewModel
{
  Q_OBJECT

public:
  static QString Type() { return "OCCViewer"; }

  OCCViewer_Viewer( bool DisplayTrihedron = true );
	virtual ~OCCViewer_Viewer();

  void update();

	virtual SUIT_ViewWindow* createView(SUIT_Desktop* theDesktop);

	virtual void                    setViewManager(SUIT_ViewManager* theViewManager);
  virtual QString                 getType() const { return Type(); }

  virtual void                    contextMenuPopup(QPopupMenu*);
  
  void                            getSelectedObjects(AIS_ListOfInteractive& theList);
  void                            setObjectsSelected(const AIS_ListOfInteractive& theList);
  void                            setSelected(const Handle(AIS_InteractiveObject)& theIO)
  { myAISContext->SetSelected(theIO);}

  void                            performSelectionChanged();
  // emit signal selectionChanged

  virtual const viewAspectList&   getViewAspects();
  virtual void                    appendViewAspect( const viewAspect& );
  virtual void                    updateViewAspects( const viewAspectList& );
  virtual void                    clearViewAspects();

  //! returns true if 3d Trihedron in viewer was created
  bool trihedronActivated() const { return !myTrihedron.IsNull(); }
  virtual void toggleTrihedron();

public slots:
	void onClearViewAspects();

public:
  Handle(V3d_Viewer)              getViewer3d()    const { return myV3dViewer;}
  Handle(V3d_Viewer)              getCollector3d() const { return myV3dCollector; }
  Handle(AIS_InteractiveContext)  getAISContext()  const { return myAISContext; }
  Handle(AIS_Trihedron)           getTrihedron()   const { return myTrihedron; }

  void                            enableSelection(bool isEnabled);
  bool                            isSelectionEnabled() const 
  { return mySelectionEnabled; }

  void                            enableMultiselection(bool isEnable);
  bool                            isMultiSelectionEnabled() const 
  { return myMultiSelectionEnabled; }

  int                             getSelectionCount() const 
  { return (!myAISContext.IsNull())? myAISContext->NbSelected():0; }

  /* Selection management */
  bool		highlight( const Handle(AIS_InteractiveObject)&, bool, bool=true );
  bool		unHighlightAll( bool=true ); 
  bool    isInViewer( const Handle(AIS_InteractiveObject)&, bool=false );
  bool    isVisible( const Handle(AIS_InteractiveObject)& );

  void    setColor( const Handle(AIS_InteractiveObject)&, const QColor&, bool=true );
  void    switchRepresentation( const Handle(AIS_InteractiveObject)&, int, bool=true );
  void    setTransparency( const Handle(AIS_InteractiveObject)&, float, bool=true );

signals:
  void selectionChanged();

protected slots:
  void onMousePress(SUIT_ViewWindow*, QMouseEvent*);
  void onMouseMove(SUIT_ViewWindow*, QMouseEvent*);
  void onMouseRelease(SUIT_ViewWindow*, QMouseEvent*);

  void onDumpView();
  void onShowToolbar();
  void onChangeBgColor();

private:
  Handle(V3d_Viewer)              myV3dViewer;
  Handle(V3d_Viewer)              myV3dCollector;
  Handle(AIS_InteractiveContext)  myAISContext;
  Handle(AIS_Trihedron)           myTrihedron;

  viewAspectList                  myViewAspects;

  bool mySelectionEnabled;
  bool myMultiSelectionEnabled;

  QPoint myStartPnt, myEndPnt;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
