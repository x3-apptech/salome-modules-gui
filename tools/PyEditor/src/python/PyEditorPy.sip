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
// File   : SalomePyQt.sip
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

%Module PyEditorPy

%Import QtCore/QtCoremod.sip
%Import QtGui/QtGuimod.sip
%If (Qt_5_0_0 -)
%Import QtWidgets/QtWidgetsmod.sip
%End

class PyEditor_Settings
{
%TypeHeaderCode
#include <PyEditor_Settings.h>
%End

public:
  PyEditor_Settings();

  void setHighlightCurrentLine( bool );
  bool highlightCurrentLine() const;

  void setTextWrapping( bool );
  bool textWrapping() const;

  void setCenterCursorOnScroll( bool );
  bool centerCursorOnScroll() const;

  void setLineNumberArea( bool );
  bool lineNumberArea() const;

  void setVerticalEdge( bool );
  bool verticalEdge() const;

  void setNumberColumns( int );
  int numberColumns() const;

  void setTabSpaceVisible( bool );
  bool tabSpaceVisible() const;

  void setTabSize( int );
  int tabSize() const;

  void setFont( const QFont& );
  QFont font() const;
};

class PyEditor_Editor : QPlainTextEdit
{ 
%TypeHeaderCode
#include <PyEditor_Editor.h>
%End

public:
  explicit PyEditor_Editor( QWidget* /TransferThis/ = 0 );
  virtual ~PyEditor_Editor();

  void setSettings( const PyEditor_Settings& );
  const PyEditor_Settings& settings() const;
  QString text() const;

public slots:
  void deleteSelected();
  void append( const QString& );  
  void setText( const QString& text );

protected:
  virtual void keyPressEvent( QKeyEvent* );
  virtual void resizeEvent( QResizeEvent* );
  virtual void paintEvent( QPaintEvent* );

private:
  PyEditor_Editor( const PyEditor_Editor& );
  PyEditor_Editor& operator=( const PyEditor_Editor& );
};
