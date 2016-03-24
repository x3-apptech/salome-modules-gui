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

#ifndef VTKVIEWER_VIEWMODEL_H
#define VTKVIEWER_VIEWMODEL_H

#include "VTKViewer.h"
#include "Qtx.h"
#include "SUIT_ViewModel.h"

#include <QColor>

class QMouseEvent;

class SUIT_ViewWindow;
class SUIT_Desktop;

class VTKVIEWER_EXPORT VTKViewer_Viewer: public SUIT_ViewModel
{
  Q_OBJECT

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

  /*!Initialize type of viewer.*/
  static QString           Type() { return "VTKViewer"; }
  static QString           backgroundData( QStringList&, QIntList&, QIntList& );

  VTKViewer_Viewer();
  virtual ~VTKViewer_Viewer();

  virtual SUIT_ViewWindow* createView(SUIT_Desktop* theDesktop);

  virtual void             setViewManager(SUIT_ViewManager* theViewManager);
  virtual void             contextMenuPopup( QMenu* );
  /*!Gets type of viewer.*/
  virtual QString          getType() const { return Type(); }

public:
  void                     enableSelection(bool isEnabled);
  /*!Checks: is selection enabled*/
  bool                     isSelectionEnabled() const { return mySelectionEnabled; }

  void                     enableMultiselection(bool isEnable);
  /*!Checks: is multi selection enabled*/
  bool                     isMultiSelectionEnabled() const { return myMultiSelectionEnabled; }

  int                      getSelectionCount() const;

  QColor                   backgroundColor() const;                  // obsolete
  void                     setBackgroundColor( const QColor& );      // obsolete
  Qtx::BackgroundData      background() const;
  void                     setBackground( const Qtx::BackgroundData& );

signals:
  void                     selectionChanged();

protected slots:
  void                     onMousePress( SUIT_ViewWindow*, QMouseEvent* );
  void                     onMouseMove( SUIT_ViewWindow*, QMouseEvent* );
  void                     onMouseRelease( SUIT_ViewWindow*, QMouseEvent* );

  void                     onDumpView();
  void                     onShowToolbar();
  void                     onChangeBackground();

private:
  Qtx::BackgroundData      myDefaultBackground;
  bool                     mySelectionEnabled;
  bool                     myMultiSelectionEnabled;
};

#endif
