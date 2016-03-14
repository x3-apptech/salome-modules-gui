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

#if !defined(_PLOT2D_VIEWMODEL_H)
#define _PLOT2D_VIEWMODEL_H

#include "Plot2d.h"
#include "SUIT_ViewModel.h"
#include <qwt_plot_item.h>

class SUIT_ViewWindow;
class SUIT_Desktop;
class Plot2d_ViewFrame;
class Plot2d_Prs;
class QString;
class QMenu;

class PLOT2D_EXPORT Plot2d_Viewer: public SUIT_ViewModel
{
  Q_OBJECT

public:
  static QString Type() { return "Plot2d"; }
  
  Plot2d_Viewer(bool theAutoDel = false);
  ~Plot2d_Viewer();

  virtual void             setViewManager( SUIT_ViewManager* );
  virtual SUIT_ViewWindow* createView(SUIT_Desktop* theDesktop);
  virtual QString getType() const { return Type(); }
  virtual void contextMenuPopup(QMenu*);
  Plot2d_Prs*  getPrs() const { return myPrs; };
  void         setPrs(Plot2d_Prs* thePrs);
  void         update();
  void         clearPrs();
  void         setAutoDel(bool theDel);
  
signals:
  void         viewCloned( SUIT_ViewWindow* );

protected slots:
  void         onChangeBgColor();
  void         onDumpView();
  void         onShowToolbar();
  virtual void onCloneView( Plot2d_ViewFrame*, Plot2d_ViewFrame* );
  virtual void onClicked( const QVariant&, int );

private:
  Plot2d_Prs* myPrs;
  bool        myAutoDel;
};

#endif // !defined(_PLOT2D_VIEWMODEL_H)

