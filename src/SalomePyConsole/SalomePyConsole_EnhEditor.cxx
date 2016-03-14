// Copyright (C) 2007-2015  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "SalomePyConsole.h"
#include <Python.h>

#include <QKeyEvent>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QRegExp>
#include <QMimeData>

#include "SalomePyConsole_EnhEditor.h"
#include "SalomePyConsole_Editor.h"

/**
 * Constructor.
 * @param interp the interpreter linked to the editor
 * @param parent parent widget
 */
SalomePyConsole_EnhEditor::SalomePyConsole_EnhEditor(PyConsole_Interp* interp, QWidget* parent) :
     PyConsole_EnhEditorBase(interp, parent)
{
}

void SalomePyConsole_EnhEditor::dumpSlot()
{
  SalomePyConsole_Editor::StaticDumpSlot(this);
}

void SalomePyConsole_EnhEditor::startLogSlot()
{
  SalomePyConsole_Editor::StaticStartLogSlot(this);
}
