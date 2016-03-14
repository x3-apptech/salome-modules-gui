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

#ifndef OCCVIEWER_VIEWMANAGER_H
#define OCCVIEWER_VIEWMANAGER_H

#include "OCCViewer_ViewModel.h"

#include "SUIT_ViewManager.h"

class SUIT_Desktop;

class OCCVIEWER_EXPORT OCCViewer_ViewManager : public SUIT_ViewManager
{
  Q_OBJECT

public:
  OCCViewer_ViewManager( SUIT_Study* study, SUIT_Desktop* theDesktop, bool DisplayTrihedron = true );
  ~OCCViewer_ViewManager();

  OCCViewer_Viewer* getOCCViewer() { return (OCCViewer_Viewer*) myViewModel; }

  virtual void      contextMenuPopup( QMenu* );
};

#endif
