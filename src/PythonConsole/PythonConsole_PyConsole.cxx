#include "PythonConsole_PyConsole.h"
#include "PythonConsole_PyEditor.h"
#include "PyInterp_base.h"

#include "SUIT_Desktop.h"

#include <qlayout.h>

using namespace std;

//****************************************************************
PythonConsole::PythonConsole(QWidget* parent, PyInterp_base* interp)
: QWidget(parent)
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

//****************************************************************
PythonConsole::~PythonConsole()
{
}

