// Copyright (C) 2010-2014  CEA/DEN, EDF R&D
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
// File   : Plot2d_ViewWindow.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#ifndef PVViewer_VIEWWINDOW_H
#define PVViewer_VIEWWINDOW_H

#include <SUIT_ViewWindow.h>
#include <QMap>

class SUIT_Desktop;
class PVViewer_Viewer;
class pqTabbedMultiViewWidget;

class PVViewer_ViewWindow : public SUIT_ViewWindow
{
  Q_OBJECT

public:
  PVViewer_ViewWindow( SUIT_Desktop*, PVViewer_Viewer* );
  virtual ~PVViewer_ViewWindow();

  virtual QString   getVisualParameters();
  virtual void      setVisualParameters( const QString& );
  
  pqTabbedMultiViewWidget*    getMultiViewManager() const;

signals:
  void applyRequest();

public slots:
  void onEmulateApply();

private:
  PVViewer_Viewer*     myModel;
  pqTabbedMultiViewWidget*    myPVMgr;
};

#endif // PLOT2D_VIEWWINDOW_H
