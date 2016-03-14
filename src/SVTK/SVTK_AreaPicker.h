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

//  SALOME VTKViewer : build VTK viewer into Salome desktop
//  File   : SVTK_AreaPicker.h
//  Author : 
//  Module : SALOME
//
#ifndef __SVTK_AreaPicker_h
#define __SVTK_AreaPicker_h

#include "SVTK.h"
#include "VTKViewer.h"

#include <map>
#include <vector>

#include <vtkAbstractPropPicker.h>
#include <vtkDataSet.h>
#include <QVector>
#include <QPoint>

class vtkRenderer;

#ifdef WIN32
#pragma warning ( disable:4251 )
#endif

/*! \class vtkAbstractPropPicker
 * \brief For more information see <a href="http://www.vtk.org/">VTK documentation
 */
/*! \class SVTK_AreaPicker
 * \brief Rectangular picker class.
 */
class SVTK_EXPORT SVTK_AreaPicker : public vtkAbstractPropPicker
{
 public:

  enum SelectionMode { RectangleMode, PolygonMode };

  static
  SVTK_AreaPicker *New();

  vtkTypeMacro(SVTK_AreaPicker,vtkAbstractPropPicker);
  
  /*! 
    Specify tolerance for performing pick operation. Tolerance is specified
    as fraction of rendering window size. (Rendering window size is measured
    across diagonal.)
  */
  vtkSetMacro(Tolerance,double);
  vtkGetMacro(Tolerance,double);

  //! Use these methods to pick points or points and cells
  vtkSetMacro(PickPoints,int);
  vtkGetMacro(PickPoints,int);
  vtkBooleanMacro(PickPoints,int);

  int
  Pick( QVector<QPoint>& thePoints,
        vtkRenderer *theRenderer,
        SelectionMode theMode );

  int
  Pick( double theSelectionX,
        double theSelectionY,
        double theSelectionX2,
        double theSelectionY2,
        vtkRenderer *theRenderer,
        SelectionMode theMode );

  static bool
  isPointInPolygon( const QPoint& thePoint,const QVector<QPoint>& thePolygon );

  typedef std::vector<vtkIdType> TVectorIds;
  typedef std::map<vtkActor*,TVectorIds> TVectorIdsMap;

  const TVectorIdsMap& 
  GetPointIdsMap() const;

  const TVectorIdsMap& 
  GetCellIdsMap() const;

 protected:
  SVTK_AreaPicker();
  ~SVTK_AreaPicker();

  //! tolerance for computation (% of window)
  double Tolerance;

  //! use the following to control picking mode
  int PickPoints;

  //! coordinates of bounding box of selection
  int mySelection[4];

  TVectorIdsMap myPointIdsMap;
  TVectorIdsMap myCellIdsMap;

 private:
  virtual 
  int
  Pick(double, 
       double, 
       double, 
       vtkRenderer*);

  void
  SelectVisiblePoints( QVector<QPoint>& thePoints,
                       vtkRenderer *theRenderer,
                       vtkDataSet *theInput,
                       SVTK_AreaPicker::TVectorIds& theVisibleIds,
                       SVTK_AreaPicker::TVectorIds& theInVisibleIds,
                       double theTolerance,
                       SelectionMode theMode );
  void
  SelectVisibleCells( QVector<QPoint>& thePoints,
                      vtkRenderer *theRenderer,
                      vtkDataSet *theInput,
                      SVTK_AreaPicker::TVectorIds& theVectorIds,
                      double theTolerance,
                      SelectionMode theMode );
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif


