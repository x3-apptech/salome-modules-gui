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

#ifndef OCCVIEWER_VIEWMODEL_H
#define OCCVIEWER_VIEWMODEL_H

#include <QColor>
#include <QPoint>
#include <QVector>

#include "OCCViewer.h"
#include "OCCViewer_ClipPlane.h"

#include "Qtx.h"
#include "SUIT_ViewModel.h"

#include <V3d_View.hxx>
#include <AIS_Trihedron.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <Graphic3d_SequenceOfHClipPlane.hxx>

class QKeyEvent;
class QMouseEvent;

class SUIT_ViewWindow;
class SUIT_Desktop;
class OCCViewer_ClippingDlg;
class OCCViewer_ViewWindow;
class OCCViewer_ViewPort3d;

class QtxAction;


#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class OCCVIEWER_EXPORT OCCViewer_Viewer: public SUIT_ViewModel
{
  Q_OBJECT

public:
  /*! supported gradient types */
  enum { 
    HorizontalGradient, VerticalGradient,
    Diagonal1Gradient,  Diagonal2Gradient,
    Corner1Gradient,    Corner2Gradient,
    Corner3Gradient,    Corner4Gradient,
    LastGradient = Corner4Gradient
  };
  /*! supported stereo types */
  enum {
    QuadBufferType,    AnaglyphType,
    RowInterlacedType, ColumnInterlacedType,
    ChessBoardType,    SideBySideType,
    OverUnderType
  };
  
  static QString           Type() { return "OCCViewer"; }
  static QString           backgroundData( QStringList&, QIntList&, QIntList& );
  static void              stereoData( QStringList&, QIntList&);

  OCCViewer_Viewer( bool DisplayTrihedron = true);
  virtual ~OCCViewer_Viewer();

  void update();

  virtual SUIT_ViewWindow*        createView(SUIT_Desktop* theDesktop);
  
  virtual void                    setViewManager(SUIT_ViewManager* theViewManager);
  virtual QString                 getType() const { return Type(); }

  virtual void                    contextMenuPopup(QMenu*);
  
  void                            getSelectedObjects(AIS_ListOfInteractive& theList);
  void                            setObjectsSelected(const AIS_ListOfInteractive& theList);
  void                            setSelected(const Handle(AIS_InteractiveObject)& theIO) { myAISContext->SetSelected(theIO);}

  void                            performSelectionChanged();
  // emit signal selectionChanged

  void                            setDefaultLights();

  QColor                          backgroundColor() const;                              // obsolete
  void                            setBackgroundColor( const QColor& );                  // obsolete
  Qtx::BackgroundData             background() const;
  void                            setBackground( const Qtx::BackgroundData& );

  QColor                          backgroundColor(int theViewId) const;                 // obsolete
  void                            setBackgroundColor( int theViewId, const QColor& );   // obsolete
  Qtx::BackgroundData             background(int theViewId) const;
  void                            setBackground( int theViewId, const Qtx::BackgroundData& );

  //! returns true if 3d Trihedron in viewer was created
  bool                            trihedronActivated() const { return !myTrihedron.IsNull(); }

  void                            toggleTrihedron();
  bool                            isTrihedronVisible() const;
  virtual void                    setTrihedronShown( const bool );

  double                          trihedronSize() const;
  virtual void                    setTrihedronSize( const double , bool isRelative = true);
  
  bool                            trihedronRelative() const {return myIsRelative; }

  // a utility functions, used by SALOME_View_s methods
  bool                            computeTrihedronSize( double& theNewSize, double& theSize );
  virtual double                  computeSceneSize(const Handle(V3d_View)&) const;

  void                            updateTrihedron();
  

  virtual OCCViewer_ViewWindow*   createSubWindow();

public:
  Handle(V3d_Viewer)              getViewer3d()    const { return myV3dViewer;}
  Handle(AIS_InteractiveContext)  getAISContext()  const { return myAISContext; }
  Handle(AIS_Trihedron)           getTrihedron()   const { return myTrihedron; }

  int                             getTopLayerId();

  int                             interactionStyle() const;
  void                            setInteractionStyle( const int );

  int                             projectionType() const;
  void                            setProjectionType( const int );
  int                             stereoType() const;
  void                            setStereoType( const int );

  int                             anaglyphFilter() const;
  void                            setAnaglyphFilter( const int );

  void                            setStereographicFocus( const int, const double );
  int                             stereographicFocusType() const;
  double                          stereographicFocusValue() const;

  void                            setInterocularDistance( const int, const double );
  int                             interocularDistanceType() const;
  double                          interocularDistanceValue() const;

  bool                            isReverseStereo() const;
  void                            setReverseStereo( const bool );

  bool                            isVSync() const;
  void                            setVSync( const bool );

  bool                            isQuadBufferSupport() const;
  void                            setQuadBufferSupport( const bool );
  int                             zoomingStyle() const;
  void                            setZoomingStyle( const int );

  void                            enablePreselection(bool isEnabled);
  bool                            isPreselectionEnabled() const;
  
  void                            enableSelection(bool isEnabled);
  bool                            isSelectionEnabled() const;

  void                            setSelectionOptions( bool, bool );

  void                            enableMultiselection(bool isEnable);
  bool                            isMultiSelectionEnabled() const { return myMultiSelectionEnabled; }

  bool                            enableDrawMode( bool on );

  void                            setClippingColor( const QColor& );
  QColor                          clippingColor() const;

  void                            setClippingTextureParams( const bool, const QString&, const bool, const double );
  bool                            isDefaultTextureUsed() const;
  QString                         clippingTexture() const;
  bool                            isTextureModulated() const;
  double                          clippingTextureScale() const;

  int                             getSelectionCount() const { return (!myAISContext.IsNull())? myAISContext->NbSelected():0; }

  void                            setStaticTrihedronDisplayed(const bool on);

  /* Clip planes management */
  Handle(Graphic3d_ClipPlane)     createClipPlane(const gp_Pln& thePlane, const Standard_Boolean theIsOn);
  void                            setClipPlanes (ClipPlanesList theList);
  ClipPlanesList                  getClipPlanes() const;
  void                            applyExistingClipPlanesToObject (const Handle(AIS_InteractiveObject)& theObject);

  OCCViewer_ClippingDlg*          getClippingDlg() const;
  void                            setClippingDlg(OCCViewer_ClippingDlg* theDlg);
  

  /* Selection management */
  bool    highlight( const Handle(AIS_InteractiveObject)&, bool, bool=true );
  bool    unHighlightAll( bool=true, bool=true ); 
  bool    isInViewer( const Handle(AIS_InteractiveObject)&, bool=false );
  bool    isVisible( const Handle(AIS_InteractiveObject)& );

  void    setColor( const Handle(AIS_InteractiveObject)&, const QColor&, bool=true );
  void    switchRepresentation( const Handle(AIS_InteractiveObject)&, int, bool=true );
  void    setTransparency( const Handle(AIS_InteractiveObject)&, float, bool=true );
  void    setIsos( const int u, const int v ); // number of isolines
  void    isos( int& u, int& v ) const;

  void    initView( OCCViewer_ViewWindow* view );

signals:
  void selectionChanged();
  void deselection();

protected slots:
  virtual void onMousePress(SUIT_ViewWindow*, QMouseEvent*);
  virtual void onMouseMove(SUIT_ViewWindow*, QMouseEvent*);
  virtual void onMouseRelease(SUIT_ViewWindow*, QMouseEvent*);
  virtual void onKeyPress(SUIT_ViewWindow*, QKeyEvent*);
  virtual void onViewClosed(OCCViewer_ViewPort3d*);
  virtual void onViewMapped(OCCViewer_ViewPort3d*);

  void onDumpView();
  void onChangeBackground();

protected:

  Handle(V3d_Viewer)              myV3dViewer;

  Handle(AIS_Trihedron)           myTrihedron;
  Handle(AIS_InteractiveContext)  myAISContext;

  int                             myInteractionStyle;
  int                             myZoomingStyle;
  int                             myProjectionType;
  int                             myStereoType;
  int                             myAnaglyphFilter;
  int                             myStereographicFocusType;
  int                             myInterocularDistanceType;

  double                          myStereographicFocusValue;
  double                          myInterocularDistanceValue;

  bool                            myPreselectionEnabled;
  bool                            mySelectionEnabled;
  bool                            myMultiSelectionEnabled;
  bool                            myIsRelative;
  bool                            myToReverseStereo;
  bool                            myVSyncMode;
  bool                            myQuadBufferSupport;

  int                             myTopLayerId;

  //QColor                          myBgColor;
  QPoint                          myStartPnt, myEndPnt, myCurPnt;

  double                          myTrihedronSize;

  QVector<Qtx::BackgroundData>    myBackgrounds;

  OCCViewer_ClippingDlg*          myClippingDlg;

  ClipPlanesList                  myClipPlanes;
  Graphic3d_SequenceOfHClipPlane  myInternalClipPlanes;

  QColor                          myClippingColor;
  bool                            myDefaultTextureUsed;
  QString                         myClippingTexture;
  bool                            myTextureModulated;
  double                          myClippingTextureScale;

};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
