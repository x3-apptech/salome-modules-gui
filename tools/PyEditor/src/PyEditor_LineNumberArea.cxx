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
// File   : PyEditor_LineNumberArea.cxx
// Author : Maxim GLIBIN, Open CASCADE S.A.S. (maxim.glibin@opencascade.com)
//

#include "PyEditor_LineNumberArea.h"

#include "PyEditor_Editor.h"

/*!
  \class PyEditor_LineNumberArea
  \brief Widget shows line number.
*/

/*!
  \brief Constructor.
  \param theCodeEditor parent widget
*/
PyEditor_LineNumberArea::PyEditor_LineNumberArea( PyEditor_Editor* theCodeEditor ) :
  QWidget( theCodeEditor )
{
  myCodeEditor = theCodeEditor;
}

QSize PyEditor_LineNumberArea::sizeHint() const
{
  return QSize( myCodeEditor->lineNumberAreaWidth(), 0 );
}

void PyEditor_LineNumberArea::paintEvent( QPaintEvent* theEvent )
{
  myCodeEditor->lineNumberAreaPaintEvent( theEvent );
  QWidget::paintEvent( theEvent );
}
