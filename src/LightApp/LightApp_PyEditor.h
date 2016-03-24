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
// File   : LightApp_PyEditor.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#ifndef LIGHTAPP_PYEDITOR_H
#define LIGHTAPP_PYEDITOR_H

#include "LightApp.h"
#include "PyConsole_Editor.h"

class LIGHTAPP_EXPORT LightApp_PyEditor : public PyConsole_Editor
{
  Q_OBJECT

public:
  LightApp_PyEditor( PyConsole_Interp*, QWidget* = 0 );
  ~LightApp_PyEditor();

protected:
  virtual QString getDumpFileName();
  virtual QString getLogFileName();
};

#endif // LIGHTAPP_PYEDITOR_H
