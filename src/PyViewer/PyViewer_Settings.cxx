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
// File   : PyViewer_Settings.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#include "PyViewer_Settings.h"

#include <QtxResourceMgr.h>

PyViewer_Settings::PyViewer_Settings( QtxResourceMgr* resMgr ) :
  myResMgr( resMgr )
{
  load();
}

PyViewer_Settings::PyViewer_Settings( QtxResourceMgr* resMgr, const QString& group ) :
  myResMgr( resMgr ),
  myGroup( group )
{
  load();
}

void PyViewer_Settings::load()
{
  if ( !myResMgr ) return;
  QString group = myGroup.isEmpty() ? option( snEditor ) : myGroup;
  setHighlightCurrentLine( myResMgr->booleanValue( group, option( snHighlightCurrentLine ), highlightCurrentLine() ) );
  setTextWrapping( myResMgr->booleanValue( group, option( snTextWrapping ), textWrapping() ) );
  setCenterCursorOnScroll( myResMgr->booleanValue( group, option( snCenterCursorOnScroll ), centerCursorOnScroll() ) );
  setLineNumberArea( myResMgr->booleanValue( group, option( snLineNumberArea ), lineNumberArea() ) );
  setVerticalEdge( myResMgr->booleanValue( group, option( snVerticalEdge  ), verticalEdge() ) );
  setNumberColumns( myResMgr->integerValue( group, option( snNumberColumns ), numberColumns() ) );
  setTabSpaceVisible( myResMgr->booleanValue( group, option( snTabSpaceVisible ), tabSpaceVisible() ) );
  setTabSize( myResMgr->integerValue( group, option( snTabSize ), tabSize() ) );
  setFont( myResMgr->fontValue( group, option( snFont ), font() ) );
}

void PyViewer_Settings::save()
{
  if ( !myResMgr ) return;
  QString group = myGroup.isEmpty() ? option( snEditor ) : myGroup;
  myResMgr->setValue( group, option( snHighlightCurrentLine ), highlightCurrentLine() );
  myResMgr->setValue( group, option( snTextWrapping ), textWrapping() );
  myResMgr->setValue( group, option( snCenterCursorOnScroll ), centerCursorOnScroll() );
  myResMgr->setValue( group, option( snLineNumberArea ), lineNumberArea() );
  myResMgr->setValue( group, option( snVerticalEdge  ), verticalEdge() );
  myResMgr->setValue( group, option( snNumberColumns ), numberColumns() );
  myResMgr->setValue( group, option( snTabSpaceVisible ), tabSpaceVisible() );
  myResMgr->setValue( group, option( snTabSize ), tabSize() );
  myResMgr->setValue( group, option( snFont ), font() );
}
