#ifndef PythonConsole_PyConsole_H
#define PythonConsole_PyConsole_H

#include "PythonConsole.h"

#include <qwidget.h>

class PythonConsole_PyEditor;
class PyInterp_base;

class PYCONSOLE_EXPORT PythonConsole: public QWidget
{
public:
  PythonConsole(QWidget* parent, PyInterp_base* interp = 0 );
  virtual ~PythonConsole();

  PyInterp_base* getInterp() { return myInterp; } 

protected:
  PyInterp_base*          myInterp;
  PythonConsole_PyEditor* myEditor;
};


#endif
