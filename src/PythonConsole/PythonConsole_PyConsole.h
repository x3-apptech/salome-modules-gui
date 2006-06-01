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
#ifndef PythonConsole_PyConsole_H
#define PythonConsole_PyConsole_H

#include "PythonConsole.h"

#include <qwidget.h>

class PythonConsole_PyEditor;
class PyInterp_base;

class PYCONSOLE_EXPORT PythonConsole: public QWidget
{
  Q_OBJECT;

public:
  PythonConsole(QWidget* parent, PyInterp_base* interp = 0 );
  virtual ~PythonConsole();

  PyInterp_base* getInterp() { return myInterp; } 
  QFont          font() const;
  virtual void   setFont( const QFont& );

  void           exec( const QString& command );

protected:
  PyInterp_base*          myInterp;
  PythonConsole_PyEditor* myEditor;
};


#endif
