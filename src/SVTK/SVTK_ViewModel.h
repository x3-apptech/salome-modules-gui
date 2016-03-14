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

#ifndef SVTK_VIEWMODEL_H
#define SVTK_VIEWMODEL_H

#include "SVTK.h"
#include "SVTK_ViewModelBase.h"
#include "SVTK_Selection.h"

#include "Qtx.h"

#include <SALOME_Prs.h>
#include <SALOME_InteractiveObject.hxx>
#include <SALOME_ListIO.hxx>

#include <QColor>
#include <QMap>

class QMouseEvent;

class SVTK_ViewWindow;
class VTKViewer_Actor;

//! Extends two interfaces #SVTK_ViewModelBase and #SALOME_View 
class SVTK_EXPORT SVTK_Viewer : public SVTK_ViewModelBase, public SALOME_View 
{
  Q_OBJECT;

public:
  /*! supported gradient types */
  enum { 
    HorizontalGradient,
    VerticalGradient,
    FirstDiagonalGradient,
    SecondDiagonalGradient,
    FirstCornerGradient,
    SecondCornerGradient,
    ThirdCornerGradient,
    FourthCornerGradient,
    LastGradient = FourthCornerGradient,
  };

  enum {
    CrystalEyesType, RedBlueType,
    InterlacedType,  LeftType,
    RightType,       DresdenType,
    AnaglyphType,    CheckerboardType,
    SplitViewPortHorizontalType
  };
  typedef SVTK_ViewWindow TViewWindow;
  
  //! Define string representation of the viewer type
  static QString           Type() { return "VTKViewer"; }
  static QString           backgroundData( QStringList&, QIntList&, QIntList& );
  static void              stereoData( QStringList&, QIntList&);

  SVTK_Viewer();
  virtual ~SVTK_Viewer();

  //! See #SUIT_ViewModel::createView
  virtual SUIT_ViewWindow* createView(SUIT_Desktop*);

  //! See #SUIT_ViewModel::createView
  virtual void setViewManager(SUIT_ViewManager* theViewManager);

  //! See #SUIT_ViewModel::contextMenuPopup
  virtual void contextMenuPopup( QMenu* );

  //! See #SUIT_ViewModel::getType
  virtual QString getType() const { return Type(); }

  //! Get background color of the viewer [obsolete]
  QColor backgroundColor() const;

  //! Set background color to the viewer [obsolete]
  void setBackgroundColor( const QColor& );

  //! Get background color of the viewer
  Qtx::BackgroundData background() const;

  //! Set background color to the viewer
  void setBackground( const Qtx::BackgroundData& );

  //! Get size of trihedron of the viewer (see #SVTK_Renderer::SetTrihedronSize)
  double trihedronSize() const;

  //! Shows if the size of trihedron relative (see #SVTK_Renderer::SetTrihedronSize)
  bool trihedronRelative() const;

  //! Set size of trihedron of the viewer (see #SVTK_Renderer::SetTrihedronSize)
  void setTrihedronSize( const double, const bool = true );

  //! Get visibility status of the static trihedron
  bool isStaticTrihedronVisible() const;

  //! Set visibility status of the static trihedron
  void setStaticTrihedronVisible( const bool );

  //! Gets projection mode
  int projectionMode() const;

  //! Sets projection mode
  void setProjectionMode( const int );

  //! Gets stereo type
  int stereoType() const;

  //! Sets stereo type
  void setStereoType( const int );

  //! Gets anaglyph filter
  int anaglyphFilter() const;

  //! Sets anaglyph filter
  void setAnaglyphFilter( const int );

  //! Get support quad-buffered stereo
  bool isQuadBufferSupport() const;

  //! Set support quad-buffered stereo
  void setQuadBufferSupport( const bool );

  //! Gets interaction style
  int interactionStyle() const;

  //! Sets interaction style
  void setInteractionStyle( const int );

  //! Gets zooming style
  int zoomingStyle() const;

  //! Sets zooming style
  void setZoomingStyle( const int );

  //! Gets current preselection mode (standard, dynamic or disabled)
  Preselection_Mode preSelectionMode() const;

  //! Sets new preselection mode
  void setPreSelectionMode( Preselection_Mode );
 
  //! Get incremental speed (see #SVTK_InteractorStyle::ControllerIncrement)
  int incrementalSpeed() const;

  //! Returns modification mode of incremental speed (see #SVTK_InteractorStyle::ControllerIncrement)
  int incrementalSpeedMode() const;

  //! Set the incremental speed for view operation (see #SVTK_InteractorStyle::ControllerIncrement)
  void setIncrementalSpeed( const int, const int = 0 );

  //! Gets spacemouse button for specified function
  int spacemouseBtn( const int ) const;

  //! Sets spacemouse buttons
  void setSpacemouseButtons( const int, const int, const int );

public:
  void enableSelection(bool isEnabled);
  bool isSelectionEnabled() const { return mySelectionEnabled; }

  void enableMultiselection(bool isEnable);
  bool isMultiSelectionEnabled() const { return myMultiSelectionEnabled; }

  int  getSelectionCount() const;

  /* Reimplemented from SALOME_View */

  //! See #SALOME_View::Display( const SALOME_Prs* )
  void Display( const SALOME_VTKPrs* ); 

  //! See #SALOME_View::Erase( const SALOME_VTKPrs*, const bool = false )
  void Erase( const SALOME_VTKPrs*, const bool = false );

  //! See #SALOME_View::EraseAll( SALOME_Displayer*, const bool = false )
  void EraseAll( SALOME_Displayer*, const bool = false );

  //! See #SALOME_View::getVisible( SALOME_ListIO& )
  virtual void GetVisible( SALOME_ListIO& );

  //! See #SALOME_View::CreatePrs( const char* entry = 0 )
  SALOME_Prs* CreatePrs( const char* entry = 0 );

  //! See #SALOME_View::isVisible( const Handle(SALOME_InteractiveObject)& )
  virtual bool isVisible( const Handle(SALOME_InteractiveObject)& );

  //! See #SALOME_View::Repaint()
  virtual void Repaint();

 signals:
  void actorAdded(SVTK_ViewWindow*, VTKViewer_Actor*);
  void actorRemoved(SVTK_ViewWindow*, VTKViewer_Actor*);

protected slots:
  void onMousePress(SUIT_ViewWindow*, QMouseEvent*);
  void onMouseMove(SUIT_ViewWindow*, QMouseEvent*);
  void onMouseRelease(SUIT_ViewWindow*, QMouseEvent*);

  void onDumpView();
  void onChangeBackground();

  void onActorAdded(VTKViewer_Actor*);
  void onActorRemoved(VTKViewer_Actor*);

private:
  void updateToolBars();

  Qtx::BackgroundData  myDefaultBackground;
  double myTrihedronSize;
  bool                 myTrihedronRelative;
  bool                 myIsStaticTrihedronVisible;
  bool                 mySelectionEnabled;
  bool                 myMultiSelectionEnabled;
  int                  myIncrementSpeed;
  int                  myIncrementMode;
  int                  myProjMode;
  int                  myStereoType;
  int                  myAnaglyphFilter;
  bool                 myQuadBufferSupport;
  int                  myStyle;
  int                  myZoomingStyle;
  Preselection_Mode    myPreSelectionMode;
  int                  mySpaceBtn[3];
};

#endif
