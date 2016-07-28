// Copyright (C) 2015-2016  OPEN CASCADE
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
// File   : PyEditor_Settings.h
// Author : Maxim GLIBIN, Open CASCADE S.A.S. (maxim.glibin@opencascade.com)
//

#ifndef PYEDITOR_SETTINGS_H
#define PYEDITOR_SETTINGS_H

#include "PyEditor.h"

#include <QFont>

class PYEDITOR_EXPORT PyEditor_Settings
{
protected:
  enum Option { snEditor, 
		snHighlightCurrentLine,
		snTextWrapping,
		snCenterCursorOnScroll,
		snLineNumberArea,
		snVerticalEdge,
		snNumberColumns,
		snTabSpaceVisible,
		snTabSize,
		snFont };

public:
  static PyEditor_Settings* settings();
  static void setSettings( PyEditor_Settings* );
  
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

  virtual void load();
  virtual void save();

  void copyFrom( const PyEditor_Settings& );
  
protected:
  QString option( Option );

private:
  // Display settings
  bool myHighlightCurrentLine;
  bool myTextWrapping;
  bool myCenterCursorOnScroll;
  bool myLineNumberArea;
  
  // Vertical edge settings
  bool myVerticalEdge;
  int  myNumberColumns;
  
  // Tab settings
  bool myTabSpaceVisible;
  int  myTabSize;
  
  // Font settings
  QFont myFont;

  static PyEditor_Settings* myGlobalSettings;
};

#endif // PYEDITOR_SETTINGS_H
