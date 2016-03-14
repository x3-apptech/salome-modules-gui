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

#ifndef GRAPHICSVIEW_DEFS_H
#define GRAPHICSVIEW_DEFS_H

#include <QList>

class GraphicsView_Object;
typedef QList<GraphicsView_Object*> GraphicsView_ObjectList;
typedef QListIterator<GraphicsView_Object*> GraphicsView_ObjectListIterator;

enum GV_SelectionChangeStatus
{
  GVSCS_Invalid,
  GVSCS_Local,
  GVSCS_Global
};

enum GV_SelectionStatus
{
  GVSS_Invalid,
  GVSS_LocalChanged,
  GVSS_GlobalChanged,
  GVSS_NoChanged
};

#endif
