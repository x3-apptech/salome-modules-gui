// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef VTKVIEWER_ARCBUILDER_H
#define VTKVIEWER_ARCBUILDER_H

#include "VTKViewer.h"
#include <list>
#include <map>
#include <vector>

#include <vtkType.h>
#include <vtkSmartPointer.h>

class vtkCell;
class vtkDataArray;
class vtkPoints;
class vtkPolyData;
class vtkUnstructuredGrid;

class Pnt;

typedef std::list<Pnt> PntList;

vtkIdType MergevtkPoints(const std::vector< vtkSmartPointer< vtkPoints > >& theCollection,
                         const std::vector< std::vector<double> >& theScalarCollection,
                         vtkPoints* thePoints,
                         std::map<int, double>& thePntId2ScalarValue,
                         vtkIdType* &theIds);

vtkIdType Build1DArc(vtkIdType cellId, 
                     vtkUnstructuredGrid* input, 
                     vtkPolyData *output,
                     vtkIdType *pts, 
                     double myMaxArcAngle);

Pnt CreatePnt(vtkCell* cell,
              vtkDataArray* scalars,
              vtkIdType index);

/*!
 * Class for represenation coordinates X,Y,Z
 */
class XYZ{
 public:
  
  XYZ();
  XYZ(double , double , double);
  ~XYZ();
  
  double X()const {return x;}
  double Y()const {return y;}
  double Z()const {return z;}

  void SetX(const double X) { x=X; }
  void SetY(const double Y) { y=Y; }
  void SetZ(const double Z) { z=Z; }
  
  void Coord (double& X, double& Y, double& Z) const { X = x; Y = y; Z = z; }

  double Modulus () const;

 private:
  double x;
  double y;
  double z;
};

/*!
  Class for the representation point in the 3D space.
*/
class Pnt{
 public:
  Pnt();
  Pnt(double, double, double, double);
  ~Pnt();

  void Coord (double& X, double& Y, double& Z) const {coord.Coord(X,Y,Z);}
  XYZ GetXYZ() const {return coord;}
  double GetScalarValue() const { return scalarValue; }

 private:
  XYZ coord;
  double scalarValue;
};

/*!
  Class for the representation Vector in the 3D space.
*/
class Vec{
 public:

  Vec(const double Xv, const double Yv, const double Zv);
  ~Vec();
  
  XYZ GetXYZ() const {return coord;}

  double AngleBetween(const Vec & Other);
  double AngleBetweenInGrad(const Vec & Other);

  Vec VectMultiplication(const Vec & Other) const;
  
 private:
  XYZ coord;
};

/*!
  Class for the representation plane in the 3D.
*/
class Plane{

 public:
  Plane(const Pnt& thePnt1, const Pnt& thePnt2, const Pnt& thePnt3);
  ~Plane();

  double A() const {return myA;}
  double B() const {return myB;}
  double C() const {return myC;} 

  Vec GetNormale() const; 
  
 private:
  void CalculatePlane(const Pnt& thePnt1, const Pnt& thePnt2, const Pnt& thePnt3);
  
 private:
        double myA;
        double myB;
        double myC;
};


class VTKViewer_ArcBuilder{
 public:
  enum ArcStatus {Arc_Done=0, Arc_Error};
  VTKViewer_ArcBuilder(const Pnt& thePnt1,
                       const Pnt& thePnt2,
                       const Pnt& thePnt3,
                       double theAngle);
  
  ~VTKViewer_ArcBuilder();  

  Vec GetNormale();
  
  ArcStatus GetStatus(){return myStatus;}

  void GetAngle(const double theAngle);

  static double GetPointAngleOnCircle(const double theXCenter, const double theYCenter,
                                      const double theXPoint, const double theYPoint);

  vtkPoints* GetPoints();
  const std::vector<double>& GetScalarValues();

 private:
  
  enum IncOrder{MINUS=0,PLUS};
  
  vtkUnstructuredGrid* BuildGrid(const PntList& theList) const;
  vtkUnstructuredGrid* TransformGrid(vtkUnstructuredGrid* theGrid, const Vec& theAxis, const double angle) const;
  vtkUnstructuredGrid* BuildArc(std::vector<double>& theScalarValues);
  IncOrder GetArcAngle( const double& P1, const double& P2, const double& P3, double* Ang);
  


  private:
  Pnt myPnt1;
  Pnt myPnt2;
  Pnt myPnt3;

  double myAngle;
  ArcStatus myStatus;
  vtkSmartPointer<vtkPoints> myPoints;
  std::vector<double> myScalarValues;
};

#endif //VTKVIEWER_ARCBUILDER_H 
