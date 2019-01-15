Unit tests are in plot_test.py.

They should be run: 
    - without any install of CurvePlot, to be sure there is no conflict
    - with the ctest command 

If anything is changed in the code, be sure to explicitely call
    cmake .
in the build directory to update the local replica of the 'curveplot' package.

PlotCurve_Standalone is a standalone Python executable that shows the
various functionalities of the package.

