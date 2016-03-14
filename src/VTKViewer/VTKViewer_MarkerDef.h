// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef VTKVIEWER_MARKERDEF_H
#define VTKVIEWER_MARKERDEF_H

#include <list>
#include <map>
#include <string>

namespace VTK
{
  //! Type of standard marker for advanced rendering of vtk points
  typedef int MarkerType;
  const MarkerType MT_NONE    = 0;
  const MarkerType MT_POINT   = 1;
  const MarkerType MT_PLUS    = 2;
  const MarkerType MT_STAR    = 3;
  const MarkerType MT_O       = 4;
  const MarkerType MT_X       = 5;
  const MarkerType MT_O_POINT = 6;
  const MarkerType MT_O_PLUS  = 7;
  const MarkerType MT_O_STAR  = 8;
  const MarkerType MT_O_X     = 9;
  const MarkerType MT_USER    = 10;

  //! Scale of standard marker for advanced rendering of vtk points
  typedef int MarkerScale;
  const MarkerScale MS_NONE   = 0;
  const MarkerScale MS_10     = 1;
  const MarkerScale MS_15     = 2;
  const MarkerScale MS_20     = 3;
  const MarkerScale MS_25     = 4;
  const MarkerScale MS_30     = 5;
  const MarkerScale MS_35     = 6;
  const MarkerScale MS_40     = 7;
  const MarkerScale MS_45     = 8;
  const MarkerScale MS_50     = 9;
  const MarkerScale MS_55     = 10;
  const MarkerScale MS_60     = 11;
  const MarkerScale MS_65     = 12;
  const MarkerScale MS_70     = 13;

  typedef std::list<unsigned short>             MarkerTexture;
  typedef std::pair<std::string, MarkerTexture> MarkerData;
  typedef std::map<int, MarkerData>             MarkerMap;
}

#endif
