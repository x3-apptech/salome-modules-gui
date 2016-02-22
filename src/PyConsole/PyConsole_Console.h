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

// File   : PyConsole_Console.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#ifndef PYCONSOLE_CONSOLE_H
#define PYCONSOLE_CONSOLE_H

#include "PyConsole.h"
#include <PyConsole_ConsoleBase.h>
#include <SUIT_PopupClient.h>
#include <QWidget>
#include <QMap>

class PyConsole_Interp;
class PyConsole_Editor;

class PYCONSOLE_EXPORT PyConsole_Console : public PyConsole_ConsoleBase, public SUIT_PopupClient
{
  Q_OBJECT
public:

  struct PyConsole_Interp_Creator : public PyConsole_Interp_CreatorBase
  {
    virtual PyConsole_EditorBase *createEditor( PyConsole_Interp *interp, PyConsole_ConsoleBase *console ) const;
    virtual PyConsole_Interp *createInterp( ) const;
  };

public:
  PyConsole_Console( QWidget* parent, PyConsole_Interp* interp = 0 );
  virtual ~PyConsole_Console();
  //! \brief Get popup client symbolic name
  virtual QString     popupClientType() const { return QString( "PyConsole" ); }
  virtual void        contextMenuPopup( QMenu* );
  virtual bool        eventFilter( QObject*, QEvent* );
protected:
  PyConsole_Console( QWidget* parent, PyConsole_Interp*,  PyConsole_Editor*);
};

/**
 * Enhance console object providing auto-completion.
 * Similar to PyConsole_Console except that an enhanced interpreter and enhanced editor
 * are encapsulated.
 */
class PYCONSOLE_EXPORT PyConsole_EnhConsole : public PyConsole_Console
{
  Q_OBJECT
public:

  struct PyConsole_Interp_EnhCreator : public PyConsole_Interp_CreatorBase
  {
    virtual PyConsole_EditorBase *createEditor( PyConsole_Interp *interp, PyConsole_ConsoleBase *console ) const;
    virtual PyConsole_Interp *createInterp( ) const;
  };

public:
  PyConsole_EnhConsole( QWidget* parent, PyConsole_Interp* interp = 0 );
  virtual ~PyConsole_EnhConsole() {}
};

#endif // PYCONSOLE_CONSOLE_H
