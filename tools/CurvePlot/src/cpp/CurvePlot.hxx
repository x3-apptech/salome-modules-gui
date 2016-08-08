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

#ifndef SRC_CPP_CURVEPLOT_HXX_
#define SRC_CPP_CURVEPLOT_HXX_

#include <string>
#include <vector>

namespace CURVEPLOT
{
  typedef int PlotID;

  /**
   * This function should be called before doing anything in the CURVEPLOT namespace.
   */
  void InitializeCurvePlot();

  class ColumnVector
  {
  public:
    friend class CurvePlot;

    virtual ~ColumnVector();

    /**
     * Build a ColumnVector from a std::vector() of double. The memory is copied for now (TODO: optimize this).
     */
    static ColumnVector BuildFromStdVector(const std::vector<double> & vec);

    /**
     * Build a ColumnVector from a block of memory which was malloc'ed.
     * The memory is not copied, and the array thus created becomes responsible of the block.
     * So do NOT free the memory that you pass.
     */
    static ColumnVector BuildFromCMemory(double * data, int size);

    /**
     * Get the current size of the vector.
     */
    int size() const;

    /** Get a string representation */
    std::string toStdString() const;

  private:
    class Internal;
    ColumnVector();

    void createPythonVar(std::string varName) const;
    void cleanPythonVar(std::string varName) const;

    Internal * _impl;
  };


  /**
   * C++ wrapping of the public API exposed in the Python package curveplot. See doc there.
   */
  class CurvePlot
  {
  public:
    static PlotID AddCurve(const ColumnVector & x, const ColumnVector & y,
                           PlotID & plot_set_id,
                           std::string curve_label="", std::string x_label="", std::string y_label="",
                           bool append=true);

    static PlotID AddPlotSet(std::string title="");

    static PlotID DeleteCurve(PlotID curve_id=-1);

    static PlotID DeletePlotSet(PlotID plot_set_id=-1);

    static PlotID ClearPlotSet(PlotID plot_set_id=-1);

    static bool SetXLabel(std::string x_label, PlotID plot_set_id=-1);

    static bool SetYLabel(std::string y_label, PlotID plot_set_id=-1);

    static bool SetPlotSetTitle(std::string title, PlotID plot_set_id=-1);

    static PlotID GetPlotSetID(PlotID curve_id);

    static PlotID GetPlotSetIDByName(std::string name);

    static PlotID GetCurrentCurveID();

    static PlotID GetCurrentPlotSetID();

    static void ToggleCurveBrowser(bool with_curve_browser);

    static bool IsValidPlotSetID(PlotID plot_set_id=-1);

    static int GetSalomeViewID(PlotID plot_set_id);

    static CurvePlot * GetInstance(bool test_mode=false);

    /**! Temporary ... */
    static void OnSalomeViewTryClose(int salome_view_id);

  protected:

  private:
    class Internal;

    static CurvePlot * _instance;

    CurvePlot(bool testMode);
    virtual ~CurvePlot();

    Internal * _impl;
  };
}

#endif /* SRC_CPP_CURVEPLOT_HXX_ */
