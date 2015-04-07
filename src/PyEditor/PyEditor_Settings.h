// Copyright (C) 2015 OPEN CASCADE
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

#include <QFont>
#include "PyEditor.h"

class QSettings;
class QtxResourceMgr;

const char* const PY_EDITOR          = "PythonEditor";
const char* const HIGHLIGHT_CURRLINE = "HighlightCurrentLine";
const char* const LINE_NUMBER_AREA   = "LineNumberArea";
const char* const TEXT_WRAP          = "TextWrapping";
const char* const CURSOR_SCROLL      = "CenterCursorOnScroll";
const char* const TAB_WHITESPACES    = "TabSpaceVisible";
const char* const TAB_SIZE           = "TabSize";
const char* const VERTICAL_EDGE      = "VerticalEdge";
const char* const NUM_COLUMNS        = "NumberColumns";
const char* const FONT_FAMILY        = "FontFamily";
const char* const FONT_SIZE          = "FontSize";

class PYEDITOR_EXPORT PyEditor_Settings
{
public:
  PyEditor_Settings( QtxResourceMgr* = 0 ,  bool isSingle = true );

  void readSettings();
  void writeSettings();

  void fromSettings( const QString& );
  void toSettings( const QString& ) const;

  void readPreferences();
  void writePreferences();

  bool isSingle();
  
  // Display settings
  bool p_HighlightCurrentLine;
  bool p_TextWrapping;
  bool p_CenterCursorOnScroll;
  bool p_LineNumberArea;
  
  // Vertical edge settings
  bool p_VerticalEdge;
  int  p_NumberColumns;
  
  // Tab settings
  bool p_TabSpaceVisible;
  int  p_TabSize;
  
  // Font settings
  QFont p_Font;

private:
  QSettings*      m_Settings;
  QtxResourceMgr* m_ResourceMgr;
  bool            m_Single;
};

#endif // PYEDITOR_SETTINGS_H
