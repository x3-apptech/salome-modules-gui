// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#include "PythonConsole_PyConsole.h"
#include "PythonConsole_PyEditor.h"
#include "PyInterp_base.h"

#include "SUIT_Desktop.h"

#include <qlayout.h>

using namespace std;

/*!
  Constructor
*/
PythonConsole::PythonConsole(QWidget* parent, PyInterp_base* interp)
: QWidget(parent), myEditor( 0 )
{
  // create python interpreter
  myInterp = interp;
  if ( !myInterp )
    myInterp = new PythonConsole_PyInterp();
  
  // initialize Python interpretator
  myInterp->initialize();

  // create editor console
  QVBoxLayout* lay = new QVBoxLayout( this );
  myEditor = new PythonConsole_PyEditor(myInterp, this,"Python Interpreter");
  lay->addWidget( myEditor );
}

/*!
  Destructor
*/
PythonConsole::~PythonConsole()
{
}

/*!
  Executes command
  \param command - string with command and arguments
*/
void PythonConsole::exec( const QString& command )
{
  if ( myEditor )
    myEditor->exec( command );
}

/*!
  Changes font of python console
  \param f - new font
*/
void PythonConsole::setFont( const QFont& f )
{
  if( myEditor )
    myEditor->setFont( f );
}

/*!
  \return font of python console
*/
QFont PythonConsole::font() const
{
  QFont res;
  if( myEditor )
    res = myEditor->font();
  return res;
}
