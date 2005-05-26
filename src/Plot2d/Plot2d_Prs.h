#ifndef PLOT2D_PRS_H
#define PLOT2D_PRS_H

#include "Plot2d_Curve.h"

#include <qptrlist.h>

typedef QPtrList<Plot2d_Curve> curveList;

class PLOT2D_EXPORT Plot2d_Prs
{
public:
  Plot2d_Prs( bool theDelete = false );
  Plot2d_Prs( const Plot2d_Curve* obj, bool theDelete = false );
  ~Plot2d_Prs();

  curveList getCurves() const;
  void AddObject( const Plot2d_Curve* obj );

  bool IsNull() const;

  bool isSecondY() const;

  void setAutoDel(bool theDel);

protected:
  curveList myCurves;
  bool      mySecondY;
};

#endif
