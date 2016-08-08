// Copyright (C) 2010-2015  CEA/DEN, EDF R&D
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
// Author : Adrien BRUNETON
//

#include <Python.h>

#define PY_ARRAY_UNIQUE_SYMBOL CURVEPLOT_ARRAY_API    // see initializeCurvePlot()
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/ndarraytypes.h>
#include <numpy/ndarrayobject.h>
#include <PyInterp_Utils.h>  // GUI

#include "CurvePlot.hxx"
#include "CurvePlot_Exception.hxx"

namespace
{
  PyObject * strToPyUnicode(std::string s)
  {
    return PyUnicode_DecodeUTF8(s.c_str(), s.size(), (char*)"strict");
  }

  void HandleAndPrintPyError(std::string msg)
  {
    if(PyErr_Occurred())
      {
        PyErr_Print();
        throw CURVEPLOT::Exception(msg);
      }
  }
};

namespace CURVEPLOT
{
  /**
   * To be called before doing anything
   */
  void InitializeCurvePlot()
  {
    PyLockWrapper lock;
    // TODO: discuss where the below should really happen:
    // doc: http://docs.scipy.org/doc/numpy/reference/c-api.array.html#importing-the-api
    import_array(); // a macro really!
  }

  class ColumnVector::Internal
  {
  public:
    Internal() : _npArray(0) {}
    ~Internal() {}

    PyArrayObject * _npArray;
  };

  ColumnVector::ColumnVector()
  {
    _impl = new Internal();
  }

  ColumnVector::~ColumnVector()
  {
    delete _impl;
  }

  int ColumnVector::size() const
  {
    if (!_impl->_npArray)
      return 0;
    {
        PyLockWrapper lock;
        int ndim = PyArray_NDIM(_impl->_npArray);
        if (ndim != 1)
          throw Exception("ColumnVector::size() : wrong number of dimensions for internal array!!");
        npy_intp * dims = PyArray_DIMS(_impl->_npArray);
        return dims[0];
    }
  }

  ColumnVector ColumnVector::BuildFromCMemory(double * data, int size)
  {
    ColumnVector ret;
    if (size <= 0)
      return ret;
    npy_intp dims[1] = {size};

    {
      PyLockWrapper lock;
      PyObject * obj = PyArray_SimpleNewFromData(1,dims,NPY_DOUBLE, data);
      PyArrayObject * aobj = (PyArrayObject * )obj;

      // Make Numpy responsible of the memory of the array (the memory will be freed
      // as soon as the array is released in NumPy)
      PyArray_ENABLEFLAGS(aobj, NPY_ARRAY_OWNDATA);

      ret._impl->_npArray = aobj;
      return ret;
    }
  }

  ColumnVector ColumnVector::BuildFromStdVector(const std::vector<double> & vec)
  {
    ColumnVector ret;
    if (vec.size() == 0)
      return ret;

    double * c_mem = (double *)malloc(sizeof(double) * vec.size());
    if (!c_mem)
      throw Exception("ColumnVector::BuildFromStdVector() : memory allocation error!");
    const double * data = &vec.front();
    std::copy(data, data+vec.size(), c_mem);
    npy_intp dims[1] = {(intptr_t) vec.size()};

    {
      PyLockWrapper lock;
      PyObject * obj = PyArray_SimpleNewFromData(1,dims,NPY_DOUBLE, c_mem);
      PyArrayObject * aobj = (PyArrayObject * )obj;

      // Make Numpy responsible of the memory of the array (the memory will be freed
      // as soon as the array is released in NumPy)
      PyArray_ENABLEFLAGS(aobj, NPY_ARRAY_OWNDATA);

      ret._impl->_npArray = aobj;
      return ret;
    }
  }

