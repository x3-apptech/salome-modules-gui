// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
//  File   : Plot2d_AnalyticalParser.cxx
//  Author : Roman NIKOLAEV, Open CASCADE S.A.S. (roman.nikolaev@opencascade.com)
#include "Plot2d_AnalyticalParser.h"
#include <structmember.h>


/* ==================================
 * ===========  PYTHON ==============
 * ==================================*/

namespace {
  typedef struct {
    PyObject_HEAD
    int softspace;
    std::string *out;
    } PyStdOut;

  static void
  PyStdOut_dealloc(PyStdOut *self)
  {
    PyObject_Del(self);
  }

  static PyObject *
  PyStdOut_write(PyStdOut *self, PyObject *args)
  {
    char *c;
    int l;
    if (!PyArg_ParseTuple(args, "t#:write",&c, &l))
      return NULL;

    *(self->out)=*(self->out)+c;

    Py_INCREF(Py_None);
    return Py_None;
  }

  static PyMethodDef PyStdOut_methods[] = {
    {"write",  (PyCFunction)PyStdOut_write,  METH_VARARGS,
      PyDoc_STR("write(string) -> None")},
    {NULL,    NULL}   /* sentinel */
  };

  static PyMemberDef PyStdOut_memberlist[] = {
    {(char*)"softspace", T_INT,  offsetof(PyStdOut, softspace), 0,
     (char*)"flag indicating that a space needs to be printed; used by print"},
    {NULL} /* Sentinel */
  };

  static PyTypeObject PyStdOut_Type = {
    /* The ob_type field must be initialized in the module init function
     * to be portable to Windows without using C++. */
    PyObject_HEAD_INIT(NULL)
    0,                            /*ob_size*/
    "PyOut",                      /*tp_name*/
    sizeof(PyStdOut),             /*tp_basicsize*/
    0,                            /*tp_itemsize*/
    /* methods */
    (destructor)PyStdOut_dealloc, /*tp_dealloc*/
    0,                            /*tp_print*/
    0,                            /*tp_getattr*/
    0,                            /*tp_setattr*/
    0,                            /*tp_compare*/
    0,                            /*tp_repr*/
    0,                            /*tp_as_number*/
    0,                            /*tp_as_sequence*/
    0,                            /*tp_as_mapping*/
    0,                            /*tp_hash*/
    0,                            /*tp_call*/
    0,                            /*tp_str*/
    PyObject_GenericGetAttr,      /*tp_getattro*/
    /* softspace is writable:  we must supply tp_setattro */
    PyObject_GenericSetAttr,      /* tp_setattro */
    0,                            /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,           /*tp_flags*/
    0,                            /*tp_doc*/
    0,                            /*tp_traverse*/
    0,                            /*tp_clear*/
    0,                            /*tp_richcompare*/
    0,                            /*tp_weaklistoffset*/
    0,                            /*tp_iter*/
    0,                            /*tp_iternext*/
    PyStdOut_methods,             /*tp_methods*/
    PyStdOut_memberlist,          /*tp_members*/
    0,                            /*tp_getset*/
    0,                            /*tp_base*/
    0,                            /*tp_dict*/
    0,                            /*tp_descr_get*/
    0,                            /*tp_descr_set*/
    0,                            /*tp_dictoffset*/
    0,                            /*tp_init*/
    0,                            /*tp_alloc*/
    0,                            /*tp_new*/
    0,                            /*tp_free*/
    0,                            /*tp_is_gc*/
  };

  PyObject * newPyStdOut( std::string& out )
  {
    PyStdOut* self = PyObject_New(PyStdOut, &PyStdOut_Type);
    if (self) {
      self->softspace = 0;
      self->out=&out;
    }
    return (PyObject*)self;
  }
}

////////////////////////END PYTHON///////////////////////////


//! The only one instance of Parser
Plot2d_AnalyticalParser* Plot2d_AnalyticalParser::myParser = 0;

//Define the script
QString Plot2d_AnalyticalParser::myScript = QString("");

/*!
  \brief Return the only instance of the Plot2d_AnalyticalParser
  \return instance of the Plot2d_AnalyticalParser
*/
Plot2d_AnalyticalParser* Plot2d_AnalyticalParser::parser()
{
  if ( !myParser )
    myParser = new Plot2d_AnalyticalParser();
  return myParser;
}

