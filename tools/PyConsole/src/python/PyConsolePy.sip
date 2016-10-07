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
// File   : PyConsolePy.sip
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

%Module PyConsolePy

%Import QtCore/QtCoremod.sip
%Import QtGui/QtGuimod.sip
%If (Qt_5_0_0 -)
%Import QtWidgets/QtWidgetsmod.sip
%End

class PyConsole_Editor : QTextEdit
{ 
%TypeHeaderCode
#include <PyConsole_Editor.h>
%End

public:
  explicit PyConsole_Editor( QWidget* /TransferThis/ = 0 );
  virtual ~PyConsole_Editor();

  virtual void   addText( const QString&, const bool = false, const bool = false );
  bool           isCommand( const QString& ) const;

  virtual void   exec( const QString& );
  void           execAndWait( const QString& );

  bool           isSync() const;
  void           setIsSync( const bool );

  bool           isSuppressOutput() const;
  void           setIsSuppressOutput( const bool );

  bool           isShowBanner() const;
  void           setIsShowBanner( const bool );

  void           setAutoCompletion( bool );
  bool           autoCompletion() const;

  bool           isLogging() const;

  virtual QSize  sizeHint() const;

  bool           startLog( const QString& );
  bool           dump( const QString& );

signals:
  void updateDoc( const QString& );

public slots:
  void           cut();
  void           paste();
  void           clear();
  void           dump();
  void           startLog();
  void           stopLog();

protected:
  virtual void   dropEvent( QDropEvent* );
  virtual void   mousePressEvent( QMouseEvent* );
  virtual void   mouseReleaseEvent( QMouseEvent* );
  virtual void   keyPressEvent ( QKeyEvent* );
  virtual void   customEvent( QEvent* );
  virtual void   insertFromMimeData( const QMimeData* );
  void           putLog( const QString& );

  virtual QString getDumpFileName();
  virtual QString getLogFileName();

private:
  PyConsole_Editor( const PyConsole_Editor& );
  PyConsole_Editor& operator=( const PyConsole_Editor& );
};

class PyConsole_Console : QWidget
{ 
%TypeHeaderCode
#include <PyConsole_Console.h>
%End

public:
  explicit PyConsole_Console( QWidget* /TransferThis/ = 0 );
  virtual ~PyConsole_Console();

  QFont               font() const;
  virtual void        setFont( const QFont& );

  bool                isSync() const;
  void                setIsSync( const bool );

  bool                isSuppressOutput() const;
  void                setIsSuppressOutput( const bool );

  bool                isShowBanner() const;
  void                setIsShowBanner( const bool );

  void                setAutoCompletion( bool );
  bool                autoCompletion() const;

  void                exec( const QString& );
  void                execAndWait( const QString& );

  void                setMenuActions( const int );
  int                 menuActions() const;

  void                startLog( const QString& );
  void                stopLog();

protected:
  virtual void        contextMenuEvent( QContextMenuEvent* );

private:
  PyConsole_Console( const PyConsole_Console& );
  PyConsole_Console& operator=( const PyConsole_Console& );
};
