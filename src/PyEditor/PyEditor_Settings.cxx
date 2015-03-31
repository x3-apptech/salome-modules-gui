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
// File   : PyEditor_Settings.cxx
// Author : Maxim GLIBIN, Open CASCADE S.A.S. (maxim.glibin@opencascade.com)
//

#include "PyEditor_Settings.h"

#include <QtxResourceMgr.h>

#include <QDir>
#include <QFile>
#include <QSettings>

/*!
  \class PyEditor_Settings
  \brief Manager of setting values.
*/

/*!
  \brief Constructor.
  \param isSingle flag determined single application or reccesed.
*/
PyEditor_Settings::PyEditor_Settings( QtxResourceMgr* theMgr, bool isSingle )
    : p_HighlightCurrentLine( true ),
      p_LineNumberArea( true ),
      p_TextWrapping( false ),
      p_CenterCursorOnScroll( true ),
      p_TabSpaceVisible( true ),
      p_TabSize( 4 ),
      p_VerticalEdge( true ),
      p_NumberColumns( 80 ),
      p_Font( "Courier", 10 ),
      m_ResourceMgr(theMgr),
      m_Single(isSingle)

{
  if ( m_Single )
  {
    m_Settings = new QSettings( "config.ini", QSettings::IniFormat );
    if ( !QFile::exists( m_Settings->fileName() ) )
      toSettings( PY_EDITOR );
  }

  readSettings();
}

/*!
  \brief Reads the setting values.
 */
void PyEditor_Settings::readSettings()
{
  if ( isSingle() )
    fromSettings( PY_EDITOR );
  else
    readPreferences();
}

/*!
  \brief Writes the setting values.
 */
void PyEditor_Settings::writeSettings()
{
  if ( isSingle() )
    toSettings( PY_EDITOR );
  else
    writePreferences();
}

/*!
  \return \c true if the application is single
 */
bool PyEditor_Settings::isSingle()
{
  return m_Single;
}

/*!
  \brief Loads the setting values from file settings.
 */
void PyEditor_Settings::fromSettings( const QString &theCategory )
{
  QString aGroup = theCategory;
  aGroup += "/";

  p_HighlightCurrentLine = m_Settings->value(aGroup + QLatin1String( HIGHLIGHT_CURRLINE ), p_HighlightCurrentLine).toBool();
  p_LineNumberArea       = m_Settings->value(aGroup + QLatin1String( LINE_NUMBER_AREA ), p_LineNumberArea).toBool();
  p_TextWrapping         = m_Settings->value(aGroup + QLatin1String( TEXT_WRAP ), p_TextWrapping).toBool();
  p_CenterCursorOnScroll = m_Settings->value(aGroup + QLatin1String( CURSOR_SCROLL ), p_CenterCursorOnScroll).toBool();
  p_TabSpaceVisible      = m_Settings->value(aGroup + QLatin1String( TAB_WHITESPACES ), p_TabSpaceVisible).toBool();
  p_TabSize              = m_Settings->value(aGroup + QLatin1String( TAB_SIZE ), p_TabSize).toInt();
  p_VerticalEdge         = m_Settings->value(aGroup + QLatin1String( VERTICAL_EDGE ), p_VerticalEdge).toBool();
  p_NumberColumns        = m_Settings->value(aGroup + QLatin1String( NUM_COLUMNS ), p_NumberColumns).toInt();
  p_Font = QFont( m_Settings->value(aGroup + QLatin1String( FONT_FAMILY ), p_Font.family()).toString(),
    m_Settings->value(aGroup + QLatin1String( FONT_SIZE ), p_Font.pointSize()).toInt() );
}

/*!
  \brief Saves the setting values into file settings.
 */
void PyEditor_Settings::toSettings( const QString &theCategory ) const
{
  m_Settings->beginGroup( theCategory );
  m_Settings->setValue( QLatin1String( HIGHLIGHT_CURRLINE ), p_HighlightCurrentLine );
  m_Settings->setValue( QLatin1String( LINE_NUMBER_AREA ),   p_LineNumberArea );
  m_Settings->setValue( QLatin1String( TEXT_WRAP ),          p_TextWrapping );
  m_Settings->setValue( QLatin1String( CURSOR_SCROLL ),      p_CenterCursorOnScroll );
  m_Settings->setValue( QLatin1String( TAB_WHITESPACES ),    p_TabSpaceVisible );
  m_Settings->setValue( QLatin1String( TAB_SIZE ),           p_TabSize );
  m_Settings->setValue( QLatin1String( VERTICAL_EDGE ),      p_VerticalEdge );
  m_Settings->setValue( QLatin1String( NUM_COLUMNS ),        p_NumberColumns );
  m_Settings->setValue( QLatin1String( FONT_FAMILY ),        p_Font.family() );
  m_Settings->setValue( QLatin1String( FONT_SIZE ),          p_Font.pointSize() );
  m_Settings->endGroup();
}

/*!
  \brief Loads the setting values from setting resources.
 */
void PyEditor_Settings::readPreferences()
{
  if(m_ResourceMgr) 
    {
      p_HighlightCurrentLine = m_ResourceMgr->booleanValue( "PyEditor", "HighlightCurrentLine", p_HighlightCurrentLine );
      p_LineNumberArea       = m_ResourceMgr->booleanValue( "PyEditor", "LineNumberArea", p_LineNumberArea );
      p_TextWrapping         = m_ResourceMgr->booleanValue( "PyEditor", "TextWrapping", p_TextWrapping );
      p_CenterCursorOnScroll = m_ResourceMgr->booleanValue( "PyEditor", "CenterCursorOnScroll", p_CenterCursorOnScroll );
      p_TabSpaceVisible      = m_ResourceMgr->booleanValue( "PyEditor", "TabSpaceVisible", p_TabSpaceVisible );
      p_TabSize              = m_ResourceMgr->integerValue( "PyEditor", "TabSize", p_TabSize );
      p_VerticalEdge         = m_ResourceMgr->booleanValue( "PyEditor", "VerticalEdge", p_VerticalEdge );
      p_NumberColumns        = m_ResourceMgr->integerValue( "PyEditor", "NumberColumns", p_NumberColumns );
      p_Font                 = m_ResourceMgr->fontValue( "PyEditor", "Font", p_Font );
    }
}

/*!
  \brief Saves the setting values into setting resources.
 */
void PyEditor_Settings::writePreferences()
{
  if(m_ResourceMgr) 
    {
      m_ResourceMgr->setValue( "PyEditor", "HighlightCurrentLine", p_HighlightCurrentLine );
      m_ResourceMgr->setValue( "PyEditor", "LineNumberArea", p_LineNumberArea );
      m_ResourceMgr->setValue( "PyEditor", "TextWrapping", p_TextWrapping );
      m_ResourceMgr->setValue( "PyEditor", "CenterCursorOnScroll", p_CenterCursorOnScroll );
      m_ResourceMgr->setValue( "PyEditor", "TabSpaceVisible", p_TabSpaceVisible );
      m_ResourceMgr->setValue( "PyEditor", "TabSize", p_TabSize );
      m_ResourceMgr->setValue( "PyEditor", "VerticalEdge", p_VerticalEdge );
      m_ResourceMgr->setValue( "PyEditor", "NumberColumns", p_NumberColumns );
      m_ResourceMgr->setValue( "PyEditor", "Font", p_Font );
    }
}
