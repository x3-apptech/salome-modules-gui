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
// Author : Adrien Bruneton (CEA/DEN)
// Created on: 4 avr. 2013

#ifndef SALOMEPYCONSOLE_ENHEDITOR_H_
#define SALOMEPYCONSOLE_ENHEDITOR_H_

#include "SalomePyConsole.h"
#include "PyConsole_EnhEditorBase.h"

#include <QObject>
#include <queue>

/**
 * Enhanced Python editor handling tab completion.
 */
class PYCONSOLE_EXPORT SalomePyConsole_EnhEditor : public PyConsole_EnhEditorBase
{
  Q_OBJECT;
public:
  SalomePyConsole_EnhEditor(PyConsole_Interp* interp, QWidget* parent = 0);
  virtual ~SalomePyConsole_EnhEditor() {}
protected:
  virtual void dumpSlot();
  virtual void startLogSlot();
};

#endif /* PYCONSOLE_ENHEDITOR_H_ */
