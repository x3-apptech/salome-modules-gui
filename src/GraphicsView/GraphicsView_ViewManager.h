// Copyright (C) 2013-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef GRAPHICSVIEW_VIEWMANAGER_H
#define GRAPHICSVIEW_VIEWMANAGER_H

#include "GraphicsView.h"

#include <SUIT_ViewManager.h>

class GraphicsView_Viewer;

/*
  Class       : GraphicsView_ViewManager
  Description : View manager of the graphics view
*/
class GRAPHICSVIEW_API GraphicsView_ViewManager : public SUIT_ViewManager
{
  Q_OBJECT

public:
  GraphicsView_ViewManager( SUIT_Study* theStudy,
                            SUIT_Desktop* theDesktop );
  virtual ~GraphicsView_ViewManager();

  GraphicsView_Viewer* getViewer();
};

#endif
