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
#ifndef PLOT2D_VIEWMANAGER_H
#define PLOT2D_VIEWMANAGER_H

#include "Plot2d.h"
#include "SUIT_ViewManager.h"

class SUIT_Desktop;
class Plot2d_Viewer;
class Plot2d_ViewFrame;

class PLOT2D_EXPORT Plot2d_ViewManager : public SUIT_ViewManager
{
  Q_OBJECT

public:
  Plot2d_ViewManager( SUIT_Study*, SUIT_Desktop* );
  ~Plot2d_ViewManager();

  Plot2d_Viewer*     getPlot2dModel() const;

protected:
  void         setViewName(SUIT_ViewWindow* theView);
  bool         insertView(SUIT_ViewWindow* theView);

public slots:
  void         createView();

protected slots:
  void         onCloneView();

signals:
  void cloneView( Plot2d_ViewFrame*, Plot2d_ViewFrame* );

protected:
  static  int  myMaxId;
  int          myId;
};

#endif
