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
//  File   : SPlot2d_ViewModel.h
//  Author : Sergey RUIN
//  Module : SUIT

#ifndef SPlot2d_ViewModel_H
#define SPlot2d_ViewModel_H

#include "SPlot2d.h"  

#include "SALOME_Prs.h"
#include "Plot2d_ViewModel.h"
#include "SALOME_InteractiveObject.hxx"
#include "Plot2d_ViewFrame.h"
#include "Plot2d_ViewWindow.h"
#include "SPlot2d_Curve.h"

class SPLOT2D_EXPORT SPlot2d_Viewer : public Plot2d_Viewer, public SALOME_View
{ 
  Q_OBJECT

public:
  static QString Type() { return "Plot2d"; }

  /* Construction/destruction */
  SPlot2d_Viewer( bool theAutoDel = false );
  virtual ~SPlot2d_Viewer();

public:
  virtual QString          getType() const { return Type(); }

  /*  interactive object management */
  void highlight( const Handle(SALOME_InteractiveObject)& IObject, bool highlight, bool update = true ) {}
  void unHighlightAll() {}
  void rename( const Handle(SALOME_InteractiveObject)&, const QString&, Plot2d_ViewFrame* = 0 );
  void renameAll( const Handle(SALOME_InteractiveObject)&, const QString& );
  bool isInViewer( const Handle(SALOME_InteractiveObject)& IObject );

  virtual   SUIT_ViewWindow* createView(SUIT_Desktop* theDesktop);
  
  /* display */		
  void Display( const Handle(SALOME_InteractiveObject)& IObject, bool update = true );
  void DisplayOnly( const Handle(SALOME_InteractiveObject)& IObject );
  void Erase( const Handle(SALOME_InteractiveObject)& IObject, bool update = true );

  /* Reimplemented from SALOME_View */
  void                 Display( const SALOME_Prs2d* );
  void                 Erase( const SALOME_Prs2d*, const bool = false );
  virtual void         EraseAll(const bool = false);
  virtual void         Repaint();
  virtual SALOME_Prs*  CreatePrs( const char* entry = 0 );
  virtual void         BeforeDisplay( SALOME_Displayer* d );
  virtual void         AfterDisplay ( SALOME_Displayer* d );
  virtual bool         isVisible( const Handle(SALOME_InteractiveObject)& IObject );


  /* operations */
  SPlot2d_Curve*                   getCurveByIO( const Handle(SALOME_InteractiveObject)&, Plot2d_ViewFrame* = 0 );
  Plot2d_ViewFrame*                getActiveViewFrame();
  Handle(SALOME_InteractiveObject) FindIObject( const char* Entry );

protected slots:
  virtual void onCloneView( Plot2d_ViewFrame*, Plot2d_ViewFrame* );
};


#endif // SPlot2d_ViewModel_H




