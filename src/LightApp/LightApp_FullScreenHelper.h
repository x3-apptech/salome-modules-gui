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

// File:      LightApp_FullScreenHelper.h
// Created:   04/10/2011 18:39:25 PM
// Author:    OCC team
//
#ifndef LIGHTAPP_FULLSCREEN_H
#define LIGHTAPP_FULLSCREEN_H

#include "LightApp.h"

#include <QMap>
#include <QDockWidget>
#include <QToolBar>


class LIGHTAPP_EXPORT LightApp_FullScreenHelper {
 public:
  
  LightApp_FullScreenHelper();
  ~LightApp_FullScreenHelper();
  
  void switchToFullScreen();
  void switchToNormalScreen();

 private:
  void toolbarVisible(SUIT_ViewWindow*, bool=false);

 private:
  typedef QMap<QDockWidget*,bool> DocWidgetMap;
  DocWidgetMap myDocWidgetMap;
  typedef QMap<QToolBar*, bool> ToolBarMap;
  ToolBarMap myToolBarMap;
  QList<SUIT_ViewWindow*> myWindowsList;
  typedef QMap<int, QList<int> > FrameHideMap;
  FrameHideMap myFrameHideMap;

  bool myStatusBarVisibility;

};


#endif //LIGHTAPP_FULLSCREEN_H
