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
// File   : PyEditor_StdSettings.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#include "PyEditor_StdSettings.h"

PyEditor_StdSettings::PyEditor_StdSettings() :
  myLanguage( "en" )
{
  load();
}

PyEditor_StdSettings::PyEditor_StdSettings( const QString& group ) :
  myGroup( group ),
  myLanguage( "en" )
{
  load();
}

PyEditor_StdSettings::PyEditor_StdSettings( const QString& group,
                                            const QString& filename,
                                            QSettings::Format format ) :
  mySettings( filename, format ),
  myGroup( group ),
  myLanguage( "en" )
{
  load();
}

void PyEditor_StdSettings::setLanguage( const QString& language )
{
  myLanguage = language;
}

QString PyEditor_StdSettings::language() const
{
  return myLanguage;
}

void PyEditor_StdSettings::load()
{
  mySettings.beginGroup( myGroup.isEmpty() ? option( snEditor ) : myGroup );

  setHighlightCurrentLine( mySettings.value( option( snHighlightCurrentLine ), highlightCurrentLine() ).toBool() );
  setTextWrapping( mySettings.value( option( snTextWrapping ), textWrapping() ).toBool() );
  setCenterCursorOnScroll( mySettings.value( option( snCenterCursorOnScroll ), centerCursorOnScroll() ).toBool() );
  setLineNumberArea( mySettings.value( option( snLineNumberArea ), lineNumberArea() ).toBool() );
  setVerticalEdge( mySettings.value( option( snVerticalEdge  ), verticalEdge() ).toBool() );
  setNumberColumns( mySettings.value( option( snNumberColumns ), numberColumns() ).toInt() );
  setTabSpaceVisible( mySettings.value( option( snTabSpaceVisible ), tabSpaceVisible() ).toBool() );
  setTabSize( mySettings.value( option( snTabSize ), tabSize() ).toInt() );
  setFont( mySettings.value( option( snFont ), font() ).value<QFont>() );
  setLanguage( mySettings.value( "language", language() ).toString() );

  mySettings.endGroup();
}

void PyEditor_StdSettings::save()
{
  mySettings.beginGroup( myGroup.isEmpty() ? option( snEditor ) : myGroup );

  mySettings.setValue( option( snHighlightCurrentLine ), highlightCurrentLine() );
  mySettings.setValue( option( snTextWrapping ), textWrapping() );
  mySettings.setValue( option( snCenterCursorOnScroll ), centerCursorOnScroll() );
  mySettings.setValue( option( snLineNumberArea ), lineNumberArea() );
  mySettings.setValue( option( snVerticalEdge  ), verticalEdge() );
  mySettings.setValue( option( snNumberColumns ), numberColumns() );
  mySettings.setValue( option( snTabSpaceVisible ), tabSpaceVisible() );
  mySettings.setValue( option( snTabSize ), tabSize() );
  mySettings.setValue( option( snFont ), font() );
  mySettings.setValue( "language", language() );

  mySettings.endGroup();
}