  std::string ColumnVector::toStdString() const
  {
    std::string ret_str = "(None)";
    if (!_impl->_npArray)
      return ret_str;

    {
       PyLockWrapper lock;
       PyObjWrapper ret_py(
                 PyObject_CallMethod((PyObject *)_impl->_npArray, (char *)"__str__", NULL)
                 );
       // Now extract the returned string
       if(!PyString_Check(ret_py))
         throw Exception("CurvePlot::toStdString(): Unexpected returned type!");
       ret_str = std::string(PyString_AsString(ret_py));
    }
    return ret_str;
  }

  void ColumnVector::createPythonVar(std::string varName) const
  {
    PyObject* main_module = PyImport_AddModule((char*)"__main__");
    PyObject* global_dict = PyModule_GetDict(main_module);
    PyDict_SetItemString(global_dict, varName.c_str(), (PyObject *)_impl->_npArray);
  }

  void ColumnVector::cleanPythonVar(std::string varName) const
  {
    // Could be a static method really ...

    std::string s = std::string("del ") + varName;
    const char * cmd = s.c_str();
    PyRun_SimpleString(cmd);
  }

  CurvePlot * CurvePlot::_instance = NULL;

  class CurvePlot::Internal
  {
  public:
    Internal() : _controller(0) {}
    ///! Plot2d controller from Python:
    PyObject * _controller;
  };

  CurvePlot::CurvePlot(bool test_mode)
  {
    // TODO: do use an intermediate variable '__cont', but use directly Py***CallMethod()
    _impl = new Internal();
    {
    PyLockWrapper lock;
    std::string code;
    if (test_mode)
       code = std::string("import curveplot; from SalomePyQt_MockUp import SalomePyQt;") +
           std::string("__cont=curveplot.PlotController.GetInstance(sgPyQt=SalomePyQt())");
    else
       code = std::string("import curveplot;")+
           std::string("__cont=curveplot.PlotController.GetInstance()");

    int ret = PyRun_SimpleString(const_cast<char*>(code.c_str()));
    if (ret == -1)
      throw Exception("CurvePlot::CurvePlot(): Unable to load curveplot Python module!");

    // Now get the reference to __engine and save the pointer.
    // All the calls below returns *borrowed* references
    PyObject* main_module = PyImport_AddModule((char*)"__main__");
    PyObject* global_dict = PyModule_GetDict(main_module);
    PyObject* tmp = PyDict_GetItemString(global_dict, "__cont");

    _impl->_controller = tmp;
    Py_INCREF(_impl->_controller);
    PyRun_SimpleString(const_cast<char*>("del __cont"));
    }
  }

  CurvePlot::~CurvePlot()
  {
    if(_impl->_controller != NULL)
      {
        PyLockWrapper lock;
        Py_XDECREF(_impl->_controller);
      }
    delete _impl;
  }

  CurvePlot * CurvePlot::GetInstance(bool test_mode)
  {
    if(!_instance)
      _instance = new CurvePlot(test_mode);
    return _instance;
  }

  void CurvePlot::ToggleCurveBrowser(bool with_curve_browser)
  {
    if(_instance)
      throw Exception("CurvePlot::ToggleCurveBrowser() must be invoked before anything else!");

    PyLockWrapper lock;
    std::string bool_s = with_curve_browser ? "True" : "False";
    std::string cod = std::string("import curveplot; curveplot.PlotController.WITH_CURVE_BROWSER=") + bool_s;
    PyRun_SimpleString(const_cast<char *>(cod.c_str()));
    HandleAndPrintPyError("CurvePlot::ToggleCurveBrowser(): Unable to toggle Curve Browser!");
  }

  PlotID CurvePlot::AddCurve(const ColumnVector & x, const ColumnVector & y,
                             PlotID & plot_set_id,
                             std::string curve_label/*=""*/, std::string x_label/*=""*/, std::string y_label/*=""*/,
                             bool append/*=true*/)
  {
    PyLockWrapper lock;
    PyObject * cont = GetInstance()->_impl->_controller;

    PyObject * xx = (PyObject *)x._impl->_npArray;
    PyObject * yy = (PyObject *)y._impl->_npArray;

    PyObjWrapper ret(
          PyObject_CallMethod(cont, (char *)"AddCurve", (char *)"OOOOOi", xx, yy,
              strToPyUnicode(curve_label), strToPyUnicode(x_label), strToPyUnicode(y_label),
              append ? 1 : 0)
          );
    HandleAndPrintPyError("CurvePlot::AddCurve(): unexpected error!");
    // Now extract curve_id and plot_set_id from the returned tuple:
    if(!PyTuple_Check(ret))
        throw Exception("CurvePlot::AddCurve(): Unexpected returned type!");
    PyObject * o1 = PyTuple_GetItem(ret, 0);
    if (!PyInt_Check(o1))
      throw Exception("CurvePlot::AddCurve(): Unexpected returned type!");
    PlotID curveId = PyInt_AsLong(o1);
    PyObject * o2 = PyTuple_GetItem(ret, 1);
    if (!PyInt_Check(o2))
      throw Exception("CurvePlot::AddCurve(): Unexpected returned type!");
    plot_set_id = PyInt_AsLong(o2);
    return curveId;
  }

  PlotID CurvePlot::AddPlotSet(std::string title/*=""*/)
  {
    PyLockWrapper lock;
    PyObject * cont = GetInstance()->_impl->_controller;

    PyObjWrapper ret(
          PyObject_CallMethod(cont, (char *)"AddPlotSet", (char *)"O", strToPyUnicode(title))
          );
    HandleAndPrintPyError("CurvePlot::AddPlotSet(): unexpected error!");
    return PyLong_AsLong(ret);
  }

  PlotID CurvePlot::DeleteCurve(PlotID curve_id/*=-1*/)
  {
    PyLockWrapper lock;
    PyObject * cont = GetInstance()->_impl->_controller;

    PyObjWrapper ret(
          PyObject_CallMethod(cont, (char *)"DeleteCurve", (char *)"i", curve_id)
          );
    HandleAndPrintPyError("CurvePlot::DeleteCurve(): unexpected error!");
    return PyLong_AsLong(ret);
  }

  PlotID CurvePlot::DeletePlotSet(PlotID plot_set_id/*=-1*/)
  {
    PyLockWrapper lock;
    PyObject * cont = GetInstance()->_impl->_controller;

    PyObjWrapper ret(
          PyObject_CallMethod(cont, (char *)"DeletePlotSet", (char *)"i", plot_set_id)
          );
    HandleAndPrintPyError("CurvePlot::DeletePlotSet(): unexpected error!");
    return PyLong_AsLong(ret);
  }

  PlotID CurvePlot::ClearPlotSet(PlotID plot_set_id/*=-1*/)
  {
    PyLockWrapper lock;
    PyObject * cont = GetInstance()->_impl->_controller;

    PyObjWrapper ret(
          PyObject_CallMethod(cont, (char *)"ClearPlotSet", (char *)"i", plot_set_id)
          );
    HandleAndPrintPyError("CurvePlot::ClearPlotSet(): unexpected error!");
    return PyLong_AsLong(ret);
  }

  bool CurvePlot::SetXLabel(std::string x_label, PlotID plot_set_id/*=-1*/)
  {
    PyLockWrapper lock;
    PyObject * cont = GetInstance()->_impl->_controller;

    PyObjWrapper ret(
          PyObject_CallMethod(cont, (char *)"SetXLabel", (char *)"Oi", strToPyUnicode(x_label), plot_set_id)
          );
    HandleAndPrintPyError("CurvePlot::SetXLabel(): unexpected error!");
    return ((PyObject *)ret == Py_True);
  }

  bool CurvePlot::SetYLabel(std::string y_label, PlotID plot_set_id/*=-1*/)
  {
    PyLockWrapper lock;
    PyObject * cont = GetInstance()->_impl->_controller;

    PyObjWrapper ret(
          PyObject_CallMethod(cont, (char *)"SetYLabel", (char *)"Oi", strToPyUnicode(y_label), plot_set_id)
          );
    HandleAndPrintPyError("CurvePlot::SetYLabel(): unexpected error!");
    return ((PyObject *)ret == Py_True);
  }
  
