#ifndef SVTK_TRIHEDRON_H
#define SVTK_TRIHEDRON_H

#include "SVTK.h"

#include "VTKViewer_Trihedron.h"

class SVTK_EXPORT SVTK_Trihedron : public VTKViewer_Trihedron
{
protected:
  SVTK_Trihedron();
  SVTK_Trihedron(const SVTK_Trihedron&); // Not implemented
  void operator = (const SVTK_Trihedron&); // Not implemented

public:
  vtkTypeMacro(SVTK_Trihedron,VTKViewer_Trihedron);
  static SVTK_Trihedron *New();
  
  virtual int GetVisibleActorCount(vtkRenderer* theRenderer);
};


#endif
