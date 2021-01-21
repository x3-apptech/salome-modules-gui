import sys

# Try importing the same VTK module as used by libSalomePy
try:
    import vtk.vtkRenderingCore
    ok = True
except Exception as e:
    ok = False

# VTK rendering Python module might change in newer versions of ParaView
# In this case ask developers to modify SalomePy.cxx correspondingly
if not ok:
    print("VTK Rendering Python module seems to have changed, libSalomePy may not work, please correct this!")
    sys.exit(1)
