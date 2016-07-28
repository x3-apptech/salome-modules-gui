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
// File   : PyEditor_Settings.cxx
// Author : Maxim GLIBIN, Open CASCADE S.A.S. (maxim.glibin@opencascade.com)
//

#include "PyEditor_Settings.h"

/*!
  \class PyEditor_Settings
  \brief Manager of setting values.
*/

PyEditor_Settings* PyEditor_Settings::myGlobalSettings = 0;

/*!
  Get global settings.
  \return reference to global settings object
*/
PyEditor_Settings* PyEditor_Settings::settings()
{
  return myGlobalSettings;
}

/*!
  Set global settings.
  \param settings reference to global settings object
*/
void PyEditor_Settings::setSettings( PyEditor_Settings* settings )
{
  if ( myGlobalSettings )
    delete myGlobalSettings;
  myGlobalSettings = settings;
}

/*!
  \brief Constructor.
*/
PyEditor_Settings::PyEditor_Settings()
  : myHighlightCurrentLine( true ),
    myTextWrapping( false ),
    myCenterCursorOnScroll( true ),
    myLineNumberArea( true ),
    myVerticalEdge( true ),
    myNumberColumns( 80 ),
    myTabSpaceVisible( true ),
    myTabSize( 4 ),
    myFont( "Courier", 10 )
{
}

/*!
  \brief Set "highlight current line" option.
  \param on option value
*/
void PyEditor_Settings::setHighlightCurrentLine( bool on )
{
  myHighlightCurrentLine = on;
}

/*!
  \brief Get "highlight current line" option.
  \return option value
*/
bool PyEditor_Settings::highlightCurrentLine() const
{
  return myHighlightCurrentLine;
}

/*!
  \brief Set "text wrapping" option.
  \param on option value
*/
void PyEditor_Settings::setTextWrapping( bool on )
{
  myTextWrapping = on;
}

/*!
  \brief Get "text wrapping line" option.
  \return option value
*/
bool PyEditor_Settings::textWrapping() const
{
  return myTextWrapping;
}

/*!
  \brief Set "center cursor on scroll" option.
  \param on option value
*/
void PyEditor_Settings::setCenterCursorOnScroll( bool on )
{
  myCenterCursorOnScroll = on;
}

/*!
  \brief Get "center cursor on scroll" option.
  \return option value
*/
bool PyEditor_Settings::centerCursorOnScroll() const
{
  return myCenterCursorOnScroll;
}

/*!
  \brief Set "show line number area" option.
  \param on option value
*/
void PyEditor_Settings::setLineNumberArea( bool on )
{
  myLineNumberArea = on;
}

/*!
  \brief Get "show line number area" option.
  \return option value
*/
bool PyEditor_Settings::lineNumberArea() const
{
  return myLineNumberArea;
}

/*!
  \brief Set "show vertical edge" option.
  \param on option value
*/
void PyEditor_Settings::setVerticalEdge( bool on )
{
  myVerticalEdge = on;
}

/*!
  \brief Get "show vertical edge" option.
  \return option value
*/
bool PyEditor_Settings::verticalEdge() const
{
  return myVerticalEdge;
}

/*!
  \brief Set "number of columns" option.
  \param value option value
*/
void PyEditor_Settings::setNumberColumns( int value )
{
  myNumberColumns = value;
}

/*!
  \brief Get "number of columns" option.
  \return option value
*/
int PyEditor_Settings::numberColumns() const
{
  return myNumberColumns;
}

/*!
  \brief Set "show tab spaces" option.
  \param on option value
*/
void PyEditor_Settings::setTabSpaceVisible( bool on )
{
  myTabSpaceVisible = on;
}

/*!
  \brief Get "show tab spaces" option.
  \return option value
*/
bool PyEditor_Settings::tabSpaceVisible() const
{
  return myTabSpaceVisible;
}

/*!
  \brief Set "tab size" option.
  \param value option value
*/
void PyEditor_Settings::setTabSize( int value )
{
  myTabSize = value;
}

/*!
  \brief Get "tab size" option.
  \return option value
*/
int PyEditor_Settings::tabSize() const
{
  return myTabSize;
}

/*!
  \brief Set "font" option.
  \param font option value
*/
void PyEditor_Settings::setFont( const QFont& font )
{
  myFont = font;
}

/*!
  \brief Get "font" option.
  \return option value
*/
QFont PyEditor_Settings::font() const
{
  return myFont;
}

/*!
  \brief Read settings from the persistence storage.
  Base implementation does nothing; it should be reimplemented in successors.
*/
void PyEditor_Settings::load()
{
}

/*!
  \brief Write settings to the persistence storage.
  Base implementation does nothing; it should be reimplemented in successors.
*/
void PyEditor_Settings::save()
{
}

/*!
  \brief Copy settings from another object.
  \param other source settings object
*/
void PyEditor_Settings::copyFrom( const PyEditor_Settings& other )
{
  setHighlightCurrentLine( other.highlightCurrentLine() );
  setTextWrapping( other.textWrapping() );
  setCenterCursorOnScroll( other.centerCursorOnScroll() );
  setLineNumberArea( other.lineNumberArea() );
  setTabSpaceVisible( other.tabSpaceVisible() );
  setTabSize( other.tabSize() );
  setVerticalEdge( other.verticalEdge() );
  setNumberColumns( other.numberColumns() );
  setFont( other.font() );
  save();
}

/*!
  \brief Get preference item's identifier.
  \return string identifier
*/
QString PyEditor_Settings::option( Option option )
{
  static const char* options[] = {
    "PythonEditor",
    "HighlightCurrentLine",
    "TextWrapping",
    "CenterCursorOnScroll",
    "LineNumberArea",
    "VerticalEdge",
    "NumberColumns",
    "TabSpaceVisible",
    "TabSize",
    "Font",
  };
  return option >= 0 && option <= snFont ? options[option] : "Unknown";
}
