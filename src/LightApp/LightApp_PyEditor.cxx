// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
// File   : LightApp_PyEditor.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#include "LightApp_PyEditor.h"
#include "LightApp_FileValidator.h"
#include "SUIT_FileDlg.h"

/*!
  \brief Constructor. 
  \param interp python interper
  \param parent parent widget
*/
LightApp_PyEditor::LightApp_PyEditor( PyConsole_Interp* interp, QWidget* parent )
  : PyConsole_Editor( parent, interp )
{
}

/*!
  \brief Destructor
*/
LightApp_PyEditor::~LightApp_PyEditor()
{
}

/*!
  \brief Get file name for Dump commands operation.
  
  This function can be redefined in successor classes to show application
  specific dialog box.

  \return path to the dump file
*/
QString LightApp_PyEditor::getDumpFileName()
{
  QStringList aFilters;
  aFilters.append( tr( "PYTHON_FILES_FILTER" ) );
  
  return SUIT_FileDlg::getFileName( this, QString(),
                                    aFilters, tr( "TOT_DUMP_PYCOMMANDS" ),
                                    false, true, new LightApp_PyFileValidator( this ) );
}

/*!
  \brief Get file name for Log Python trace operation.
  
  This function can be redefined in successor classes to show application
  specific dialog box.

  \return path to the log file
*/
QString LightApp_PyEditor::getLogFileName()
{
  QStringList aFilters;
  aFilters.append( tr( "LOG_FILES_FILTER" ) );
  
  return SUIT_FileDlg::getFileName( this, QString(),
                                    aFilters, tr( "TOT_SAVE_PYLOG" ),
                                    false, true );
}
