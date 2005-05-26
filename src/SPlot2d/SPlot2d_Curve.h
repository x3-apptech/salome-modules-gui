//  File   : SPlot2d_Curve.h
//  Author : Sergey RUIN
//  Module : SUIT

#ifndef SPlot2d_Curve_h
#define SPlot2d_Curve_h

#include "SPlot2d.h"  

#include "Plot2d_Curve.h"

#include <qstring.h>
#include <qcolor.h>
#include <qmemarray.h>
#ifndef _Handle_SALOME_InteractiveObject_HeaderFile
#include <Handle_SALOME_InteractiveObject.hxx>
#endif
#include "SALOME_InteractiveObject.hxx"

class SPLOT2D_EXPORT SPlot2d_Curve : public Plot2d_Curve
{
public:

  SPlot2d_Curve();
  virtual ~SPlot2d_Curve();
  SPlot2d_Curve( const SPlot2d_Curve& curve );
  SPlot2d_Curve& operator= ( const SPlot2d_Curve& curve );

  void setData( const double* hData, const double* vData, long size );

  virtual bool                             hasIO() { return !myIO.IsNull(); }
  virtual Handle(SALOME_InteractiveObject) getIO() const { return myIO; }
  virtual void                             setIO( const Handle(SALOME_InteractiveObject)& io ) { myIO = io; }

  virtual bool                             hasTableIO() { return !myTableIO.IsNull(); }
  virtual Handle(SALOME_InteractiveObject) getTableIO() const { return myTableIO; }
  virtual void                             setTableIO( const Handle(SALOME_InteractiveObject)& io ) { myTableIO = io; }
  
private:
  Handle(SALOME_InteractiveObject) myIO;
  Handle(SALOME_InteractiveObject) myTableIO;
};

#endif // SPlot2d_Curve_h


