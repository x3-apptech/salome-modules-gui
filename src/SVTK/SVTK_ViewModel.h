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
// See http://www.salome-platform.org/
//
#ifndef SVTK_VIEWMODEL_H
#define SVTK_VIEWMODEL_H

#include "SVTK.h"
#include "SUIT_ViewModel.h"

#include "SALOME_Prs.h"
#include "SALOME_InteractiveObject.hxx"

#include <qcolor.h>

class SVTK_EXPORT SVTK_Viewer : public SUIT_ViewModel, public SALOME_View 
{
  Q_OBJECT;

public:
  static QString Type() { return "VTKViewer"; }

  SVTK_Viewer();
  virtual ~SVTK_Viewer();

  virtual SUIT_ViewWindow* createView(SUIT_Desktop*);

  virtual void             setViewManager(SUIT_ViewManager* theViewManager);
  virtual void             contextMenuPopup( QPopupMenu* );
  virtual QString          getType() const { return Type(); }

  QColor backgroundColor() const;
  void   setBackgroundColor( const QColor& );

  int    trihedronSize() const;
  void   setTrihedronSize( const int );

public:
  void enableSelection(bool isEnabled);
  bool isSelectionEnabled() const { return mySelectionEnabled; }

  void enableMultiselection(bool isEnable);
  bool isMultiSelectionEnabled() const { return myMultiSelectionEnabled; }

  int  getSelectionCount() const;

  /* Reimplemented from SALOME_View */
  void                           Display( const SALOME_VTKPrs* );
  void                           Erase( const SALOME_VTKPrs*, const bool = false );
  void                           EraseAll( const bool = false );
  SALOME_Prs*                    CreatePrs( const char* entry = 0 );
  virtual void                   BeforeDisplay( SALOME_Displayer* d );
  virtual void                   AfterDisplay ( SALOME_Displayer* d );
  virtual bool                   isVisible( const Handle(SALOME_InteractiveObject)& );
  virtual void                   Repaint();

  //implemented in SOCC, but were not found in VTKVIEwer_ViewFrame, so not implemented here..
  //?virtual void                LocalSelection( const SALOME_OCCPrs*, const int );
  //?virtual void                GlobalSelection( const bool = false ) const;

public slots:
  void onSelectionChanged();

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
  QColor myBgColor;
  int    myTrihedronSize;
  bool   mySelectionEnabled;
  bool   myMultiSelectionEnabled;
};

#endif
