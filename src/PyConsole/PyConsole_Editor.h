// Copyright (C) 2007-2015  CEA/DEN, EDF R&D, OPEN CASCADE
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

//  SALOME SALOMEGUI : implementation of desktop and GUI kernel
// File   : PyConsole_Editor.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#ifndef PYCONSOLE_EDITOR_H
#define PYCONSOLE_EDITOR_H

#include "PyConsole.h"
#include "PyConsole_EditorBase.h"

#include <QTextEdit>

class PyConsole_Interp;
class PyInterp_Request;
class QEventLoop;

class PYCONSOLE_EXPORT PyConsole_Editor : public PyConsole_EditorBase
{
  Q_OBJECT;
public:
  PyConsole_Editor( PyConsole_Interp* theInterp, QWidget *theParent = 0 );
  ~PyConsole_Editor();
  static void StaticDumpSlot(PyConsole_EditorBase *base);
  static void StaticStartLogSlot(PyConsole_EditorBase *base);
protected:
  virtual void dumpSlot();
  virtual void startLogSlot();
};

#endif // PYCONSOLE_EDITOR_H
