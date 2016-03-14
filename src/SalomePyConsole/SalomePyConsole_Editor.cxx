// Copyright (C) 2007-2015  CEA/DEN, EDF R&D, OPEN CASCADE
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

//  SALOME SALOMEGUI : implementation of desktop and GUI kernel
// File   : PyConsole_Editor.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
/*!
  \class PyConsole_Editor
  \brief Python command line interpreter front-end GUI widget.
  
  This class provides simple GUI interface to the Python interpreter, including basic 
  navigation operations, executing commands (both interactively and programmatically), 
  copy-paste operations, history of the commands and so on.

  Here below is the shortcut keyboard boundings used for navigation and other operations:
  - <Enter>              : execute current command
  - <Ctrl><Break>        : clear current command
  - <Escape>             : clear current command
  - <Up>                 : previous command in the history
  - <Shift><Up>          : move cursor one row up with selection
  - <Ctrl><Up>           : move cursor one row up without selection
  - <Ctrl><Shift><Up>    : move cursor one row up with selection
  - <Down>               : next command in the history
  - <Shift><Down>        : move cursor one row down with selection
  - <Ctrl><Down>         : move cursor one row down without selection
  - <Ctrl><Shift><Down>  : move cursor one row down with selection
  - <Left>               : move one symbol left without selection
  - <Shift><Left>        : move one symbol left with selection
  - <Ctrl><Left>         : move one word left without selection
  - <Ctrl><Shift><Left>  : move one word left with selection
  - <Right>              : move one symbol right without selection
  - <Shift><Right>       : move one symbol right with selection
  - <Ctrl><Right>        : move one word right without selection
  - <Ctrl><Shift><Right> : move one word right with selection
  - <PgUp>               : first command in the history
  - <Shift><PgUp>        : move one page up with selection
  - <Ctrl><PgUp>         : move one page up without selection
  - <Ctrl><Shift><PgUp>  : scroll one page up
  - <PgDn>               : last command in the history
  - <Shift><PgDn>        : move one page down with selection
  - <Ctrl><PgDn>         : move one page down without selection
  - <Ctrl><Shift><PgDn>  : scroll one page down
  - <Home>               : move to the beginning of the line without selection
  - <Shift><Home>        : move to the beginning of the line with selection
  - <Ctrl><Home>         : move to the very first symbol without selection
  - <Ctrl><Shift><Home>  : move to the very first symbol with selection
  - <End>                : move to the end of the line without selection
  - <Shift><End>         : move to the end of the line with selection
  - <Ctrl><End>          : move to the very last symbol without selection
  - <Ctrl><Shift><End>   : move to the very last symbol with selection
  - <Backspace>          : delete symbol before the cursor
                           / remove selected text and put it to the clipboard (cut)
  - <Shift><Backspace>   : delete previous word
                           / remove selected text and put it to the clipboard (cut)
  - <Ctrl><Backspace>    : delete text from the cursor to the beginning of the line 
                           / remove selected text and put it to the clipboard (cut)
  - <Delete>             : delete symbol after the cursor 
                           / remove selected text and put it to the clipboard (cut)
  - <Shift><Delete>      : delete next word
                           / remove selected text and put it to the clipboard (cut)
  - <Ctrl><Delete>       : delete text from the cursor to the end of the line
                           / remove selected text and put it to the clipboard (cut)
  - <Ctrl><Insert>       : copy
  - <Shift><Insert>      : paste
  - <Ctrl><V>            : paste
  - <Ctrl><C>            : copy
  - <Ctrl><X>            : cut
  - <Ctrl><V>            : paste
*/

#include "PyConsole_Interp.h"   // !!! WARNING !!! THIS INCLUDE MUST BE THE VERY FIRST !!!
#include "SalomePyConsole_Editor.h"
#include "PyConsole_Event.h"
#include "PyInterp_Event.h"
#include "PyInterp_Dispatcher.h"
#include "PyConsole_Request.h"

#include "SUIT_FileValidator.h"
#include "SUIT_MessageBox.h"
#include "SUIT_FileDlg.h"

#include <QApplication>
#include <QClipboard>
#include <QDropEvent>
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextStream>
#include <QChar>
#include <QFileInfo>

class DumpCommandsFileValidator : public SUIT_FileValidator
{
 public:
  DumpCommandsFileValidator( QWidget* parent = 0 ) : SUIT_FileValidator ( parent ) {};
  virtual ~DumpCommandsFileValidator() {};
  virtual bool canSave( const QString& file, bool permissions );
};

bool DumpCommandsFileValidator::canSave(const QString& file, bool permissions)
{
  QFileInfo fi( file );
  if ( !QRegExp( "[A-Za-z_][A-Za-z0-9_]*" ).exactMatch( fi.completeBaseName() ) ) {
    SUIT_MessageBox::critical( parent(),
                               QObject::tr("WRN_WARNING"),
                               QObject::tr("WRN_FILE_NAME_BAD") );
    return false;
  }
  return SUIT_FileValidator::canSave( file, permissions);
}

/*!
  \brief Constructor. 
  
  Creates python editor window.
  \param theInterp python interper
  \param theParent parent widget
*/
SalomePyConsole_Editor::SalomePyConsole_Editor( PyConsole_Interp* theInterp, 
                                                QWidget*          theParent )
  : PyConsole_EditorBase(theInterp,theParent)
{
}

/*!
  \brief Destructor.
*/
SalomePyConsole_Editor::~SalomePyConsole_Editor()
{
}


void SalomePyConsole_Editor::StaticDumpSlot(PyConsole_EditorBase *base)
{
  QStringList aFilters;
  aFilters.append( tr( "PYTHON_FILES_FILTER" ) );
  
  QString fileName = SUIT_FileDlg::getFileName( base, QString(),
						aFilters, tr( "TOT_DUMP_PYCOMMANDS" ),
						false, true, new DumpCommandsFileValidator( base ) );
  base->dumpImpl(fileName);
}

/*!
  \brief "Dump commands" operation.
 */
void SalomePyConsole_Editor::dumpSlot()
{
  SalomePyConsole_Editor::StaticDumpSlot(this);
}

void SalomePyConsole_Editor::StaticStartLogSlot(PyConsole_EditorBase *base)
{
  QStringList aFilters;
  aFilters.append( tr( "LOG_FILES_FILTER" ) );

  while (1) {
    QString fileName = SUIT_FileDlg::getFileName( base, QString(),
						  aFilters, tr( "TOT_SAVE_PYLOG" ),
						  false, true );
    if ( !fileName.isEmpty() ) {
      if ( base->startLogImpl( fileName ) ) {
	break;
      }
      else {
	SUIT_MessageBox::critical( base,
				   QObject::tr("ERR_ERROR"),
				   QObject::tr("ERR_FILE_NOT_WRITABLE") );
      }
    }
    else {
      break;
    }
  }
}

/*!
  \brief "Start log" operation.
 */
void SalomePyConsole_Editor::startLogSlot()
{
  StaticStartLogSlot(this);
}