/*!
  \brief Constructor.

  Construct the Parser and initialize python interpritator.
*/
Plot2d_AnalyticalParser::Plot2d_AnalyticalParser() 
{
  /* Initialize the Python interpreter */
  if (Py_IsInitialized()) {
    PyGILState_STATE gstate = PyGILState_Ensure();
    myMainMod = PyImport_AddModule("__main__");
    myMainDict = PyModule_GetDict(myMainMod);
    PyGILState_Release(gstate);
    initScript();
  }
}

int Plot2d_AnalyticalParser::calculate( const QString& theExpr,
				      const double theMin,
				      const double theMax,
				      const int theNbStep,
				      double** theX,
				      double** theY) {
  
  QString aPyScript = myScript;
  aPyScript = aPyScript.arg(theExpr);
  int result = -1;
  PyGILState_STATE gstate = PyGILState_Ensure();
  PyObject* obj = PyRun_String(qPrintable(aPyScript), Py_file_input, myMainDict, NULL);

  if(obj == NULL) {
    PyErr_Print();
    PyGILState_Release(gstate);        
    return result;
    
  } else {
    Py_DECREF(obj);
  }
  
  PyObject* func = NULL;
  PyObject* f_y = NULL;
  
  if(PyObject_HasAttrString(myMainMod, "Y")) {
    f_y = PyObject_GetAttrString(myMainMod, "Y");
  }

  if(PyObject_HasAttrString(myMainMod, "coordCalculator")) {
    func = PyObject_GetAttrString(myMainMod, "coordCalculator");
  }
  
  if( f_y == NULL || func == NULL )  {
    fflush(stderr);
    std::string err_description="";
    PyObject* new_stderr = newPyStdOut(err_description);
    PyObject* old_stderr = PySys_GetObject((char*)"stderr");
    Py_INCREF(old_stderr);
    PySys_SetObject((char*)"stderr", new_stderr);
    PyErr_Print();
    PySys_SetObject((char*)"stderr", old_stderr);
    Py_DECREF(new_stderr);
    PyGILState_Release(gstate);
    return result;
  }
    
  PyObject* coords = PyObject_CallFunction(func,(char*)"(d, d, i)", theMin, theMax, theNbStep );
  
  if (coords == NULL){
    fflush(stderr);
    std::string err_description="";
    PyObject* new_stderr = newPyStdOut(err_description);
    PyObject* old_stderr = PySys_GetObject((char*)"stderr");
    Py_INCREF(old_stderr);
    PySys_SetObject((char*)"stderr", new_stderr);
    PyErr_Print();
    PySys_SetObject((char*)"stderr", old_stderr);
    Py_DECREF(new_stderr);
    PyGILState_Release(gstate);
    return result;
  }

  Py_ssize_t size = PyList_Size( coords );
  if( size <= 0 ) {
    Py_DECREF(coords);
    return result;
  }

  result = size;

  *theX = new double[size];
  *theY = new double[size];
  
  for ( Py_ssize_t i = 0; i< size; ++i ) {
    PyObject* coord = PyList_GetItem( coords, i );
    (*theX)[i] =  PyFloat_AsDouble(PyList_GetItem(coord, 0));
    (*theY)[i] =  PyFloat_AsDouble(PyList_GetItem(coord, 1));
  }

  PyGILState_Release(gstate);
  return result;
}

/*!
  \brief Initialize python script.
*/
void Plot2d_AnalyticalParser::initScript() {
  myScript.clear();
  myScript += "from math import *                      \n";
  myScript += "def Y(x):                               \n";
  myScript += "    return ";
  myScript += "%1\n";

  myScript += "def coordCalculator(xmin, xmax, nstep):     \n";
  myScript += "   coords = []                              \n";
  myScript += "   xstep  = (xmax - xmin) / nstep           \n";
  myScript += "   n = 0                                    \n";
  myScript += "   while n <= nstep :                       \n";
  myScript += "      x = xmin + n*xstep                    \n";
  myScript += "      try:                                  \n";
  myScript += "			y = Y(x)                           \n";
  myScript += "			coords.append([x,y])               \n";
  myScript += "      except ValueError, ZeroDivisionError: \n";
  myScript += "			pass                               \n";
  myScript += "      n = n+1                               \n";
  myScript += "   return coords                            \n";
}
