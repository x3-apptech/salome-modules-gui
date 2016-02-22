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
#ifndef PYCONSOLE_CONSOLEBASE_H
#define PYCONSOLE_CONSOLEBASE_H

#include "PyConsoleBase.h"

#include <QWidget>
#include <QMenu>
#include <QMap>

class PyConsole_Interp;
class PyConsole_EditorBase;

class PYCONSOLEBASE_EXPORT PyConsole_ConsoleBase : public QWidget
{
  Q_OBJECT

public:

  struct PyConsole_Interp_CreatorBase
  {
    virtual PyConsole_EditorBase *createEditor( PyConsole_Interp *interp, PyConsole_ConsoleBase *console ) const;
    virtual PyConsole_Interp *createInterp( ) const;
  };

public:
  //! Context popup menu actions flags
  enum
  {
    CopyId         = 0x01,  //!< "Copy" menu action
    PasteId        = 0x02,  //!< "Paste" menu action
    ClearId        = 0x04,  //!< "Clear" menu action
    SelectAllId    = 0x08,  //!< "Select All" menu action
    DumpCommandsId = 0x10,  //!< "DumpCommands" menu action
    StartLogId     = 0x20,  //!< "Start log" menu action
    StopLogId      = 0x40,  //!< "Stop log" menu action
    All            = 0xFF,  //!< all menu actions 
  };

public:
  PyConsole_ConsoleBase( QWidget* parent, PyConsole_Interp* interp = 0 );
  virtual ~PyConsole_ConsoleBase();

  //! \brief Get python interperter
  PyConsole_Interp*   getInterp() const;
  QFont               font() const;
  virtual void        setFont( const QFont& );

  bool                isSync() const;
  void                setIsSync( const bool );

  bool                isSuppressOutput() const;
  void                setIsSuppressOutput( const bool );

  bool                isShowBanner() const;
  void                setIsShowBanner( const bool );

  void                exec( const QString& );
  void                execAndWait( const QString& );

  void                setMenuActions( const int );
  int                 menuActions() const;

  void                startLog( const QString& );
  void                stopLog();

  virtual void        contextMenuPopup( QMenu* );

protected:
  void                createActions();
  void                updateActions();
  //!  MUST BE NON VIRTUAL ! (called from constructor !!!!)
  void defaultConstructor( PyConsole_Interp* interp, const PyConsole_Interp_CreatorBase& crea );
  PyConsole_ConsoleBase( QWidget* parent, PyConsole_Interp*,  PyConsole_EditorBase*);

  PyConsole_EditorBase*   myEditor;    //!< python console editor widget
  QMap<int, QAction*> myActions;   //!< menu actions list
};

/**
 * Enhance console object providing auto-completion.
 * Similar to PyConsole_Console except that an enhanced interpreter and enhanced editor
 * are encapsulated.
 */
class PYCONSOLEBASE_EXPORT PyConsole_EnhConsoleBase : public PyConsole_ConsoleBase
{
  Q_OBJECT
public:

  struct PyConsole_Interp_EnhCreatorBase : public PyConsole_Interp_CreatorBase
  {
    virtual PyConsole_EditorBase *createEditor( PyConsole_Interp *interp, PyConsole_ConsoleBase *console ) const;
    virtual PyConsole_Interp *createInterp( ) const;
  };

public:
  PyConsole_EnhConsoleBase( QWidget* parent, PyConsole_Interp* interp = 0 );
  virtual ~PyConsole_EnhConsoleBase() {}
  virtual bool eventFilter( QObject * o, QEvent * e );
  virtual void contextMenuRequest( QContextMenuEvent * e ) ;
};

#endif // PYCONSOLE_CONSOLEBASE_H
