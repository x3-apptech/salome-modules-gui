// Copyright (C) 2015-2019  OPEN CASCADE
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
// File   : PyEditor_Widget.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#include "PyEditor_Editor.h"
#include "PyEditor_FindTool.h"
#include "PyEditor_Widget.h"

#include <QVBoxLayout>

/*!
  \class PyEditor_Widget
  \brief Wraps Python editor with the find/replace functionality to a single widget.
*/

/*!
  \brief Constructor.
  \param parent Parent widget.
*/
PyEditor_Widget::PyEditor_Widget( QWidget* parent )
{
  // Create editor.
  myEditor = new PyEditor_Editor( this );

  // Create find tool.
  myFindTool = new PyEditor_FindTool( myEditor, this );

  // Set-up layout
  QVBoxLayout* layout = new QVBoxLayout( this );
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->setSpacing( 3 );
  layout->addWidget( myEditor );
  layout->addWidget( myFindTool );

  connect( myEditor, SIGNAL( modificationChanged( bool ) ),
           this, SIGNAL( modificationChanged( bool ) ) );
  connect( myEditor, SIGNAL( undoAvailable( bool ) ),
           this, SIGNAL( undoAvailable( bool ) ) );
  connect( myEditor, SIGNAL( redoAvailable( bool ) ),
           this, SIGNAL( redoAvailable( bool ) ) );
  connect( myEditor, SIGNAL( copyAvailable( bool ) ),
           this, SIGNAL( copyAvailable( bool ) ) );

  connect( myEditor, SIGNAL( selectionChanged() ),
           this, SIGNAL( selectionChanged() ) );
  connect( myEditor, SIGNAL( textChanged() ),
           this, SIGNAL( textChanged() ) );
  connect( myEditor, SIGNAL( cursorPositionChanged() ),
           this, SIGNAL( cursorPositionChanged() ) );

  setFocusProxy( myEditor );
}

/*!
  \brief Get editor.
  \return Pointer to editor.
*/
PyEditor_Editor* PyEditor_Widget::editor()
{
  return myEditor;
}

/*!
  \brief Get find tool.
  \return Pointer to find tool.
*/
PyEditor_FindTool* PyEditor_Widget::findTool()
{
  return myFindTool;
}

/*!
  \brief Get all custom keywords from editor.
  \return List of keywords.
*/
QStringList PyEditor_Widget::keywords() const
{
  return myEditor->keywords();
}

/*!
  \brief Set custom keywords to editor.
  \param keywords List of keywords.
  \param type Type of keywords (group id).
  \param color Color of keywords.
*/
void PyEditor_Widget::appendKeywords( const QStringList& keywords, int type, const QColor& color )
{
  myEditor->appendKeywords( keywords, type, color );
}

/*!
  \brief Remove given custom keywords from editor.
  \param keywords List of keywords to remove.
*/
void PyEditor_Widget::removeKeywords( const QStringList& keywords )
{
  myEditor->removeKeywords( keywords );
}

/*!
  \brief Get current editor's completion policy.
  \return Completion policy (see PyEditor_Editor::CompletionPolicy).
*/
int PyEditor_Widget::completionPolicy() const
{
  return (int) myEditor->completionPolicy();
}

/*!
  \brief Set editor's completion policy.
  \param policy Completion policy (see PyEditor_Editor::CompletionPolicy).
*/
void PyEditor_Widget::setCompletionPolicy( int policy )
{
  myEditor->setCompletionPolicy( (PyEditor_Editor::CompletionPolicy) policy );
}

/*!
  \brief Activate Find dialog.
*/
void PyEditor_Widget::find()
{
  myFindTool->activateFind();
}

/*!
  \brief Activate Replace dialog.
*/
void PyEditor_Widget::replace()
{
  myFindTool->activateReplace();
}

/*!
  \brief Undo last editor's operation.
*/
void PyEditor_Widget::undo()
{
  myEditor->undo();
}

/*!
  \brief Redo last undone editor's operation.
*/
void PyEditor_Widget::redo()
{
  myEditor->redo();
}

/*!
  \brief Cut text selected in editor and put it into clipboard.
*/
void PyEditor_Widget::cut()
{
  myEditor->cut();
}

/*!
  \brief Copy text selected in editor into clipboard.
*/
void PyEditor_Widget::copy()
{
  myEditor->copy();
}

/*!
  \brief Paste text from clipboard into editor.
*/
void PyEditor_Widget::paste()
{
  myEditor->paste();
}

/*!
  \brief Delete text selected in editor.
*/
void PyEditor_Widget::deleteSelected()
{
  myEditor->deleteSelected();
}

/*!
  \brief Select all text in editor.
*/
void PyEditor_Widget::selectAll()
{
  myEditor->selectAll();
}

/*!
  \brief Clear content of editor.
*/
void PyEditor_Widget::clear()
{
  myEditor->clear();
}

/*!
  \brief Set/clear modified flag of editor.
  \param on 'Modified' flag's value.
*/
void PyEditor_Widget::setModified( bool on )
{
  myEditor->document()->setModified( on );
}

/*!
  \brief Get modified flag of editor.
  \return 'Modified' flag's value.
*/
bool PyEditor_Widget::isModified()
{
  return myEditor->document()->isModified();
}

/*!
  \brief Set text to editor.
  \param text Text to be put into editor.
*/
void PyEditor_Widget::setText( const QString& text )
{
  myEditor->setPlainText( text );
}

/*!
  \brief Get text from editor.
  \return Current editor contents.
*/
QString PyEditor_Widget::text() const
{
  return myEditor->toPlainText();
}

/*!
  \brief Set editor's settings.
  \param settings Settings object.
*/
void PyEditor_Widget::setSettings( const PyEditor_Settings& settings )
{
  myEditor->setSettings( settings );
}

/*!
  \brief Get editor's settings.
  \return Settings object.
*/
const PyEditor_Settings& PyEditor_Widget::settings() const
{
  return myEditor->settings();
}

/*!
  \brief Move editor's cursor to the given line.
  \note Line count starts from 1.
  \param line Line number.
*/
void PyEditor_Widget::setCurrentLine( int line )
{
  myEditor->setCurrentLine( line );
}