  bool CurvePlot::SetPlotSetTitle(std::string title, PlotID plot_set_id/*=-1*/)
  {
    PyLockWrapper lock;
    PyObject * cont = GetInstance()->_impl->_controller;

    PyObjWrapper ret(
          PyObject_CallMethod(cont, (char *)"SetPlotSetTitle", (char *)"Oi", strToPyUnicode(title), plot_set_id));
    HandleAndPrintPyError("CurvePlot::SetPlotSetTitle(): unexpected error!");
    return ((PyObject *)ret == Py_True);
  }

  PlotID CurvePlot::GetPlotSetID(PlotID curve_id)
  {
    PyLockWrapper lock;
    PyObject * cont = GetInstance()->_impl->_controller;

    PyObjWrapper ret(
          PyObject_CallMethod(cont, (char *)"GetPlotSetID", (char *)"i", curve_id)
          );
    HandleAndPrintPyError("CurvePlot::GetPlotSetID(): unexpected error!");
    return PyLong_AsLong(ret);
  }

  PlotID CurvePlot::GetPlotSetIDByName(std::string name)
  {
    PyLockWrapper lock;
    PyObject * cont = GetInstance()->_impl->_controller;

    PyObjWrapper ret(
          PyObject_CallMethod(cont, (char *)"GetPlotSetIDByName", (char *)"O", strToPyUnicode(name))
          );
    HandleAndPrintPyError("CurvePlot::GetPlotSetIDByName(): unexpected error!");
    return PyLong_AsLong(ret);
  }

  PlotID CurvePlot::GetCurrentCurveID()
  {
    PyLockWrapper lock;
    PyObject * cont = GetInstance()->_impl->_controller;

    PyObjWrapper ret(
          PyObject_CallMethod(cont, (char *)"GetCurrentCurveID", (char *)"")
          );
    HandleAndPrintPyError("CurvePlot::GetCurrentCurveID(): unexpected error!");
    return PyLong_AsLong(ret);
  }

  PlotID CurvePlot::GetCurrentPlotSetID()
  {
    PyLockWrapper lock;
    PyObject * cont = GetInstance()->_impl->_controller;

    PyObjWrapper ret(
          PyObject_CallMethod(cont, (char *)"GetCurrentPlotSetID", (char *)"")
          );
    HandleAndPrintPyError("CurvePlot::GetCurrentPlotSetID(): unexpected error!");
    return PyLong_AsLong(ret);
  }

  bool CurvePlot::IsValidPlotSetID(PlotID plot_set_id)
  {
    PyLockWrapper lock;
    PyObject * cont = GetInstance()->_impl->_controller;

    PyObjWrapper ret(
          PyObject_CallMethod(cont, (char *)"IsValidPlotSetID", (char *)"i", plot_set_id));
    HandleAndPrintPyError("CurvePlot::IsValidPlotSetID(): unexpected error!");
    return ((PyObject *)ret == Py_True);
  }

  int CurvePlot::GetSalomeViewID(PlotID plot_set_id)
  {
    PyLockWrapper lock;
    PyObject * cont = GetInstance()->_impl->_controller;

    PyObjWrapper ret(
          PyObject_CallMethod(cont, (char *)"GetSalomeViewID", (char *)"i", plot_set_id));
    HandleAndPrintPyError("CurvePlot::GetSalomeViewID(): unexpected error!");
    return PyLong_AsLong(ret);
  }

  void CurvePlot::OnSalomeViewTryClose(int salome_view_id)
  {
    PyLockWrapper lock;
    PyObject * cont = GetInstance()->_impl->_controller;

    PyObjWrapper ret(
          PyObject_CallMethod(cont, (char *)"OnSalomeViewTryClose", (char *)"i", salome_view_id));
    HandleAndPrintPyError("CurvePlot::OnSalomeViewTryClose(): unexpected error!");
  }

}
