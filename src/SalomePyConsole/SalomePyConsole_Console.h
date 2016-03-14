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

// File   : PyConsole_Console.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#ifndef SALOMEPYCONSOLE_CONSOLE_H
#define SALOMEPYCONSOLE_CONSOLE_H

#include "SalomePyConsole.h"
#include <PyConsole_ConsoleBase.h>
#include <SUIT_PopupClient.h>
#include <QWidget>
#include <QMap>

class SalomePyConsole_Interp;
class SalomePyConsole_Editor;

class SALOMEPYCONSOLE_EXPORT SalomePyConsole_Console : public PyConsole_ConsoleBase, public SUIT_PopupClient
{
  Q_OBJECT
public:

  struct SalomePyConsole_Interp_Creator : public PyConsole_Interp_CreatorBase
  {
    virtual PyConsole_EditorBase *createEditor( PyConsole_Interp *interp, PyConsole_ConsoleBase *console ) const;
    virtual PyConsole_Interp *createInterp( ) const;
  };

public:
  SalomePyConsole_Console( QWidget* parent, PyConsole_Interp* interp = 0 );
  virtual ~SalomePyConsole_Console();
  //! \brief Get popup client symbolic name
  virtual QString     popupClientType() const { return QString( "PyConsole" ); }
  virtual void        contextMenuPopup( QMenu* );
  virtual bool        eventFilter( QObject*, QEvent* );
protected:
  SalomePyConsole_Console( QWidget* parent, PyConsole_Interp*,  SalomePyConsole_Editor*);
};

/**
 * Enhance console object providing auto-completion.
 * Similar to PyConsole_Console except that an enhanced interpreter and enhanced editor
 * are encapsulated.
 */
class PYCONSOLE_EXPORT SalomePyConsole_EnhConsole : public SalomePyConsole_Console
{
  Q_OBJECT
public:

  struct SalomePyConsole_Interp_EnhCreator : public PyConsole_Interp_CreatorBase
  {
    virtual PyConsole_EditorBase *createEditor( PyConsole_Interp *interp, PyConsole_ConsoleBase *console ) const;
    virtual PyConsole_Interp *createInterp( ) const;
  };

public:
  SalomePyConsole_EnhConsole( QWidget* parent, PyConsole_Interp* interp = 0 );
  virtual ~SalomePyConsole_EnhConsole() {}
};

#endif // SALOMEPYCONSOLE_CONSOLE_H
